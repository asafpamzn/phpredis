/*
  +----------------------------------------------------------------------+
  | Valkey Glide Z-Commands Common Utilities                             |
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Import the string conversion functions from command_response.c */
extern char *long_to_string(long value, size_t *len);
extern char *double_to_string(double value, size_t *len);

extern zend_class_entry *redis_ce;

/* ====================================================================
 * OPTIONS PARSING HELPERS
 * ==================================================================== */

/**
 * Parse range command options (withscores, byscore, bylex, rev, limit)
 * Returns 1 on success, 0 on failure
 */
int parse_range_options(zval *options, range_options_t *opts)
{
    if (!opts)
    {
        return 0;
    }

    /* Initialize options structure */
    memset(opts, 0, sizeof(range_options_t));

    if (!options)
    {
        return 1; /* No options is valid */
    }

    if (Z_TYPE_P(options) == IS_TRUE)
    {
        /* Direct boolean TRUE means WITHSCORES */
        opts->withscores = 1;
        return 1;
    }

    if (Z_TYPE_P(options) != IS_ARRAY)
    {
        return 1; /* No options is valid */
    }

    HashTable *ht = Z_ARRVAL_P(options);
    zval *entry;

    /* Check for withscores option */
    if ((entry = zend_hash_str_find(ht, "withscores", sizeof("withscores") - 1)) != NULL ||
        (entry = zend_hash_str_find(ht, "WITHSCORES", sizeof("WITHSCORES") - 1)) != NULL)
    {
        if (Z_TYPE_P(entry) == IS_TRUE)
        {
            opts->withscores = 1;
        }
    }

    /* Check for byscore option */
    if ((entry = zend_hash_str_find(ht, "byscore", sizeof("byscore") - 1)) != NULL ||
        (entry = zend_hash_str_find(ht, "BYSCORE", sizeof("BYSCORE") - 1)) != NULL)
    {
        opts->byscore = 1;
    }
    else
    {
        /* Check if 'byscore' exists as a value in the array */
        ZEND_HASH_FOREACH_VAL(ht, entry)
        {
            if (Z_TYPE_P(entry) == IS_STRING &&
                strncasecmp(Z_STRVAL_P(entry), "byscore", Z_STRLEN_P(entry)) == 0)
            {
                opts->byscore = 1;
                break;
            }
        }
        ZEND_HASH_FOREACH_END();
    }

    /* Check for bylex option */
    if ((entry = zend_hash_str_find(ht, "bylex", sizeof("bylex") - 1)) != NULL ||
        (entry = zend_hash_str_find(ht, "BYLEX", sizeof("BYLEX") - 1)) != NULL)
    {
        opts->bylex = 1;
    }
    else
    {
        /* Check if 'bylex' exists as a value in the array */
        ZEND_HASH_FOREACH_VAL(ht, entry)
        {
            if (Z_TYPE_P(entry) == IS_STRING &&
                strncasecmp(Z_STRVAL_P(entry), "bylex", Z_STRLEN_P(entry)) == 0)
            {
                opts->bylex = 1;
                break;
            }
        }
        ZEND_HASH_FOREACH_END();
    }

    /* Check for rev option */
    if ((entry = zend_hash_str_find(ht, "rev", sizeof("rev") - 1)) != NULL ||
        (entry = zend_hash_str_find(ht, "REV", sizeof("REV") - 1)) != NULL)
    {
        opts->rev = 1;
    }
    else
    {
        /* Check if 'rev' exists as a value in the array */
        ZEND_HASH_FOREACH_VAL(ht, entry)
        {
            if (Z_TYPE_P(entry) == IS_STRING &&
                strncasecmp(Z_STRVAL_P(entry), "rev", Z_STRLEN_P(entry)) == 0)
            {
                opts->rev = 1;
                break;
            }
        }
        ZEND_HASH_FOREACH_END();
    }

    /* Check for limit option */
    if ((entry = zend_hash_str_find(ht, "limit", sizeof("limit") - 1)) != NULL ||
        (entry = zend_hash_str_find(ht, "LIMIT", sizeof("LIMIT") - 1)) != NULL)
    {
        if (Z_TYPE_P(entry) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(entry)) >= 2)
        {
            zval *z_offset, *z_count;
            HashTable *limit_ht = Z_ARRVAL_P(entry);

            /* Get offset (first element) */
            z_offset = zend_hash_index_find(limit_ht, 0);
            if (z_offset && Z_TYPE_P(z_offset) == IS_LONG)
            {
                opts->limit_offset = Z_LVAL_P(z_offset);

                /* Get count (second element) */
                z_count = zend_hash_index_find(limit_ht, 1);
                if (z_count && Z_TYPE_P(z_count) == IS_LONG)
                {
                    opts->limit_count = Z_LVAL_P(z_count);
                    opts->has_limit = 1;
                }
            }
        }
    }

    return 1;
}

