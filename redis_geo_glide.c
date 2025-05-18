/*
  +----------------------------------------------------------------------+
  | Redis Glide FFI integration for phpredis - Geo Commands              |
  +----------------------------------------------------------------------+
  | Copyright (c) 2023-2025 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
*/

#include "php_redis.h"
#include "redis_glide.h"
#include "command_response.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Execute a GEOADD command using the Valkey Glide client */
int execute_geoadd_command(const void *glide_client, const char *key, size_t key_len,
                           zval *z_args, int argc, long *output_value)
{
    /* Check if client, key, and args are valid */
    if (!glide_client || !key || !z_args || argc < 3 || argc % 3 != 0)
    {
        return 0; // Invalid arguments
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1 + argc; /* key + (longitude, latitude, member) triplets */
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return 0;
    }

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add longitude/latitude/member triplets as arguments */
    int i;
    for (i = 0; i < argc; i++)
    {
        zval *value = &z_args[i];
        char *str_val = NULL;
        size_t str_len = 0;

        if (Z_TYPE_P(value) == IS_STRING)
        {
            args[i + 1] = (uintptr_t)Z_STRVAL_P(value);
            args_len[i + 1] = Z_STRLEN_P(value);
        }
        else
        {
            /* Convert non-string values to string */
            if (Z_TYPE_P(value) == IS_LONG)
            {
                str_val = long_to_string(Z_LVAL_P(value), &str_len);
            }
            else if (Z_TYPE_P(value) == IS_DOUBLE)
            {
                str_val = double_to_string(Z_DVAL_P(value), &str_len);
            }
            else if (Z_TYPE_P(value) == IS_TRUE)
            {
                str_val = strdup("1");
                str_len = 1;
            }
            else if (Z_TYPE_P(value) == IS_FALSE)
            {
                str_val = strdup("0");
                str_len = 1;
            }
            else
            {
                /* Handle other types or error */
                free(args);
                free(args_len);
                return 0;
            }

            if (str_val)
            {
                args[i + 1] = (uintptr_t)str_val;
                args_len[i + 1] = str_len;
            }
            else
            {
                free(args);
                free(args_len);
                return 0;
            }
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        GeoAdd,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated strings for non-string arguments */
    for (i = 0; i < argc; i++)
    {
        zval *value = &z_args[i];
        if (Z_TYPE_P(value) != IS_STRING)
        {
            free((void *)args[i + 1]);
        }
    }

    /* Free the argument arrays */
    free(args);
    free(args_len);

    /* Process the result */
    return handle_int_response(result, output_value);
}

/* Execute a GEODIST command using the Valkey Glide client */
int execute_geodist_command(const void *glide_client, const char *key, size_t key_len,
                            char *src, size_t src_len,
                            char *dst, size_t dst_len,
                            char *unit, size_t unit_len,
                            double *output_value)
{
    /* Check if client, key, src, dst are valid */
    if (!glide_client || !key || !src || !dst)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = unit ? 4 : 3;
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return 0;
    }

    /* Set arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    args[1] = (uintptr_t)src;
    args_len[1] = src_len;

    args[2] = (uintptr_t)dst;
    args_len[2] = dst_len;

    /* Optional unit argument */
    if (unit)
    {
        args[3] = (uintptr_t)unit;
        args_len[3] = unit_len;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        GeoDist,   /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        return 0;
    }

    /* Process the result (a string representing a double or nil) */
    int ret_val = 0;
    if (result->response)
    {
        if (result->response->response_type == String)
        {
            *output_value = atof(result->response->string_value);
            ret_val = 1;
        }
        else if (result->response->response_type == Float)
        {
            *output_value = result->response->float_value;
            ret_val = 1;
        }
        else if (result->response->response_type == Null)
        {
            ret_val = 0; // Distance not available
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute a GEOHASH command using the Valkey Glide client */
int execute_geohash_command(const void *glide_client, const char *key, size_t key_len,
                            zval *members, int member_count, zval *return_value)
{
    /* Check if client, key, and members are valid */
    if (!glide_client || !key || !members || member_count <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1 + member_count; /* key + members */
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return 0;
    }

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add members as arguments */
    int i;
    for (i = 0; i < member_count; i++)
    {
        zval *member = &members[i];
        char *str_val = NULL;
        size_t str_len = 0;

        if (Z_TYPE_P(member) == IS_STRING)
        {
            args[i + 1] = (uintptr_t)Z_STRVAL_P(member);
            args_len[i + 1] = Z_STRLEN_P(member);
        }
        else
        {
            /* Convert non-string values to string */
            if (Z_TYPE_P(member) == IS_LONG)
            {
                str_val = long_to_string(Z_LVAL_P(member), &str_len);
            }
            else if (Z_TYPE_P(member) == IS_DOUBLE)
            {
                str_val = double_to_string(Z_DVAL_P(member), &str_len);
            }
            else if (Z_TYPE_P(member) == IS_TRUE)
            {
                str_val = strdup("1");
                str_len = 1;
            }
            else if (Z_TYPE_P(member) == IS_FALSE)
            {
                str_val = strdup("0");
                str_len = 1;
            }
            else
            {
                /* Handle other types or error */
                free(args);
                free(args_len);
                return 0;
            }

            if (str_val)
            {
                args[i + 1] = (uintptr_t)str_val;
                args_len[i + 1] = str_len;
            }
            else
            {
                free(args);
                free(args_len);
                return 0;
            }
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        GeoHash,   /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated strings for non-string arguments */
    for (i = 0; i < member_count; i++)
    {
        zval *member = &members[i];
        if (Z_TYPE_P(member) != IS_STRING)
        {
            free((void *)args[i + 1]);
        }
    }

    /* Free the argument arrays */
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        return 0;
    }

    /* Process the result (array of geohash strings or nil) */
    int ret_val = 0;
    if (result->response && result->response->response_type == Array)
    {
        size_t i;
        for (i = 0; i < result->response->array_value_len; i++)
        {
            struct CommandResponse *element = &result->response->array_value[i];
            if (element->response_type == String)
            {
                add_next_index_stringl(return_value, element->string_value, element->string_value_len);
            }
            else if (element->response_type == Null)
            {
                add_next_index_null(return_value);
            }
        }
        ret_val = 1;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute a GEOPOS command using the Valkey Glide client */
int execute_geopos_command(const void *glide_client, const char *key, size_t key_len,
                           zval *members, int member_count, zval *return_value)
{
    /* Check if client, key, and members are valid */
    if (!glide_client || !key || !members || member_count <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1 + member_count; /* key + members */
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return 0;
    }

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add members as arguments */
    int i;
    for (i = 0; i < member_count; i++)
    {
        zval *member = &members[i];
        char *str_val = NULL;
        size_t str_len = 0;

        if (Z_TYPE_P(member) == IS_STRING)
        {
            args[i + 1] = (uintptr_t)Z_STRVAL_P(member);
            args_len[i + 1] = Z_STRLEN_P(member);
        }
        else
        {
            /* Convert non-string values to string */
            if (Z_TYPE_P(member) == IS_LONG)
            {
                str_val = long_to_string(Z_LVAL_P(member), &str_len);
            }
            else if (Z_TYPE_P(member) == IS_DOUBLE)
            {
                str_val = double_to_string(Z_DVAL_P(member), &str_len);
            }
            else if (Z_TYPE_P(member) == IS_TRUE)
            {
                str_val = strdup("1");
                str_len = 1;
            }
            else if (Z_TYPE_P(member) == IS_FALSE)
            {
                str_val = strdup("0");
                str_len = 1;
            }
            else
            {
                /* Handle other types or error */
                free(args);
                free(args_len);
                return 0;
            }

            if (str_val)
            {
                args[i + 1] = (uintptr_t)str_val;
                args_len[i + 1] = str_len;
            }
            else
            {
                free(args);
                free(args_len);
                return 0;
            }
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        GeoPos,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated strings for non-string arguments */
    for (i = 0; i < member_count; i++)
    {
        zval *member = &members[i];
        if (Z_TYPE_P(member) != IS_STRING)
        {
            free((void *)args[i + 1]);
        }
    }

    /* Free the argument arrays */
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        return 0;
    }

    /* Process the result (array of [longitude, latitude] arrays or nil) */
    int ret_val = 0;
    if (result->response && result->response->response_type == Array)
    {
        size_t i;
        for (i = 0; i < result->response->array_value_len; i++)
        {
            struct CommandResponse *element = &result->response->array_value[i];
            if (element->response_type == Array && element->array_value_len == 2)
            {
                /* Create a position array */
                zval position_array;
                array_init(&position_array);

                /* Add longitude and latitude */
                if (element->array_value[0].response_type == String)
                {
                    add_next_index_double(&position_array, atof(element->array_value[0].string_value));
                }
                else if (element->array_value[0].response_type == Float)
                {
                    add_next_index_double(&position_array, element->array_value[0].float_value);
                }

                if (element->array_value[1].response_type == String)
                {
                    add_next_index_double(&position_array, atof(element->array_value[1].string_value));
                }
                else if (element->array_value[1].response_type == Float)
                {
                    add_next_index_double(&position_array, element->array_value[1].float_value);
                }

                add_next_index_zval(return_value, &position_array);
            }
            else if (element->response_type == Null)
            {
                add_next_index_null(return_value);
            }
        }
        ret_val = 1;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute a GEORADIUS command using the Valkey Glide client */
int execute_georadius_command(const void *glide_client, const char *key, size_t key_len,
                              double longitude, double latitude, double radius,
                              const char *unit, size_t unit_len,
                              zval *opts, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !unit)
    {
        return 0;
    }

    /* Count arguments - start with base parameters */
    unsigned long arg_count = 6; /* key + longitude + latitude + radius + unit */

    /* Check for additional options */
    int withcoord = 0, withdist = 0, withhash = 0;
    long count = -1;
    const char *sort = NULL;
    size_t sort_len = 0;

    /* Check and process options if provided */
    if (opts != NULL && Z_TYPE_P(opts) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(opts);
        zval *z_withcoord, *z_withdist, *z_withhash, *z_count, *z_sort;

        /* Check for WITHCOORD option */
        if ((z_withcoord = zend_hash_str_find(ht, "WITHCOORD", sizeof("WITHCOORD") - 1)) != NULL)
        {
            withcoord = zval_is_true(z_withcoord);
            if (withcoord)
                arg_count++;
        }

        /* Check for WITHDIST option */
        if ((z_withdist = zend_hash_str_find(ht, "WITHDIST", sizeof("WITHDIST") - 1)) != NULL)
        {
            withdist = zval_is_true(z_withdist);
            if (withdist)
                arg_count++;
        }

        /* Check for WITHHASH option */
        if ((z_withhash = zend_hash_str_find(ht, "WITHHASH", sizeof("WITHHASH") - 1)) != NULL)
        {
            withhash = zval_is_true(z_withhash);
            if (withhash)
                arg_count++;
        }

        /* Check for COUNT option */
        if ((z_count = zend_hash_str_find(ht, "COUNT", sizeof("COUNT") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_count) == IS_LONG)
            {
                count = Z_LVAL_P(z_count);
                arg_count += 2; /* COUNT + value */
            }
        }

        /* Check for sorting option (ASC/DESC) */
        if ((z_sort = zend_hash_str_find(ht, "SORT", sizeof("SORT") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_sort) == IS_STRING)
            {
                sort = Z_STRVAL_P(z_sort);
                sort_len = Z_STRLEN_P(z_sort);
                arg_count++;
            }
        }
    }

    /* Allocate argument arrays */
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));
    char *longitude_str = NULL;
    char *latitude_str = NULL;
    char *radius_str = NULL;
    char *count_str = NULL;

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return 0;
    }

    /* Convert numeric values to strings */
    size_t longitude_len = 0, latitude_len = 0, radius_len = 0, count_len = 0;
    longitude_str = double_to_string(longitude, &longitude_len);
    latitude_str = double_to_string(latitude, &latitude_len);
    radius_str = double_to_string(radius, &radius_len);

    if (count > 0)
    {
        count_str = long_to_string(count, &count_len);
    }

    if (!longitude_str || !latitude_str || !radius_str || (count > 0 && !count_str))
    {
        if (longitude_str)
            free(longitude_str);
        if (latitude_str)
            free(latitude_str);
        if (radius_str)
            free(radius_str);
        if (count_str)
            free(count_str);
        free(args);
        free(args_len);
        return 0;
    }

    /* Set up base arguments */
    unsigned int arg_idx = 0;
    args[arg_idx] = (uintptr_t)key;
    args_len[arg_idx++] = key_len;

    args[arg_idx] = (uintptr_t)longitude_str;
    args_len[arg_idx++] = longitude_len;

    args[arg_idx] = (uintptr_t)latitude_str;
    args_len[arg_idx++] = latitude_len;

    args[arg_idx] = (uintptr_t)radius_str;
    args_len[arg_idx++] = radius_len;

    args[arg_idx] = (uintptr_t)unit;
    args_len[arg_idx++] = unit_len;

    /* Add optional arguments */
    if (withcoord)
    {
        args[arg_idx] = (uintptr_t)"WITHCOORD";
        args_len[arg_idx++] = sizeof("WITHCOORD") - 1;
    }

    if (withdist)
    {
        args[arg_idx] = (uintptr_t)"WITHDIST";
        args_len[arg_idx++] = sizeof("WITHDIST") - 1;
    }

    if (withhash)
    {
        args[arg_idx] = (uintptr_t)"WITHHASH";
        args_len[arg_idx++] = sizeof("WITHHASH") - 1;
    }

    if (count > 0)
    {
        args[arg_idx] = (uintptr_t)"COUNT";
        args_len[arg_idx++] = sizeof("COUNT") - 1;

        args[arg_idx] = (uintptr_t)count_str;
        args_len[arg_idx++] = count_len;
    }

    if (sort && sort_len > 0)
    {
        args[arg_idx] = (uintptr_t)sort;
        args_len[arg_idx++] = sort_len;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        GeoRadius, /* command type */
        arg_idx,   /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated memory */
    free(longitude_str);
    free(latitude_str);
    free(radius_str);
    if (count_str)
        free(count_str);
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        return 0;
    }

    /* Process the result (array of points) */
    int ret_val = 0;
    if (result->response && result->response->response_type == Array)
    {
        size_t i;
        for (i = 0; i < result->response->array_value_len; i++)
        {
            struct CommandResponse *element = &result->response->array_value[i];

            if (!withcoord && !withdist && !withhash)
            {
                /* Simple case: just the member names */
                if (element->response_type == String)
                {
                    add_next_index_stringl(return_value, element->string_value, element->string_value_len);
                }
            }
            else
            {
                /* Complex case: member name with additional data */
                zval member_info;
                array_init(&member_info);

                if (element->response_type == Array)
                {
                    /* First element is always the member name */
                    if (element->array_value_len > 0 && element->array_value[0].response_type == String)
                    {
                        add_next_index_stringl(&member_info,
                                               element->array_value[0].string_value,
                                               element->array_value[0].string_value_len);

                        /* Process additional return values based on options */
                        int idx = 1;

                        /* Distance if requested */
                        if (withdist && idx < element->array_value_len)
                        {
                            if (element->array_value[idx].response_type == String)
                            {
                                add_next_index_double(&member_info,
                                                      atof(element->array_value[idx].string_value));
                            }
                            else if (element->array_value[idx].response_type == Float)
                            {
                                add_next_index_double(&member_info,
                                                      element->array_value[idx].float_value);
                            }
                            idx++;
                        }

                        /* Hash if requested */
                        if (withhash && idx < element->array_value_len)
                        {
                            if (element->array_value[idx].response_type == Int)
                            {
                                add_next_index_long(&member_info,
                                                    element->array_value[idx].int_value);
                            }
                            idx++;
                        }

                        /* Coordinates if requested */
                        if (withcoord && idx < element->array_value_len)
                        {
                            if (element->array_value[idx].response_type == Array &&
                                element->array_value[idx].array_value_len == 2)
                            {
                                /* Create a coordinates array */
                                zval coordinates;
                                array_init(&coordinates);

                                /* Add longitude */
                                if (element->array_value[idx].array_value[0].response_type == String)
                                {
                                    add_next_index_double(&coordinates,
                                                          atof(element->array_value[idx].array_value[0].string_value));
                                }
                                else if (element->array_value[idx].array_value[0].response_type == Float)
                                {
                                    add_next_index_double(&coordinates,
                                                          element->array_value[idx].array_value[0].float_value);
                                }

                                /* Add latitude */
                                if (element->array_value[idx].array_value[1].response_type == String)
                                {
                                    add_next_index_double(&coordinates,
                                                          atof(element->array_value[idx].array_value[1].string_value));
                                }
                                else if (element->array_value[idx].array_value[1].response_type == Float)
                                {
                                    add_next_index_double(&coordinates,
                                                          element->array_value[idx].array_value[1].float_value);
                                }

                                add_next_index_zval(&member_info, &coordinates);
                            }
                        }
                    }
                }

                add_next_index_zval(return_value, &member_info);
            }
        }

        ret_val = 1;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute a GEORADIUS_RO command using the Valkey Glide client */
int execute_georadius_ro_command(const void *glide_client, const char *key, size_t key_len,
                                 double longitude, double latitude, double radius,
                                 const char *unit, size_t unit_len,
                                 zval *opts, zval *return_value)
{
    /* This is essentially the same as GEORADIUS but with a different command type */
    /* Check if client and key are valid */
    if (!glide_client || !key || !unit)
    {
        return 0;
    }

    /* Count arguments - start with base parameters */
    unsigned long arg_count = 6; /* key + longitude + latitude + radius + unit */

    /* Check for additional options */
    int withcoord = 0, withdist = 0, withhash = 0;
    long count = -1;
    const char *sort = NULL;
    size_t sort_len = 0;

    /* Check and process options if provided */
    if (opts != NULL && Z_TYPE_P(opts) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(opts);
        zval *z_withcoord, *z_withdist, *z_withhash, *z_count, *z_sort;

        /* Check for WITHCOORD option */
        if ((z_withcoord = zend_hash_str_find(ht, "WITHCOORD", sizeof("WITHCOORD") - 1)) != NULL)
        {
            withcoord = zval_is_true(z_withcoord);
            if (withcoord)
                arg_count++;
        }

        /* Check for WITHDIST option */
        if ((z_withdist = zend_hash_str_find(ht, "WITHDIST", sizeof("WITHDIST") - 1)) != NULL)
        {
            withdist = zval_is_true(z_withdist);
            if (withdist)
                arg_count++;
        }

        /* Check for WITHHASH option */
        if ((z_withhash = zend_hash_str_find(ht, "WITHHASH", sizeof("WITHHASH") - 1)) != NULL)
        {
            withhash = zval_is_true(z_withhash);
            if (withhash)
                arg_count++;
        }

        /* Check for COUNT option */
        if ((z_count = zend_hash_str_find(ht, "COUNT", sizeof("COUNT") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_count) == IS_LONG)
            {
                count = Z_LVAL_P(z_count);
                arg_count += 2; /* COUNT + value */
            }
        }

        /* Check for sorting option (ASC/DESC) */
        if ((z_sort = zend_hash_str_find(ht, "SORT", sizeof("SORT") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_sort) == IS_STRING)
            {
                sort = Z_STRVAL_P(z_sort);
                sort_len = Z_STRLEN_P(z_sort);
                arg_count++;
            }
        }
    }

    /* Allocate argument arrays */
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));
    char *longitude_str = NULL;
    char *latitude_str = NULL;
    char *radius_str = NULL;
    char *count_str = NULL;

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return 0;
    }

    /* Convert numeric values to strings */
    size_t longitude_len = 0, latitude_len = 0, radius_len = 0, count_len = 0;
    longitude_str = double_to_string(longitude, &longitude_len);
    latitude_str = double_to_string(latitude, &latitude_len);
    radius_str = double_to_string(radius, &radius_len);

    if (count > 0)
    {
        count_str = long_to_string(count, &count_len);
    }

    if (!longitude_str || !latitude_str || !radius_str || (count > 0 && !count_str))
    {
        if (longitude_str)
            free(longitude_str);
        if (latitude_str)
            free(latitude_str);
        if (radius_str)
            free(radius_str);
        if (count_str)
            free(count_str);
        free(args);
        free(args_len);
        return 0;
    }

    /* Set up base arguments */
    unsigned int arg_idx = 0;
    args[arg_idx] = (uintptr_t)key;
    args_len[arg_idx++] = key_len;

    args[arg_idx] = (uintptr_t)longitude_str;
    args_len[arg_idx++] = longitude_len;

    args[arg_idx] = (uintptr_t)latitude_str;
    args_len[arg_idx++] = latitude_len;

    args[arg_idx] = (uintptr_t)radius_str;
    args_len[arg_idx++] = radius_len;

    args[arg_idx] = (uintptr_t)unit;
    args_len[arg_idx++] = unit_len;

    /* Add optional arguments */
    if (withcoord)
    {
        args[arg_idx] = (uintptr_t)"WITHCOORD";
        args_len[arg_idx++] = sizeof("WITHCOORD") - 1;
    }

    if (withdist)
    {
        args[arg_idx] = (uintptr_t)"WITHDIST";
        args_len[arg_idx++] = sizeof("WITHDIST") - 1;
    }

    if (withhash)
    {
        args[arg_idx] = (uintptr_t)"WITHHASH";
        args_len[arg_idx++] = sizeof("WITHHASH") - 1;
    }

    if (count > 0)
    {
        args[arg_idx] = (uintptr_t)"COUNT";
        args_len[arg_idx++] = sizeof("COUNT") - 1;

        args[arg_idx] = (uintptr_t)count_str;
        args_len[arg_idx++] = count_len;
    }

    if (sort && sort_len > 0)
    {
        args[arg_idx] = (uintptr_t)sort;
        args_len[arg_idx++] = sort_len;
    }

    /* Execute the command as GeoRadius type - no special read-only variant in the enum */
    CommandResult *result = execute_command(
        glide_client,
        GeoRadius, /* command type - using same as GEORADIUS since RO variant isn't defined */
        arg_idx,   /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated memory */
    free(longitude_str);
    free(latitude_str);
    free(radius_str);
    if (count_str)
        free(count_str);
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        return 0;
    }

    /* Process the result (array of points) - same as georadius processing */
    int ret_val = 0;
    if (result->response && result->response->response_type == Array)
    {
        size_t i;
        for (i = 0; i < result->response->array_value_len; i++)
        {
            struct CommandResponse *element = &result->response->array_value[i];

            if (!withcoord && !withdist && !withhash)
            {
                /* Simple case: just the member names */
                if (element->response_type == String)
                {
                    add_next_index_stringl(return_value, element->string_value, element->string_value_len);
                }
            }
            else
            {
                /* Complex case: member name with additional data */
                zval member_info;
                array_init(&member_info);

                if (element->response_type == Array)
                {
                    /* First element is always the member name */
                    if (element->array_value_len > 0 && element->array_value[0].response_type == String)
                    {
                        add_next_index_stringl(&member_info,
                                               element->array_value[0].string_value,
                                               element->array_value[0].string_value_len);

                        /* Process additional return values based on options */
                        int idx = 1;

                        /* Distance if requested */
                        if (withdist && idx < element->array_value_len)
                        {
                            if (element->array_value[idx].response_type == String)
                            {
                                add_next_index_double(&member_info,
                                                      atof(element->array_value[idx].string_value));
                            }
                            else if (element->array_value[idx].response_type == Float)
                            {
                                add_next_index_double(&member_info,
                                                      element->array_value[idx].float_value);
                            }
                            idx++;
                        }

                        /* Hash if requested */
                        if (withhash && idx < element->array_value_len)
                        {
                            if (element->array_value[idx].response_type == Int)
                            {
                                add_next_index_long(&member_info,
                                                    element->array_value[idx].int_value);
                            }
                            idx++;
                        }

                        /* Coordinates if requested */
                        if (withcoord && idx < element->array_value_len)
                        {
                            if (element->array_value[idx].response_type == Array &&
                                element->array_value[idx].array_value_len == 2)
                            {
                                /* Create a coordinates array */
                                zval coordinates;
                                array_init(&coordinates);

                                /* Add longitude */
                                if (element->array_value[idx].array_value[0].response_type == String)
                                {
                                    add_next_index_double(&coordinates,
                                                          atof(element->array_value[idx].array_value[0].string_value));
                                }
                                else if (element->array_value[idx].array_value[0].response_type == Float)
                                {
                                    add_next_index_double(&coordinates,
                                                          element->array_value[idx].array_value[0].float_value);
                                }

                                /* Add latitude */
                                if (element->array_value[idx].array_value[1].response_type == String)
                                {
                                    add_next_index_double(&coordinates,
                                                          atof(element->array_value[idx].array_value[1].string_value));
                                }
                                else if (element->array_value[idx].array_value[1].response_type == Float)
                                {
                                    add_next_index_double(&coordinates,
                                                          element->array_value[idx].array_value[1].float_value);
                                }

                                add_next_index_zval(&member_info, &coordinates);
                            }
                        }
                    }
                }

                add_next_index_zval(return_value, &member_info);
            }
        }

        ret_val = 1;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}
