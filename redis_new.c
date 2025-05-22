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

/* {{{ proto string Redis::echo(string msg) */
PHP_METHOD(Redis, echo)
{
    zval *object;
    redis_object *redis;
    char *msg = NULL;
    size_t msg_len;
    char *response = NULL;
    size_t response_len = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os",
                                     &object, redis_ce, &msg, &msg_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the ECHO command using the Glide client */
        int result = execute_echo_command(redis->glide_client, msg, msg_len, &response, &response_len);
        /* Process the result */
        if (result == 1 && response != NULL)
        {
            /* Return the echoed message */
            RETVAL_STRINGL(response, response_len);
            free(response);
            return;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }
}
/* }}} */

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
        long result_value;
        if (execute_bitop_command(redis->glide_client, op, op_len, key, key_len, keys, keys_count, &result_value))
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
        long result_value;
        if (execute_getbit_command(redis->glide_client, key, key_len, offset, &result_value))
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
        long result_value;
        if (execute_setbit_command(redis->glide_client, key, key_len, offset, value ? 1 : 0, &result_value))
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

/* {{{ proto long Redis::del(string key, ...) or Redis::del(array keys) */
PHP_METHOD(Redis, del)
{
    zval *object;
    redis_object *redis;
    zval *args = NULL;
    int argc = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O*",
                                     &object, redis_ce, &args, &argc) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Check if we have a single array argument */
        if (argc == 1 && Z_TYPE(args[0]) == IS_ARRAY)
        {
            /* Use array elements as keys */
            long result_value = 0;
            if (execute_del_array(redis->glide_client, Z_ARRVAL(args[0]), &result_value))
            {
                /* Command succeeded, return the value */
                RETURN_LONG(result_value);
            }
        }
        else
        {
            /* Multiple arguments - use standard del command */
            long result_value = 0;
            if (execute_del_command(redis->glide_client, args, argc, &result_value))
            {
                /* Command succeeded, return the value */
                RETURN_LONG(result_value);
            }
        }
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
        long result_value;
        if (execute_bitcount_command(redis->glide_client, key, key_len, start, end, bybit, &result_value))
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

/* {{{ proto integer Redis::bitpos(string key, int bit, [int start, int end]) */
PHP_METHOD(Redis, bitpos)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long bit, start = 0, end = -1;
    zend_bool bybit = 0;
    long result_value;

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
        /* Execute the BITPOS command using the Glide client with new output parameter pattern */
        if (execute_bitpos_command(redis->glide_client, key, key_len, bit, start, end, bybit, &result_value))
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

    /* If no Glide client, pass through to original implementation */
    REDIS_PROCESS_CMD(bitpos, redis_long_response);
}
/* }}} */

/* }}} */

/* {{{ proto boolean Redis::set(string key, mixed val, double|int|array timeout,
 *                              [array opt) */
