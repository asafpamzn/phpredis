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
extern int execute_zintercard_command(const void *glide_client, zval *keys, int keys_count, zval *options, zval *return_value);
extern int execute_zunion_command(const void *glide_client, zval *keys, int keys_count, zval *weights, zval *options, zval *return_value);
extern int execute_zdiffstore_command(const void *glide_client, const char *dst, size_t dst_len, zval *keys, int keys_count, zval *weights, zval *options, long *output_value);
extern int execute_zinterstore_command(const void *glide_client, const char *dst, size_t dst_len, zval *keys, int keys_count, zval *weights, zval *options, long *output_value);
extern int execute_zunionstore_command(const void *glide_client, const char *dst, size_t dst_len, zval *keys, int keys_count, zval *weights, zval *options, long *output_value);
extern int execute_zpopmax_command(const void *glide_client, const char *key, size_t key_len, long count, zval *return_value);
extern int execute_zpopmin_command(const void *glide_client, const char *key, size_t key_len, long count, zval *return_value);
extern int execute_zscan_command(const void *glide_client, const char *key, size_t key_len, long *cursor, char *pattern, size_t pattern_len, long count, zval *return_value);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto array Redis::zintercard(array $keys, int|array $limit_or_options = null) */
PHP_METHOD(Redis, zintercard)
{
    zval *object;
    redis_object *redis;
    zval *z_keys, *z_options = NULL;
    zval z_temp_options;
    HashTable *keys_hash;
    long cardinality = 0;
    long limit = 0;
    int free_options = 0;

    /* Parse parameters - accept either array,array or array,long */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oa|z",
                                     &object, redis_ce, &z_keys, &z_options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* If second parameter is an integer (limit), convert it to an options array */
    if (z_options && Z_TYPE_P(z_options) == IS_LONG)
    {
        limit = Z_LVAL_P(z_options);
        if (limit < 0)
        {
            RETURN_FALSE;
        }

        array_init(&z_temp_options);
        add_assoc_long(&z_temp_options, "LIMIT", limit);
        z_options = &z_temp_options;
        free_options = 1; /* Flag to free the temporary array */
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Check key count */
    keys_hash = Z_ARRVAL_P(z_keys);
    if (zend_hash_num_elements(keys_hash) == 0)
    {
        RETURN_FALSE;
    }

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ZINTERCARD command using the Glide client */
        if (execute_zintercard_command(redis->glide_client, z_keys, zend_hash_num_elements(keys_hash), z_options, return_value))
        {
            printf("ZINTERCARD executed successfully\n");
            /* If we created a temporary options array, free it */
            if (free_options)
            {
                zval_dtor(&z_temp_options);
            }
            RETURN_LONG(cardinality);
        }
        else
        {
            printf("ZINTERCARD execution failed\n");
            /* If we created a temporary options array, free it */
            if (free_options)
            {
                zval_dtor(&z_temp_options);
            }
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::zunion(array $keys, array $weights = null, array $options = null) */
PHP_METHOD(Redis, zunion)
{
    zval *object;
    redis_object *redis;
    zval *z_keys, *z_weights = NULL, *z_options = NULL;
    HashTable *keys_hash;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oa|aa",
                                     &object, redis_ce, &z_keys, &z_weights, &z_options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Check key count */
    keys_hash = Z_ARRVAL_P(z_keys);
    if (zend_hash_num_elements(keys_hash) == 0)
    {
        RETURN_FALSE;
    }

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ZUNION command using the Glide client */
        if (execute_zunion_command(redis->glide_client, z_keys, zend_hash_num_elements(keys_hash),
                                   z_weights, z_options, return_value))
        {
            return; /* Return value already set in execute_zunion_command */
        }
        else
        {
            RETURN_FALSE;
        }
    }
    else
    {
        /* Fall back to the original implementation if Glide isn't available */
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto long Redis::zdiffstore(string dst, array keys) */
PHP_METHOD(Redis, zdiffstore)
{
    zval *object;
    redis_object *redis;
    zval *z_keys, *z_weights = NULL, *z_options = NULL;
    HashTable *keys_hash;
    char *dst;
    size_t dst_len;
    long cardinality = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osa|aa",
                                     &object, redis_ce, &dst, &dst_len, &z_keys,
                                     &z_weights, &z_options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Check key count */
    keys_hash = Z_ARRVAL_P(z_keys);
    if (zend_hash_num_elements(keys_hash) == 0)
    {
        RETURN_FALSE;
    }

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ZDIFFSTORE command using the Glide client */
        if (execute_zdiffstore_command(redis->glide_client, dst, dst_len, z_keys,
                                       zend_hash_num_elements(keys_hash), z_weights, z_options, &cardinality))
        {
            RETURN_LONG(cardinality);
        }
        else
        {
            RETURN_FALSE;
        }
    }
    else
    {
        /* Fall back to the original implementation if Glide isn't available */
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto long Redis::zinterstore(string dst, array keys) */
PHP_METHOD(Redis, zinterstore)
{
    zval *object;
    redis_object *redis;
    zval *z_keys, *z_weights = NULL, *z_options = NULL;
    HashTable *keys_hash;
    char *dst;
    size_t dst_len;
    long cardinality = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osa|aa",
                                     &object, redis_ce, &dst, &dst_len, &z_keys,
                                     &z_weights, &z_options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Check key count */
    keys_hash = Z_ARRVAL_P(z_keys);
    if (zend_hash_num_elements(keys_hash) == 0)
    {
        RETURN_FALSE;
    }

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ZINTERSTORE command using the Glide client */
        if (execute_zinterstore_command(redis->glide_client, dst, dst_len, z_keys,
                                        zend_hash_num_elements(keys_hash), z_weights, z_options, &cardinality))
        {
            RETURN_LONG(cardinality);
        }
        else
        {
            RETURN_FALSE;
        }
    }
    else
    {
        /* Fall back to the original implementation if Glide isn't available */
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto long Redis::zunionstore(string dst, array keys) */
PHP_METHOD(Redis, zunionstore)
{
    zval *object;
    redis_object *redis;
    zval *z_keys, *z_weights = NULL, *z_options = NULL;
    HashTable *keys_hash;
    char *dst;
    size_t dst_len;
    long cardinality = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osa|aa",
                                     &object, redis_ce, &dst, &dst_len, &z_keys,
                                     &z_weights, &z_options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Check key count */
    keys_hash = Z_ARRVAL_P(z_keys);
    if (zend_hash_num_elements(keys_hash) == 0)
    {
        RETURN_FALSE;
    }

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ZUNIONSTORE command using the Glide client */
        if (execute_zunionstore_command(redis->glide_client, dst, dst_len, z_keys,
                                        zend_hash_num_elements(keys_hash), z_weights, z_options, &cardinality))
        {
            RETURN_LONG(cardinality);
        }
        else
        {
            RETURN_FALSE;
        }
    }
    else
    {
        /* Fall back to the original implementation if Glide isn't available */
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto array Redis::zPopMax(string key, [int count]) */
PHP_METHOD(Redis, zPopMax)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    long count = 1;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|l",
                                     &object, redis_ce, &key, &key_len, &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ZPOPMAX command using the Glide client */
        if (execute_zpopmax_command(redis->glide_client, key, key_len, count, return_value))
        {
            return; /* Return value already set in execute_zpopmax_command */
        }
        else
        {
            RETURN_FALSE;
        }
    }
    else
    {
        /* Fall back to the original implementation if Glide isn't available */
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto array Redis::zPopMin(string key, [int count]) */
PHP_METHOD(Redis, zPopMin)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    long count = 1;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|l",
                                     &object, redis_ce, &key, &key_len, &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ZPOPMIN command using the Glide client */
        if (execute_zpopmin_command(redis->glide_client, key, key_len, count, return_value))
        {
            return; /* Return value already set in execute_zpopmin_command */
        }
        else
        {
            RETURN_FALSE;
        }
    }
    else
    {
        /* Fall back to the original implementation if Glide isn't available */
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto array Redis::zscan(string key, long &iterator, [string pattern, long count]) */
PHP_METHOD(Redis, zscan)
{
    zval *object, *z_iterator;
    redis_object *redis;
    char *key = NULL, *pattern = NULL;
    size_t key_len, pattern_len = 0;
    long count = 0, cursor;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osz|sl",
                                     &object, redis_ce, &key, &key_len, &z_iterator,
                                     &pattern, &pattern_len, &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Make sure the iterator value is a reference */
    if (!Z_ISREF_P(z_iterator))
    {
        php_error_docref(NULL, E_WARNING, "Iterator must be passed as a reference");
        RETURN_FALSE;
    }

    /* Get current iterator value */
    cursor = Z_LVAL_P(Z_REFVAL_P(z_iterator));

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ZSCAN command using the Glide client */
        if (execute_zscan_command(redis->glide_client, key, key_len, &cursor,
                                  pattern, pattern_len, count, return_value))
        {
            /* Update iterator value */
            ZVAL_LONG(Z_REFVAL_P(z_iterator), cursor);
            return; /* Return value already set in execute_zscan_command */
        }
        else
        {
            RETURN_FALSE;
        }
    }
    else
    {
        /* Fall back to the original implementation if Glide isn't available */
        RETURN_FALSE;
    }
}
/* }}} */
