/* -*- Mode: C; tab-width: 4 -*- */
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
#include "command_response.h"         /* Include command_response.h for string conversion functions */
#include "valkey_glide_hash_common.h" /* Include hash command framework */
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

#ifdef HAVE_REDIS_ZSTD
#include <zstd.h>
#endif

#ifdef HAVE_REDIS_LZ4
#include <lz4.h>
#endif

#ifdef PHP_SESSION
extern ps_module ps_mod_redis;
extern ps_module ps_mod_redis_cluster;
#endif

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

#if PHP_VERSION_ID < 80000
#include "redis_legacy_arginfo.h"
#else
#include "zend_attributes.h"
#include "redis_arginfo.h"
#endif

/* {{{ proto string Redis::ping([string message])
 */
PHP_METHOD(Redis, ping)
{
    zval *object;
    redis_object *redis;
    char *msg = NULL, *response = NULL;
    size_t msg_len = 0, response_len = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|s",
                                     &object, redis_ce, &msg, &msg_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the PING command using the Glide client */
        int result = execute_ping_command(redis->glide_client, msg, msg_len, &response, &response_len);

        /* If the result is -1, there was an error */
        if (result == -1)
        {
            RETURN_FALSE;
        }

        /* Return the response */
        if (response)
        {
            if (strncmp(response, "PONG", 4) == 0)
            {
                efree(response);
                RETURN_TRUE;
            }

            /* Return the response */
            RETVAL_STRINGL(response, response_len);
            efree(response);
            return;
        }
        else
        {
            RETURN_TRUE;
        }
    }
    RETURN_FALSE;
}
/* }}} */

/** {{{ proto bool Redis::reset()
 */
PHP_METHOD(Redis, reset)
{
    RETURN_FALSE;
    // TODO
}
/* }}} */

/* {{{ proto long Redis::hSet(string key, string field, string value) */
HSET_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto bool Redis::hSetNx(string key, string field, string value) */
HSETNX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string Redis::hGet(string key, string field) */
HGET_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::hLen(string key) */
HLEN_METHOD_IMPL(Redis);
/* }}} */

/* {{{ proto long Redis::hDel(string key, string field1, ... fieldN) */
HDEL_METHOD_IMPL(Redis);
/* }}} */

/* {{{ proto bool Redis::hExists(string key, string field) */
HEXISTS_METHOD_IMPL(Redis);
/* }}} */

/* {{{ proto array Redis::hKeys(string key) */
HKEYS_METHOD_IMPL(Redis);
/* }}} */

/* {{{ proto array Redis::hVals(string key) */
HVALS_METHOD_IMPL(Redis);

/* }}} */

/* {{{ proto array Redis::hGetAll(string key) */
HGETALL_METHOD_IMPL(Redis);

/* }}} */

/* {{{ proto double Redis::hIncrByFloat(string key, string field, double increment) */
HINCRBYFLOAT_METHOD_IMPL(Redis);
/* }}} */

/* {{{ proto long Redis::hIncrBy(string key, string field, long increment) */
HINCRBY_METHOD_IMPL(Redis);
/* }}} */

/* {{{ proto array Redis::hMget(string key, array fields) */
HMGET_METHOD_IMPL(Redis);
/* }}} */

/* {{{ proto boolean Redis::hMset(string key, array key_values) */
HMSET_METHOD_IMPL(Redis);
/* }}} */

/* {{{ proto array|string Redis::hRandField(string key [, array options]) */
HRANDFIELD_METHOD_IMPL(Redis);

/* }}} */

/* {{{ proto long Redis::hStrLen(string key, string field) */
HSTRLEN_METHOD_IMPL(Redis);
/* }}} */
