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

/* {{{ proto bool Redis::expire(string key, long seconds [, string mode]) */
PHP_METHOD(Redis, expire)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *mode = NULL;
    size_t key_len, mode_len = 0;
    zend_long seconds;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osl|s",
                                     &object, redis_ce, &key, &key_len,
                                     &seconds, &mode, &mode_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* If mode is specified but not one of the valid options, return FALSE */
        if (mode_len > 0)
        {
            if (strncasecmp(mode, "NX", mode_len) != 0 &&
                strncasecmp(mode, "XX", mode_len) != 0 &&
                strncasecmp(mode, "GT", mode_len) != 0 &&
                strncasecmp(mode, "LT", mode_len) != 0)
            {
                RETURN_FALSE;
            }
        }

        /* Execute the EXPIRE command using the Glide client */
        if (execute_expire_command(redis->glide_client, key, key_len, seconds, mode, mode_len))
        {
            /* Return TRUE if key was expired (output_value == 1), FALSE otherwise */
            RETURN_TRUE;
        }
        else
        {
            printf("Command failed\n");
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto bool Redis::expireAt(string key, long timestamp [, string mode]) */
PHP_METHOD(Redis, expireAt)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *mode = NULL;
    size_t key_len, mode_len = 0;
    zend_long timestamp;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osl|s",
                                     &object, redis_ce, &key, &key_len,
                                     &timestamp, &mode, &mode_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* If mode is specified but not one of the valid options, return FALSE */
        if (mode_len > 0)
        {
            if (strncasecmp(mode, "NX", mode_len) != 0 &&
                strncasecmp(mode, "XX", mode_len) != 0 &&
                strncasecmp(mode, "GT", mode_len) != 0 &&
                strncasecmp(mode, "LT", mode_len) != 0)
            {
                RETURN_FALSE;
            }
        }

        /* Execute the EXPIREAT command using the Glide client */
        if (execute_expireat_command(redis->glide_client, key, key_len, timestamp, mode, mode_len))
        {
            /* Return TRUE if key was expired */
            RETURN_TRUE;
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto bool Redis::pexpire(string key, long milliseconds [, string mode]) */
PHP_METHOD(Redis, pexpire)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *mode = NULL;
    size_t key_len, mode_len = 0;
    zend_long milliseconds;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osl|s",
                                     &object, redis_ce, &key, &key_len,
                                     &milliseconds, &mode, &mode_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* If mode is specified but not one of the valid options, return FALSE */
        if (mode_len > 0)
        {
            if (strncasecmp(mode, "NX", mode_len) != 0 &&
                strncasecmp(mode, "XX", mode_len) != 0 &&
                strncasecmp(mode, "GT", mode_len) != 0 &&
                strncasecmp(mode, "LT", mode_len) != 0)
            {
                RETURN_FALSE;
            }
        }

        /* Execute the PEXPIRE command using the Glide client */
        if (execute_pexpire_command(redis->glide_client, key, key_len, milliseconds, mode, mode_len))
        {
            /* Return TRUE if key was expired */
            RETURN_TRUE;
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto bool Redis::pexpireAt(string key, long milliseconds_timestamp [, string mode]) */
PHP_METHOD(Redis, pexpireAt)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *mode = NULL;
    size_t key_len, mode_len = 0;
    zend_long timestamp_ms;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osl|s",
                                     &object, redis_ce, &key, &key_len,
                                     &timestamp_ms, &mode, &mode_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* If mode is specified but not one of the valid options, return FALSE */
        if (mode_len > 0)
        {
            if (strncasecmp(mode, "NX", mode_len) != 0 &&
                strncasecmp(mode, "XX", mode_len) != 0 &&
                strncasecmp(mode, "GT", mode_len) != 0 &&
                strncasecmp(mode, "LT", mode_len) != 0)
            {
                RETURN_FALSE;
            }
        }

        /* Execute the PEXPIREAT command using the Glide client */
        if (execute_pexpireat_command(redis->glide_client, key, key_len, timestamp_ms, mode, mode_len))
        {
            /* Return TRUE if key was expired */
            RETURN_TRUE;
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto bool Redis::persist(string key) */
PHP_METHOD(Redis, persist)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;

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
        /* Execute the PERSIST command using the Glide client */
        if (execute_persist_command(redis->glide_client, key, key_len))
        {
            /* Return TRUE if key was persisted */
            RETURN_TRUE;
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::expiretime(string key) */
PHP_METHOD(Redis, expiretime)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    long output_value;

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
        /* Execute the EXPIRETIME command using the Glide client */
        if (execute_expiretime_command(redis->glide_client, key, key_len, &output_value))
        {
            /* Return the expiration time */
            RETURN_LONG(output_value);
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::pexpireTime(string key) */
PHP_METHOD(Redis, pexpiretime)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    long output_value;

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
        /* Execute the PEXPIRETIME command using the Glide client */
        if (execute_pexpiretime_command(redis->glide_client, key, key_len, &output_value))
        {
            /* Return the expiration time in milliseconds */
            RETURN_LONG(output_value);
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

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
    long output_value = 0;

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

            RETURN_TRUE;
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */
