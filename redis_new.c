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
#include "redis_sentinel.h"
#include "redis_glide.h"
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

PHP_METHOD(Redis, bitop)
{
    zval *object;
    redis_object *redis;
    char *op = NULL, *key = NULL;
    size_t op_len, key_len;
    zval *keys = NULL;
    int keys_count = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss*",
                                     &object, redis_ce, &op, &op_len,
                                     &key, &key_len, &keys, &keys_count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the BITOP command using the Glide client */
        long result = execute_bitop_command(redis->glide_client, op, op_len, key, key_len, keys, keys_count);

        /* If the result is -1, there was an error */
        if (result == -1)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_LONG(result);
    }
}

/* }}} */

/* {{{ proto long Redis::getBit(string key, long offset) */
PHP_METHOD(Redis, getBit)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long offset;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osl",
                                     &object, redis_ce, &key, &key_len,
                                     &offset) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the GETBIT command using the Glide client */
        long result = execute_getbit_command(redis->glide_client, key, key_len, offset);

        /* If the result is -1, there was an error */
        if (result == -1)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_LONG(result);
    }
}
/* }}} */

/* {{{ proto long Redis::setBit(string key, long offset, int value) */
PHP_METHOD(Redis, setBit)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long offset;
    zend_bool value;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oslb",
                                     &object, redis_ce, &key, &key_len,
                                     &offset, &value) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SETBIT command using the Glide client */
        long result = execute_setbit_command(redis->glide_client, key, key_len, offset, value ? 1 : 0);

        /* If the result is -1, there was an error */
        if (result == -1)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_LONG(result);
    }
}
/* }}} */

/* {{{ proto long Redis::del(string key, ...) */
PHP_METHOD(Redis, del)
{
    zval *object;
    redis_object *redis;
    zval *keys = NULL;
    int keys_count = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O*",
                                     &object, redis_ce, &keys, &keys_count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the DEL command using the Glide client */
        long result = execute_del_command(redis->glide_client, keys, keys_count);

        /* If the result is -1, there was an error */
        if (result == -1)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_LONG(result);
    }
}
/* }}} */

/* {{{ proto long Redis::bitcount(string key, [int start], [int end])
 */
PHP_METHOD(Redis, bitcount)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long start = 0, end = -1;
    zend_bool bybit = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|llb",
                                     &object, redis_ce, &key, &key_len,
                                     &start, &end, &bybit) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the BITCOUNT command using the Glide client */
        long result = execute_bitcount_command(redis->glide_client, key, key_len, start, end, bybit);

        /* If the result is -1, there was an error */
        if (result == -1)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_LONG(result);
    }
}
/* }}} */

/* {{{ proto integer Redis::bitpos(string key, int bit, [int start, int end]) */
PHP_METHOD(Redis, bitpos)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long bit, start = 0, end = -1;
    zend_bool bybit = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osl|llb",
                                     &object, redis_ce, &key, &key_len, &bit,
                                     &start, &end, &bybit) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the BITPOS command using the Glide client */
        long result = execute_bitpos_command(redis->glide_client, key, key_len, bit, start, end, bybit);

        /* If the result is -1, there was an error */
        if (result == -1)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_LONG(result);
    }
}
/* }}} */

/* }}} */

/* {{{ proto boolean Redis::set(string key, mixed val, long timeout,
 *                              [array opt) */
PHP_METHOD(Redis, set)
{
    zval *object, *z_value, *z_opts = NULL;
    redis_object *redis;
    char *key = NULL, *val = NULL;
    size_t key_len, val_len;
    zend_long expire = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osz|la",
                                     &object, redis_ce, &key, &key_len,
                                     &z_value, &expire, &z_opts) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Convert value to string if needed */
        val = Z_STRVAL_P(z_value);
        val_len = Z_STRLEN_P(z_value);

        /* Execute the SET command using the Glide client */
        int result = execute_set_command(redis->glide_client, key, key_len, val, val_len, expire, z_opts);

        /* Process the result */
        switch (result)
        {
        case 1: /* Success */
            RETURN_TRUE;
        case 0: /* Not set (NX/XX condition not met) */
            RETURN_FALSE;
        case 2: /* GET option returned a value */
            /* This case is not fully handled yet, would need to return the value */
            RETURN_TRUE;
        default: /* Error */
            RETURN_FALSE;
        }
    }
}

