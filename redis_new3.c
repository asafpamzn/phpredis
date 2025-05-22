/* -*- Mode: C; tab-width: 4 -*- */
/*
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2009 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Original author: Alfonso Jimenez <yo@alfonsojimenez.com>             |
  | Maintainer: Nicolas Favre-Felix <n.favre-felix@owlient.eu>           |
  | Maintainer: Nasreddine Bouafif <n.bouafif@owlient.eu>                |
  | Maintainer: Michael Grunder <michael.grunder@gmail.com>              |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_redis.h"
#include "redis_array.h"
#include "redis_cluster.h"
#include "redis_commands.h"
#include "redis_glide.h"
#include "command_response.h" /* Include command_response.h for string conversion functions */
#include <ext/spl/spl_exceptions.h>
#include <zend_exceptions.h>
#include <ext/standard/info.h>
#include <ext/hash/php_hash.h>

#if PHP_VERSION_ID < 80400
#include <ext/standard/php_random.h>
#else
#include <ext/random/php_random.h>
#endif

#ifdef PHP_SESSION
#include <ext/session/php_session.h>
#endif

#include "library.h"

/* Import the string conversion functions from command_response.c */
extern char *long_to_string(long value, size_t *len);
extern char *double_to_string(double value, size_t *len);

#ifdef HAVE_REDIS_ZSTD
#include <zstd.h>
#endif

#ifdef HAVE_REDIS_LZ4
#include <lz4.h>
#endif

#ifdef PHP_SESSION
extern ps_module ps_mod_redis;
extern ps_module ps_mod_redis_cluster;
#endif

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

#if PHP_VERSION_ID < 80000
#include "redis_legacy_arginfo.h"
#else
#include "zend_attributes.h"
#include "redis_arginfo.h"
#endif

/* {{{ proto boolean Redis::rename(string key_src, string key_dst) */
PHP_METHOD(Redis, rename)
{
    zval *object;
    redis_object *redis;
    char *src = NULL, *dst = NULL;
    size_t src_len, dst_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &src, &src_len,
                                     &dst, &dst_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the RENAME command using the Glide client */
        int result = execute_rename_command(redis->glide_client, src, src_len, dst, dst_len);

        /* Return TRUE if successful, FALSE otherwise */
        if (result == 1)
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

/* {{{ proto boolean Redis::renameNx(string key_src, string key_dst) */
PHP_METHOD(Redis, renameNx)
{
    zval *object;
    redis_object *redis;
    char *src = NULL, *dst = NULL;
    size_t src_len, dst_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &src, &src_len,
                                     &dst, &dst_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the RENAMENX command using the Glide client */
        int result = execute_renamenx_command(redis->glide_client, src, src_len, dst, dst_len);

        /* Return TRUE if successful, FALSE otherwise */
        if (result == 1)
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

/* {{{ proto string Redis::getWithMeta(string key) */
PHP_METHOD(Redis, getWithMeta)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    char *response = NULL;
    size_t response_len = 0;

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
        /* Execute the GETWITHMETA command using the Glide client */
        int result = execute_getwithmeta_command(redis->glide_client, key, key_len, &response, &response_len);

        /* Process the result */
        if (result == 1 && response != NULL)
        {
            /* Return the value */
            RETVAL_STRINGL(response, response_len);
            free(response);
            return;
        }
        else if (result == 0)
        {
            /* Key didn't exist */
            RETURN_NULL();
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto string Redis::getDel(string key) */
PHP_METHOD(Redis, getDel)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    char *response = NULL;
    size_t response_len = 0;

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
        /* Execute the GETDEL command using the Glide client */
        int result = execute_getdel_command(redis->glide_client, key, key_len, &response, &response_len);

        /* Process the result */
        if (result == 1 && response != NULL)
        {
            /* Return the value */
            RETVAL_STRINGL(response, response_len);
            free(response);
            return;
        }
        else if (result == 0)
        {
            /* Key didn't exist */
            RETURN_NULL();
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto string Redis::getEx(string key, array opts) */
PHP_METHOD(Redis, getEx)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *opts = NULL;
    char *response = NULL;
    size_t response_len = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|a",
                                     &object, redis_ce, &key, &key_len, &opts) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the GETEX command using the Glide client */
        int result = execute_getex_command(redis->glide_client, key, key_len, opts, &response, &response_len);

        /* Process the result */
        if (result == 1 && response != NULL)
        {
            /* Return the value */
            RETVAL_STRINGL(response, response_len);
            free(response);
            return;
        }
        else if (result == 0)
        {
            /* Key didn't exist */
            RETURN_NULL();
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }
}

/* }}} */

/* {{{ proto long Redis::incr(string key, [long value]) */
PHP_METHOD(Redis, incr)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long value = 1;
    long result_value;
    int argc = ZEND_NUM_ARGS();

    /* Parse parameters */
    if (argc == 1)
    {
        /* Only key parameter provided */
        if (zend_parse_method_parameters(argc, getThis(), "Os",
                                         &object, redis_ce, &key, &key_len) == FAILURE)
        {
            RETURN_FALSE;
        }
    }
    else
    {
        /* Both key and value parameters provided */
        if (zend_parse_method_parameters(argc, getThis(), "Osl",
                                         &object, redis_ce, &key, &key_len, &value) == FAILURE)
        {
            RETURN_FALSE;
        }
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        if (argc == 1)
        {
            /* Standard INCR command if only key is provided */
            result_value = execute_incr_command(redis->glide_client, key, key_len);
        }
        else
        {
            /* Use INCRBY if both key and value are provided */
            result_value = execute_incrby_command(redis->glide_client, key, key_len, value);
        }

        /* Return the result */
        RETURN_LONG(result_value);
    }
}
/* }}} */

/* {{{ proto long Redis::incrBy(string key, long value) */
PHP_METHOD(Redis, incrBy)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long value;
    long result_value;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osl",
                                     &object, redis_ce, &key, &key_len, &value) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the INCRBY command using the Glide client */
        result_value = execute_incrby_command(redis->glide_client, key, key_len, value);

        /* Return the result */
        RETURN_LONG(result_value);
    }
}
/* }}} */