/**
 * Parse ZADD command options (XX, NX, LT, GT, CH, INCR)
 * Returns 1 on success, 0 on failure
 */
int parse_zadd_options(zval *options, zadd_options_t *opts)
{
    if (!opts)
    {
        return 0;
    }

    /* Initialize options structure */
    memset(opts, 0, sizeof(zadd_options_t));

    if (!options || Z_TYPE_P(options) != IS_ARRAY)
    {
        return 1;
    }

    HashTable *ht = Z_ARRVAL_P(options);
    zval *entry;
    zend_string *key;
    zend_ulong num_key;

    /* Process each option */
    ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, key, entry)
    {
        if (Z_TYPE_P(entry) == IS_STRING)
        {
            char *opt_str = Z_STRVAL_P(entry);

            if (strcasecmp(opt_str, "XX") == 0)
            {
                opts->xx = 1;
            }
            else if (strcasecmp(opt_str, "NX") == 0)
            {
                opts->nx = 1;
            }
            else if (strcasecmp(opt_str, "LT") == 0)
            {
                opts->lt = 1;
            }
            else if (strcasecmp(opt_str, "GT") == 0)
            {
                opts->gt = 1;
            }
            else if (strcasecmp(opt_str, "CH") == 0)
            {
                opts->ch = 1;
            }
            else if (strcasecmp(opt_str, "INCR") == 0)
            {
                opts->incr = 1;
            }
        }
        else if (key != NULL)
        {
            /* Handle associative array options */
            char *opt_key = ZSTR_VAL(key);

            if (strcasecmp(opt_key, "XX") == 0 && Z_TYPE_P(entry) == IS_TRUE)
            {
                opts->xx = 1;
            }
            else if (strcasecmp(opt_key, "NX") == 0 && Z_TYPE_P(entry) == IS_TRUE)
            {
                opts->nx = 1;
            }
            else if (strcasecmp(opt_key, "LT") == 0 && Z_TYPE_P(entry) == IS_TRUE)
            {
                opts->lt = 1;
            }
            else if (strcasecmp(opt_key, "GT") == 0 && Z_TYPE_P(entry) == IS_TRUE)
            {
                opts->gt = 1;
            }
            else if (strcasecmp(opt_key, "CH") == 0 && Z_TYPE_P(entry) == IS_TRUE)
            {
                opts->ch = 1;
            }
            else if (strcasecmp(opt_key, "INCR") == 0 && Z_TYPE_P(entry) == IS_TRUE)
            {
                opts->incr = 1;
            }
        }
    }
    ZEND_HASH_FOREACH_END();

    return 1;
}

/**
 * Parse store command options (weights, aggregate) for ZUNIONSTORE-style commands
 * Returns 1 on success, 0 on failure
 */
int parse_store_options(zval *weights, zval *options, store_options_t *opts)
{
    if (!opts)
    {
        return 0;
    }

    /* Initialize options structure */
    memset(opts, 0, sizeof(store_options_t));

    /* Parse weights */
    if (weights && Z_TYPE_P(weights) == IS_ARRAY)
    {
        opts->weights = weights;
        opts->has_weights = 1;
    }

    /* Parse aggregate option */
    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(options);
        zval *aggregate = zend_hash_str_find(ht, "AGGREGATE", sizeof("AGGREGATE") - 1);
        if (!aggregate)
        {
            aggregate = zend_hash_str_find(ht, "aggregate", sizeof("aggregate") - 1);
        }

        if (aggregate && Z_TYPE_P(aggregate) == IS_STRING)
        {
            const char *agg_str = Z_STRVAL_P(aggregate);
            if (strcasecmp(agg_str, "SUM") == 0 ||
                strcasecmp(agg_str, "MIN") == 0 ||
                strcasecmp(agg_str, "MAX") == 0)
            {
                opts->aggregate = aggregate;
                opts->has_aggregate = 1;
            }
        }

        /* Check for withscores option */
        zval *withscores = zend_hash_str_find(ht, "withscores", sizeof("withscores") - 1);
        if (!withscores)
        {
            withscores = zend_hash_str_find(ht, "WITHSCORES", sizeof("WITHSCORES") - 1);
        }

        if (withscores && (Z_TYPE_P(withscores) == IS_TRUE ||
                           (Z_TYPE_P(withscores) == IS_LONG && Z_LVAL_P(withscores) == 1)))
        {
            opts->withscores = 1;
        }
    }

    return 1;
}

/* ====================================================================
 * CONVERSION & UTILITY HELPERS
 * ==================================================================== */

/**
 * Safe zval to string conversion with memory management
 * Returns allocated string that must be freed, or NULL on error
 * Sets need_free to 1 if returned string must be freed
 */
char *zval_to_string_safe(zval *z, size_t *len, int *need_free)
{
    char *str = NULL;
    *need_free = 0;

    if (!z || !len)
    {
        return NULL;
    }

    switch (Z_TYPE_P(z))
    {
    case IS_STRING:
        str = Z_STRVAL_P(z);
        *len = Z_STRLEN_P(z);
        break;

    case IS_LONG:
        str = long_to_string(Z_LVAL_P(z), len);
        *need_free = 1;
        break;

    case IS_DOUBLE:
        str = double_to_string(Z_DVAL_P(z), len);
        *need_free = 1;
        break;

    case IS_TRUE:
        str = estrdup("1");
        *len = 1;
        *need_free = 1;
        break;

    case IS_FALSE:
        str = estrdup("0");
        *len = 1;
        *need_free = 1;
        break;

    default:
        /* Convert other types to string */
        zval copy;
        ZVAL_COPY(&copy, z);
        convert_to_string(&copy);
        str = estrndup(Z_STRVAL(copy), Z_STRLEN(copy));
        *len = Z_STRLEN(copy);
        zval_dtor(&copy);
        *need_free = 1;
        break;
    }

    return str;
}

/**
 * Prepare keys array from zval for multi-key operations
 * Returns 1 on success, 0 on failure
 */
int prepare_keys_array(zval *keys, uintptr_t **args, unsigned long **args_len)
{
    if (!keys || Z_TYPE_P(keys) != IS_ARRAY)
    {
        return 0;
    }

    HashTable *keys_hash = Z_ARRVAL_P(keys);
    int keys_count = zend_hash_num_elements(keys_hash);

    if (keys_count <= 0)
    {
        return 0;
    }

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

    return keys_count;
}

/**
 * Create LIMIT arguments (offset, count)
 * Returns number of arguments added (0 or 3)
 */
int create_limit_args(range_options_t *opts, uintptr_t *args, unsigned long *args_len,
                      int start_idx, char **allocated_strings, int *allocated_count)
{
    if (!opts->has_limit)
    {
        return 0;
    }

    /* Add LIMIT keyword */
    args[start_idx] = (uintptr_t)"LIMIT";
    args_len[start_idx] = 5;

    /* Add offset parameter */
    size_t len;
    char *offset_str = long_to_string(opts->limit_offset, &len);
    if (!offset_str)
    {
        return 0;
    }
    args[start_idx + 1] = (uintptr_t)offset_str;
    args_len[start_idx + 1] = len;
    allocated_strings[(*allocated_count)++] = offset_str;

    /* Add count parameter */
    char *count_str = long_to_string(opts->limit_count, &len);
    if (!count_str)
    {
        return 0;
    }
    args[start_idx + 2] = (uintptr_t)count_str;
    args_len[start_idx + 2] = len;
    allocated_strings[(*allocated_count)++] = count_str;

    return 3; /* LIMIT + offset + count */
}

