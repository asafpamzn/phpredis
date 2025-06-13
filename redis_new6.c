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
#include "redis_glide_list_common.h"
#include "command_response.h"
#include <ext/spl/spl_exceptions.h>
#include <zend_exceptions.h>
#include <ext/standard/info.h>

#ifdef PHP_SESSION
#include <ext/session/php_session.h>
#endif

/* Import needed external variables */
extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto long Redis::lPush(string key, mixed value1, mixed value2, mixed valueN) */
LPUSH_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::lPushx(string key, mixed value) */
LPUSHX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::rPushx(string key, mixed value) */
RPUSHX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string|array Redis::lPop(string key [, int count]) */
PHP_METHOD(Redis, lPop)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long count = 0;
    int has_count = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|l",
                                     &object, redis_ce, &key, &key_len, &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Check if count parameter was provided */
    has_count = (ZEND_NUM_ARGS() > 1);

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the LPOP command using the Glide client */
        if (has_count && count > 1)
        {
            /* When count > 1, return an array */
            array_init(return_value);
        }
        int result = execute_list_pop_command(redis->glide_client, LPop, key, key_len, has_count ? count : 0, return_value);

        /* Return value is already set by execute_list_pop_command if successful */
        if (result != 1)
        {
            /* Command failed */
            if (has_count && count > 1)
            {
                zval_dtor(return_value);
            }
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto string|array Redis::rPop(string key [, int count]) */
PHP_METHOD(Redis, rPop)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long count = 0;
    int has_count = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|l",
                                     &object, redis_ce, &key, &key_len, &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Check if count parameter was provided */
    has_count = (ZEND_NUM_ARGS() > 1);

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the RPOP command using the Glide client */
        if (has_count && count > 1)
        {
            /* When count > 1, return an array */
            array_init(return_value);
        }
        int result = execute_list_pop_command(redis->glide_client, RPop, key, key_len, has_count ? count : 0, return_value);

        /* Return value is already set by execute_list_pop_command if successful */
        if (result != 1)
        {
            /* Command failed */
            if (has_count && count > 1)
            {
                zval_dtor(return_value);
            }
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::blPop(array keys, double timeout) */
PHP_METHOD(Redis, blPop)
{
    zval *object;
    redis_object *redis;
    zval *keys;
    double timeout = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ozd",
                                     &object, redis_ce, &keys, &timeout) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the BLPOP command using the Glide client */

        int result = execute_list_blocking_pop_command(redis->glide_client, BLPop, keys, timeout, return_value);
        /* Return value is already set by execute_list_blocking_pop_command if successful */
        if (result != 1)
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::brPop(array keys, double timeout) */
PHP_METHOD(Redis, brPop)
{
    zval *object;
    redis_object *redis;
    zval *keys;
    double timeout = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ozd",
                                     &object, redis_ce, &keys, &timeout) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the BRPOP command using the Glide client */
        int result = execute_list_blocking_pop_command(redis->glide_client, BRPop, keys, timeout, return_value);

        /* Return value is already set by execute_list_blocking_pop_command if successful */
        if (result != 1)
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */
