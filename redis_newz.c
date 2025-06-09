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
ZLEXCOUNT_METHOD_IMPL(Redis)
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
ZCOUNT_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zCard(string key) */
ZCARD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto double Redis::zScore(string key, string member) */
ZSCORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zMscore(string key, string member, string member2...)
   proto array Redis::zMscore(string key, array members) */
ZMSCORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zRank(string key, string member) */
ZRANK_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zRevRank(string key, string member) */
ZREVRANK_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto double Redis::zIncrBy(string key, double value, string member) */
ZINCRBY_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zdiff(array keys [, array options]) */
ZDIFF_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zinter(array keys [, array weights] [, array options]) */
ZINTER_METHOD_IMPL(Redis)
/* }}} */
