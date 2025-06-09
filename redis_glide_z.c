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

#include "valkey_glide_z_common.h"
#include "command_response.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_redis.h"

#include "redis_cluster.h"

#include "redis_glide.h"
#include "valkey_glide_z_common.h"

#include "command_response.h" /* Include command_response.h for string conversion functions */
#include <ext/spl/spl_exceptions.h>
#include <zend_exceptions.h>
#include <ext/standard/info.h>
#include <ext/hash/php_hash.h>

#if PHP_VERSION_ID < 80400
#include <ext/standard/php_random.h>
#else
#include <ext/random/php_random.h>
#endif

#ifdef PHP_SESSION
#include <ext/session/php_session.h>
#endif

/* Import the string conversion functions from command_response.c */
extern char *long_to_string(long value, size_t *len);
extern char *double_to_string(double value, size_t *len);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

#if PHP_VERSION_ID < 80000
#include "redis_legacy_arginfo.h"
#else
#include "zend_attributes.h"
#include "redis_arginfo.h"
#endif

/* {{{ proto array Redis::zRandMember(string key [, int|array options [, bool withscores]]) */
ZRANDMEMBER_METHOD_IMPL(Redis)
/* }}} */

int execute_zrandmember_command(zval *object, int argc, zval *return_value)
{
    char *key = NULL;
    size_t key_len;
    zend_long count = 1;
    zend_bool withscores = 0;

    zval *z_opts = NULL;
    const void *glide_client = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Os|zb",
                                     &object, redis_ce, &key, &key_len, &z_opts, &withscores) == FAILURE)
    {

        return 0;
    }

    /* Get Redis object */
    redis_object *redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    glide_client = redis->glide_client;

    /* Check if we have a valid glide client */
    if (!glide_client)
    {
        return 0;
    }

    /* Process the options if provided */
    if (argc >= 2)
    {
        /* If the second parameter is an array, it contains options */
        if (z_opts && Z_TYPE_P(z_opts) == IS_ARRAY)
        {
            /* Reset default values as we'll get them from the array */
            count = 1;
            withscores = 0;

            /* Look for 'count' option */
            zval *z_count;
            if ((z_count = zend_hash_str_find(Z_ARRVAL_P(z_opts), "count", sizeof("count") - 1)) != NULL ||
                (z_count = zend_hash_str_find(Z_ARRVAL_P(z_opts), "COUNT", sizeof("COUNT") - 1)) != NULL)
            {
                if (z_count && Z_TYPE_P(z_count) == IS_LONG)
                {
                    count = Z_LVAL_P(z_count);
                }
            }

            /* Look for 'withscores' option */
            zval *z_withscores;
            if ((z_withscores = zend_hash_str_find(Z_ARRVAL_P(z_opts), "withscores", sizeof("withscores") - 1)) != NULL ||
                (z_withscores = zend_hash_str_find(Z_ARRVAL_P(z_opts), "WITHSCORES", sizeof("WITHSCORES") - 1)) != NULL)
            {
                if (z_withscores && Z_TYPE_P(z_withscores) == IS_TRUE)
                {
                    withscores = 1;
                }
            }
        }
        /* If the second parameter is a long, it's a count (backward compatibility) */
        else if (Z_TYPE_P(z_opts) == IS_LONG)
        {
            count = Z_LVAL_P(z_opts);
            /* If there's a third argument, it's withscores (backward compatibility) */
            if (argc >= 3)
            {
                /* withscores was already parsed above via zend_parse_method_parameters */
            }
        }
        /* If the second parameter is boolean, it's withscores without a count */
        else if (Z_TYPE_P(z_opts) == IS_TRUE)
        {
            withscores = 1;
        }
    }

    /* Use framework for command execution */
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.start = count; /* reuse start field for count */
    args.withscores = withscores;

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, withscores};

    return execute_z_generic_command(
        glide_client,
        ZRandMember,
        &args,
        &array_data,
        process_z_array_zrand_result);
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

int execute_zrange_command(zval *object, int argc, zval *return_value)
{
    char *key = NULL;
    size_t key_len;
    zval *z_start, *z_end, *options = NULL;
    const void *glide_client = NULL;

    /* Parse parameters - allow either boolean or array for the optional 4th parameter */
    if (zend_parse_method_parameters(argc, object, "Oszz|z",
                                     &object, redis_ce, &key, &key_len, &z_start, &z_end,
                                     &options) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis_object *redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    glide_client = redis->glide_client;

    /* Check if we have a valid glide client */
    if (!glide_client)
    {
        return 0;
    }

    /* Initialize return array */
    array_init(return_value);

    /* Use framework for command execution */
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

    int result = execute_z_generic_command(
        glide_client,
        ZRange,
        &args,
        &array_data,
        process_z_array_result);

    /* If the command failed, clean up the return array */
    if (!result)
    {
        zval_dtor(return_value);
    }

    return result;
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
    z_command_args_t args = {0};
    args.key = dst; /* Store commands use destination as key */
    args.key_len = dst_len;
    args.members = keys; /* Reuse members field for keys array */
    args.member_count = keys_count;
    args.weights = weights;
    args.options = options;

    return execute_z_generic_command(
        glide_client,
        cmd_type,
        &args,
        output_value,
        process_z_int_result);
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

/* Execute a ZREVRANGE command using the Valkey Glide client */
int execute_zrevrange_command(zval *object, int argc, zval *return_value)
{
    char *key = NULL;
    size_t key_len;
    zval *z_start, *z_end, *options = NULL;
    const void *glide_client = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Oszz|a",
                                     &object, redis_ce, &key, &key_len, &z_start, &z_end,
                                     &options) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis_object *redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    glide_client = redis->glide_client;

    /* Check if we have a valid glide client */
    if (!glide_client)
    {
        return 0;
    }

    /* Initialize return array */
    array_init(return_value);

    /* Use framework for command execution */
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

    int result = execute_z_generic_command(
        glide_client,
        ZRevRange,
        &args,
        &array_data,
        process_z_array_result);

    /* If the command failed, clean up the return array */
    if (!result)
    {
        zval_dtor(return_value);
    }

    return result;
}

/* Execute a ZRANGEBYSCORE command using the Valkey Glide client */
int execute_zrangebyscore_command(zval *object, int argc, zval *return_value)
{
    char *key = NULL;
    size_t key_len;
    zval *z_min = NULL, *z_max = NULL, *z_opts = NULL;
    const void *glide_client = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Oszz|z",
                                     &object, redis_ce, &key, &key_len, &z_min, &z_max,
                                     &z_opts) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis_object *redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    glide_client = redis->glide_client;

    /* Check if we have a valid glide client */
    if (!glide_client)
    {
        return 0;
    }

    /* Initialize return array */
    array_init(return_value);

    /* Use framework for command execution */
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.z_start = z_min;
    args.z_end = z_max;
    args.options = z_opts;

    /* Parse options to determine if withscores is set */
    range_options_t range_opts = {0};
    parse_range_options(z_opts, &range_opts);

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, range_opts.withscores};

    int result = execute_z_generic_command(
        glide_client,
        ZRangeByScore,
        &args,
        &array_data,
        process_z_array_result);

    /* If the command failed, clean up the return array */
    if (!result)
    {
        zval_dtor(return_value);
    }

    return result;
}

/* Execute a ZREVRANGEBYSCORE command using the Valkey Glide client */
int execute_zrevrangebyscore_command(zval *object, int argc, zval *return_value)
{
    char *key = NULL;
    size_t key_len;
    zval *z_max, *z_min, *options = NULL;
    const void *glide_client = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Oszz|a",
                                     &object, redis_ce, &key, &key_len, &z_max, &z_min,
                                     &options) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis_object *redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    glide_client = redis->glide_client;

    /* Check if we have a valid glide client */
    if (!glide_client)
    {
        return 0;
    }

    /* Initialize return array */
    array_init(return_value);

    /* Use framework for command execution */
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.z_start = z_max; /* For ZREVRANGEBYSCORE, start is max */
    args.z_end = z_min;   /* For ZREVRANGEBYSCORE, end is min */
    args.options = options;

    /* Parse options to determine if withscores is set */
    range_options_t range_opts = {0};
    parse_range_options(options, &range_opts);

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, range_opts.withscores};

    int result = execute_z_generic_command(
        glide_client,
        ZRevRangeByScore,
        &args,
        &array_data,
        process_z_array_result);

    /* If the command failed, clean up the return array */
    if (!result)
    {
        zval_dtor(return_value);
    }

    return result;
}

/* Execute a ZRANGEBYLEX command using the Valkey Glide client */
int execute_zrangebylex_command(const void *glide_client, const char *key, size_t key_len,
                                zval *z_min, zval *z_max, zval *options, zval *return_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.z_start = z_min;
    args.z_end = z_max;
    args.options = options;

    /* Parse options to determine if withscores is set */
    range_options_t range_opts = {0};
    parse_range_options(options, &range_opts);
    range_opts.bylex = 1; /* ZRANGEBYLEX always has BYLEX */

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, range_opts.withscores};

    return execute_z_generic_command(
        glide_client,
        ZRangeByLex,
        &args,
        &array_data,
        process_z_array_result);
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
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.start = count; /* Reuse start field for count */

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, 0};

    return execute_z_generic_command(
        glide_client,
        ZPopMax,
        &args,
        &array_data,
        process_z_array_result);
}

