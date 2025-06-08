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

/* ====================================================================
 * COMMON EXECUTION FRAMEWORK IMPLEMENTATION
 * ==================================================================== */

/**
 * Generic Z-command execution framework
 */
int execute_z_generic_command(
    const void *glide_client,
    enum RequestType cmd_type,
    z_command_args_t *args,
    void *result_ptr,
    z_result_processor_t process_result)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
    }

    uintptr_t *arg_values = NULL;
    unsigned long *arg_lens = NULL;
    char **allocated_strings = NULL;
    int allocated_count = 0;
    int arg_count = 0;
    int success = 0;

    /* Determine argument preparation method based on command type */
    switch (cmd_type)
    {
    case ZCard:
        arg_count = prepare_z_key_args(args, &arg_values, &arg_lens);
        break;

    case ZScore:
    case ZRank:
    case ZRevRank:
        arg_count = prepare_z_member_args(args, &arg_values, &arg_lens);
        break;

    case ZCount:
    case ZLexCount:
    case ZRemRangeByScore:
    case ZRemRangeByLex:
        arg_count = prepare_z_range_args(args, &arg_values, &arg_lens);
        break;

    case ZRem:
    case ZMScore:
        allocated_strings = (char **)emalloc(args->member_count * sizeof(char *));
        if (!allocated_strings)
        {
            return 0;
        }
        arg_count = prepare_z_members_args(args, &arg_values, &arg_lens,
                                           &allocated_strings, &allocated_count);
        break;

    case ZRange:
    case ZRevRange:
    case ZRangeByScore:
    case ZRangeByLex:
    case ZRevRangeByScore:
    case ZRevRangeByLex:
        allocated_strings = (char **)emalloc(10 * sizeof(char *)); /* Enough for typical options */
        if (!allocated_strings)
        {
            return 0;
        }
        arg_count = prepare_z_complex_range_args(args, &arg_values, &arg_lens,
                                                 &allocated_strings, &allocated_count);
        break;

    case ZIncrBy:
        arg_count = 3; /* key + increment + member */
        arg_values = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
        arg_lens = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));
        allocated_strings = (char **)emalloc(1 * sizeof(char *));

        if (!arg_values || !arg_lens || !allocated_strings)
        {
            if (arg_values)
                efree(arg_values);
            if (arg_lens)
                efree(arg_lens);
            if (allocated_strings)
                efree(allocated_strings);
            return 0;
        }

        /* Set arguments */
        arg_values[0] = (uintptr_t)args->key;
        arg_lens[0] = args->key_len;

        /* Add increment parameter */
        char increment_str[64];
        int increment_str_len = snprintf(increment_str, sizeof(increment_str), "%.17g", args->increment);
        char *increment_str_copy = estrndup(increment_str, increment_str_len);
        if (!increment_str_copy)
        {
            efree(arg_values);
            efree(arg_lens);
            efree(allocated_strings);
            return 0;
        }

        arg_values[1] = (uintptr_t)increment_str_copy;
        arg_lens[1] = increment_str_len;
        allocated_strings[0] = increment_str_copy;
        allocated_count = 1;

        /* Add member parameter */
        arg_values[2] = (uintptr_t)args->member;
        arg_lens[2] = args->member_len;
        break;

    case ZRemRangeByRank:
        arg_count = 3; /* key + start + stop */
        arg_values = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
        arg_lens = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));
        allocated_strings = (char **)emalloc(2 * sizeof(char *));

        if (!arg_values || !arg_lens || !allocated_strings)
        {
            if (arg_values)
                efree(arg_values);
            if (arg_lens)
                efree(arg_lens);
            if (allocated_strings)
                efree(allocated_strings);
            return 0;
        }

        /* Set arguments */
        arg_values[0] = (uintptr_t)args->key;
        arg_lens[0] = args->key_len;

        /* Add start and end parameters */
        char start_str[32], end_str[32];
        int start_str_len = snprintf(start_str, sizeof(start_str), "%ld", args->start);
        int end_str_len = snprintf(end_str, sizeof(end_str), "%ld", args->end);

        char *start_str_copy = estrndup(start_str, start_str_len);
        char *end_str_copy = estrndup(end_str, end_str_len);
        if (!start_str_copy || !end_str_copy)
        {
            if (start_str_copy)
                efree(start_str_copy);
            efree(arg_values);
            efree(arg_lens);
            efree(allocated_strings);
            return 0;
        }

        arg_values[1] = (uintptr_t)start_str_copy;
        arg_lens[1] = start_str_len;
        allocated_strings[0] = start_str_copy;

        arg_values[2] = (uintptr_t)end_str_copy;
        arg_lens[2] = end_str_len;
        allocated_strings[1] = end_str_copy;
        allocated_count = 2;
        break;

    default:
        /* Unsupported command type */
        return 0;
    }

    /* Check if argument preparation was successful */
    if (arg_count <= 0)
    {
        if (arg_values)
            efree(arg_values);
        if (arg_lens)
            efree(arg_lens);
        if (allocated_strings)
            efree(allocated_strings);
        return 0;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        cmd_type,
        arg_count,
        arg_values,
        arg_lens);

    /* Free allocated strings */
    int i;
    for (i = 0; i < allocated_count; i++)
    {
        if (allocated_strings[i])
        {
            efree(allocated_strings[i]);
        }
    }
    if (allocated_strings)
        efree(allocated_strings);
    if (arg_values)
        efree(arg_values);
    if (arg_lens)
        efree(arg_lens);

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

    /* Process the result */
    success = process_result(result, result_ptr);

    /* Free the result */
    free_command_result(result);

    return success;
}

