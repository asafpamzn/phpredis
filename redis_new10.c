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
PHP_METHOD(Redis, lInsert)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *pos = NULL, *pivot = NULL, *val = NULL;
    size_t key_len, pos_len, pivot_len, val_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ossss",
                                     &object, redis_ce, &key, &key_len,
                                     &pos, &pos_len, &pivot, &pivot_len,
                                     &val, &val_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {

        /* Make position uppercase for comparison */
        char *upper_pos = NULL;
        if (pos_len > 0)
        {
            upper_pos = emalloc(pos_len + 1);
            int i;
            for (i = 0; i < pos_len; i++)
            {
                upper_pos[i] = toupper(pos[i]);
            }
            upper_pos[pos_len] = '\0';
        }

        /* Check if position is BEFORE or AFTER */
        if (upper_pos == NULL ||
            (strcmp(upper_pos, "BEFORE") != 0 && strcmp(upper_pos, "AFTER") != 0))
        {

            if (upper_pos)
                efree(upper_pos);
            RETURN_FALSE;
        }

        /* Execute the LINSERT command using the Glide client */
        long result_value;
        if (execute_list_insert_command(redis->glide_client, key, key_len,
                                        upper_pos, strlen(upper_pos),
                                        pivot, pivot_len, val, val_len,
                                        &result_value))
        {

            /* Clean up */
            if (upper_pos)
                efree(upper_pos);

            /* Return the value */
            RETURN_LONG(result_value);
        }
        else
        {
            if (upper_pos)
                efree(upper_pos);
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto string Redis::lPos(string key, mixed value, [array options = null]) */
PHP_METHOD(Redis, lPos)
{
    zval *object, *z_value, *z_opts = NULL;
    redis_object *redis;
    char *key = NULL, *val = NULL;
    size_t key_len, val_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osz|a",
                                     &object, redis_ce, &key, &key_len,
                                     &z_value, &z_opts) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Convert value to string if needed */
        int val_free = 0;

        switch (Z_TYPE_P(z_value))
        {
        case IS_STRING:
            val = Z_STRVAL_P(z_value);
            val_len = Z_STRLEN_P(z_value);
            break;
        default:
            printf("This function is from the original PHP redis\n");
            RETURN_FALSE; // val_free = redis_serialize(redis, z_value, &val, &val_len); //TODO
            break;
        }

        /* Execute the LPOS command using the Glide client */
        if (execute_list_position_command(redis->glide_client, key, key_len,
                                          val, val_len, z_opts, return_value))
        {
            /* Free allocated memory */

            if (val_free)
                efree(val);
            /* Return value already set in execute_lpos_command */
            return;
        }
        else
        {
            /* Free allocated memory */

            if (val_free)
                efree(val);
            RETURN_FALSE;
        }
    }
}

/* }}} */

/* {{{ proto int Redis::lLen(string key) */
PHP_METHOD(Redis, lLen)
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

        /* Execute the LLEN command using the Glide client */
        long result_value;
        if (execute_list_len_command(redis->glide_client, key, key_len, &result_value))
        {

            /* Return the value */
            RETURN_LONG(result_value);
        }
        else
        {
            /* Free allocated memory */

            RETURN_FALSE;
        }
    }
}
/* }}} */