PHP_METHOD(Redis, set)
{
    zval *object, *z_value, *z_expire = NULL, *z_opts = NULL;
    redis_object *redis;
    char *key = NULL, *val = NULL;
    size_t key_len, val_len;
    double expire = 0;
    zend_long expire_int = 0;
    zval *z_set_opts = NULL; /* Will hold our options either from z_expire or z_opts */
    zval *z_ifeq_val = NULL; /* Special holder for IFEQ value */
    int has_get_opt = 0;     /* Flag to indicate if GET option is present */
    char *old_val = NULL;    /* For storing GET response */
    size_t old_val_len = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osz|za",
                                     &object, redis_ce, &key, &key_len,
                                     &z_value, &z_expire, &z_opts) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Check if expire parameter was provided */
    if (z_expire != NULL)
    {
        switch (Z_TYPE_P(z_expire))
        {
        case IS_DOUBLE:
            /* Double - use as timeout */
            expire = Z_DVAL_P(z_expire);
            expire_int = (zend_long)expire;
            break;
        case IS_LONG:
            /* Long - use as timeout */
            expire = (double)Z_LVAL_P(z_expire);
            expire_int = Z_LVAL_P(z_expire);
            break;
        case IS_ARRAY:
            /* Array - use as options */
            z_set_opts = z_expire;
            break;
        case IS_NULL:
            /* NULL - ignore */
            break;
        default:
            /* Not a supported type - return false */
            RETURN_FALSE;
        }
    }

    /* If options were passed in z_opts, use those instead */
    if (z_opts != NULL && Z_TYPE_P(z_opts) == IS_ARRAY)
    {
        z_set_opts = z_opts;
    }
    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        char *val = NULL;
        size_t val_len = 0;
        int free_val = 0; // Flag to track if we need to free val

        /* Convert value based on its type */
        switch (Z_TYPE_P(z_value))
        {
        case IS_STRING:
            /* It's already a string, use directly */
            val = Z_STRVAL_P(z_value);
            val_len = Z_STRLEN_P(z_value);
            break;
        case IS_LONG:
            /* Convert integer to string */
            val = long_to_string(Z_LVAL_P(z_value), &val_len);
            free_val = 1; // We'll need to free this
            break;
        case IS_DOUBLE:
            /* Convert float to string */
            val = double_to_string(Z_DVAL_P(z_value), &val_len);
            free_val = 1; // We'll need to free this
            break;
        case IS_TRUE:
            /* Convert boolean TRUE to "1" */
            val = strdup("1");
            val_len = 1;
            free_val = 1;
            break;
        case IS_FALSE:
            /* Convert boolean FALSE to "0" */
            val = strdup("0");
            val_len = 1;
            free_val = 1;
            break;
        case IS_NULL:
            /* Convert NULL to empty string */
            val = strdup("");
            val_len = 0;
            free_val = 1;
            break;
        default:
            /* Unsupported type */
            RETURN_FALSE;
        }

        /* Check if conversion succeeded for integer case */
        if (!val)
        {
            RETURN_FALSE;
        }

        /* Check if z_set_opts contains GET option for response handling */
        if (z_set_opts != NULL && Z_TYPE_P(z_set_opts) == IS_ARRAY)
        {
            HashTable *options_ht = Z_ARRVAL_P(z_set_opts);
            zval *z_option;
            zend_string *option_key;
            zend_ulong num_key;

            /* Only scan for GET option to handle response */
            ZEND_HASH_FOREACH_KEY_VAL(options_ht, num_key, option_key, z_option)
            {
                if (option_key == NULL && Z_TYPE_P(z_option) == IS_STRING)
                {
                    /* Check for GET flag (case insensitive) */
                    if (strcasecmp(Z_STRVAL_P(z_option), "GET") == 0)
                    {
                        has_get_opt = 1;
                        break;
                    }
                }
            }
            ZEND_HASH_FOREACH_END();
        }

        /* Execute the SET command using the Glide client - pass variables to receive old value */
        int result = execute_set_command(redis->glide_client, key, key_len, val, val_len,
                                         expire_int, z_set_opts, &old_val, &old_val_len);

        /* Free the allocated string if needed */
        if (free_val)
        {
            free(val);
        }

        /* Process the result */
        switch (result)
        {
        case 1: /* Success */
            RETURN_TRUE;
        case 0: /* Not set (NX/XX/IFEQ condition not met) */
            RETURN_FALSE;
        case 2: /* GET option returned a value */
            /* If GET option was used and old value was returned */
            if (has_get_opt && old_val != NULL)
            {
                /* Return the old value */
                RETVAL_STRINGL(old_val, old_val_len);
                free(old_val); /* Free the allocated old value */
                return;
            }
            /* Fallback to returning TRUE when GET is used but handling fails */
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
    zval *object;
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
        /* Create a zval array for the GET option */
        zval z_opts;
        array_init(&z_opts);
        add_next_index_string(&z_opts, "GET");

        /* Execute the SET command with GET option using the Glide client */
        int result = execute_set_command(redis->glide_client, key, key_len, val, val_len,
                                         0,       /* No expiry */
                                         &z_opts, /* Use GET option */
                                         &response, &response_len);

        /* Free the zval array */
        zval_dtor(&z_opts);

        /* Process the result */
        if ((result == 1 || result == 2) && response != NULL)
        {
            /* Return the old value */
            RETVAL_STRINGL(response, response_len);
            free(response);
            return;
        }
        else if (result == 0 || (result == 2 && response == NULL))
        {
            /* Key didn't exist */
            RETURN_FALSE;
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
            RETURN_FALSE;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
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
        if (ret == 0)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_ZVAL(&result, 0, 1);
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
        long result_value;
        if (execute_setrange_command(redis->glide_client, key, key_len, offset, val, val_len, &result_value))
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
        long result_value;
        if (execute_strlen_command(redis->glide_client, key, key_len, &result_value))
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

/* {{{ proto Redis|array|false Redis::bzmpop(double $timeout, array $keys, string $from, int $count = 1) */
PHP_METHOD(Redis, bzmpop)
{
    zval *object, *z_keys;
    redis_object *redis;
    double timeout;
    zend_long count = 1;
    char *from = NULL;
    size_t from_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Odas|l",
                                     &object, redis_ce, &timeout, &z_keys,
                                     &from, &from_len, &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Make sure we have a glide client */
    if (!redis->glide_client)
    {
        RETURN_FALSE;
    }

    /* Execute the BZMPOP command using the Glide client */
    zval result;
    ZVAL_NULL(&result);

    /* Note: we need to pass "BZMPOP" as the cmd parameter for messaging purposes,
     * but the actual command type is determined by the "BZMPop" enum in the execute_zmpop_command function */
    int ret = execute_zmpop_command(redis->glide_client, "BZMPOP", timeout, z_keys, from, from_len, count, &result);

    /* If the result is 0, there was an error */
    if (ret == 0)
    {
        RETURN_FALSE;
    }

    /* Return the result */
    RETURN_ZVAL(&result, 0, 1);
}

/* }}} */
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
        if (ret == 0)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_ZVAL(&result, 0, 1);
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
        if (ret == 0)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_ZVAL(&result, 0, 1);
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
        if (ret == 0)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_ZVAL(&result, 0, 1);
    }
}
/* }}} */

