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

int execute_zrandmember_command(const void *glide_client, const char *key, size_t key_len, long count, int withscores, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* ZRANDMEMBER has unique optional parameter handling, so we use custom argument preparation */
    /* but leverage the framework for result processing */

    /* Prepare command arguments */
    unsigned long arg_count = 1; /* Start with key */
    if (count != 0)
    {
        arg_count++; /* Add count parameter */
    }
    if (withscores)
    {
        arg_count++; /* Add WITHSCORES parameter */
    }

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

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add count parameter if not default */
    int arg_idx = 1;
    char count_str[32] = {0};
    if (count != 0)
    {
        int count_str_len = snprintf(count_str, sizeof(count_str), "%ld", count);
        args[arg_idx] = (uintptr_t)count_str;
        args_len[arg_idx] = count_str_len;
        arg_idx++;
    }

    /* Add WITHSCORES if required */
    if (withscores)
    {
        const char *withscores_str = "WITHSCORES";
        args[arg_idx] = (uintptr_t)withscores_str;
        args_len[arg_idx] = 10; /* length of "WITHSCORES" */
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZRandMember, /* command type from RequestType enum */
        arg_count,   /* number of arguments */
        args,        /* arguments array */
        args_len     /* argument lengths array */
    );

    /* Free the argument arrays */
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
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    int success = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
    if (withscores && success && Z_TYPE_P(return_value) == IS_ARRAY)
    {
        /* Use common helper to flatten withscores array */
        flatten_withscores_array(return_value);
    }

    /* Free the result */
    free_command_result(result);

    return success;
}

int execute_zscore_command(const void *glide_client, const char *key, size_t key_len,
                           const char *member, size_t member_len, double *output_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.member = member;
    args.member_len = member_len;

    int success = execute_z_generic_command(
        glide_client,
        ZScore,
        &args,
        output_value,
        process_z_double_result);

    /* Convert result to expected format for ZSCORE (-1/0/1 instead of 0/1) */
    if (success == 0)
    {
        success = -1; /* Member not found */
    }
    else if (success == 1)
    {
        success = 1; /* Success */
    }

    return success;
}

int execute_zmscore_command(const void *glide_client, const char *key, size_t key_len,
                            zval *members, int members_count, zval *return_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.members = members;
    args.member_count = members_count;

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, 0}; /* ZMSCORE doesn't use withscores */

    return execute_z_generic_command(
        glide_client,
        ZMScore,
        &args,
        &array_data,
        process_z_array_result);
}

int execute_zrank_command(const void *glide_client, const char *key, size_t key_len,
                          const char *member, size_t member_len, int withscore,
                          long *rank_value, double *score_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.member = member;
    args.member_len = member_len;
    args.withscores = withscore;

    struct
    {
        long *rank;
        double *score;
        int withscore;
    } rank_data = {rank_value, score_value, withscore};

    return execute_z_generic_command(
        glide_client,
        ZRank,
        &args,
        &rank_data,
        process_z_rank_result);
}

int execute_zrevrank_command(const void *glide_client, const char *key, size_t key_len,
                             const char *member, size_t member_len, int withscore,
                             long *rank_value, double *score_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.member = member;
    args.member_len = member_len;
    args.withscores = withscore;

    struct
    {
        long *rank;
        double *score;
        int withscore;
    } rank_data = {rank_value, score_value, withscore};

    return execute_z_generic_command(
        glide_client,
        ZRevRank,
        &args,
        &rank_data,
        process_z_rank_result);
}

int execute_zincrby_command(const void *glide_client, const char *key, size_t key_len,
                            double increment, const char *member, size_t member_len,
                            double *output_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.increment = increment;
    args.member = member;
    args.member_len = member_len;

    int success = execute_z_generic_command(
        glide_client,
        ZIncrBy,
        &args,
        output_value,
        process_z_double_result);

    /* Convert result to expected format for ZINCRBY (0/1 instead of -1/0/1) */
    if (success == -1)
    {
        success = 0;
    }

    return success;
}

int execute_zcount_command(const void *glide_client, const char *key, size_t key_len,
                           const char *min, size_t min_len, const char *max, size_t max_len,
                           long *output_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.min = min;
    args.min_len = min_len;
    args.max = max;
    args.max_len = max_len;

    return execute_z_generic_command(
        glide_client,
        ZCount,
        &args,
        output_value,
        process_z_int_result);
}

int execute_zlexcount_command(const void *glide_client, const char *key, size_t key_len,
                              const char *min, size_t min_len, const char *max, size_t max_len,
                              long *output_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.min = min;
    args.min_len = min_len;
    args.max = max;
    args.max_len = max_len;

    return execute_z_generic_command(
        glide_client,
        ZLexCount,
        &args,
        output_value,
        process_z_int_result);
}

int execute_zrem_command(const void *glide_client, const char *key, size_t key_len,
                         zval *members, int members_count, long *output_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.members = members;
    args.member_count = members_count;

    return execute_z_generic_command(
        glide_client,
        ZRem,
        &args,
        output_value,
        process_z_int_result);
}

int execute_zremrangebylex_command(const void *glide_client, const char *key, size_t key_len,
                                   const char *min, size_t min_len, const char *max, size_t max_len,
                                   long *output_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.min = min;
    args.min_len = min_len;
    args.max = max;
    args.max_len = max_len;

    return execute_z_generic_command(
        glide_client,
        ZRemRangeByLex,
        &args,
        output_value,
        process_z_int_result);
}

int execute_zremrangebyrank_command(const void *glide_client, const char *key, size_t key_len,
                                    long start, long end, long *output_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.start = start;
    args.end = end;

    return execute_z_generic_command(
        glide_client,
        ZRemRangeByRank,
        &args,
        output_value,
        process_z_int_result);
}

int execute_zremrangebyscore_command(const void *glide_client, const char *key, size_t key_len,
                                     const char *min, size_t min_len, const char *max, size_t max_len,
                                     long *output_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.min = min;
    args.min_len = min_len;
    args.max = max;
    args.max_len = max_len;

    return execute_z_generic_command(
        glide_client,
        ZRemRangeByScore,
        &args,
        output_value,
        process_z_int_result);
}

int execute_zrange_command(const void *glide_client, const char *key, size_t key_len,
                           zval *z_start, zval *z_end, zval *options, zval *return_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.z_start = z_start;
    args.z_end = z_end;
    args.options = options;

    /* Parse options to determine if withscores is set */
    range_options_t range_opts = {0};
    parse_range_options(options, &range_opts);

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, range_opts.withscores};

    return execute_z_generic_command(
        glide_client,
        ZRange,
        &args,
        &array_data,
        process_z_array_result);
}

int execute_zcard_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;

    return execute_z_generic_command(
        glide_client,
        ZCard,
        &args,
        output_value,
        process_z_int_result);
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
