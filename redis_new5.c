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
#include "redis_array.h"
#include "redis_cluster.h"
#include "redis_commands.h"
#include "redis_sentinel.h"
#include "redis_glide.h"
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

#include "library.h"

/* Import the string conversion functions from command_response.c */
extern char *long_to_string(long value, size_t *len);
extern char *double_to_string(double value, size_t *len);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto array Redis::keys(string pattern) */
PHP_METHOD(Redis, keys)
{
    zval *object;
    redis_object *redis;
    char *pattern = NULL;
    size_t pattern_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os",
                                     &object, redis_ce, &pattern, &pattern_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the KEYS command using the Glide client */
        int result = execute_keys_command(redis->glide_client, pattern, pattern_len, return_value);

        /* Return the result directly if successful, otherwise return FALSE */
        if (result == 1)
        {
            return; /* Return value already set by execute_keys_command */
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::lrange(string key, long start, long end) */
PHP_METHOD(Redis, lrange)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long start, end;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osll",
                                     &object, redis_ce, &key, &key_len, &start, &end) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the LRANGE command using the Glide client */
        int result = execute_lrange_command(redis->glide_client, key, key_len, start, end, return_value);

        /* Return the result directly if successful, otherwise return FALSE */
        if (result == 1)
        {
            return; /* Return value already set by execute_lrange_command */
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */
/* {{{ proto boolean Redis::setOption(long option, mixed value) */
PHP_METHOD(Redis, setOption)
{
    zval *object;
    redis_object *redis;
    zend_long option;
    zval *value;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Olz",
                                     &object, redis_ce, &option, &value) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the setOption command using the Glide client */
        int result = execute_setOption_command(redis->glide_client, option, value);

        /* Return TRUE if successful, FALSE otherwise */
        if (result == 1)
        {
            RETURN_TRUE;
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto boolean Redis::mset(array key_values) */
PHP_METHOD(Redis, mset)
{
    zval *object;
    redis_object *redis;
    zval *z_arr;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oa",
                                     &object, redis_ce, &z_arr) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the MSET command using the Glide client */
        int result = execute_mset_command(redis->glide_client, z_arr);

        /* Return TRUE if successful, FALSE otherwise */
        if (result == 1)
        {
            RETURN_TRUE;
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto boolean Redis::msetnx(array key_values) */
PHP_METHOD(Redis, msetnx)
{
    zval *object;
    redis_object *redis;
    zval *z_arr;
    int output_value = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oa",
                                     &object, redis_ce, &z_arr) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the MSETNX command using the Glide client */
        if (execute_msetnx_command(redis->glide_client, z_arr, &output_value))
        {
            /* Return TRUE if all keys were set (output_value == 1), FALSE otherwise */
            if (output_value == 1)
            {
                RETURN_TRUE;
            }
            else
            {
                RETURN_FALSE;
            }
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto string Redis::rpoplpush(string srckey, string dstkey) */
PHP_METHOD(Redis, rpoplpush)
{
    zval *object;
    redis_object *redis;
    char *src = NULL, *dst = NULL;
    size_t src_len, dst_len;
    char *response = NULL;
    size_t response_len = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &src, &src_len,
                                     &dst, &dst_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the RPOPLPUSH command using the Glide client */
        int result = execute_rpoplpush_command(redis->glide_client, src, src_len, dst, dst_len, &response, &response_len);

        /* Process the result */
        if (result == 1 && response != NULL)
        {
            /* Return the value */
            RETVAL_STRINGL(response, response_len);
            free(response);
            return;
        }
        else if (result == 0)
        {
            /* Source list was empty */
            RETURN_FALSE;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto string Redis::brpoplpush(string src, string dst, int timeout) */
PHP_METHOD(Redis, brpoplpush)
{
    zval *object;
    redis_object *redis;
    char *src = NULL, *dst = NULL;
    size_t src_len, dst_len;
    zend_long timeout;
    char *response = NULL;
    size_t response_len = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ossl",
                                     &object, redis_ce, &src, &src_len,
                                     &dst, &dst_len, &timeout) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the BRPOPLPUSH command using the Glide client */
        int result = execute_brpoplpush_command(redis->glide_client, src, src_len, dst, dst_len, timeout, &response, &response_len);

        /* Process the result */
        if (result == 1 && response != NULL)
        {
            /* Return the value */
            RETVAL_STRINGL(response, response_len);
            free(response);
            return;
        }
        else if (result == 0)
        {
            /* Timeout occurred or source list was empty */
            RETURN_NULL();
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }
}
/* }}} */