/* ====================================================================
 * RESPONSE PROCESSING HELPERS
 * ==================================================================== */

/**
 * Handle score response (for ZSCORE, ZINCRBY)
 * Returns: 1 = success with score, 0 = member not found, -1 = error
 */
int handle_score_response(CommandResult *result, double *score)
{
    if (!result || result->command_error || !result->response)
    {
        return -1;
    }

    if (result->response->response_type == Null)
    {
        return 0; /* Member doesn't exist */
    }

    if (result->response->response_type == String)
    {
        /* Parse string as double */
        char *endptr;
        *score = strtod(result->response->string_value, &endptr);
        if (*endptr == '\0' || endptr == result->response->string_value + result->response->string_value_len)
        {
            return 1;
        }
        return -1;
    }

    if (result->response->response_type == Float)
    {
        *score = result->response->float_value;
        return 1;
    }

    return -1;
}

/**
 * Handle rank response (for ZRANK, ZREVRANK)
 * Returns: 1 = success with rank, 0 = member not found, -1 = error
 */
int handle_rank_response(CommandResult *result, long *rank, double *score, int withscore)
{
    if (!result || result->command_error || !result->response)
    {
        return -1;
    }

    if (result->response->response_type == Null)
    {
        return 0; /* Member doesn't exist */
    }

    if (result->response->response_type == Int)
    {
        *rank = result->response->int_value;
        return 1;
    }

    if (result->response->response_type == Array && withscore)
    {
        /* Array with rank and score [rank, score] */
        if (result->response->array_value_len >= 2)
        {
            CommandResponse *rank_resp = &result->response->array_value[0];
            CommandResponse *score_resp = &result->response->array_value[1];

            if (rank_resp->response_type == Int &&
                (score_resp->response_type == String || score_resp->response_type == Float))
            {
                *rank = rank_resp->int_value;

                if (score_resp->response_type == String)
                {
                    char *endptr;
                    *score = strtod(score_resp->string_value, &endptr);
                }
                else
                {
                    *score = score_resp->float_value;
                }

                return 1;
            }
        }
    }

    return -1;
}

/**
 * Flatten withscores array from [[member, score]] to [member => score]
 * Returns 1 on success, 0 on failure
 */
int flatten_withscores_array(zval *return_value)
{
    if (!return_value || Z_TYPE_P(return_value) != IS_ARRAY)
    {
        return 0;
    }

    HashTable *ht = Z_ARRVAL_P(return_value);
    zval tmp_arr;
    array_init(&tmp_arr);

    zval *entry;
    ZEND_HASH_FOREACH_VAL(ht, entry)
    {
        if (Z_TYPE_P(entry) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(entry)) == 2)
        {
            zval *z_member = zend_hash_index_find(Z_ARRVAL_P(entry), 0);
            zval *z_score = zend_hash_index_find(Z_ARRVAL_P(entry), 1);

            if (z_member && z_score)
            {
                /* Convert member to string if needed for use as key */
                if (Z_TYPE_P(z_member) != IS_STRING)
                {
                    convert_to_string_ex(z_member);
                }

                /* Add to associative array: member => score */
                Z_TRY_ADDREF_P(z_score);
                add_assoc_zval(&tmp_arr, Z_STRVAL_P(z_member), z_score);
            }
        }
    }
    ZEND_HASH_FOREACH_END();

    /* Replace the original array with our flattened array */
    zval_ptr_dtor(return_value);
    ZVAL_COPY_VALUE(return_value, &tmp_arr);

    return 1;
}

/**
 * Free array of allocated strings
 */
void free_allocated_strings(char **strings, int count)
{
    if (!strings)
        return;

    for (int i = 0; i < count; i++)
    {
        if (strings[i])
        {
            efree(strings[i]);
        }
    }
}
