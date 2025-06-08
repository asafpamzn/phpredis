/*
  +----------------------------------------------------------------------+
  | Redis Glide FFI integration for phpredis                             |
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
#include "valkey_glide_z_common.h"
#include "command_response.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Helper function to prepare aggregate option */
int prepare_aggregate_option(zval *options, uintptr_t *agg_arg, unsigned long *agg_len)
{
    if (!options || Z_TYPE_P(options) != IS_ARRAY)
    {
        return 0;
    }

    /* Check for AGGREGATE option */
    HashTable *ht = Z_ARRVAL_P(options);
    zval *aggregate = zend_hash_str_find(ht, "AGGREGATE", sizeof("AGGREGATE") - 1);
    if (!aggregate || Z_TYPE_P(aggregate) != IS_STRING)
    {
        return 0;
    }

    /* Get aggregate value */
    const char *agg_str = Z_STRVAL_P(aggregate);
    size_t agg_str_len = Z_STRLEN_P(aggregate);

    /* Convert to uppercase for comparison */
    char *upper_agg = emalloc(agg_str_len + 1);
    for (size_t i = 0; i < agg_str_len; i++)
    {
        upper_agg[i] = toupper(agg_str[i]);
    }
    upper_agg[agg_str_len] = '\0';

    /* Check if valid aggregate option */
    int valid = 0;
    if (strcmp(upper_agg, "SUM") == 0 ||
        strcmp(upper_agg, "MIN") == 0 ||
        strcmp(upper_agg, "MAX") == 0)
    {
        *agg_arg = (uintptr_t)upper_agg;
        *agg_len = agg_str_len;
        valid = 1;
    }
    else
    {
        efree(upper_agg);
    }

    return valid;
}

/* Helper function to prepare weights array from zval */
int prepare_weights_array(zval *weights, int weights_count, uintptr_t **args, unsigned long **args_len)
{
    /* Allocate memory for arguments */
    *args = (uintptr_t *)emalloc(weights_count * sizeof(uintptr_t));
    *args_len = (unsigned long *)emalloc(weights_count * sizeof(unsigned long));

    if (!(*args) || !(*args_len))
    {
        if (*args)
            efree(*args);
        if (*args_len)
            efree(*args_len);
        return 0;
    }

    /* Convert weights to strings */
    HashTable *weights_hash = Z_ARRVAL_P(weights);
    zval *weight;
    int idx = 0;
    char weight_str[32];

    ZEND_HASH_FOREACH_VAL(weights_hash, weight)
    {
        /* Special handling for string weights that represent infinity */
        if (Z_TYPE_P(weight) == IS_STRING)
        {
            const char *str_val = Z_STRVAL_P(weight);
            if (strcasecmp(str_val, "inf") == 0 ||
                strcasecmp(str_val, "+inf") == 0 ||
                strcasecmp(str_val, "-inf") == 0)
            {
                /* Use the infinity string directly */
                (*args)[idx] = (uintptr_t)estrdup(str_val);
                (*args_len)[idx] = Z_STRLEN_P(weight);
                idx++;
                continue; /* Skip the rest of this iteration */
            }
        }

        /* For non-infinity values, convert as usual */
        if (Z_TYPE_P(weight) != IS_LONG && Z_TYPE_P(weight) != IS_DOUBLE)
        {
            convert_to_double(weight);
        }

        if (Z_TYPE_P(weight) == IS_LONG)
        {
            snprintf(weight_str, sizeof(weight_str), "%ld", Z_LVAL_P(weight));
        }
        else
        {
            snprintf(weight_str, sizeof(weight_str), "%.6f", Z_DVAL_P(weight));
        }

        (*args)[idx] = (uintptr_t)estrdup(weight_str);
        (*args_len)[idx] = strlen(weight_str);
        idx++;
    }
    ZEND_HASH_FOREACH_END();

    return 1;
}

/* Helper function to free weights strings */
void free_weights_strings(uintptr_t *args, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (args[i])
        {
            efree((void *)args[i]);
        }
    }
}

/* Execute a ZINTERCARD command using the Valkey Glide client */
int execute_zintercard_command(const void *glide_client, zval *keys, int keys_count, zval *options, zval *return_value)
{
    z_command_args_t args = {0};
    args.members = keys; /* Reuse members field for keys */
    args.member_count = keys_count;
    args.options = options;

    return execute_z_generic_command(
        glide_client,
        ZInterCard,
        &args,
        return_value,
        process_z_long_to_zval_result);
}

/* Execute a ZUNION command using the Valkey Glide client */
int execute_zunion_command(const void *glide_client, zval *keys, int keys_count, zval *weights, zval *options, zval *return_value)
{
    z_command_args_t args = {0};
    args.members = keys; /* Reuse members field for keys */
    args.member_count = keys_count;
    args.weights = weights;
    args.options = options;

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, 0}; /* withscores determined by options */

    return execute_z_generic_command(
        glide_client,
        ZUnion,
        &args,
        &array_data,
        process_z_array_result);
}