/* {{{ proto boolean Redis::setex(string key, long expire, string value)
 */
PHP_METHOD(Redis, setex)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *val = NULL;
    size_t key_len, val_len;
    zend_long expire;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osls",
                                     &object, redis_ce, &key, &key_len,
                                     &expire, &val, &val_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SETEX command using the Glide client */
        int result = execute_setex_command(redis->glide_client, key, key_len, expire, val, val_len);

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

/* {{{ proto boolean Redis::psetex(string key, long expire, string value)
 */
PHP_METHOD(Redis, psetex)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *val = NULL;
    size_t key_len, val_len;
    zend_long expire;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osls",
                                     &object, redis_ce, &key, &key_len,
                                     &expire, &val, &val_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the PSETEX command using the Glide client */
        int result = execute_psetex_command(redis->glide_client, key, key_len, expire, val, val_len);

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

/* {{{ proto boolean Redis::setnx(string key, string value)
 */
PHP_METHOD(Redis, setnx)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *val = NULL;
    size_t key_len, val_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &key, &key_len,
                                     &val, &val_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SETNX command using the Glide client */
        int result = execute_setnx_command(redis->glide_client, key, key_len, val, val_len);

        /* Return TRUE if key was set (result == 1), FALSE otherwise */
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

/* {{{ proto string Redis::getSet(string key, string value)
 */
PHP_METHOD(Redis, getset)
{
    zval *object, *z_value;
    redis_object *redis;
    char *key = NULL, *val = NULL;
    size_t key_len, val_len;
    char *response = NULL;
    size_t response_len = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &key, &key_len,
                                     &val, &val_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the GETSET command using the Glide client */
        int result = execute_getset_command(redis->glide_client, key, key_len, val, val_len, &response, &response_len);

        /* Process the result */
        if (result == 1 && response != NULL)
        {
            /* Return the old value */
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

/* {{{ proto string Redis::get(string key) */
PHP_METHOD(Redis, get)
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
        /* Execute the GET command using the Glide client */
        int result = execute_get_command(redis->glide_client, key, key_len, &response, &response_len);

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
    else
    {
        /* Fall back to the original implementation */
        REDIS_PROCESS_KW_CMD("GET", redis_key_cmd, redis_string_response);
    }
}
/* }}} */

/* {{{ proto string Redis::randomKey()
 */
PHP_METHOD(Redis, randomKey)
{
    zval *object;
    redis_object *redis;
    char *response = NULL;
    size_t response_len = 0;

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
        /* Execute the RANDOMKEY command using the Glide client */
        int result = execute_randomkey_command(redis->glide_client, &response, &response_len);

        /* Process the result */
        if (result == 1 && response != NULL)
        {
            /* Return the random key */
            RETVAL_STRINGL(response, response_len);
            free(response);
            return;
        }
        else if (result == 0)
        {
            /* No keys in the database */
            RETURN_NULL();
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }
}

/* {{{ proto mixed Redis::lcs(string $key1, string $key2, ?array $options = NULL); */
PHP_METHOD(Redis, lcs)
{
    zval *object;
    redis_object *redis;
    char *key1 = NULL, *key2 = NULL;
    size_t key1_len, key2_len;
    zval *options = NULL;
    zval result;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss|a",
                                     &object, redis_ce, &key1, &key1_len,
                                     &key2, &key2_len, &options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Initialize result zval */
        ZVAL_NULL(&result);

        /* Execute the LCS command using the Glide client */
        int ret = execute_lcs_command(redis->glide_client, key1, key1_len, key2, key2_len, options, &result);

        /* If the result is -1, there was an error */
        if (ret == -1)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_ZVAL(&result, 0, 1);
    }
    else
    {
        /* Fall back to the original implementation */
        REDIS_PROCESS_CMD(lcs, redis_read_variant_reply);
    }
}
/* }}} */

/* {{{ proto string Redis::setRange(string key, long start, string value) */
PHP_METHOD(Redis, setRange)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *val = NULL;
    size_t key_len, val_len;
    zend_long offset;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osls",
                                     &object, redis_ce, &key, &key_len,
                                     &offset, &val, &val_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SETRANGE command using the Glide client */
        long result = execute_setrange_command(redis->glide_client, key, key_len, offset, val, val_len);

        /* If the result is -1, there was an error */
        if (result == -1)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_LONG(result);
    }
    else
    {
        /* Fall back to the original implementation */
        REDIS_PROCESS_KW_CMD("SETRANGE", redis_key_long_str_cmd, redis_long_response);
    }
}
/* }}} */