/* ====================================================================
 * ARGUMENT PREPARATION UTILITIES IMPLEMENTATION
 * ==================================================================== */

/**
 * Prepare basic Z-command arguments (just key)
 */
int prepare_z_key_args(z_command_args_t *args, uintptr_t **args_out,
                       unsigned long **args_len_out)
{
    if (!args || !args->key || !args_out || !args_len_out)
    {
        return 0;
    }

    unsigned long arg_count = 1; /* just key */

    *args_out = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    *args_len_out = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!(*args_out) || !(*args_len_out))
    {
        if (*args_out)
            efree(*args_out);
        if (*args_len_out)
            efree(*args_len_out);
        return 0;
    }

    /* Set arguments */
    (*args_out)[0] = (uintptr_t)args->key;
    (*args_len_out)[0] = args->key_len;

    return arg_count;
}

/**
 * Prepare member-based Z-command arguments (key + member)
 */
int prepare_z_member_args(z_command_args_t *args, uintptr_t **args_out,
                          unsigned long **args_len_out)
{
    if (!args || !args->key || !args->member || !args_out || !args_len_out)
    {
        return 0;
    }

    unsigned long arg_count = 2; /* key + member */

    if (args->withscores)
    {
        arg_count++; /* Add WITHSCORE parameter */
    }

    *args_out = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    *args_len_out = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!(*args_out) || !(*args_len_out))
    {
        if (*args_out)
            efree(*args_out);
        if (*args_len_out)
            efree(*args_len_out);
        return 0;
    }

    /* Set arguments */
    (*args_out)[0] = (uintptr_t)args->key;
    (*args_len_out)[0] = args->key_len;

    (*args_out)[1] = (uintptr_t)args->member;
    (*args_len_out)[1] = args->member_len;

    /* Add WITHSCORE if required */
    if (args->withscores)
    {
        const char *withscore_str = "WITHSCORE";
        (*args_out)[2] = (uintptr_t)withscore_str;
        (*args_len_out)[2] = 9; /* length of "WITHSCORE" */
    }

    return arg_count;
}

/**
 * Prepare range-based Z-command arguments (key + min + max)
 */
int prepare_z_range_args(z_command_args_t *args, uintptr_t **args_out,
                         unsigned long **args_len_out)
{
    if (!args || !args->key || !args->min || !args->max || !args_out || !args_len_out)
    {
        return 0;
    }

    unsigned long arg_count = 3; /* key + min + max */

    *args_out = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    *args_len_out = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!(*args_out) || !(*args_len_out))
    {
        if (*args_out)
            efree(*args_out);
        if (*args_len_out)
            efree(*args_len_out);
        return 0;
    }

    /* Set arguments */
    (*args_out)[0] = (uintptr_t)args->key;
    (*args_len_out)[0] = args->key_len;

    (*args_out)[1] = (uintptr_t)args->min;
    (*args_len_out)[1] = args->min_len;

    (*args_out)[2] = (uintptr_t)args->max;
    (*args_len_out)[2] = args->max_len;

    return arg_count;
}

/**
 * Prepare multi-member Z-command arguments (key + multiple members)
 */
