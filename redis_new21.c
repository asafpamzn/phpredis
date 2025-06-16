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

/* Forward declarations for the Glide execute functions */
extern int execute_get_persistent_id_command(const void *glide_client, char **result, size_t *result_len);
extern int execute_get_auth_command(const void *glide_client, char **result, size_t *result_len);
extern int execute_client_command(const void *glide_client, zval *args, int args_count, zval *return_value);
extern int execute_rawcommand_command(const void *glide_client, zval *args, int args_count, zval *return_value);
extern int execute_command_method(const void *glide_client, zval *args, int args_count, zval *return_value);
extern int execute_dbsize_command(const void *glide_client, long *output_value);
extern int execute_auth_command(const void *glide_client, const char *password, size_t password_len,
                                const char *username, size_t username_len);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto mixed Redis::client(string cmd, ...) */
PHP_METHOD(Redis, client)
{
    zval *object;
    redis_object *redis;
    zval *z_args;
    int argc;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O+",
                                     &object, redis_ce, &z_args, &argc) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the client command using the Glide client */
        if (execute_client_command(redis->glide_client, z_args, argc, return_value))
        {
            /* Return value already set in execute_client_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto mixed Redis::rawcommand(string cmd, ...) */
PHP_METHOD(Redis, rawcommand)
{
    zval *object;
    redis_object *redis;
    zval *z_args;
    int argc;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O+",
                                     &object, redis_ce, &z_args, &argc) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the raw command using the Glide client */
        if (execute_rawcommand_command(redis->glide_client, z_args, argc, return_value))
        {
            /* Return value already set in execute_rawcommand_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::dbSize() */
PHP_METHOD(Redis, dbSize)
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
        /* Execute the DBSIZE command using the Glide client */
        long dbsize;

        if (execute_dbsize_command(redis->glide_client, &dbsize))
        {
            RETURN_LONG(dbsize);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */
