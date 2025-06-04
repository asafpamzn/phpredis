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
#include "command_response.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Helper function to prepare keys array from zval */
static int prepare_keys_array(zval *keys, int keys_count, uintptr_t **args, unsigned long **args_len)
{
    /* Allocate memory for arguments */
    *args = (uintptr_t *)emalloc(keys_count * sizeof(uintptr_t));
    *args_len = (unsigned long *)emalloc(keys_count * sizeof(unsigned long));

    if (!(*args) || !(*args_len))
    {
        if (*args)
            efree(*args);
        if (*args_len)
            efree(*args_len);
        return 0;
    }

    /* Populate arguments array */
    HashTable *keys_hash = Z_ARRVAL_P(keys);
    zval *key;
    int idx = 0;

    ZEND_HASH_FOREACH_VAL(keys_hash, key)
    {
        if (Z_TYPE_P(key) != IS_STRING)
        {
            convert_to_string(key);
        }

        (*args)[idx] = (uintptr_t)Z_STRVAL_P(key);
        (*args_len)[idx] = Z_STRLEN_P(key);
        idx++;
    }
    ZEND_HASH_FOREACH_END();

    return 1;
}

/* Helper function to prepare weights array from zval */
static int prepare_weights_array(zval *weights, int weights_count, uintptr_t **args, unsigned long **args_len)
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
static void free_weights_strings(uintptr_t *args, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (args[i])
        {
            efree((void *)args[i]);
        }
    }
}

/* Helper function to prepare aggregate option */
static int prepare_aggregate_option(zval *options, uintptr_t *agg_arg, unsigned long *agg_len)
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

/* Execute a ZINTERCARD command using the Valkey Glide client */
int execute_zintercard_command(const void *glide_client, zval *keys, int keys_count, zval *options, zval *return_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys || keys_count <= 0)
    {
        return 0;
    }

    /* Prepare keys arguments */
    uintptr_t *keys_args = NULL;
    unsigned long *keys_len = NULL;

    if (!prepare_keys_array(keys, keys_count, &keys_args, &keys_len))
    {
        return 0;
    }

    /* Calculate total arguments (numkeys + keys + LIMIT if present) */
    unsigned long arg_count = keys_count + 1; /* +1 for numkeys */
    int has_limit = 0;
    long limit = 0;

    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(options);
        zval *limit_val = zend_hash_str_find(ht, "LIMIT", sizeof("LIMIT") - 1);
        if (limit_val && Z_TYPE_P(limit_val) == IS_LONG)
        {
            has_limit = 1;
            limit = Z_LVAL_P(limit_val);
            arg_count += 2; /* LIMIT + value */
        }
    }

    /* Allocate final args arrays */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (keys_args)
            efree(keys_args);
        if (keys_len)
            efree(keys_len);
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Add numkeys as the first argument */
    char numkeys_str[32];
    snprintf(numkeys_str, sizeof(numkeys_str), "%d", keys_count);
    args[0] = (uintptr_t)estrdup(numkeys_str);
    args_len[0] = strlen(numkeys_str);

    /* Copy keys to args array (offset by 1 for numkeys) */
    memcpy(args + 1, keys_args, keys_count * sizeof(uintptr_t));
    memcpy(args_len + 1, keys_len, keys_count * sizeof(unsigned long));

    /* Add LIMIT option if present */
    if (has_limit)
    {
        char limit_str[32];
        unsigned int offset = keys_count + 1; /* +1 for numkeys */

        /* Add LIMIT keyword */
        args[offset] = (uintptr_t)"LIMIT";
        args_len[offset] = 5;
        offset++;

        /* Add limit value */
        snprintf(limit_str, sizeof(limit_str), "%ld", limit);
        args[offset] = (uintptr_t)estrdup(limit_str);
        args_len[offset] = strlen(limit_str);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZInterCard, /* command type */
        arg_count,  /* number of arguments */
        args,       /* arguments */
        args_len    /* argument lengths */
    );

    /* Free the argument arrays */
    if (has_limit)
    {
        /* Free the limit value string */
        efree((void *)args[keys_count + 2]); /* +2 for numkeys and LIMIT */
    }
    efree((void *)args[0]); /* Free the numkeys string */
    efree(keys_args);
    efree(keys_len);
    efree(args);
    efree(args_len);

    /* Handle the result */
    long output_value = 0;
    int status = handle_int_response(result, &output_value);

    /* Set output value as the return value for PHP */
    if (status)
    {
        ZVAL_LONG(return_value, output_value);
    }

    return status;
}