int prepare_z_members_args(z_command_args_t *args, uintptr_t **args_out,
                           unsigned long **args_len_out,
                           char ***allocated_strings, int *allocated_count)
{
    if (!args || !args->key || !args->members || args->member_count <= 0 ||
        !args_out || !args_len_out || !allocated_strings || !allocated_count)
    {
        return 0;
    }

    *allocated_count = 0;

    /* Prepare command arguments */
    unsigned long arg_count = 1 + args->member_count; /* key + members */

    *args_out = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    *args_len_out = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!(*args_out) || !(*args_len_out))
    {
        if (*args_out)
            efree(*args_out);
        if (*args_len_out)
            efree(*args_len_out);
        return 0;
    }

    /* First argument: key */
    (*args_out)[0] = (uintptr_t)args->key;
    (*args_len_out)[0] = args->key_len;

    /* Add members as arguments */
    int i;
    for (i = 0; i < args->member_count; i++)
    {
        zval *z_member = &args->members[i];

        if (Z_TYPE_P(z_member) == IS_STRING)
        {
            (*args_out)[i + 1] = (uintptr_t)Z_STRVAL_P(z_member);
            (*args_len_out)[i + 1] = Z_STRLEN_P(z_member);
        }
        else
        {
            /* Convert non-string values to string */
            char *str_val = NULL;
            size_t str_len = 0;
            int need_free = 0;

            str_val = zval_to_string_safe(z_member, &str_len, &need_free);

            if (!str_val)
            {
                int j;
                for (j = 0; j < *allocated_count; j++)
                {
                    efree((*allocated_strings)[j]);
                }
                efree(*args_out);
                efree(*args_len_out);
                return 0;
            }

            (*args_out)[i + 1] = (uintptr_t)str_val;
            (*args_len_out)[i + 1] = str_len;

            if (need_free)
            {
                (*allocated_strings)[(*allocated_count)++] = str_val;
            }
        }
    }

    return arg_count;
}

/**
 * Convert a zval to a string argument
 */
static int convert_zval_to_string_arg(zval *z_value, uintptr_t *arg_ptr, unsigned long *arg_len_ptr,
                                      char ***allocated_strings, int *allocated_count)
{
    if (Z_TYPE_P(z_value) == IS_STRING)
    {
        *arg_ptr = (uintptr_t)Z_STRVAL_P(z_value);
        *arg_len_ptr = Z_STRLEN_P(z_value);
        return 1;
    }
    else
    {
        /* Convert non-string values to string */
        char *str_val = NULL;
        size_t str_len = 0;
        int need_free = 0;

        str_val = zval_to_string_safe(z_value, &str_len, &need_free);

        if (!str_val)
        {
            return 0;
        }

        *arg_ptr = (uintptr_t)str_val;
        *arg_len_ptr = str_len;

        if (need_free)
        {
            (*allocated_strings)[(*allocated_count)++] = str_val;
        }

        return 1;
    }
}

/**
 * Prepare complex range Z-command arguments with options
 */
int prepare_z_complex_range_args(z_command_args_t *args, uintptr_t **args_out,
                                 unsigned long **args_len_out,
                                 char ***allocated_strings, int *allocated_count)
{
    if (!args || !args->key || !args->z_start || !args->z_end ||
        !args_out || !args_len_out || !allocated_strings || !allocated_count)
    {
        return 0;
    }

    *allocated_count = 0;

    /* Parse range options */
    range_options_t range_opts = {0};
    if (!parse_range_options(args->options, &range_opts))
    {
        return 0;
    }

    /* Calculate argument count based on options */
    unsigned long arg_count = 3; /* key + start + end */
    if (range_opts.withscores)
        arg_count++; /* Add WITHSCORES parameter */
    if (range_opts.byscore)
        arg_count++; /* Add BYSCORE parameter */
    if (range_opts.bylex)
        arg_count++; /* Add BYLEX parameter */
    if (range_opts.rev)
        arg_count++; /* Add REV parameter */
    if (range_opts.has_limit)
        arg_count += 3; /* Add LIMIT + offset + count parameters */

    /* Allocate memory for arguments */
    *args_out = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    *args_len_out = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!(*args_out) || !(*args_len_out))
    {
        if (*args_out)
            efree(*args_out);
        if (*args_len_out)
            efree(*args_len_out);
        return 0;
    }

    /* First argument: key */
    (*args_out)[0] = (uintptr_t)args->key;
    (*args_len_out)[0] = args->key_len;

    /* Convert start and end to strings if needed */
    if (!convert_zval_to_string_arg(args->z_start, &((*args_out)[1]), &((*args_len_out)[1]),
                                    allocated_strings, allocated_count))
    {
        efree(*args_out);
        efree(*args_len_out);
        return 0;
    }

    if (!convert_zval_to_string_arg(args->z_end, &((*args_out)[2]), &((*args_len_out)[2]),
                                    allocated_strings, allocated_count))
    {
        int i;
        for (i = 0; i < *allocated_count; i++)
        {
            efree((*allocated_strings)[i]);
        }
        efree(*args_out);
        efree(*args_len_out);
        return 0;
    }

    /* Add optional parameters in the correct order */
    int arg_idx = 3; /* Start after key, start, end */

    /* Add BYSCORE parameter if required */
    if (range_opts.byscore)
    {
        const char *byscore_str = "BYSCORE";
        (*args_out)[arg_idx] = (uintptr_t)byscore_str;
        (*args_len_out)[arg_idx] = 7; /* length of "BYSCORE" */
        arg_idx++;
    }

    /* Add BYLEX parameter if required */
    if (range_opts.bylex)
    {
        const char *bylex_str = "BYLEX";
        (*args_out)[arg_idx] = (uintptr_t)bylex_str;
        (*args_len_out)[arg_idx] = 5; /* length of "BYLEX" */
        arg_idx++;
    }

    /* Add REV parameter if required */
    if (range_opts.rev)
    {
        const char *rev_str = "REV";
        (*args_out)[arg_idx] = (uintptr_t)rev_str;
        (*args_len_out)[arg_idx] = 3; /* length of "REV" */
        arg_idx++;
    }

    /* Add LIMIT parameter if required */
    if (range_opts.has_limit)
    {
        /* Add LIMIT + offset + count using common helper */
        arg_idx += create_limit_args(&range_opts, *args_out, *args_len_out,
                                     arg_idx, *allocated_strings, allocated_count);
    }

    /* Add WITHSCORES if required - add it last as per Redis command syntax */
    if (range_opts.withscores)
    {
        const char *withscores_str = "WITHSCORES";
        (*args_out)[arg_idx] = (uintptr_t)withscores_str;
        (*args_len_out)[arg_idx] = 10; /* length of "WITHSCORES" */
        arg_idx++;
    }

    return arg_idx; /* Return actual number of arguments used */
}

