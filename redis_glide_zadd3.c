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
#include "redis_glide_zadd.h"
#include "command_response.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Execute a ZREVRANGEBYLEX command using the Valkey Glide client */
int execute_zrevrangebylex_command(const void *glide_client, const char *key, size_t key_len,
                                   zval *z_max, zval *z_min, zval *options, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !z_min || !z_max)
    {
        return 0;
    }

    /* Parse range options */
    int has_withscores = 0, has_byscore = 0, has_bylex = 1, has_rev = 1; /* ZREVRANGEBYLEX uses BYLEX + REV */
    int has_limit = 0;
    long offset = 0, count = 0;

    if (options)
    {
        /* Handle limit options */
        if (Z_TYPE_P(options) == IS_ARRAY)
        {
            HashTable *options_ht = Z_ARRVAL_P(options);
            zval *z_tmp;

            /* Look for LIMIT option */
            if ((z_tmp = zend_hash_str_find(options_ht, "LIMIT", sizeof("LIMIT") - 1)) != NULL)
            {
                if (Z_TYPE_P(z_tmp) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(z_tmp)) == 2)
                {
                    zval *z_offset, *z_count;
                    HashTable *ht_limit = Z_ARRVAL_P(z_tmp);

                    /* Get offset and count from LIMIT array */
                    if ((z_offset = zend_hash_index_find(ht_limit, 0)) != NULL &&
                        (z_count = zend_hash_index_find(ht_limit, 1)) != NULL)
                    {
                        offset = zval_get_long(z_offset);
                        count = zval_get_long(z_count);
                        has_limit = 1;
                    }
                }
            }
        }
    }

    /* Use the common range command helper */
    return build_range_cmd_args(glide_client, ZRange, key, key_len,
                                z_max, z_min, has_withscores, has_byscore, has_bylex,
                                has_rev, has_limit, offset, count, return_value);
}

/* Helper function to process common options for ZDIFF/ZINTER commands */
static int parse_multi_options(zval *options, int *has_withscores, char **agg_op, size_t *agg_op_len)
{
    if (!options || Z_TYPE_P(options) != IS_ARRAY)
    {
        return 0;
    }

    HashTable *options_ht = Z_ARRVAL_P(options);
    zval *z_tmp;

    /* Look for WITHSCORES option */
    if ((z_tmp = zend_hash_str_find(options_ht, "WITHSCORES", sizeof("WITHSCORES") - 1)) != NULL)
    {
        if (Z_TYPE_P(z_tmp) == IS_TRUE ||
            (Z_TYPE_P(z_tmp) == IS_LONG && Z_LVAL_P(z_tmp) == 1))
        {
            *has_withscores = 1;
        }
    }

    /* Look for AGGREGATE option */
    if (agg_op && (z_tmp = zend_hash_str_find(options_ht, "AGGREGATE", sizeof("AGGREGATE") - 1)) != NULL)
    {
        if (Z_TYPE_P(z_tmp) == IS_STRING)
        {
            *agg_op = Z_STRVAL_P(z_tmp);
            *agg_op_len = Z_STRLEN_P(z_tmp);
        }
    }

    return 1;
}

/* Execute a ZDIFF command using the Valkey Glide client */
int execute_zdiff_command(const void *glide_client, zval *keys, zval *options, zval *return_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys || Z_TYPE_P(keys) != IS_ARRAY)
    {
        return 0;
    }

    /* Get the number of keys */
    int num_keys = zend_hash_num_elements(Z_ARRVAL_P(keys));
    if (num_keys < 1)
    {
        return 0;
    }

    /* Parse options */
    int has_withscores = 0;
    parse_multi_options(options, &has_withscores, NULL, NULL);

    /* Calculate maximum arguments: number of keys + withscores option */
    unsigned long max_args = num_keys + 1;
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

    /* Add all keys as arguments */
    unsigned long arg_idx = 0;
    HashTable *keys_ht = Z_ARRVAL_P(keys);
    zval *z_key;

    /* First argument: number of keys */
    char num_keys_str[32];
    size_t num_keys_len = snprintf(num_keys_str, sizeof(num_keys_str), "%d", num_keys);
    args[arg_idx] = (uintptr_t)num_keys_str;
    args_len[arg_idx++] = num_keys_len;

    /* Add all keys */
    ZEND_HASH_FOREACH_VAL(keys_ht, z_key)
    {
        if (Z_TYPE_P(z_key) == IS_STRING)
        {
            args[arg_idx] = (uintptr_t)Z_STRVAL_P(z_key);
            args_len[arg_idx++] = Z_STRLEN_P(z_key);
        }
    }
    ZEND_HASH_FOREACH_END();

    /* Add WITHSCORES option if needed */
    if (has_withscores)
    {
        /* We need to reallocate arrays to add space for WITHSCORES */
        uintptr_t *new_args = (uintptr_t *)erealloc(args, (max_args + 1) * sizeof(uintptr_t));
        unsigned long *new_args_len = (unsigned long *)erealloc(args_len, (max_args + 1) * sizeof(unsigned long));

        if (!new_args || !new_args_len)
        {
            efree(args);
            efree(args_len);
            return 0;
        }

        args = new_args;
        args_len = new_args_len;

        args[arg_idx] = (uintptr_t)"WITHSCORES";
        args_len[arg_idx++] = 10;
    }

    /* Execute ZDIFF command */
    CommandResult *result = execute_command(
        glide_client,
        ZDiff,   /* command type */
        arg_idx, /* number of arguments */
        args,    /* arguments */
        args_len /* argument lengths */
    );

    /* Free allocated memory */
    efree(args);
    efree(args_len);

    /* Process the result */
    return process_zrange_response(result, return_value, has_withscores);
}