/* {{{ proto long Redis::strlen(string key) */
PHP_METHOD(Redis, strlen)
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
        /* Execute the STRLEN command using the Glide client */
        long result = execute_strlen_command(redis->glide_client, key, key_len);

        /* If the result is -1, there was an error */
        if (result == -1)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_LONG(result);
    }
    else
    {
        /* Fall back to the original implementation */
        REDIS_PROCESS_KW_CMD("STRLEN", redis_key_cmd, redis_long_response);
    }
}

/* {{{ proto Redis|array|false Redis::lmpop(array $keys, string $from, int $count = 1) */
PHP_METHOD(Redis, lmpop)
{
    zval *object;
    redis_object *redis;
    zval *keys = NULL;
    char *from = NULL;
    size_t from_len;
    zend_long count = 1;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oas|l",
                                     &object, redis_ce, &keys, &from, &from_len,
                                     &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the LMPOP command using the Glide client */
        zval result;
        ZVAL_NULL(&result);

        int ret = execute_mpop_command(redis->glide_client, "LMPOP", 0.0, keys, from, from_len, count, &result);

        /* If the result is -1, there was an error */
        if (ret == -1)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_ZVAL(&result, 0, 1);
    }
    else
    {
        /* Fall back to the original implementation */
        REDIS_PROCESS_KW_CMD("LMPOP", redis_mpop_cmd, redis_mpop_response);
    }
}
/* }}} */

/* {{{ proto Redis|array|false Redis::blmpop(double $timeout, array $keys, string $from, int $count = 1) */
PHP_METHOD(Redis, blmpop)
{
    zval *object;
    redis_object *redis;
    zval *keys = NULL;
    char *from = NULL;
    size_t from_len;
    double timeout;
    zend_long count = 1;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Odas|l",
                                     &object, redis_ce, &timeout, &keys, &from, &from_len,
                                     &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the BLMPOP command using the Glide client */
        zval result;
        ZVAL_NULL(&result);

        int ret = execute_mpop_command(redis->glide_client, "BLMPOP", timeout, keys, from, from_len, count, &result);

        /* If the result is -1, there was an error */
        if (ret == -1)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_ZVAL(&result, 0, 1);
    }
    else
    {
        /* Fall back to the original implementation */
        REDIS_PROCESS_KW_CMD("BLMPOP", redis_mpop_cmd, redis_mpop_response);
    }
}
/* }}} */

/* {{{ proto Redis|array|false Redis::zmpop(array $keys, string $from, int $count = 1) */
PHP_METHOD(Redis, zmpop)
{
    zval *object;
    redis_object *redis;
    zval *keys = NULL;
    char *from = NULL;
    size_t from_len;
    zend_long count = 1;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oas|l",
                                     &object, redis_ce, &keys, &from, &from_len,
                                     &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ZMPOP command using the Glide client */
        zval result;
        ZVAL_NULL(&result);

        int ret = execute_mpop_command(redis->glide_client, "ZMPOP", 0.0, keys, from, from_len, count, &result);

        /* If the result is -1, there was an error */
        if (ret == -1)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_ZVAL(&result, 0, 1);
    }
    else
    {
        /* Fall back to the original implementation */
        REDIS_PROCESS_KW_CMD("ZMPOP", redis_mpop_cmd, redis_mpop_response);
    }
}
/* }}} */

/* {{{ proto boolean Redis::rPush(string key, string value)
 */
PHP_METHOD(Redis, rPush)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *z_args;
    int argc;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os*",
                                     &object, redis_ce, &key, &key_len,
                                     &z_args, &argc) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the RPUSH command using the Glide client */
        long result = execute_rpush_command(redis->glide_client, key, key_len, z_args, argc);

        /* If the result is -1, there was an error */
        if (result == -1)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_LONG(result);
    }
    else
    {
        /* Fall back to the original implementation */
        REDIS_PROCESS_KW_CMD("RPUSH", redis_key_varval_cmd, redis_long_response);
    }
}
/* }}} */
