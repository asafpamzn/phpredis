/* -*- Mode: C; tab-width: 4 -*- */
/*
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2009 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Your Name                                                     |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_redis.h"
#include "redis_glide.h"
#include "command_response.h"
#include <php.h>
#include "zend_API.h"

/* Import string conversion functions */
extern char *long_to_string(long value, size_t *len);
extern char *double_to_string(double value, size_t *len);

/* Helper function to process GEOSEARCH response with WITH* options */
static int process_geosearch_response(CommandResponse *response, zval *return_value, int withcoord, int withdist, int withhash)
{
    if (!response || response->response_type != Array)
    {
        return 0;
    }

    /* Initialize the return array */
    array_init(return_value);

    /* Process each element in the response array */
    for (int64_t i = 0; i < response->array_value_len; i++)
    {
        CommandResponse *element = &response->array_value[i];

        /* Each element should be an array with [member, data...] */
        if (element->response_type == Array && element->array_value_len >= 2)
        {
            /* First element is the member name */
            CommandResponse *member_resp = &element->array_value[0];
            if (member_resp->response_type != String)
            {
                continue;
            }

            char *member_name = member_resp->string_value;
            size_t member_len = member_resp->string_value_len;

            /* Create data array for this member */
            zval data_array;
            array_init(&data_array);

            /* Debug: Print the structure */
            printf("GEOSEARCH Response for member '%s':\n", member_name);
            printf("  Total elements in array: %ld\n", element->array_value_len);
            for (int j = 1; j < element->array_value_len; j++)
            {
                CommandResponse *item = &element->array_value[j];
                printf("  Element %d type: %d\n", j, item->response_type);
                if (item->response_type == String)
                {
                    printf("    String value: %.*s\n", (int)item->string_value_len, item->string_value);
                }
                else if (item->response_type == Int)
                {
                    printf("    Int value: %ld\n", item->int_value);
                }
                else if (item->response_type == Array)
                {
                    printf("    Array with %ld elements\n", item->array_value_len);
                }
            }

            /* Since we're searching from the same member, distance should be 0 */
            if (withdist)
            {
                add_next_index_double(&data_array, 0.0);
            }

            /* Look for hash value - it might be at different positions */
            int found_hash = 0;
            for (int j = 1; j < element->array_value_len && !found_hash; j++)
            {
                CommandResponse *item = &element->array_value[j];
                if (item->response_type == Int)
                {
                    if (withhash)
                    {
                        add_next_index_long(&data_array, item->int_value);
                    }
                    found_hash = 1;
                }
                else if (item->response_type == String && withhash)
                {
                    /* Try to parse as number */
                    char *endptr;
                    long hash_val = strtol(item->string_value, &endptr, 10);
                    if (*endptr == '\0')
                    {
                        add_next_index_long(&data_array, hash_val);
                        found_hash = 1;
                    }
                }
            }

            /* Look for coordinates array */
            if (withcoord)
            {
                int found_coords = 0;
                for (int j = 1; j < element->array_value_len && !found_coords; j++)
                {
                    CommandResponse *item = &element->array_value[j];
                    if (item->response_type == Array && item->array_value_len == 2)
                    {
                        zval coord_array;
                        array_init(&coord_array);

                        /* Longitude */
                        CommandResponse *lon_resp = &item->array_value[0];
                        if (lon_resp->response_type == String)
                        {
                            double lon = atof(lon_resp->string_value);
                            add_next_index_double(&coord_array, lon);
                        }
                        else if (lon_resp->response_type == Float)
                        {
                            add_next_index_double(&coord_array, lon_resp->float_value);
                        }

                        /* Latitude */
                        CommandResponse *lat_resp = &item->array_value[1];
                        if (lat_resp->response_type == String)
                        {
                            double lat = atof(lat_resp->string_value);
                            add_next_index_double(&coord_array, lat);
                        }
                        else if (lat_resp->response_type == Float)
                        {
                            add_next_index_double(&coord_array, lat_resp->float_value);
                        }

                        add_next_index_zval(&data_array, &coord_array);
                        found_coords = 1;
                    }
                }

                /* If no coordinates found, add empty array */
                if (!found_coords)
                {
                    zval empty_array;
                    array_init(&empty_array);
                    add_next_index_zval(&data_array, &empty_array);
                }
            }

            /* Add the data array with member name as key */
            add_assoc_zval_ex(return_value, member_name, member_len, &data_array);
        }
    }

    return 1;
}

