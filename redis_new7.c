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

#include "php_redis.h"
#include "redis_glide.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Declaration of functions from redis_glide_str.c */
int execute_type_command(const void *glide_client, const char *key, size_t key_len, long *result);
int execute_append_command(const void *glide_client, const char *key, size_t key_len, const char *value, size_t value_len, long *output_value);
int execute_getrange_command(const void *glide_client, const char *key, size_t key_len, long start, long end, char **result, size_t *result_len);
int execute_sort_command(const void *glide_client, const char *key, size_t key_len, zval *sort_pattern, zend_bool alpha, zend_bool desc, zval *return_value);
int execute_sort_ro_command(const void *glide_client, const char *key, size_t key_len, zval *sort_pattern, zend_bool alpha, zend_bool desc, zval *return_value);
int execute_expiremember_command(const void *glide_client, const char *key, size_t key_len, const char *member, size_t member_len, long seconds, long *output_value);
int execute_expirememberat_command(const void *glide_client, const char *key, size_t key_len, const char *member, size_t member_len, long timestamp, long *output_value);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto long Redis::type(string key)
 * Returns the type of data pointed by a given key */
PHP_METHOD(Redis, type)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len = 0;
    long type_value = 0;

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
        /* Execute the TYPE command using the Glide client */
        if (execute_type_command(redis->glide_client, key, key_len, &type_value))
        {
            RETURN_LONG(type_value);
        }
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto long Redis::append(string key, string value)
 * Append specified string to the string stored in specified key */
PHP_METHOD(Redis, append)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *value = NULL;
    size_t key_len = 0, value_len = 0;
    long result_value = 0;

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
        /* Execute the APPEND command using the Glide client */
        if (execute_append_command(redis->glide_client, key, key_len, value, value_len, &result_value))
        {
            RETURN_LONG(result_value);
        }
        else
        {
            RETURN_FALSE;
        }
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto string Redis::getRange(string key, long start, long end)
 * Return a substring of a larger string */
PHP_METHOD(Redis, getRange)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *result = NULL;
    size_t key_len = 0, result_len = 0;
    long start = 0, end = 0;

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
        /* Execute the GETRANGE command using the Glide client */
        int ret = execute_getrange_command(redis->glide_client, key, key_len, start, end, &result, &result_len);

        if (ret > 0)
        {
            /* Command succeeded with data */
            RETVAL_STRINGL(result, result_len);
            efree(result);
            return;
        }
        else if (ret == 0)
        {
            /* Key didn't exist, return empty string */
            RETURN_EMPTY_STRING();
        }
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Redis::sort(string key, array options)
 * Sort a list, set or sorted set */
PHP_METHOD(Redis, sort)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len = 0;
    zval *z_opts = NULL;
    zend_bool alpha = 0, desc = 0;

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
        if (execute_sort_command(redis->glide_client, key, key_len, z_opts, alpha, desc, return_value))
        {
            return;
        }
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Redis::sort_ro(string key, array options)
 * Sort a list, set or sorted set in read-only mode */
PHP_METHOD(Redis, sort_ro)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len = 0;
    zval *z_opts = NULL;
    zend_bool alpha = 0, desc = 0;

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
        if (execute_sort_ro_command(redis->glide_client, key, key_len, z_opts, alpha, desc, return_value))
        {
            return;
        }
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Redis::sortAsc(string key, array options)
 * Sort a list, set or sorted set in ascending order */
PHP_METHOD(Redis, sortAsc)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len = 0;
    zval *z_opts = NULL;
    zend_bool alpha = 0, desc = 0;

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
        /* Execute the SORT command in ascending order */
        desc = 0; /* Ascending order */
        if (execute_sort_command(redis->glide_client, key, key_len, z_opts, alpha, desc, return_value))
        {
            return;
        }
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Redis::sortAscAlpha(string key, array options)
 * Sort a list, set or sorted set in ascending order with alpha flag */
PHP_METHOD(Redis, sortAscAlpha)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len = 0;
    zval *z_opts = NULL;
    zend_bool alpha = 1, desc = 0;

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
        /* Execute the SORT command in ascending order with alpha flag */
        desc = 0;  /* Ascending order */
        alpha = 1; /* Alpha flag */
        if (execute_sort_command(redis->glide_client, key, key_len, z_opts, alpha, desc, return_value))
        {
            return;
        }
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Redis::sortDesc(string key, array options)
 * Sort a list, set or sorted set in descending order */
PHP_METHOD(Redis, sortDesc)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len = 0;
    zval *z_opts = NULL;
    zend_bool alpha = 0, desc = 1;

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
        /* Execute the SORT command in descending order */
        desc = 1; /* Descending order */
        if (execute_sort_command(redis->glide_client, key, key_len, z_opts, alpha, desc, return_value))
        {
            return;
        }
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Redis::sortDescAlpha(string key, array options)
 * Sort a list, set or sorted set in descending order with alpha flag */
PHP_METHOD(Redis, sortDescAlpha)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len = 0;
    zval *z_opts = NULL;
    zend_bool alpha = 1, desc = 1;

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
        /* Execute the SORT command in descending order with alpha flag */
        desc = 1;  /* Descending order */
        alpha = 1; /* Alpha flag */
        if (execute_sort_command(redis->glide_client, key, key_len, z_opts, alpha, desc, return_value))
        {
            return;
        }
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool Redis::expiremember(string key, string member, int ttl)
 * Set a timeout on a member of a collection */
PHP_METHOD(Redis, expiremember)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *member = NULL;
    size_t key_len = 0, member_len = 0;
    long seconds = 0, result_value = 0;

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
        /* Execute the EXPIREMEMBER command using the Glide client */
        if (execute_expiremember_command(redis->glide_client, key, key_len,
                                         member, member_len, seconds, &result_value))
        {
            RETURN_BOOL(result_value);
        }
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool Redis::expirememberat(string key, string member, int timestamp)
 * Set a timeout on a member of a collection at a given timestamp */
PHP_METHOD(Redis, expirememberat)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *member = NULL;
    size_t key_len = 0, member_len = 0;
    long timestamp = 0, result_value = 0;

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
        /* Execute the EXPIREMEMBERAT command using the Glide client */
        if (execute_expirememberat_command(redis->glide_client, key, key_len,
                                           member, member_len, timestamp, &result_value))
        {
            RETURN_BOOL(result_value);
        }
    }

    RETURN_FALSE;
}
/* }}} */