/* {{{ proto double Redis::incrByFloat(string key, double value) */
PHP_METHOD(Redis, incrByFloat)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    double value, result;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osd",
                                     &object, redis_ce, &key, &key_len, &value) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the INCRBYFLOAT command using the Glide client */
        if (execute_incrbyfloat_command(redis->glide_client, key, key_len, value, &result))
        {
            /* Return the result */
            RETURN_DOUBLE(result);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::decr(string key, [long value]) */
PHP_METHOD(Redis, decr)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long value = 1;
    long result_value;
    int argc = ZEND_NUM_ARGS();

    /* Parse parameters */
    if (argc == 1)
    {
        /* Only key parameter provided - standard DECR */
        if (zend_parse_method_parameters(argc, getThis(), "Os",
                                         &object, redis_ce, &key, &key_len) == FAILURE)
        {
            RETURN_FALSE;
        }
    }
    else
    {
        /* Both key and value parameters provided - like DECRBY */
        if (zend_parse_method_parameters(argc, getThis(), "Osl",
                                         &object, redis_ce, &key, &key_len, &value) == FAILURE)
        {
            RETURN_FALSE;
        }
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        if (argc == 1)
        {
            /* Standard DECR command if only key is provided */
            if (execute_decr_command(redis->glide_client, key, key_len, &result_value))
            {
                /* Return the result */
                RETURN_LONG(result_value);
            }
        }
        else
        {
            /* Use DECRBY if both key and value are provided */
            if (execute_decrby_command(redis->glide_client, key, key_len, value, &result_value))
            {
                /* Return the result */
                RETURN_LONG(result_value);
            }
        }

        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto long Redis::decrBy(string key, long value) */
PHP_METHOD(Redis, decrBy)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long value;
    long result_value;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osl",
                                     &object, redis_ce, &key, &key_len, &value) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the DECRBY command using the Glide client */
        if (execute_decrby_command(redis->glide_client, key, key_len, value, &result_value))
        {
            /* Return the result */
            RETURN_LONG(result_value);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::mget(array keys) */
PHP_METHOD(Redis, mget)
{
    zval *object;
    redis_object *redis;
    zval *z_args, *z_array;
    int argc;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oa/",
                                     &object, redis_ce, &z_array) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the MGET command using the Glide client */
        array_init(return_value);
        if (execute_mget_command(redis->glide_client, z_array, return_value))
        {
            /* Command succeeded, return_value is already set */
            return;
        }
        else
        {
            /* Command failed */
            zval_dtor(return_value);
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::exists(string key | array keys) */
PHP_METHOD(Redis, exists)
{
    zval *object;
    redis_object *redis;
    zval *z_args;
    int argc;
    long result_value;

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
        /* Check if we received an array as a single argument */
        if (argc == 1 && Z_TYPE_P(z_args) == IS_ARRAY)
        {
            /* Extract keys from the array */
            HashTable *ht = Z_ARRVAL_P(z_args);
            int num_keys = zend_hash_num_elements(ht);

            if (num_keys == 0)
            {
                /* Empty array, return 0 */
                RETURN_LONG(0);
            }

            /* Allocate memory for keys */
            zval *keys = ecalloc(num_keys, sizeof(zval));
            if (!keys)
            {
                RETURN_FALSE;
            }

            /* Copy each array value to our keys array */
            zval *entry;
            int i = 0;

            ZEND_HASH_FOREACH_VAL(ht, entry)
            {
                /* Convert any non-string values to string */
                if (Z_TYPE_P(entry) != IS_STRING)
                {
                    zval tmp;
                    ZVAL_DUP(&tmp, entry);
                    convert_to_string(&tmp);
                    ZVAL_COPY_VALUE(&keys[i], &tmp);
                }
                else
                {
                    ZVAL_COPY(&keys[i], entry);
                }
                i++;
            }
            ZEND_HASH_FOREACH_END();

            /* Execute the EXISTS command with the array elements as keys */
            if (execute_exists_command(redis->glide_client, keys, num_keys, &result_value))
            {
                /* Free the keys array */
                for (i = 0; i < num_keys; i++)
                {
                    zval_ptr_dtor(&keys[i]);
                }
                efree(keys);

                /* Command succeeded, return the value */
                RETURN_LONG(result_value);
            }
            else
            {
                /* Free the keys array */
                for (i = 0; i < num_keys; i++)
                {
                    zval_ptr_dtor(&keys[i]);
                }
                efree(keys);

                /* Command failed */
                RETURN_FALSE;
            }
        }
        else
        {
            /* Normal case - one or more arguments directly passed */
            /* Execute the EXISTS command using the Glide client */
            if (execute_exists_command(redis->glide_client, z_args, argc, &result_value))
            {
                /* Command succeeded, return the value */
                RETURN_LONG(result_value);
            }
            else
            {
                /* Command failed */
                RETURN_FALSE;
            }
        }
    }
}
/* }}} */

/* {{{ proto long Redis::touch(string key | array keys) */
PHP_METHOD(Redis, touch)
{
    zval *object;
    redis_object *redis;
    zval *z_args;
    int argc;
    long result_value;

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
        /* Execute the TOUCH command using the Glide client */
        if (execute_touch_command(redis->glide_client, z_args, argc, &result_value))
        {
            /* Command succeeded, return the value */
            RETURN_LONG(result_value);
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::unlink(string key | array keys) */
PHP_METHOD(Redis, unlink)
{
    zval *object;
    redis_object *redis;
    zval *z_args;
    int argc;
    long result_value;

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
        /* Check if we have a single array argument */
        if (argc == 1 && Z_TYPE(z_args[0]) == IS_ARRAY)
        {
            /* Use array elements as keys */
            long result_value = 0;
            if (execute_unlink_array(redis->glide_client, Z_ARRVAL(z_args[0]), &result_value))
            {
                /* Command succeeded, return the value */
                RETURN_LONG(result_value);
            }
        }
        else
        {
            /* Multiple arguments - use standard unlink command */
            long result_value = 0;
            if (execute_unlink_command(redis->glide_client, z_args, argc, &result_value))
            {
                /* Command succeeded, return the value */
                RETURN_LONG(result_value);
            }
        }

        /* If we reach here, the command failed */
        RETURN_FALSE;
    }
}
/* }}} */
