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

/* Function declarations for the execute_x_command functions in redis_expire_glide.c */
extern int execute_expireat_command(const void *glide_client, const char *key, size_t key_len,
                                    long timestamp, const char *mode, size_t mode_len, int *output_value);
extern int execute_pexpire_command(const void *glide_client, const char *key, size_t key_len,
                                   long milliseconds, const char *mode, size_t mode_len, int *output_value);
extern int execute_pexpireat_command(const void *glide_client, const char *key, size_t key_len,
                                     long timestamp_ms, const char *mode, size_t mode_len, int *output_value);
extern int execute_persist_command(const void *glide_client, const char *key, size_t key_len, int *output_value);
extern int execute_expiretime_command(const void *glide_client, const char *key, size_t key_len, long *output_value);
extern int execute_pexpiretime_command(const void *glide_client, const char *key, size_t key_len, long *output_value);

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
    int output_value;

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
        if (execute_expireat_command(redis->glide_client, key, key_len, timestamp, mode, mode_len, &output_value))
        {
            /* Return TRUE if key was expired (output_value == 1), FALSE otherwise */
            RETURN_BOOL(output_value);
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto bool Redis::pExpire(string key, long milliseconds [, string mode]) */
PHP_METHOD(Redis, pExpire)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *mode = NULL;
    size_t key_len, mode_len = 0;
    zend_long milliseconds;
    int output_value;

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
        if (execute_pexpire_command(redis->glide_client, key, key_len, milliseconds, mode, mode_len, &output_value))
        {
            /* Return TRUE if key was expired (output_value == 1), FALSE otherwise */
            RETURN_BOOL(output_value);
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto bool Redis::pExpireAt(string key, long milliseconds_timestamp [, string mode]) */
PHP_METHOD(Redis, pExpireAt)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *mode = NULL;
    size_t key_len, mode_len = 0;
    zend_long timestamp_ms;
    int output_value;

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
        if (execute_pexpireat_command(redis->glide_client, key, key_len, timestamp_ms, mode, mode_len, &output_value))
        {
            /* Return TRUE if key was expired (output_value == 1), FALSE otherwise */
            RETURN_BOOL(output_value);
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
    int output_value;

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
        if (execute_persist_command(redis->glide_client, key, key_len, &output_value))
        {
            /* Return TRUE if key was persisted (output_value == 1), FALSE otherwise */
            RETURN_BOOL(output_value);
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::expireTime(string key) */
PHP_METHOD(Redis, expireTime)
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

/* {{{ proto long Redis::pExpireTime(string key) */
PHP_METHOD(Redis, pExpireTime)
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
