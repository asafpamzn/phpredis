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

/* Import the execute functions */
extern int execute_type_command(const void *glide_client, const char *key, size_t key_len, char **result, size_t *result_len);
extern int execute_append_command(const void *glide_client, const char *key, size_t key_len, const char *value, size_t value_len, long *output_value);
extern int execute_getrange_command(const void *glide_client, const char *key, size_t key_len, long start, long end, char **result, size_t *result_len);
extern int execute_sort_command(const void *glide_client, const char *key, size_t key_len, zval *sort_pattern, zend_bool alpha, zend_bool desc, zval *return_value);
extern int execute_sort_ro_command(const void *glide_client, const char *key, size_t key_len, zval *sort_pattern, zend_bool alpha, zend_bool desc, zval *return_value);
extern int execute_expiremember_command(const void *glide_client, const char *key, size_t key_len, const char *member, size_t member_len, long seconds, long *output_value);
extern int execute_expirememberat_command(const void *glide_client, const char *key, size_t key_len, const char *member, size_t member_len, long timestamp, long *output_value);

/* {{{ proto string Redis::type(string key) */
PHP_METHOD(Redis, type)
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
        char *result = NULL;
        size_t result_len = 0;

        /* Execute the TYPE command using the Glide client */
        int cmd_result = execute_type_command(redis->glide_client, key, key_len, &result, &result_len);

        /* Process the result */
        if (cmd_result > 0 && result)
        {
            RETVAL_STRINGL(result, result_len);
            free(result); /* Free the result string */
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::append(string key, string value) */
PHP_METHOD(Redis, append)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *value = NULL;
    size_t key_len, value_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &key, &key_len,
                                     &value, &value_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        long result = 0;

        /* Execute the APPEND command using the Glide client */
        if (execute_append_command(redis->glide_client, key, key_len, value, value_len, &result))
        {
            RETURN_LONG(result);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto string Redis::getRange(string key, long start, long end) */
PHP_METHOD(Redis, getRange)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long start, end;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osll",
                                     &object, redis_ce, &key, &key_len,
                                     &start, &end) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        char *result = NULL;
        size_t result_len = 0;

        /* Execute the GETRANGE command using the Glide client */
        int cmd_result = execute_getrange_command(redis->glide_client, key, key_len, start, end, &result, &result_len);

        /* Process the result */
        if (cmd_result > 0 && result)
        {
            RETVAL_STRINGL(result, result_len);
            free(result); /* Free the result string */
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::sort(string key [, array options]) */
PHP_METHOD(Redis, sort)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *z_opts = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|a",
                                     &object, redis_ce, &key, &key_len,
                                     &z_opts) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SORT command using the Glide client */
        /* Default: numeric, ascending */
        if (!execute_sort_command(redis->glide_client, key, key_len, z_opts, 0, 0, return_value))
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::sort_ro(string key [, array options]) */
PHP_METHOD(Redis, sort_ro)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *z_opts = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|a",
                                     &object, redis_ce, &key, &key_len,
                                     &z_opts) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SORT_RO command using the Glide client */
        /* Default: numeric, ascending */
        if (!execute_sort_ro_command(redis->glide_client, key, key_len, z_opts, 0, 0, return_value))
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::sortAsc(string key [, array options]) */
PHP_METHOD(Redis, sortAsc)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *z_opts = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|a",
                                     &object, redis_ce, &key, &key_len,
                                     &z_opts) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SORT command using the Glide client */
        /* Ascending, numeric */
        if (!execute_sort_command(redis->glide_client, key, key_len, z_opts, 0, 0, return_value))
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::sortAscAlpha(string key [, array options]) */
PHP_METHOD(Redis, sortAscAlpha)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *z_opts = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|a",
                                     &object, redis_ce, &key, &key_len,
                                     &z_opts) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SORT command using the Glide client */
        /* Ascending, alpha */
        if (!execute_sort_command(redis->glide_client, key, key_len, z_opts, 1, 0, return_value))
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::sortDesc(string key [, array options]) */
PHP_METHOD(Redis, sortDesc)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *z_opts = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|a",
                                     &object, redis_ce, &key, &key_len,
                                     &z_opts) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SORT command using the Glide client */
        /* Descending, numeric */
        if (!execute_sort_command(redis->glide_client, key, key_len, z_opts, 0, 1, return_value))
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::sortDescAlpha(string key [, array options]) */
PHP_METHOD(Redis, sortDescAlpha)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *z_opts = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|a",
                                     &object, redis_ce, &key, &key_len,
                                     &z_opts) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SORT command using the Glide client */
        /* Descending, alpha */
        if (!execute_sort_command(redis->glide_client, key, key_len, z_opts, 1, 1, return_value))
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto boolean Redis::expiremember(string key, string member, long seconds) */
PHP_METHOD(Redis, expiremember)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *member = NULL;
    size_t key_len, member_len;
    zend_long seconds;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ossl",
                                     &object, redis_ce, &key, &key_len,
                                     &member, &member_len, &seconds) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        long result = 0;

        /* Execute the EXPIREMEMBER command using the Glide client */
        if (execute_expiremember_command(redis->glide_client, key, key_len, member, member_len, seconds, &result))
        {
            if (result)
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
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto boolean Redis::expirememberat(string key, string member, long timestamp) */
PHP_METHOD(Redis, expirememberat)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *member = NULL;
    size_t key_len, member_len;
    zend_long timestamp;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ossl",
                                     &object, redis_ce, &key, &key_len,
                                     &member, &member_len, &timestamp) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        long result = 0;

        /* Execute the EXPIREMEMBERAT command using the Glide client */
        if (execute_expirememberat_command(redis->glide_client, key, key_len, member, member_len, timestamp, &result))
        {
            if (result)
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
            RETURN_FALSE;
        }
    }
}
/* }}} */
