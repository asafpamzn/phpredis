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
extern int execute_copy_command(const void *glide_client, const char *src, size_t src_len,
                                const char *dst, size_t dst_len, int replace);
extern int execute_hscan_command(const void *glide_client, const char *key, size_t key_len,
                                 long cursor, char *pattern, size_t pattern_len,
                                 long count, zval *return_value);
extern int execute_pfadd_command(const void *glide_client, const char *key, size_t key_len,
                                 zval *elements, int elements_count, int *output_value);
extern int execute_pfcount_command(const void *glide_client, zval *keys, int keys_count, long *output_value);
extern int execute_pfmerge_command(const void *glide_client, const char *dst, size_t dst_len,
                                   zval *keys, int keys_count);
extern int execute_get_timeout_command(const void *glide_client, double *output_value);
extern int execute_get_read_timeout_command(const void *glide_client, double *output_value);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto long Redis::copy(string $source, string $destination, array $options = null) */
PHP_METHOD(Redis, copy)
{
    zval *object;
    redis_object *redis;
    char *src = NULL, *dst = NULL;
    size_t src_len, dst_len;
    zend_bool replace = 0;
    zval *z_opts = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss|a",
                                     &object, redis_ce, &src, &src_len,
                                     &dst, &dst_len, &z_opts) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Check for the REPLACE option if options array was passed */
    if (z_opts && Z_TYPE_P(z_opts) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(z_opts);
        zval *replace_val;
        replace_val = zend_hash_str_find(ht, "replace", sizeof("replace") - 1);
        if (replace_val && Z_TYPE_P(replace_val) == IS_TRUE)
        {
            replace = 1;
        }
    }

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the COPY command using the Glide client */
        if (execute_copy_command(redis->glide_client, src, src_len, dst, dst_len, replace))
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

/* {{{ proto array Redis::hscan(string key, long &iterator, [string pattern, [long count]]) */
PHP_METHOD(Redis, hscan)
{
    zval *object, *z_iter;
    redis_object *redis;
    char *key = NULL, *pattern = NULL;
    size_t key_len, pattern_len = 0;
    long count = 0;

    /* Parse arguments */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osz|sl",
                                     &object, redis_ce, &key, &key_len, &z_iter,
                                     &pattern, &pattern_len, &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Make sure we have a valid cursor */
    if (Z_TYPE_P(z_iter) != IS_LONG && Z_TYPE_P(z_iter) != IS_STRING)
    {
        php_error_docref(NULL, E_WARNING, "Cursor must be numeric or string");
        RETURN_FALSE;
    }

    /* If the cursor is a string, convert it to a long */
    long cursor;
    if (Z_TYPE_P(z_iter) == IS_STRING)
    {
        cursor = atol(Z_STRVAL_P(z_iter));
    }
    else
    {
        cursor = Z_LVAL_P(z_iter);
    }

    /* Initialize return array */
    array_init(return_value);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute HSCAN command */
        if (execute_hscan_command(redis->glide_client, key, key_len, cursor,
                                  pattern, pattern_len, count, return_value))
        {
            /* Get the cursor from the first element of the returned array */
            zval *z_new_cursor = zend_hash_index_find(Z_ARRVAL_P(return_value), 0);
            if (z_new_cursor)
            {
                /* Update the passed-in cursor */
                if (Z_TYPE_P(z_new_cursor) == IS_STRING)
                {
                    ZVAL_STRINGL(z_iter, Z_STRVAL_P(z_new_cursor), Z_STRLEN_P(z_new_cursor));
                }
                else if (Z_TYPE_P(z_new_cursor) == IS_LONG)
                {
                    ZVAL_LONG(z_iter, Z_LVAL_P(z_new_cursor));
                }
            }
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::pfadd(string key, array elements) */
PHP_METHOD(Redis, pfadd)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *z_elements;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osa",
                                     &object, redis_ce, &key, &key_len,
                                     &z_elements) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the PFADD command using the Glide client */
        int result_value = 0;
        int elements_count = zend_hash_num_elements(Z_ARRVAL_P(z_elements));

        if (execute_pfadd_command(redis->glide_client, key, key_len, z_elements,
                                  elements_count, &result_value))
        {
            RETURN_LONG(result_value);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::pfcount(string key[, string key2, string key3...]) */
PHP_METHOD(Redis, pfcount)
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
        /* Execute the PFCOUNT command using the Glide client */
        long result_value;

        if (execute_pfcount_command(redis->glide_client, z_args, argc, &result_value))
        {
            RETURN_LONG(result_value);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto bool Redis::pfmerge(string dst, array keys) */
PHP_METHOD(Redis, pfmerge)
{
    zval *object;
    redis_object *redis;
    char *dst = NULL;
    size_t dst_len;
    zval *z_keys;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osa",
                                     &object, redis_ce, &dst, &dst_len,
                                     &z_keys) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the PFMERGE command using the Glide client */
        int keys_count = zend_hash_num_elements(Z_ARRVAL_P(z_keys));

        if (execute_pfmerge_command(redis->glide_client, dst, dst_len, z_keys, keys_count))
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

/* {{{ proto double Redis::getTimeout() */
PHP_METHOD(Redis, getTimeout)
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
        /* Execute the getTimeout command using the Glide client */
        double timeout;

        if (execute_get_timeout_command(redis->glide_client, &timeout))
        {
            RETURN_DOUBLE(timeout);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto double Redis::getReadTimeout() */
PHP_METHOD(Redis, getReadTimeout)
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
        /* Execute the getReadTimeout command using the Glide client */
        double timeout;

        if (execute_get_read_timeout_command(redis->glide_client, &timeout))
        {
            RETURN_DOUBLE(timeout);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */
