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
#include "valkey_glide_z_common.h"

/* Forward declarations for the Glide execute functions */
extern int execute_select_command(const void *glide_client, long database);
extern int execute_swapdb_command(const void *glide_client, long db1, long db2);
extern int execute_move_command(const void *glide_client, const char *key, size_t key_len, long db, int *output_value);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto boolean Redis::select(int dbindex) */
PHP_METHOD(Redis, select)
{
    zval *object;
    redis_object *redis;
    long dbindex;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol",
                                     &object, redis_ce, &dbindex) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SELECT command using the Glide client */
        if (execute_select_command(redis->glide_client, dbindex))
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

/* {{{ proto boolean Redis::swapdb(int db1, int db2) */
PHP_METHOD(Redis, swapdb)
{
    zval *object;
    redis_object *redis;
    long db1, db2;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oll",
                                     &object, redis_ce, &db1, &db2) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SWAPDB command using the Glide client */
        if (execute_swapdb_command(redis->glide_client, db1, db2))
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

/* {{{ proto boolean Redis::move(string key, int dbindex) */
PHP_METHOD(Redis, move)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    long dbindex;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osl",
                                     &object, redis_ce, &key, &key_len,
                                     &dbindex) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the MOVE command using the Glide client */
        int result_value = 0;

        if (execute_move_command(redis->glide_client, key, key_len, dbindex, &result_value))
        {
            if (result_value == 1)
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
