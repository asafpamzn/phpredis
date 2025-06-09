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
  | Original author: Alfonso Jimenez <yo@alfonsojimenez.com>             |
  | Maintainer: Nicolas Favre-Felix <n.favre-felix@owlient.eu>           |
  | Maintainer: Nasreddine Bouafif <n.bouafif@owlient.eu>                |
  | Maintainer: Michael Grunder <michael.grunder@gmail.com>              |
  +----------------------------------------------------------------------+
*/

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

/* {{{ proto mixed Redis::object(string subcommand, string key) */
PHP_METHOD(Redis, object)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *subcommand = NULL;
    size_t key_len, subcommand_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &subcommand, &subcommand_len,
                                     &key, &key_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the OBJECT command using the Glide client */
        if (execute_object_command(redis->glide_client, subcommand, subcommand_len, key, key_len, return_value) >= 0)
        {
            return;
        }
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto array Redis::zRange(string key, mixed start, mixed end [, bool|array options]) */
ZRANGE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto int Redis::zRangeStore(string dest, string src, mixed start, mixed end [, array options]) */
ZRANGESTORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zRevRange(string key, mixed start, mixed end [, array options]) */
ZREVRANGE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zRangeByScore(string key, mixed min, mixed max [, array options]) */
ZRANGEBYSCORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zRevRangeByScore(string key, mixed max, mixed min [, array options]) */
ZREVRANGEBYSCORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zRangeByLex(string key, mixed min, mixed max [, array options | long offset, long count]) */
ZRANGEBYLEX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zRevRangeByLex(string key, mixed max, mixed min [, array options]) */
ZREVRANGEBYLEX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zLexCount(string key, mixed min, mixed max) */
PHP_METHOD(Redis, zLexCount)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    char *min, *max;
    size_t min_len, max_len;
    long count;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osss",
                                     &object, redis_ce, &key, &key_len, &min, &min_len,
                                     &max, &max_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ZLEXCOUNT command using the Glide client */
        if (execute_zlexcount_command(redis->glide_client, key, key_len, min, min_len, max, max_len, &count))
        {
            RETURN_LONG(count);
        }
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto long Redis::zRemRangeByLex(string key, mixed min, mixed max) */
ZREMRANGEBYLEX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zRem(string key, string member, ...) */
ZREM_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zRemRangeByScore(string key, mixed min, mixed max) */
ZREMRANGEBYSCORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zRemRangeByRank(string key, long start, long end) */
ZREMRANGEBYRANK_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zCount(string key, mixed min, mixed max) */
PHP_METHOD(Redis, zCount)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    char *min, *max;
    size_t min_len, max_len;
    long count;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osss",
                                     &object, redis_ce, &key, &key_len, &min, &min_len,
                                     &max, &max_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ZCOUNT command using the Glide client */
        if (execute_zcount_command(redis->glide_client, key, key_len, min, min_len, max, max_len, &count))
        {
            RETURN_LONG(count);
        }
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto long Redis::zCard(string key) */
PHP_METHOD(Redis, zCard)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    long card;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os",
                                     &object, redis_ce, &key, &key_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ZCARD command using the Glide client */
        if (execute_zcard_command(redis->glide_client, key, key_len, &card))
        {
            RETURN_LONG(card);
        }
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto double Redis::zScore(string key, string member) */
PHP_METHOD(Redis, zScore)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *member = NULL;
    size_t key_len, member_len;
    double score;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &key, &key_len,
                                     &member, &member_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ZSCORE command using the Glide client */
        int result = execute_zscore_command(redis->glide_client, key, key_len, member, member_len, &score);

        if (result == 1)
        {
            RETURN_DOUBLE(score);
        }
        else if (result == 0)
        {
            RETURN_FALSE; /* Member doesn't exist */
        }
        else
        {
            RETURN_FALSE; /* Error */
        }
    }
}
/* }}} */

/* {{{ proto array Redis::zMscore(string key, string member, string member2...)
   proto array Redis::zMscore(string key, array members) */
