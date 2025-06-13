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
#include "redis_glide_list_common.h"
extern int execute_flushdb_command(const void *glide_client, int async);
extern int execute_flushall_command(const void *glide_client, int async);
extern int execute_time_command(const void *glide_client, zval *return_value);
extern int execute_role_command(const void *glide_client, zval *return_value);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto string Redis::blmove(string src, string dst, string wherefrom, string whereto, int timeout) */
BLMOVE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string Redis::lMove(string src, string dst, string wherefrom, string whereto) */
LMOVE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::lrem(string key, string value [, long count = 0]) */
LREM_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto boolean Redis::ltrim(string key, long start, long end) */
LTRIM_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string Redis::lindex(string key, long index) */
LINDEX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto boolean Redis::lSet(string key, long index, string value) */
LSET_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto boolean Redis::flushDB([boolean async]) */
PHP_METHOD(Redis, flushDB)
{
    zval *object;
    redis_object *redis;
    zend_bool async = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|b",
                                     &object, redis_ce, &async) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        int status;

        /* Execute the FLUSHDB command using the Glide client */
        status = execute_flushdb_command(redis->glide_client, async);

        if (status)
        {
            /* Success */
            RETURN_TRUE;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }

    /* If we reach here, either we don't have a Glide client or something went wrong */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean Redis::flushAll([boolean async]) */
PHP_METHOD(Redis, flushAll)
{
    zval *object;
    redis_object *redis;
    zend_bool async = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|b",
                                     &object, redis_ce, &async) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        int status;

        /* Execute the FLUSHALL command using the Glide client */
        status = execute_flushall_command(redis->glide_client, async);

        if (status)
        {
            /* Success */
            RETURN_TRUE;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }

    /* If we reach here, either we don't have a Glide client or something went wrong */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Redis::time() */
PHP_METHOD(Redis, time)
{
    zval *object;
    redis_object *redis;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
                                     &object, redis_ce) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the TIME command using the Glide client */
        if (execute_time_command(redis->glide_client, return_value))
        {
            /* Return value already set in execute_time_command */
            return;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }

    /* If we reach here, either we don't have a Glide client or something went wrong */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Redis::role() */
PHP_METHOD(Redis, role)
{
    zval *object;
    redis_object *redis;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
                                     &object, redis_ce) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ROLE command using the Glide client */
        if (execute_role_command(redis->glide_client, return_value))
        {
            /* Return value already set in execute_role_command */
            return;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }

    /* If we reach here, either we don't have a Glide client or something went wrong */
    RETURN_FALSE;
}
/* }}} */