/* {{{ proto array Redis::info() */
PHP_METHOD(Redis, info)
{
    zval *object;
    redis_object *redis;
    char *section = NULL, *response = NULL;
    size_t section_len = 0, response_len = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|s",
                                     &object, redis_ce, &section, &section_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the INFO command using the Glide client */
        int result = execute_info_command(redis->glide_client, section, section_len, &response, &response_len);

        /* Process the result */
        if (result == 1 && response != NULL)
        {
            zval z_ret;
            ZVAL_UNDEF(&z_ret);

            /* Parse the INFO response into a zval array */
            redis_parse_info_response(response, &z_ret);

            /* Free the response string */
            free(response);

            /* Return the parsed array */
            RETVAL_ZVAL(&z_ret, 0, 1);
            return;
        }
        else
        {
            /* Error or empty response */
            RETURN_FALSE;
        }
    }
    else
    {
        /* Fall back to the original implementation */
        REDIS_PROCESS_CMD(info, redis_info_response);
    }
}
/* }}} */

/* {{{ proto long Redis::ttl(string key) */
PHP_METHOD(Redis, ttl)
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
        /* Execute the TTL command using the Glide client */
        long result_value;
        if (execute_ttl_command(redis->glide_client, key, key_len, &result_value))
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

/* {{{ proto long Redis::pttl(string key) */
PHP_METHOD(Redis, pttl)
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
        /* Execute the PTTL command using the Glide client */
        long result_value;
        if (execute_pttl_command(redis->glide_client, key, key_len, &result_value))
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
        if (result == 0)
        {
            RETURN_FALSE;
        }

        /* Return the result */
        RETURN_LONG(result);
    }
}
/* }}} */

/* {{{ proto long Redis::zadd(string key, double score, string member, ...) */
PHP_METHOD(Redis, zAdd)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *z_args;
    int argc;
    int flags = 0;

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
        /* Execute the ZADD command using the Glide client */
        long result_value;
        if (execute_zadd_command(redis->glide_client, key, key_len, z_args, argc, flags, &result_value))
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