/* Execute a ZUNION command using the Valkey Glide client */
int execute_zunion_command(const void *glide_client, zval *keys, int keys_count, zval *weights, zval *options, zval *return_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys || keys_count <= 0)
    {
        return 0;
    }

    /* Prepare keys arguments */
    uintptr_t *keys_args = NULL;
    unsigned long *keys_len = NULL;

    if (!prepare_keys_array(keys, keys_count, &keys_args, &keys_len))
    {
        return 0;
    }

    /* Calculate total arguments (numkeys + keys + WEIGHTS + AGGREGATE + WITHSCORES if present) */
    unsigned long arg_count = 1 + keys_count; /* +1 for numkeys */
    int has_weights = 0;
    int has_aggregate = 0;
    int has_withscores = 0;
    uintptr_t *weights_args = NULL;
    unsigned long *weights_len = NULL;
    uintptr_t agg_type = 0;
    unsigned long agg_len = 0;

    /* Check for weights */
    if (weights && Z_TYPE_P(weights) == IS_ARRAY)
    {
        int weights_count = zend_hash_num_elements(Z_ARRVAL_P(weights));
        if (weights_count > 0)
        {
            has_weights = 1;
            if (!prepare_weights_array(weights, weights_count, &weights_args, &weights_len))
            {
                efree(keys_args);
                efree(keys_len);
                return 0;
            }
            arg_count += 1 + weights_count; /* WEIGHTS + values */
        }
    }

    /* Check for AGGREGATE and WITHSCORES options */
    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        /* Check for aggregate option */
        if (prepare_aggregate_option(options, &agg_type, &agg_len))
        {
            has_aggregate = 1;
            arg_count += 2; /* AGGREGATE + value */
        }

        /* Check for withscores option */
        HashTable *ht = Z_ARRVAL_P(options);
        zval *withscores = zend_hash_str_find(ht, "withscores", sizeof("withscores") - 1);
        if (withscores && (Z_TYPE_P(withscores) == IS_TRUE ||
                           (Z_TYPE_P(withscores) == IS_LONG && Z_LVAL_P(withscores) == 1)))
        {
            has_withscores = 1;
            arg_count += 1; /* WITHSCORES */
        }
    }

    /* Allocate final args arrays */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (keys_args)
            efree(keys_args);
        if (keys_len)
            efree(keys_len);
        if (weights_args)
        {
            free_weights_strings(weights_args, zend_hash_num_elements(Z_ARRVAL_P(weights)));
            efree(weights_args);
        }
        if (weights_len)
            efree(weights_len);
        if (has_aggregate)
            efree((void *)agg_type);
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Add numkeys as the first argument */
    char numkeys_str[32];
    snprintf(numkeys_str, sizeof(numkeys_str), "%d", keys_count);
    args[0] = (uintptr_t)estrdup(numkeys_str);
    args_len[0] = strlen(numkeys_str);

    /* Copy keys to args array (offset by 1 for numkeys) */
    unsigned int offset = 1;
    memcpy(args + offset, keys_args, keys_count * sizeof(uintptr_t));
    memcpy(args_len + offset, keys_len, keys_count * sizeof(unsigned long));
    offset += keys_count;

    /* Add WEIGHTS if present */
    if (has_weights)
    {
        int weights_count = zend_hash_num_elements(Z_ARRVAL_P(weights));

        /* Add WEIGHTS keyword */
        args[offset] = (uintptr_t)"WEIGHTS";
        args_len[offset] = 7;
        offset++;

        /* Add weights values */
        memcpy(args + offset, weights_args, weights_count * sizeof(uintptr_t));
        memcpy(args_len + offset, weights_len, weights_count * sizeof(unsigned long));
        offset += weights_count;
    }

    /* Add AGGREGATE if present */
    if (has_aggregate)
    {
        /* Add AGGREGATE keyword */
        args[offset] = (uintptr_t)"AGGREGATE";
        args_len[offset] = 9;
        offset++;

        /* Add aggregate value */
        args[offset] = agg_type;
        args_len[offset] = agg_len;
        offset++;
    }

    /* Add WITHSCORES if present */
    if (has_withscores)
    {
        /* Add WITHSCORES keyword */
        args[offset] = (uintptr_t)"WITHSCORES";
        args_len[offset] = 10;
        offset++;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZUnion,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree((void *)args[0]); /* Free the numkeys string */
    efree(keys_args);
    efree(keys_len);
    if (has_weights)
    {
        free_weights_strings(weights_args, zend_hash_num_elements(Z_ARRVAL_P(weights)));
        efree(weights_args);
        efree(weights_len);
    }
    if (has_aggregate)
    {
        efree((void *)agg_type);
    }
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

        if (result->response)
        {
            /* ZUNION returns array of members with scores */
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_ASSOSIATIVE_ARRAY);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Helper function for ZDIFFSTORE, ZINTERSTORE and ZUNIONSTORE commands */
int execute_zstore_command(const void *glide_client, enum RequestType cmd_type, const char *dst, size_t dst_len,
                           zval *keys, int keys_count, zval *weights, zval *options, long *output_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !dst || dst_len <= 0 || !keys || keys_count <= 0)
    {
        return 0;
    }

    /* Prepare keys arguments */
    uintptr_t *keys_args = NULL;
    unsigned long *keys_len = NULL;

    if (!prepare_keys_array(keys, keys_count, &keys_args, &keys_len))
    {
        return 0;
    }

    /* Calculate total arguments (destination + numkeys + keys + WEIGHTS + AGGREGATE if present) */
    unsigned long arg_count = 2 + keys_count; /* destination + numkeys + keys */
    int has_weights = 0;
    int has_aggregate = 0;
    uintptr_t *weights_args = NULL;
    unsigned long *weights_len = NULL;
    uintptr_t agg_type = 0;
    unsigned long agg_len = 0;

    /* Check for weights */
    if (weights && Z_TYPE_P(weights) == IS_ARRAY)
    {
        int weights_count = zend_hash_num_elements(Z_ARRVAL_P(weights));
        if (weights_count > 0)
        {
            has_weights = 1;
            if (!prepare_weights_array(weights, weights_count, &weights_args, &weights_len))
            {
                efree(keys_args);
                efree(keys_len);
                return 0;
            }
            arg_count += 1 + weights_count; /* WEIGHTS + values */
        }
    }

    /* Check for AGGREGATE option */
    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        /* Add aggregate if present */
        if (prepare_aggregate_option(options, &agg_type, &agg_len))
        {
            has_aggregate = 1;
            arg_count += 2; /* AGGREGATE + value */
        }
    }

    /* Allocate final args arrays */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (keys_args)
            efree(keys_args);
        if (keys_len)
            efree(keys_len);
        if (weights_args)
        {
            free_weights_strings(weights_args, zend_hash_num_elements(Z_ARRVAL_P(weights)));
            efree(weights_args);
        }
        if (weights_len)
            efree(weights_len);
        if (has_aggregate)
            efree((void *)agg_type);
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Set destination */
    args[0] = (uintptr_t)dst;
    args_len[0] = dst_len;

    /* Add numkeys as the second argument - required by the Redis command format */
    char numkeys_str[32];
    snprintf(numkeys_str, sizeof(numkeys_str), "%d", keys_count);
    args[1] = (uintptr_t)estrdup(numkeys_str);
    args_len[1] = strlen(numkeys_str);

    /* Copy keys to args array starting from index 2 */
    memcpy(args + 2, keys_args, keys_count * sizeof(uintptr_t));
    memcpy(args_len + 2, keys_len, keys_count * sizeof(unsigned long));
    unsigned int offset = 2 + keys_count;

    /* Add WEIGHTS if present */
    if (has_weights)
    {
        int weights_count = zend_hash_num_elements(Z_ARRVAL_P(weights));

        /* Add WEIGHTS keyword */
        args[offset] = (uintptr_t)"WEIGHTS";
        args_len[offset] = 7;
        offset++;

        /* Add weights values */
        memcpy(args + offset, weights_args, weights_count * sizeof(uintptr_t));
        memcpy(args_len + offset, weights_len, weights_count * sizeof(unsigned long));
        offset += weights_count;
    }

    /* Add AGGREGATE if present */
    if (has_aggregate)
    {
        /* Add AGGREGATE keyword */
        args[offset] = (uintptr_t)"AGGREGATE";
        args_len[offset] = 9;
        offset++;

        /* Add aggregate value */
        args[offset] = agg_type;
        args_len[offset] = agg_len;
        offset++;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        cmd_type,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the numkeys string we allocated */
    efree((void *)args[1]);

    /* Free the argument arrays */
    efree(keys_args);
    efree(keys_len);
    if (has_weights)
    {
        free_weights_strings(weights_args, zend_hash_num_elements(Z_ARRVAL_P(weights)));
        efree(weights_args);
        efree(weights_len);
    }
    if (has_aggregate)
    {
        efree((void *)agg_type);
    }
    efree(args);
    efree(args_len);

    /* Process the result */
    return handle_int_response(result, output_value);
}

