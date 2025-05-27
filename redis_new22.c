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
extern int execute_select_command(const void *glide_client, long database);
extern int execute_swapdb_command(const void *glide_client, long db1, long db2);
extern int execute_move_command(const void *glide_client, const char *key, size_t key_len, long db, int *output_value);
extern int execute_bzpopmax_command(const void *glide_client, zval *keys, int keys_count,
                                    double timeout, zval *return_value);
extern int execute_bzpopmin_command(const void *glide_client, zval *keys, int keys_count,
                                    double timeout, zval *return_value);

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

/* {{{ proto array Redis::bzPopMax(string|array key [, string otherkeys, ...,], float timeout) */
PHP_METHOD(Redis, bzPopMax)
{
    zval *object;
    redis_object *redis;
    zval *z_args = NULL, z_processed_keys;
    int argc = 0;
    double timeout = 0.0;
    zend_bool is_array_arg = 0;

    /* Try the variadic format first */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O+d",
                                     &object, redis_ce, &z_args, &argc, &timeout) == SUCCESS)
    {
        /* Need at least one key */
        if (argc < 1)
        {
            RETURN_FALSE;
        }

        /* Get Redis object */
        redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
        is_array_arg = 0;
    }
    /* Try array format if variadic format fails */
    else
    {
        zval *z_keys = NULL, *z_timeout = NULL;
        HashTable *ht_keys = NULL;

        /* Parse as array + timeout */
        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ozz",
                                         &object, redis_ce, &z_keys, &z_timeout) == FAILURE)
        {
            RETURN_FALSE;
        }

        /* Make sure first argument is an array */
        if (Z_TYPE_P(z_keys) != IS_ARRAY)
        {
            php_error_docref(NULL, E_WARNING, "First argument should be an array if using two-argument form");
            RETURN_FALSE;
        }

        ht_keys = Z_ARRVAL_P(z_keys);

        /* Extract timeout */
        if (Z_TYPE_P(z_timeout) == IS_LONG)
        {
            timeout = (double)Z_LVAL_P(z_timeout);
        }
        else if (Z_TYPE_P(z_timeout) == IS_DOUBLE)
        {
            timeout = Z_DVAL_P(z_timeout);
        }
        else
        {
            php_error_docref(NULL, E_WARNING, "Timeout must be a numeric value");
            RETURN_FALSE;
        }

        /* Create a new array for processed keys */
        array_init(&z_processed_keys);

        /* Copy all keys to the new array */
        zval *key_entry;
        ZEND_HASH_FOREACH_VAL(ht_keys, key_entry)
        {
            if (Z_TYPE_P(key_entry) != IS_STRING)
            {
                /* Convert to string if possible */
                zval tmp;
                ZVAL_COPY(&tmp, key_entry);
                convert_to_string(&tmp);
                add_next_index_zval(&z_processed_keys, &tmp);
            }
            else
            {
                /* Add as-is if already string */
                zval tmp;
                ZVAL_COPY(&tmp, key_entry);
                add_next_index_zval(&z_processed_keys, &tmp);
            }
        }
        ZEND_HASH_FOREACH_END();

        /* Update arguments count */
        argc = zend_hash_num_elements(Z_ARRVAL(z_processed_keys));
        z_args = NULL;

        /* Get Redis object */
        redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
        is_array_arg = 1;
    }

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the BZPOPMAX command using the Glide client */
        int success;

        /* Handle different argument formats */
        if (is_array_arg)
        {
            /* We need to extract the keys from the array into a format that the execute function expects */
            int i = 0;
            zval *temp_args = emalloc(sizeof(zval) * argc);

            /* Iterate through the hash table and extract the values */
            HashTable *ht = Z_ARRVAL(z_processed_keys);
            zval *entry;

            ZEND_HASH_FOREACH_VAL(ht, entry)
            {
                /* Copy each array element to our temp array */
                ZVAL_COPY_VALUE(&temp_args[i], entry);
                i++;
            }
            ZEND_HASH_FOREACH_END();

            /* Call the execution function with the properly formatted array */
            success = execute_bzpopmax_command(redis->glide_client, temp_args, argc, timeout, return_value);

            /* Clean up our temporary array */
            efree(temp_args);
        }
        else
        {
            success = execute_bzpopmax_command(redis->glide_client, z_args, argc, timeout, return_value);
        }

        /* Clean up if we created a processed keys array */
        if (is_array_arg)
        {
            zval_ptr_dtor(&z_processed_keys);
        }

        if (success)
        {
            /* Return value already set by execute_bzpopmax_command */
            return;
        }
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Redis::bzPopMin(string|array key [, string otherkeys, ...,], float timeout) */
PHP_METHOD(Redis, bzPopMin)
{
    zval *object;
    redis_object *redis;
    zval *z_args = NULL, z_processed_keys;
    int argc = 0;
    double timeout = 0.0;
    zend_bool is_array_arg = 0;

    /* Try the variadic format first */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O+d",
                                     &object, redis_ce, &z_args, &argc, &timeout) == SUCCESS)
    {
        /* Need at least one key */
        if (argc < 1)
        {
            RETURN_FALSE;
        }

        /* Get Redis object */
        redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
        is_array_arg = 0;
    }
    /* Try array format if variadic format fails */
    else
    {
        zval *z_keys = NULL, *z_timeout = NULL;
        HashTable *ht_keys = NULL;

        /* Parse as array + timeout */
        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ozz",
                                         &object, redis_ce, &z_keys, &z_timeout) == FAILURE)
        {
            RETURN_FALSE;
        }

        /* Make sure first argument is an array */
        if (Z_TYPE_P(z_keys) != IS_ARRAY)
        {
            php_error_docref(NULL, E_WARNING, "First argument should be an array if using two-argument form");
            RETURN_FALSE;
        }

        ht_keys = Z_ARRVAL_P(z_keys);

        /* Extract timeout */
        if (Z_TYPE_P(z_timeout) == IS_LONG)
        {
            timeout = (double)Z_LVAL_P(z_timeout);
        }
        else if (Z_TYPE_P(z_timeout) == IS_DOUBLE)
        {
            timeout = Z_DVAL_P(z_timeout);
        }
        else
        {
            php_error_docref(NULL, E_WARNING, "Timeout must be a numeric value");
            RETURN_FALSE;
        }

        /* Create a new array for processed keys */
        array_init(&z_processed_keys);

        /* Copy all keys to the new array */
        zval *key_entry;
        ZEND_HASH_FOREACH_VAL(ht_keys, key_entry)
        {
            if (Z_TYPE_P(key_entry) != IS_STRING)
            {
                /* Convert to string if possible */
                zval tmp;
                ZVAL_COPY(&tmp, key_entry);
                convert_to_string(&tmp);
                add_next_index_zval(&z_processed_keys, &tmp);
            }
            else
            {
                /* Add as-is if already string */
                zval tmp;
                ZVAL_COPY(&tmp, key_entry);
                add_next_index_zval(&z_processed_keys, &tmp);
            }
        }
        ZEND_HASH_FOREACH_END();

        /* Update arguments count */
        argc = zend_hash_num_elements(Z_ARRVAL(z_processed_keys));
        z_args = NULL;

        /* Get Redis object */
        redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
        is_array_arg = 1;
    }

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the BZPOPMIN command using the Glide client */
        int success;

        /* Handle different argument formats */
        if (is_array_arg)
        {
            /* We need to extract the keys from the array into a format that the execute function expects */
            int i = 0;
            zval *temp_args = emalloc(sizeof(zval) * argc);

            /* Iterate through the hash table and extract the values */
            HashTable *ht = Z_ARRVAL(z_processed_keys);
            zval *entry;

            ZEND_HASH_FOREACH_VAL(ht, entry)
            {
                /* Copy each array element to our temp array */
                ZVAL_COPY_VALUE(&temp_args[i], entry);
                i++;
            }
            ZEND_HASH_FOREACH_END();

            /* Call the execution function with the properly formatted array */
            success = execute_bzpopmin_command(redis->glide_client, temp_args, argc, timeout, return_value);

            /* Clean up our temporary array */
            efree(temp_args);
        }
        else
        {
            success = execute_bzpopmin_command(redis->glide_client, z_args, argc, timeout, return_value);
        }

        /* Clean up if we created a processed keys array */
        if (is_array_arg)
        {
            zval_ptr_dtor(&z_processed_keys);
        }

        if (success)
        {
            /* Return value already set by execute_bzpopmin_command */
            return;
        }
    }

    RETURN_FALSE;
}
/* }}} */
