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
#include "redis_commands.h"
#include "redis_glide.h"

/* Forward declarations for the Glide execute functions */
extern int execute_wait_command(const void *glide_client, long numreplicas, long timeout, long *output_value);
extern int execute_function_command(const void *glide_client, zval *args, int args_count, zval *return_value);
extern int execute_multi_command(const void *glide_client);
extern int execute_discard_command(const void *glide_client);
extern int execute_exec_command(const void *glide_client, zval *return_value);
extern int execute_fcall_command(const void *glide_client, const char *name, size_t name_len,
                                 int numkeys, zval *args, int args_count, zval *return_value);
extern int execute_fcall_ro_command(const void *glide_client, const char *name, size_t name_len,
                                    int numkeys, zval *args, int args_count, zval *return_value);
extern int execute_dump_command(const void *glide_client, const char *key, size_t key_len,
                                char **output, size_t *output_len);
extern int execute_restore_command(const void *glide_client, const char *key, size_t key_len,
                                   long ttl, const char *serialized, size_t serialized_len,
                                   zval *options);
extern int execute_config_command(const void *glide_client, const char *operation, size_t operation_len,
                                  zval *key, zval *value, zval *return_value);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto int Redis::wait(int numreplicas, int timeout) */
PHP_METHOD(Redis, wait)
{
    zval *object;
    redis_object *redis;
    long numreplicas, timeout;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oll",
                                     &object, redis_ce, &numreplicas, &timeout) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the WAIT command using the Glide client */
        long result_value;
        if (execute_wait_command(redis->glide_client, numreplicas, timeout, &result_value))
        {
            /* Return the number of replicas that acknowledged the write */
            RETURN_LONG(result_value);
        }
        else
        {
            RETURN_FALSE;
        }
    }

    /* If we don't have a Glide client, fall back to standard implementation */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto mixed Redis::config(string operation, mixed key [, mixed value]) */
PHP_METHOD(Redis, config)
{
    zval *object;
    redis_object *redis;
    char *operation = NULL;
    size_t operation_len;
    zval *key = NULL, *value = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|z!z!",
                                     &object, redis_ce, &operation, &operation_len,
                                     &key, &value) == FAILURE)
    {
        printf("Failed to parse parameters for Redis::config\n");
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the CONFIG command using the Glide client */
        if (execute_config_command(redis->glide_client, operation, operation_len,
                                   key, value, return_value))
        {
            /* Return value already set in execute_config_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
    }

    /* If we don't have a Glide client, fall back to standard implementation */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto mixed Redis::function(string cmd, ...) */
PHP_METHOD(Redis, function)
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
        /* Execute the FUNCTION command using the Glide client */
        if (execute_function_command(redis->glide_client, z_args, argc, return_value))
        {
            /* Return value already set in execute_function_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
    }

    /* If we don't have a Glide client, fall back to standard implementation */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto Redis Redis::multi() */
PHP_METHOD(Redis, multi)
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
        /* Execute the MULTI command using the Glide client */
        if (execute_multi_command(redis->glide_client))
        {
            /* Return $this */
            RETURN_ZVAL(object, 1, 0);
        }
        else
        {
            RETURN_FALSE;
        }
    }

    /* If we don't have a Glide client, fall back to standard implementation */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool Redis::discard() */
PHP_METHOD(Redis, discard)
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
        /* Execute the DISCARD command using the Glide client */
        if (execute_discard_command(redis->glide_client))
        {
            RETURN_TRUE;
        }
        else
        {
            RETURN_FALSE;
        }
    }

    /* If we don't have a Glide client, fall back to standard implementation */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Redis::exec() */
PHP_METHOD(Redis, exec)
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
        /* Execute the EXEC command using the Glide client */
        if (execute_exec_command(redis->glide_client, return_value))
        {
            /* Return value already set in execute_exec_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
    }

    /* If we don't have a Glide client, fall back to standard implementation */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto mixed Redis::fcall(string name, int numkeys, mixed ...args) */
PHP_METHOD(Redis, fcall)
{
    zval *object;
    redis_object *redis;
    char *name = NULL;
    size_t name_len;
    long numkeys = 0;
    zval *z_args = NULL;
    int argc = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osl*",
                                     &object, redis_ce, &name, &name_len,
                                     &numkeys, &z_args, &argc) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the FCALL command using the Glide client */
        if (execute_fcall_command(redis->glide_client, name, name_len,
                                  numkeys, z_args, argc, return_value))
        {
            /* Return value already set in execute_fcall_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
    }

    /* If we don't have a Glide client, fall back to standard implementation */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto mixed Redis::fcall_ro(string name, int numkeys, mixed ...args) */
PHP_METHOD(Redis, fcall_ro)
{
    zval *object;
    redis_object *redis;
    char *name = NULL;
    size_t name_len;
    long numkeys = 0;
    zval *z_args = NULL;
    int argc = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osl*",
                                     &object, redis_ce, &name, &name_len,
                                     &numkeys, &z_args, &argc) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the FCALL_RO command using the Glide client */
        if (execute_fcall_ro_command(redis->glide_client, name, name_len,
                                     numkeys, z_args, argc, return_value))
        {
            /* Return value already set in execute_fcall_ro_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
    }

    /* If we don't have a Glide client, fall back to standard implementation */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto string Redis::dump(string key) */
PHP_METHOD(Redis, dump)
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
        /* Execute the DUMP command using the Glide client */
        char *output;
        size_t output_len;
        int result = execute_dump_command(redis->glide_client, key, key_len, &output, &output_len);

        if (result == 1)
        {
            /* Return serialized value */
            RETVAL_STRINGL(output, output_len);
            efree(output);
            return;
        }
        else if (result == 0)
        {
            /* Key doesn't exist */
            RETURN_FALSE;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }

    /* If we don't have a Glide client, fall back to standard implementation */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool Redis::restore(string key, int ttl, string serialized_value [, array options]) */
PHP_METHOD(Redis, restore)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *serialized = NULL;
    size_t key_len, serialized_len;
    long ttl;
    zval *options = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osls|a",
                                     &object, redis_ce, &key, &key_len, &ttl,
                                     &serialized, &serialized_len, &options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the RESTORE command using the Glide client */
        if (execute_restore_command(redis->glide_client, key, key_len, ttl,
                                    serialized, serialized_len, options))
        {
            RETURN_TRUE;
        }
        else
        {
            RETURN_FALSE;
        }
    }

    /* If we don't have a Glide client, fall back to standard implementation */
    RETURN_FALSE;
}
/* }}} */
