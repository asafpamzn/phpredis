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

/* Forward declarations for the non-list Glide execute functions */
extern int execute_watch_command(const void *glide_client, zval *keys, int keys_count);
extern int execute_unwatch_command(const void *glide_client);
extern int execute_acl_command(const void *glide_client, zval *args, int args_count, zval *return_value);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto boolean Redis::watch(string key1, string key2...) */
PHP_METHOD(Redis, watch)
{
    zval *object;
    redis_object *redis;
    zval *z_args;
    int argc;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O*",
                                     &object, redis_ce, &z_args, &argc) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the WATCH command using the Glide client */
        if (execute_watch_command(redis->glide_client, z_args, argc))
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

/* {{{ proto boolean Redis::unwatch() */
PHP_METHOD(Redis, unwatch)
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
        /* Execute the UNWATCH command using the Glide client */
        if (execute_unwatch_command(redis->glide_client))
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

/* {{{ proto mixed Redis::acl(string $op, ...) */
PHP_METHOD(Redis, acl)
{
    zval *object;
    redis_object *redis;
    zval *z_args;
    int argc;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O*",
                                     &object, redis_ce, &z_args, &argc) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ACL command using the Glide client */
        if (execute_acl_command(redis->glide_client, z_args, argc, return_value))
        {
            /* Return value already set in execute_acl_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::lInsert(string key, string position, string pivot, string value) */
LINSERT_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string Redis::lPos(string key, mixed value, [array options = null]) */
LPOS_METHOD_IMPL(Redis)

/* }}} */

/* {{{ proto int Redis::lLen(string key) */
LLEN_METHOD_IMPL(Redis)
/* }}} */
