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
                free(response);
                RETURN_TRUE;
            }

            /* Return the response */
            RETVAL_STRINGL(response, response_len);
            free(response);
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
    char *response;
    int response_len;
    RedisSock *redis_sock;
    smart_string cmd = {0};
    zend_bool ret = 0;

    if ((redis_sock = redis_sock_get(getThis(), 0)) == NULL)
    {
        RETURN_FALSE;
    }

    if (IS_PIPELINE(redis_sock))
    {
        php_error_docref(NULL, E_ERROR, "Reset isn't allowed in pipeline mode!");
        RETURN_FALSE;
    }

    redis_cmd_init_sstr(&cmd, 0, "RESET", 5);

    REDIS_PROCESS_REQUEST(redis_sock, cmd.c, cmd.len);

    if ((response = redis_sock_read(redis_sock, &response_len)) != NULL)
    {
        ret = REDIS_STRCMP_STATIC(response, response_len, "+RESET");
        efree(response);
    }

    if (!ret)
    {
        if (IS_ATOMIC(redis_sock))
        {
            RETURN_FALSE;
        }
        REDIS_THROW_EXCEPTION("Reset failed in multi mode!", 0);
        RETURN_ZVAL(getThis(), 1, 0);
    }

    redis_free_reply_callbacks(redis_sock);
    redis_sock->status = REDIS_SOCK_STATUS_CONNECTED;
    redis_sock->mode = ATOMIC;
    redis_sock->dbNumber = 0;
    redis_sock->watching = 0;

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto long Redis::hSet(string key, string field, string value) */
PHP_METHOD(Redis, hSet)
{
    zval *object, *z_args;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    int argc;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os*",
                                     &object, redis_ce, &key, &key_len,
                                     &z_args, &argc) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the HSET command using the Glide client */
        long result_value;
        if (execute_hset_command(redis->glide_client, key, key_len, z_args, argc, &result_value))
        {
            /* Command succeeded, return the value */
            RETURN_LONG(result_value);
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto bool Redis::hSetNx(string key, string field, string value) */
PHP_METHOD(Redis, hSetNx)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *field = NULL, *val = NULL;
    size_t key_len, field_len, val_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osss",
                                     &object, redis_ce, &key, &key_len,
                                     &field, &field_len, &val, &val_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the HSETNX command using the Glide client */
        int result;
        if (execute_hsetnx_command(redis->glide_client, key, key_len, field, field_len, val, val_len, &result))
        {
            /* Command succeeded, return the value */
            if (result == 1)
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

/* {{{ proto string Redis::hGet(string key, string field) */
PHP_METHOD(Redis, hGet)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *field = NULL, *response = NULL;
    size_t key_len, field_len, response_len = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &key, &key_len,
                                     &field, &field_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the HGET command using the Glide client */
        int result = execute_hget_command(redis->glide_client, key, key_len, field, field_len, &response, &response_len);

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
            /* Key didn't exist */
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

/* {{{ proto long Redis::hLen(string key) */
PHP_METHOD(Redis, hLen)
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
        /* Execute the HLEN command using the Glide client */
        long result_value;
        if (execute_hlen_command(redis->glide_client, key, key_len, &result_value))
        {
            /* Command succeeded, return the value */
            RETURN_LONG(result_value);
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::hDel(string key, string field1, ... fieldN) */
PHP_METHOD(Redis, hDel)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *fields = NULL;
    int fields_count = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os*",
                                     &object, redis_ce, &key, &key_len,
                                     &fields, &fields_count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the HDEL command using the Glide client */
        long result_value;
        if (execute_hdel_command(redis->glide_client, key, key_len, fields, fields_count, &result_value))
        {
            /* Command succeeded, return the value */
            RETURN_LONG(result_value);
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto bool Redis::hExists(string key, string field) */
PHP_METHOD(Redis, hExists)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *field = NULL;
    size_t key_len, field_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &key, &key_len,
                                     &field, &field_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the HEXISTS command using the Glide client */
        int result;
        if (execute_hexists_command(redis->glide_client, key, key_len, field, field_len, &result))
        {
            /* Command succeeded, return the value */
            if (result == 1)
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

/* {{{ proto array Redis::hKeys(string key) */
PHP_METHOD(Redis, hKeys)
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
        /* Execute the HKEYS command using the Glide client */
        array_init(return_value);
        if (execute_hkeys_command(redis->glide_client, key, key_len, return_value))
        {
            /* Command succeeded, return_value is already set */
            return;
        }
        else
        {
            /* Command failed */
            zval_dtor(return_value);
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::hVals(string key) */
PHP_METHOD(Redis, hVals)
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
        /* Execute the HVALS command using the Glide client */
        array_init(return_value);
        if (execute_hvals_command(redis->glide_client, key, key_len, return_value))
        {
            /* Command succeeded, return_value is already set */
            return;
        }
        else
        {
            /* Command failed */
            zval_dtor(return_value);
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::hGetAll(string key) */
PHP_METHOD(Redis, hGetAll)
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
        /* Execute the HGETALL command using the Glide client */
        array_init(return_value);
        if (execute_hgetall_command(redis->glide_client, key, key_len, return_value))
        {
            /* Command succeeded, return_value is already set */
            return;
        }
        else
        {
            /* Command failed */
            zval_dtor(return_value);
            RETURN_FALSE;
        }
    }
}
/* }}} */