PHP_METHOD(Redis, zMscore)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    int argc = ZEND_NUM_ARGS();
    zval *z_args = NULL;
    int member_count = 0;

    /* Method signature can be either of the following:
     * - zMscore(string key, string member [, string ...])
     * - zMscore(string key, array members)
     */

    /* First, check if we have the second signature with an array */
    if (argc == 2)
    {
        zval *z_members;

        /* Try to parse as (key, array) */
        if (zend_parse_method_parameters(argc, getThis(), "Osa",
                                         &object, redis_ce, &key, &key_len,
                                         &z_members) == SUCCESS)
        {
            /* Get Redis object */
            redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

            /* If we have a Glide client, use it */
            if (redis->glide_client)
            {
                HashTable *ht_members = Z_ARRVAL_P(z_members);
                member_count = zend_hash_num_elements(ht_members);

                if (member_count == 0)
                {
                    RETURN_FALSE;
                }

                /* Create an array of members from the associative array */
                zval *members = emalloc(sizeof(zval) * member_count);
                zval *data;
                int idx = 0;

                ZEND_HASH_FOREACH_VAL(ht_members, data)
                {
                    ZVAL_COPY_VALUE(&members[idx++], data);
                }
                ZEND_HASH_FOREACH_END();

                /* Initialize return array */
                array_init(return_value);

                /* Execute the ZMSCORE command using the Glide client */
                if (execute_zmscore_command(redis->glide_client, key, key_len, members, member_count, return_value))
                {
                    efree(members);
                    return;
                }

                /* Command failed */
                efree(members);
                zval_dtor(return_value);
                RETURN_FALSE;
            }
        }
    }

    /* If we got here, either the array format failed or we have variadic args */
    /* Parse as (key, member, member, ...) format */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os*",
                                     &object, redis_ce, &key, &key_len,
                                     &z_args, &member_count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Create an array for the member arguments */
        zval *members = z_args; /* z_args already contains our variadic arguments */

        /* Initialize return array */
        array_init(return_value);

        /* Execute the ZMSCORE command using the Glide client */
        if (execute_zmscore_command(redis->glide_client, key, key_len, members, member_count, return_value))
        {
            return;
        }

        /* Command failed */
        zval_dtor(return_value);
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto long Redis::zRank(string key, string member) */
PHP_METHOD(Redis, zRank)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *member = NULL;
    size_t key_len, member_len;
    long rank;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &key, &key_len,
                                     &member, &member_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ZRANK command using the Glide client */
        int result = execute_zrank_command(redis->glide_client, key, key_len, member, member_len, 0, &rank, NULL);

        if (result == 1)
        {
            RETURN_LONG(rank);
        }
        else if (result == 0)
        {
            RETURN_NULL(); /* Member doesn't exist */
        }
        else
        {
            RETURN_FALSE; /* Error */
        }
    }
}
/* }}} */

/* {{{ proto long Redis::zRevRank(string key, string member) */
ZREVRANK_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto double Redis::zIncrBy(string key, double value, string member) */
PHP_METHOD(Redis, zIncrBy)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *member = NULL;
    size_t key_len, member_len;
    double increment, new_score;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osds",
                                     &object, redis_ce, &key, &key_len,
                                     &increment, &member, &member_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ZINCRBY command using the Glide client */
        if (execute_zincrby_command(redis->glide_client, key, key_len, increment, member, member_len, &new_score))
        {
            RETURN_DOUBLE(new_score);
        }
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto array Redis::zdiff(array keys [, array options]) */
PHP_METHOD(Redis, zdiff)
{
    zval *object, *z_keys, *z_opts = NULL;
    redis_object *redis;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oa|a",
                                     &object, redis_ce, &z_keys, &z_opts) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Initialize return array */
        array_init(return_value);

        /* Execute the ZDIFF command using the Glide client */
        if (execute_zdiff_command(redis->glide_client, z_keys, z_opts, return_value))
        {
            return;
        }

        /* If the command failed, clean up and return FALSE */
        zval_dtor(return_value);
    }
}
/* }}} */

/* {{{ proto array Redis::zinter(array keys [, array weights] [, array options]) */
PHP_METHOD(Redis, zinter)
{
    zval *object, *z_keys, *z_weights = NULL, *z_opts = NULL;
    redis_object *redis;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oa|za",
                                     &object, redis_ce, &z_keys, &z_weights, &z_opts) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Initialize return array */
        array_init(return_value);

        /* Execute the ZINTER command using the Glide client */
        if (execute_zinter_command(redis->glide_client, z_keys, z_weights, z_opts, return_value))
        {
            return;
        }

        /* If the command failed, clean up and return FALSE */
        zval_dtor(return_value);
    }
}
/* }}} */
