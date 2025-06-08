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
    int success = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE);
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