/* Execute a ZPOPMAX command using the Valkey Glide client */
int execute_zpopmax_command(const void *glide_client, const char *key, size_t key_len, long count, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || key_len <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = count > 1 ? 2 : 1; /* key + count (optional) */
    uintptr_t args[2];
    unsigned long args_len[2];
    char count_str[32];

    /* Set key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add count if needed */
    if (count > 1)
    {
        snprintf(count_str, sizeof(count_str), "%ld", count);
        args[1] = (uintptr_t)count_str;
        args_len[1] = strlen(count_str);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZPopMax,   /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Process the result */
    int status = 0;
    if (result)
    {
        if (result->command_error)
        {
            /* Command failed */
            free_command_result(result);
            return 0;
        }

        if (result->response)
        {
            /* ZPOPMAX returns an array of member-score pairs */
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_ASSOSIATIVE_ARRAY, false);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute a ZPOPMIN command using the Valkey Glide client */
int execute_zpopmin_command(const void *glide_client, const char *key, size_t key_len, long count, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || key_len <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = count > 1 ? 2 : 1; /* key + count (optional) */
    uintptr_t args[2];
    unsigned long args_len[2];
    char count_str[32];

    /* Set key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add count if needed */
    if (count > 1)
    {
        snprintf(count_str, sizeof(count_str), "%ld", count);
        args[1] = (uintptr_t)count_str;
        args_len[1] = strlen(count_str);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZPopMin,   /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Process the result */
    int status = 0;
    if (result)
    {
        if (result->command_error)
        {
            /* Command failed */
            free_command_result(result);
            return 0;
        }

        if (result->response)
        {
            /* ZPOPMIN returns an array of member-score pairs */
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_ASSOSIATIVE_ARRAY, false);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute a ZSCAN command using the Valkey Glide client */
int execute_zscan_command(const void *glide_client, const char *key, size_t key_len, long *cursor,
                          char *pattern, size_t pattern_len, long count, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || key_len <= 0 || !cursor)
    {
        return 0;
    }

    /* Calculate number of arguments */
    unsigned long arg_count = 2; /* key + cursor */
    if (pattern && pattern_len > 0)
    {
        arg_count += 2; /* MATCH + pattern */
    }
    if (count > 0)
    {
        arg_count += 2; /* COUNT + count */
    }

    /* Prepare command arguments */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Convert cursor to string */
    char cursor_str[32];
    snprintf(cursor_str, sizeof(cursor_str), "%ld", *cursor);

    /* Set key and cursor */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;
    args[1] = (uintptr_t)cursor_str;
    args_len[1] = strlen(cursor_str);

    unsigned int offset = 2;

    /* Add MATCH if needed */
    if (pattern && pattern_len > 0)
    {
        args[offset] = (uintptr_t)"MATCH";
        args_len[offset] = 5;
        offset++;

        args[offset] = (uintptr_t)pattern;
        args_len[offset] = pattern_len;
        offset++;
    }

    /* Add COUNT if needed */
    if (count > 0)
    {
        args[offset] = (uintptr_t)"COUNT";
        args_len[offset] = 5;
        offset++;

        char count_str[32];
        snprintf(count_str, sizeof(count_str), "%ld", count);
        args[offset] = (uintptr_t)estrdup(count_str);
        args_len[offset] = strlen(count_str);
        offset++;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZScan,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated memory for COUNT */
    if (count > 0)
    {
        efree((void *)args[arg_count - 1]);
    }

    /* Free arrays */
    efree(args);
    efree(args_len);

    /* Process the result */
    int status = 0;
    if (result)
    {
        if (result->command_error)
        {
            /* Command failed */
            free_command_result(result);
            return 0;
        }

        if (result->response && result->response->response_type == Array && result->response->array_value_len >= 2)
        {
            /* Get new cursor from first element */
            CommandResponse *cursor_resp = &result->response->array_value[0];
            if (cursor_resp->response_type == String)
            {
                *cursor = atol(cursor_resp->string_value);
            }

            /* Initialize result array */
            array_init(return_value);

            /* Add cursor as first element */
            zval z_cursor;
            ZVAL_LONG(&z_cursor, *cursor);
            add_next_index_zval(return_value, &z_cursor);

            /* Add elements array as second element */
            zval z_elements;
            CommandResponse *elements = &result->response->array_value[1];
            if (elements->response_type == Array)
            {
                command_response_to_zval(elements, &z_elements, COMMAND_RESPONSE_ASSOSIATIVE_ARRAY, false);
                add_next_index_zval(return_value, &z_elements);
            }
            else
            {
                array_init(&z_elements);
                add_next_index_zval(return_value, &z_elements);
            }

            status = 1;
        }

        free_command_result(result);
    }

    return status;
}