/* Execute a ZPOPMIN command using the Valkey Glide client */
int execute_zpopmin_command(const void *glide_client, const char *key, size_t key_len, long count, zval *return_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.start = count; /* Reuse start field for count */

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, 0};

    return execute_z_generic_command(
        glide_client,
        ZPopMin,
        &args,
        &array_data,
        process_z_array_result);
}

/* Execute a ZADD command using the Valkey Glide client */
int execute_zadd_command(const void *glide_client, const char *key, size_t key_len, zval *z_args, int argc, int flags, long *output_value, double *output_value_double)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.members = z_args;
    args.member_count = argc;

    /* Determine if INCR option is present by parsing first element if it's an array */
    int has_incr = 0;
    if (argc > 0 && Z_TYPE(z_args[0]) == IS_ARRAY)
    {
        zadd_options_t zadd_opts = {0};
        parse_zadd_options(&z_args[0], &zadd_opts);
        has_incr = zadd_opts.incr;
    }

    struct
    {
        long *output_value;
        double *output_value_double;
        int is_incr;
    } zadd_data = {output_value, output_value_double, has_incr};

    return execute_z_generic_command(
        glide_client,
        ZAdd,
        &args,
        &zadd_data,
        process_z_zadd_result);
}

/* Execute a ZRANGESTORE command using the Valkey Glide client */
int execute_zrangestore_command(zval *object, int argc, zval *return_value)
{
    char *src = NULL, *dst = NULL;
    size_t src_len, dst_len;
    zval *z_start, *z_end, *options = NULL;
    const void *glide_client = NULL;
    long result_count;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Osszz|a",
                                     &object, redis_ce, &dst, &dst_len, &src, &src_len,
                                     &z_start, &z_end, &options) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis_object *redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    glide_client = redis->glide_client;

    /* Check if we have a valid glide client */
    if (!glide_client)
    {
        return 0;
    }

    /* Use framework for command execution */
    z_command_args_t args = {0};
    args.key = dst; /* dst is the destination key */
    args.key_len = dst_len;
    args.member = src; /* src is the source key (reuse member field) */
    args.member_len = src_len;
    args.z_start = z_start;
    args.z_end = z_end;
    args.options = options;

    int result = execute_z_generic_command(
        glide_client,
        ZRangeStore,
        &args,
        &result_count,
        process_z_int_result);

    if (result)
    {
        ZVAL_LONG(return_value, result_count);
    }

    return result;
}

/* Execute a ZREVRANGEBYLEX command using the Valkey Glide client */
int execute_zrevrangebylex_command(const void *glide_client, const char *key, size_t key_len,
                                   zval *z_max, zval *z_min, zval *options, zval *return_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.z_start = z_max; /* For ZREVRANGEBYLEX, start is max */
    args.z_end = z_min;   /* For ZREVRANGEBYLEX, end is min */
    args.options = options;

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, 0}; /* ZREVRANGEBYLEX never has withscores */

    return execute_z_generic_command(
        glide_client,
        ZRevRangeByLex,
        &args,
        &array_data,
        process_z_array_result);
}

/* Execute a ZDIFF command using the Valkey Glide client */
int execute_zdiff_command(const void *glide_client, zval *keys, zval *options, zval *return_value)
{
    z_command_args_t args = {0};
    args.members = keys; /* Reuse members field for keys */
    args.member_count = zend_hash_num_elements(Z_ARRVAL_P(keys));
    args.options = options;

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, 0};

    return execute_z_generic_command(
        glide_client,
        ZDiff,
        &args,
        &array_data,
        process_z_array_result);
}

/* Execute a ZINTER command using the Valkey Glide client */
int execute_zinter_command(const void *glide_client, zval *keys, zval *z_weights, zval *options, zval *return_value)
{
    z_command_args_t args = {0};
    args.members = keys; /* Reuse members field for keys */
    args.member_count = zend_hash_num_elements(Z_ARRVAL_P(keys));
    args.weights = z_weights;
    args.options = options;

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, 0};

    return execute_z_generic_command(
        glide_client,
        ZInter,
        &args,
        &array_data,
        process_z_array_result);
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
