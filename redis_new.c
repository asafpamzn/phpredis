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

#include "redis_cluster.h"

#include "redis_glide.h"
#include "valkey_glide_z_common.h"
#include "valkey_glide_list_common.h"
#include "valkey_glide_commands_common.h"
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
ECHO_METHOD_IMPL(Redis)
/* }}} */

BITOP_METHOD_IMPL(Redis)

/* }}} */

/* {{{ proto long Redis::getBit(string key, long offset) */
GETBIT_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::setBit(string key, long offset, int value) */
SETBIT_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::del(string key, ...) or Redis::del(array keys) */
DEL_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::bitcount(string key, [int start], [int end])
 */
BITCOUNT_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto integer Redis::bitpos(string key, int bit, [int start, int end]) */
BITPOS_METHOD_IMPL(Redis)
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
            val = estrdup("1");
            val_len = 1;
            free_val = 1;
            break;
        case IS_FALSE:
            /* Convert boolean FALSE to "0" */
            val = estrdup("0");
            val_len = 1;
            free_val = 1;
            break;
        case IS_NULL:
            /* Convert NULL to empty string */
            val = estrdup("");
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

            efree(val);
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
                efree(old_val); /* Free the allocated old value */
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
SETEX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto boolean Redis::psetex(string key, long expire, string value)
 */
PSETEX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto boolean Redis::setnx(string key, string value)
 */
SETNX_METHOD_IMPL(Redis)
/* }}} */

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
            efree(response);
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
GET_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string Redis::randomKey()
 */
RANDOMKEY_METHOD_IMPL(Redis)
/* }}} */

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
SETRANGE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::strlen(string key) */
STRLEN_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::info([string section [, string section...]]) */
INFO_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::ttl(string key) */
TTL_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::pttl(string key) */
PTTL_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string Redis::ping([string message])
 */
PING_METHOD_IMPL(Redis)
/* }}} */

/** {{{ proto bool Redis::reset()
 */
PHP_METHOD(Redis, reset)
{
    RETURN_FALSE;
    // TODO
}
/* }}} */