/* Execute a ZINTER command using the Valkey Glide client */
int execute_zinter_command(const void *glide_client, zval *keys, zval *z_weights, zval *options, zval *return_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys || Z_TYPE_P(keys) != IS_ARRAY)
    {
        return 0;
    }

    /* Get the number of keys */
    int num_keys = zend_hash_num_elements(Z_ARRVAL_P(keys));
    if (num_keys < 1)
    {
        return 0;
    }

    /* Parse options */
    int has_withscores = 0;
    char *agg_op = NULL;
    size_t agg_op_len = 0;
    parse_multi_options(options, &has_withscores, &agg_op, &agg_op_len);

    /* Check if weights array is valid */
    int has_weights = (z_weights && Z_TYPE_P(z_weights) == IS_ARRAY &&
                       zend_hash_num_elements(Z_ARRVAL_P(z_weights)) > 0);

    /* Calculate maximum arguments:
       - num_keys (number of keys arg + actual keys)
       - WEIGHTS keyword + weights if present
       - AGGREGATE + aggregation type if specified
       - WITHSCORES if present
    */
    unsigned long max_args = num_keys + 1 +
                             (has_weights ? (1 + num_keys) : 0) +
                             (agg_op ? 2 : 0) +
                             (has_withscores ? 1 : 0);

    uintptr_t *args = (uintptr_t *)emalloc(max_args * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(max_args * sizeof(unsigned long));

    /* Track allocated strings that need to be freed after command execution */
    char **allocated_strings = (char **)ecalloc(num_keys, sizeof(char *));
    int allocated_count = 0;

    if (!args || !args_len || !allocated_strings)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        if (allocated_strings)
            efree(allocated_strings);
        return 0;
    }

    /* Add arguments */
    unsigned long arg_idx = 0;
    HashTable *keys_ht = Z_ARRVAL_P(keys);
    zval *z_key;

    /* First argument: number of keys */
    char num_keys_str[32];
    size_t num_keys_len = snprintf(num_keys_str, sizeof(num_keys_str), "%d", num_keys);
    args[arg_idx] = (uintptr_t)num_keys_str;
    args_len[arg_idx++] = num_keys_len;

    /* Add all keys */
    ZEND_HASH_FOREACH_VAL(keys_ht, z_key)
    {
        if (Z_TYPE_P(z_key) == IS_STRING)
        {
            args[arg_idx] = (uintptr_t)Z_STRVAL_P(z_key);
            args_len[arg_idx++] = Z_STRLEN_P(z_key);
        }
    }
    ZEND_HASH_FOREACH_END();

    /* Add weights if specified */
    if (has_weights)
    {
        args[arg_idx] = (uintptr_t)"WEIGHTS";
        args_len[arg_idx++] = 7;

        HashTable *weights_ht = Z_ARRVAL_P(z_weights);
        zval *z_weight;

        /* Add each weight */
        ZEND_HASH_FOREACH_VAL(weights_ht, z_weight)
        {
            if (Z_TYPE_P(z_weight) == IS_LONG)
            {
                /* Convert long to string */
                size_t len;
                char *str = long_to_string(Z_LVAL_P(z_weight), &len);
                if (str)
                {
                    args[arg_idx] = (uintptr_t)str;
                    args_len[arg_idx++] = len;
                    allocated_strings[allocated_count++] = str;
                }
            }
            else if (Z_TYPE_P(z_weight) == IS_DOUBLE)
            {
                /* Convert double to string */
                size_t len;
                char *str = double_to_string(Z_DVAL_P(z_weight), &len);
                if (str)
                {
                    args[arg_idx] = (uintptr_t)str;
                    args_len[arg_idx++] = len;
                    allocated_strings[allocated_count++] = str;
                }
            }
            else if (Z_TYPE_P(z_weight) == IS_STRING)
            {
                /* Use string value directly */
                args[arg_idx] = (uintptr_t)Z_STRVAL_P(z_weight);
                args_len[arg_idx++] = Z_STRLEN_P(z_weight);
            }

            /* Only add weights up to the number of keys */
            if (arg_idx >= (1 + num_keys + num_keys))
                break;
        }
        ZEND_HASH_FOREACH_END();
    }

    /* Add AGGREGATE option if specified */
    if (agg_op && agg_op_len > 0)
    {
        args[arg_idx] = (uintptr_t)"AGGREGATE";
        args_len[arg_idx++] = 9;
        args[arg_idx] = (uintptr_t)agg_op;
        args_len[arg_idx++] = agg_op_len;
    }

    /* Add WITHSCORES option if needed */
    if (has_withscores)
    {
        args[arg_idx] = (uintptr_t)"WITHSCORES";
        args_len[arg_idx++] = 10;
    }

    /* Execute ZINTER command */
    CommandResult *result = execute_command(
        glide_client,
        ZInter,  /* command type */
        arg_idx, /* number of arguments */
        args,    /* arguments */
        args_len /* argument lengths */
    );

    /* Free allocated strings */
    for (int j = 0; j < allocated_count; j++)
    {
        efree(allocated_strings[j]);
    }
    efree(allocated_strings);
    efree(args);
    efree(args_len);

    /* Process the result */
    return process_zrange_response(result, return_value, has_withscores);
}