/* ====================================================================
 * RESULT PROCESSING FUNCTIONS
 * ==================================================================== */

/**
 * Process integer result (for commands returning count)
 */
int process_z_int_result(CommandResult *result, void *output)
{
    long *output_value = (long *)output;

    if (!result || !result->response || !output_value)
    {
        return 0;
    }

    if (result->response->response_type == Int)
    {
        *output_value = result->response->int_value;
        return 1;
    }

    return 0;
}

/**
 * Process double result (for commands returning scores)
 */
int process_z_double_result(CommandResult *result, void *output)
{
    double *output_value = (double *)output;

    if (!result || !result->response || !output_value)
    {
        return 0;
    }

    if (result->response->response_type == String)
    {
        /* Parse string as double */
        char *endptr;
        *output_value = strtod(result->response->string_value, &endptr);
        if (*endptr == '\0' || endptr == result->response->string_value + result->response->string_value_len)
        {
            return 1;
        }
        return 0;
    }

    if (result->response->response_type == Float)
    {
        *output_value = result->response->float_value;
        return 1;
    }

    return 0;
}

/**
 * Process null/exists result (for exists-type commands)
 */
int process_z_exists_result(CommandResult *result, void *output)
{
    if (!result || !result->response)
    {
        return -1;
    }

    if (result->response->response_type == Null)
    {
        return 0; /* Member doesn't exist */
    }

    return 1; /* Member exists */
}

/**
 * Process rank result with optional score
 */
int process_z_rank_result(CommandResult *result, void *output)
{
    struct
    {
        long *rank;
        double *score;
        int withscore;
    } *rank_data = output;

    if (!result || !result->response || !rank_data || !rank_data->rank)
    {
        return -1;
    }

    if (result->response->response_type == Null)
    {
        return 0; /* Member doesn't exist */
    }

    if (result->response->response_type == Int)
    {
        *rank_data->rank = result->response->int_value;
        return 1;
    }

    if (result->response->response_type == Array && rank_data->withscore && rank_data->score)
    {
        /* Array with rank and score [rank, score] */
        if (result->response->array_value_len >= 2)
        {
            CommandResponse *rank_resp = &result->response->array_value[0];
            CommandResponse *score_resp = &result->response->array_value[1];

            if (rank_resp->response_type == Int &&
                (score_resp->response_type == String || score_resp->response_type == Float))
            {
                *rank_data->rank = rank_resp->int_value;

                if (score_resp->response_type == String)
                {
                    char *endptr;
                    *rank_data->score = strtod(score_resp->string_value, &endptr);
                }
                else
                {
                    *rank_data->score = score_resp->float_value;
                }

                return 1;
            }
        }
    }

    return -1;
}

/**
 * Process array result (for commands returning arrays)
 */
int process_z_array_result(CommandResult *result, void *output)
{
    struct
    {
        zval *return_value;
        int withscores;
    } *array_data = output;

    if (!result || !result->response || !array_data || !array_data->return_value)
    {
        return 0;
    }

    /* Process the result */
    int success = command_response_to_zval(result->response, array_data->return_value,
                                           COMMAND_RESPONSE_NOT_ASSOSIATIVE);

    if (array_data->withscores && success && Z_TYPE_P(array_data->return_value) == IS_ARRAY)
    {
        /* Use common helper to flatten withscores array */
        flatten_withscores_array(array_data->return_value);
    }

    return success;
}