/* Execute a ZDIFFSTORE command using the Valkey Glide client */
int execute_zdiffstore_command(const void *glide_client, const char *dst, size_t dst_len, zval *keys, int keys_count,
                               zval *weights, zval *options, long *output_value)
{
    return execute_zstore_command(glide_client, ZDiffStore, dst, dst_len, keys, keys_count, weights, options, output_value);
}

/* Execute a ZINTERSTORE command using the Valkey Glide client */
int execute_zinterstore_command(const void *glide_client, const char *dst, size_t dst_len, zval *keys, int keys_count,
                                zval *weights, zval *options, long *output_value)
{
    return execute_zstore_command(glide_client, ZInterStore, dst, dst_len, keys, keys_count, weights, options, output_value);
}

/* Execute a ZUNIONSTORE command using the Valkey Glide client */
int execute_zunionstore_command(const void *glide_client, const char *dst, size_t dst_len, zval *keys, int keys_count,
                                zval *weights, zval *options, long *output_value)
{
    return execute_zstore_command(glide_client, ZUnionStore, dst, dst_len, keys, keys_count, weights, options, output_value);
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
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_ASSOSIATIVE_ARRAY);
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
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_ASSOSIATIVE_ARRAY);
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
                command_response_to_zval(elements, &z_elements, COMMAND_RESPONSE_ASSOSIATIVE_ARRAY);
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