/* GEOSEARCH implementation */
int execute_geosearch_command(const void *glide_client, const char *key, size_t key_len,
                              zval *from, double *by_radius, const char *by_unit, size_t by_unit_len,
                              zval *options, zval *return_value)
{
    /* Check if client is valid */
    if (!glide_client || !key || !from || !by_radius)
    {
        return 0;
    }

    /* Calculate the maximum arguments we might need */
    unsigned long max_args = 15; /* Conservative estimate */
    uintptr_t *args = (uintptr_t *)emalloc(max_args * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(max_args * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Track allocated strings that need to be freed */
    char **allocated_strings = (char **)ecalloc(5, sizeof(char *));
    int allocated_count = 0;

    if (!allocated_strings)
    {
        efree(args);
        efree(args_len);
        return 0;
    }

    /* Start building command arguments */
    unsigned long arg_idx = 0;

    /* First argument: key */
    args[arg_idx] = (uintptr_t)key;
    args_len[arg_idx++] = key_len;

    /* Handle FROM parameter - could be member name or coordinates */
    if (Z_TYPE_P(from) == IS_STRING)
    {
        /* FROMMEMBER <member> */
        args[arg_idx] = (uintptr_t)"FROMMEMBER";
        args_len[arg_idx++] = strlen("FROMMEMBER");

        args[arg_idx] = (uintptr_t)Z_STRVAL_P(from);
        args_len[arg_idx++] = Z_STRLEN_P(from);
    }
    else if (Z_TYPE_P(from) == IS_ARRAY)
    {
        /* FROMLONLAT <lon> <lat> */
        zval *lon, *lat;
        lon = zend_hash_index_find(Z_ARRVAL_P(from), 0);
        lat = zend_hash_index_find(Z_ARRVAL_P(from), 1);

        if (lon && lat)
        {
            args[arg_idx] = (uintptr_t)"FROMLONLAT";
            args_len[arg_idx++] = strlen("FROMLONLAT");

            size_t str_len;
            char *str_val = double_to_string(zval_get_double(lon), &str_len);
            if (!str_val)
            {
                efree(allocated_strings);
                efree(args);
                efree(args_len);
                return 0;
            }
            args[arg_idx] = (uintptr_t)str_val;
            args_len[arg_idx++] = str_len;
            allocated_strings[allocated_count++] = str_val;

            str_val = double_to_string(zval_get_double(lat), &str_len);
            if (!str_val)
            {
                for (int i = 0; i < allocated_count; i++)
                    efree(allocated_strings[i]);
                efree(allocated_strings);
                efree(args);
                efree(args_len);
                return 0;
            }
            args[arg_idx] = (uintptr_t)str_val;
            args_len[arg_idx++] = str_len;
            allocated_strings[allocated_count++] = str_val;
        }
    }

    /* Handle BY parameter */
    if (by_radius != NULL)
    {
        /* BYRADIUS <radius> <unit> */
        args[arg_idx] = (uintptr_t)"BYRADIUS";
        args_len[arg_idx++] = strlen("BYRADIUS");

        size_t str_len;
        char *str_val = double_to_string(*by_radius, &str_len);
        if (!str_val)
        {
            for (int i = 0; i < allocated_count; i++)
                efree(allocated_strings[i]);
            efree(allocated_strings);
            efree(args);
            efree(args_len);
            return 0;
        }
        args[arg_idx] = (uintptr_t)str_val;
        args_len[arg_idx++] = str_len;
        allocated_strings[allocated_count++] = str_val;

        args[arg_idx] = (uintptr_t)by_unit;
        args_len[arg_idx++] = by_unit_len;
    }

    /* Handle options if provided */
    int withcoord = 0, withdist = 0, withhash = 0;
    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht_options = Z_ARRVAL_P(options);
        zval *opt_val;

        /* Check for each option in the array */
        ZEND_HASH_FOREACH_VAL(ht_options, opt_val)
        {
            if (Z_TYPE_P(opt_val) == IS_STRING)
            {
                if (strcasecmp(Z_STRVAL_P(opt_val), "withcoord") == 0)
                {
                    withcoord = 1;
                    args[arg_idx] = (uintptr_t)"WITHCOORD";
                    args_len[arg_idx++] = strlen("WITHCOORD");
                }
                else if (strcasecmp(Z_STRVAL_P(opt_val), "withdist") == 0)
                {
                    withdist = 1;
                    args[arg_idx] = (uintptr_t)"WITHDIST";
                    args_len[arg_idx++] = strlen("WITHDIST");
                }
                else if (strcasecmp(Z_STRVAL_P(opt_val), "withhash") == 0)
                {
                    withhash = 1;
                    args[arg_idx] = (uintptr_t)"WITHHASH";
                    args_len[arg_idx++] = strlen("WITHHASH");
                }
            }
        }
        ZEND_HASH_FOREACH_END();
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        GeoSearch, /* command type = 509 */
        arg_idx,   /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated strings */
    for (int i = 0; i < allocated_count; i++)
    {
        efree(allocated_strings[i]);
    }
    efree(allocated_strings);
    efree(args);
    efree(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing GEOSEARCH command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Process the response based on whether we have WITH* options */
    int ret_val = 0;
    if (result->response)
    {
        if (withcoord || withdist || withhash)
        {
            /* Complex response with additional data - use custom processor */
            ret_val = process_geosearch_response(result->response, return_value, withcoord, withdist, withhash);
        }
        else
        {
            /* Simple array of member names */
            ret_val = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE);
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* GEOSEARCHSTORE implementation */
int execute_geosearchstore_command(const void *glide_client, const char *dest, size_t dest_len,
                                   const char *src, size_t src_len, zval *from, double *by_radius,
                                   const char *by_unit, size_t by_unit_len, zval *options, long *output_value)
{
    /* Check if client is valid */
    if (!glide_client || !dest || !src || !from || !by_radius)
    {
        return 0;
    }

    /* Calculate the maximum arguments we might need */
    unsigned long max_args = 15; /* Conservative estimate */
    uintptr_t *args = (uintptr_t *)emalloc(max_args * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(max_args * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Track allocated strings that need to be freed */
    char **allocated_strings = (char **)ecalloc(5, sizeof(char *));
    int allocated_count = 0;

    if (!allocated_strings)
    {
        efree(args);
        efree(args_len);
        return 0;
    }

    /* Start building command arguments */
    unsigned long arg_idx = 0;

    /* First argument: destination key */
    args[arg_idx] = (uintptr_t)dest;
    args_len[arg_idx++] = dest_len;

    /* Second argument: source key */
    args[arg_idx] = (uintptr_t)src;
    args_len[arg_idx++] = src_len;

    /* Handle FROM parameter - could be member name or coordinates */
    if (Z_TYPE_P(from) == IS_STRING)
    {
        /* FROMMEMBER <member> */
        args[arg_idx] = (uintptr_t)"FROMMEMBER";
        args_len[arg_idx++] = strlen("FROMMEMBER");

        args[arg_idx] = (uintptr_t)Z_STRVAL_P(from);
        args_len[arg_idx++] = Z_STRLEN_P(from);
    }
    else if (Z_TYPE_P(from) == IS_ARRAY)
    {
        /* FROMLONLAT <lon> <lat> */
        zval *lon, *lat;
        lon = zend_hash_index_find(Z_ARRVAL_P(from), 0);
        lat = zend_hash_index_find(Z_ARRVAL_P(from), 1);

        if (lon && lat)
        {
            args[arg_idx] = (uintptr_t)"FROMLONLAT";
            args_len[arg_idx++] = strlen("FROMLONLAT");

            size_t str_len;
            char *str_val = double_to_string(zval_get_double(lon), &str_len);
            if (!str_val)
            {
                efree(allocated_strings);
                efree(args);
                efree(args_len);
                return 0;
            }
            args[arg_idx] = (uintptr_t)str_val;
            args_len[arg_idx++] = str_len;
            allocated_strings[allocated_count++] = str_val;

            str_val = double_to_string(zval_get_double(lat), &str_len);
            if (!str_val)
            {
                for (int i = 0; i < allocated_count; i++)
                    efree(allocated_strings[i]);
                efree(allocated_strings);
                efree(args);
                efree(args_len);
                return 0;
            }
            args[arg_idx] = (uintptr_t)str_val;
            args_len[arg_idx++] = str_len;
            allocated_strings[allocated_count++] = str_val;
        }
    }

    /* Handle BY parameter */
    if (by_radius != NULL)
    {
        /* BYRADIUS <radius> <unit> */
        args[arg_idx] = (uintptr_t)"BYRADIUS";
        args_len[arg_idx++] = strlen("BYRADIUS");

        size_t str_len;
        char *str_val = double_to_string(*by_radius, &str_len);
        if (!str_val)
        {
            for (int i = 0; i < allocated_count; i++)
                efree(allocated_strings[i]);
            efree(allocated_strings);
            efree(args);
            efree(args_len);
            return 0;
        }
        args[arg_idx] = (uintptr_t)str_val;
        args_len[arg_idx++] = str_len;
        allocated_strings[allocated_count++] = str_val;

        args[arg_idx] = (uintptr_t)by_unit;
        args_len[arg_idx++] = by_unit_len;
    }

    /* Handle options if provided - for GEOSEARCHSTORE, we support COUNT, ASC/DESC, STOREDIST */
    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht_options = Z_ARRVAL_P(options);
        zval *opt_val;

        /* COUNT option */
        if ((opt_val = zend_hash_str_find(ht_options, "count", sizeof("count") - 1)) != NULL)
        {
            long count = zval_get_long(opt_val);
            if (count > 0)
            {
                args[arg_idx] = (uintptr_t)"COUNT";
                args_len[arg_idx++] = strlen("COUNT");

                size_t str_len;
                char *str_val = long_to_string(count, &str_len);
                if (!str_val)
                {
                    for (int i = 0; i < allocated_count; i++)
                        efree(allocated_strings[i]);
                    efree(allocated_strings);
                    efree(args);
                    efree(args_len);
                    return 0;
                }
                args[arg_idx] = (uintptr_t)str_val;
                args_len[arg_idx++] = str_len;
                allocated_strings[allocated_count++] = str_val;
            }
        }

        /* SORT option */
        if ((opt_val = zend_hash_str_find(ht_options, "sort", sizeof("sort") - 1)) != NULL)
        {
            if (Z_TYPE_P(opt_val) == IS_STRING)
            {
                char *sort_order = Z_STRVAL_P(opt_val);
                if (strcasecmp(sort_order, "ASC") == 0)
                {
                    args[arg_idx] = (uintptr_t)"ASC";
                    args_len[arg_idx++] = strlen("ASC");
                }
                else if (strcasecmp(sort_order, "DESC") == 0)
                {
                    args[arg_idx] = (uintptr_t)"DESC";
                    args_len[arg_idx++] = strlen("DESC");
                }
            }
        }

        /* STOREDIST option */
        if ((opt_val = zend_hash_str_find(ht_options, "storedist", sizeof("storedist") - 1)) != NULL)
        {
            if (zval_is_true(opt_val))
            {
                args[arg_idx] = (uintptr_t)"STOREDIST";
                args_len[arg_idx++] = strlen("STOREDIST");
            }
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        GeoSearchStore, /* command type = 510 */
        arg_idx,        /* number of arguments */
        args,           /* arguments */
        args_len        /* argument lengths */
    );

    /* Free allocated strings */
    for (int i = 0; i < allocated_count; i++)
    {
        efree(allocated_strings[i]);
    }
    efree(allocated_strings);
    efree(args);
    efree(args_len);

    /* Use the integer response handler */
    return handle_int_response(result, output_value);
}
