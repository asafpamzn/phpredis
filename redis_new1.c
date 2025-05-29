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

/* {{{ proto string Redis::ping([string message])
 */
PHP_METHOD(Redis, ping)
{
    zval *object;
    redis_object *redis;
    char *msg = NULL, *response = NULL;
    size_t msg_len = 0, response_len = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|s",
                                     &object, redis_ce, &msg, &msg_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the PING command using the Glide client */
        int result = execute_ping_command(redis->glide_client, msg, msg_len, &response, &response_len);

        /* If the result is -1, there was an error */
        if (result == -1)
        {
            RETURN_FALSE;
        }

        /* Return the response */
        if (response)
        {
            if (strncmp(response, "PONG", 4) == 0)
            {
                efree(response);
                RETURN_TRUE;
            }

            /* Return the response */
            RETVAL_STRINGL(response, response_len);
            efree(response);
            return;
        }
        else
        {
            RETURN_TRUE;
        }
    }
    RETURN_FALSE;
}
/* }}} */

/** {{{ proto bool Redis::reset()
 */
PHP_METHOD(Redis, reset)
{
    char *response;
    int response_len;
    RedisSock *redis_sock;
    smart_string cmd = {0};
    zend_bool ret = 0;

    if ((redis_sock = redis_sock_get(getThis(), 0)) == NULL)
    {
        RETURN_FALSE;
    }

    if (IS_PIPELINE(redis_sock))
    {
        php_error_docref(NULL, E_ERROR, "Reset isn't allowed in pipeline mode!");
        RETURN_FALSE;
    }

    redis_cmd_init_sstr(&cmd, 0, "RESET", 5);

    REDIS_PROCESS_REQUEST(redis_sock, cmd.c, cmd.len);

    if ((response = redis_sock_read(redis_sock, &response_len)) != NULL)
    {
        ret = REDIS_STRCMP_STATIC(response, response_len, "+RESET");
        efree(response);
    }

    if (!ret)
    {
        if (IS_ATOMIC(redis_sock))
        {
            RETURN_FALSE;
        }
        REDIS_THROW_EXCEPTION("Reset failed in multi mode!", 0);
        RETURN_ZVAL(getThis(), 1, 0);
    }

    redis_free_reply_callbacks(redis_sock);
    redis_sock->status = REDIS_SOCK_STATUS_CONNECTED;
    redis_sock->mode = ATOMIC;
    redis_sock->dbNumber = 0;
    redis_sock->watching = 0;

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto long Redis::hSet(string key, string field, string value) */
PHP_METHOD(Redis, hSet)
{
    zval *object, *z_args, *z_array = NULL;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
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
        /* Check if we have a single array argument */
        int is_array_arg = 0;
        if (argc == 1 && Z_TYPE(z_args[0]) == IS_ARRAY)
        {
            z_array = &z_args[0];
            is_array_arg = 1;
        }

        /* Execute the HSET command using the Glide client */
        long result_value;
        if (execute_hset_command(redis->glide_client, key, key_len, z_args, argc, &result_value, is_array_arg))
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

/* {{{ proto bool Redis::hSetNx(string key, string field, string value) */
PHP_METHOD(Redis, hSetNx)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *field = NULL, *val = NULL;
    size_t key_len, field_len, val_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osss",
                                     &object, redis_ce, &key, &key_len,
                                     &field, &field_len, &val, &val_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the HSETNX command using the Glide client */
        int result;
        if (execute_hsetnx_command(redis->glide_client, key, key_len, field, field_len, val, val_len, &result))
        {
            /* Command succeeded, return the value */
            if (result == 1)
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
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto string Redis::hGet(string key, string field) */
PHP_METHOD(Redis, hGet)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *field = NULL, *response = NULL;
    size_t key_len, field_len, response_len = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &key, &key_len,
                                     &field, &field_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the HGET command using the Glide client */
        int result = execute_hget_command(redis->glide_client, key, key_len, field, field_len, &response, &response_len);

        /* Process the result */
        if (result == 1 && response != NULL)
        {
            /* Return the value */
            RETVAL_STRINGL(response, response_len);
            efree(response);
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

/* {{{ proto long Redis::hLen(string key) */
PHP_METHOD(Redis, hLen)
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
        /* Execute the HLEN command using the Glide client */
        long result_value;
        if (execute_hlen_command(redis->glide_client, key, key_len, &result_value))
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

/* {{{ proto long Redis::hDel(string key, string field1, ... fieldN) */
PHP_METHOD(Redis, hDel)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *fields = NULL;
    int fields_count = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os*",
                                     &object, redis_ce, &key, &key_len,
                                     &fields, &fields_count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the HDEL command using the Glide client */
        long result_value;
        if (execute_hdel_command(redis->glide_client, key, key_len, fields, fields_count, &result_value))
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

/* {{{ proto bool Redis::hExists(string key, string field) */
PHP_METHOD(Redis, hExists)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *field = NULL;
    size_t key_len, field_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &key, &key_len,
                                     &field, &field_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the HEXISTS command using the Glide client */
        int result;
        if (execute_hexists_command(redis->glide_client, key, key_len, field, field_len, &result))
        {
            /* Command succeeded, return the value */
            if (result == 1)
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
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::hKeys(string key) */
PHP_METHOD(Redis, hKeys)
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
        /* Execute the HKEYS command using the Glide client */
        array_init(return_value);
        if (execute_hkeys_command(redis->glide_client, key, key_len, return_value))
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

/* {{{ proto array Redis::hVals(string key) */
PHP_METHOD(Redis, hVals)
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
        /* Execute the HVALS command using the Glide client */
        array_init(return_value);
        if (execute_hvals_command(redis->glide_client, key, key_len, return_value))
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

/* {{{ proto array Redis::hGetAll(string key) */
PHP_METHOD(Redis, hGetAll)
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
        /* Execute the HGETALL command using the Glide client */
        array_init(return_value);
        if (execute_hgetall_command(redis->glide_client, key, key_len, return_value))
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

/* {{{ proto double Redis::hIncrByFloat(string key, string field, double increment) */
PHP_METHOD(Redis, hIncrByFloat)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *field = NULL;
    size_t key_len, field_len;
    double increment, result;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ossd",
                                     &object, redis_ce, &key, &key_len,
                                     &field, &field_len, &increment) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the HINCRBYFLOAT command using the Glide client */
        if (execute_hincrbyfloat_command(redis->glide_client, key, key_len, field, field_len, increment, &result))
        {
            /* Command succeeded, return the value */
            RETURN_DOUBLE(result);
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::hIncrBy(string key, string field, long increment) */
PHP_METHOD(Redis, hIncrBy)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *field = NULL;
    size_t key_len, field_len;
    zend_long increment;
    long result_value;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ossl",
                                     &object, redis_ce, &key, &key_len,
                                     &field, &field_len, &increment) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the HINCRBY command using the Glide client */
        if (execute_hincrby_command(redis->glide_client, key, key_len, field, field_len, increment, &result_value))
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

/* {{{ proto array Redis::hMget(string key, array fields) */
PHP_METHOD(Redis, hMget)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *fields = NULL;
    HashTable *fields_hash;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osa",
                                     &object, redis_ce, &key, &key_len,
                                     &fields) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the HMGET command using the Glide client */
        fields_hash = Z_ARRVAL_P(fields);

        /* Get field count */
        int fields_count = zend_hash_num_elements(fields_hash);
        if (fields_count == 0)
        {
            /* No fields provided */
            RETURN_FALSE;
        }

        /* First, count how many valid fields we have */
        int valid_fields_count = 0;
        zval *data, *real_data;
        zend_string *hash_key;
        zend_ulong num_idx;

        ZEND_HASH_FOREACH_KEY_VAL(fields_hash, num_idx, hash_key, data)
        {
            /* Handle references - if data is a reference, get the referenced value */
            if (Z_ISREF_P(data))
            {
                real_data = Z_REFVAL_P(data);
            }
            else
            {
                real_data = data;
            }

            if (hash_key)
            {
                /* Associative array, use the keys */
                valid_fields_count++;
            }
            else if (Z_TYPE_P(real_data) == IS_STRING && Z_STRLEN_P(real_data) > 0)
            {
                valid_fields_count++;
            }
            else if (Z_TYPE_P(real_data) == IS_LONG || Z_TYPE_P(real_data) == IS_DOUBLE ||
                     Z_TYPE_P(real_data) == IS_TRUE)
            {
                valid_fields_count++;
            }
        }
        ZEND_HASH_FOREACH_END();

        /* If no valid fields, return FALSE */
        if (valid_fields_count == 0)
        {
            RETURN_FALSE;
        }

        /* Create array for valid field values */
        zval *field_array = ecalloc(fields_count, sizeof(zval));
        if (!field_array)
        {
            RETURN_FALSE;
        }

        /* Initialize return array */
        array_init(return_value);

        /* Fill the field array from hash table */
        int i = 0;
        ZEND_HASH_FOREACH_KEY_VAL(fields_hash, num_idx, hash_key, data)
        {
            /* Handle references - dereference if needed */
            zval *real_data = data;
            if (Z_ISREF_P(data))
            {
                real_data = Z_REFVAL_P(data);
            }

            if (hash_key)
            {
                /* Associative array, use the keys */
                ZVAL_STR_COPY(&field_array[i], hash_key);
                i++;
            }
            else
            {
                /* Indexed array, use the values if they're valid */
                if (Z_TYPE_P(real_data) == IS_STRING && Z_STRLEN_P(real_data) > 0)
                {
                    /* Valid string field */
                    ZVAL_COPY(&field_array[i], real_data);
                    i++;
                }
                else if (Z_TYPE_P(real_data) == IS_LONG || Z_TYPE_P(real_data) == IS_DOUBLE ||
                         Z_TYPE_P(real_data) == IS_TRUE)
                {
                    /* Other valid types */
                    ZVAL_COPY(&field_array[i], real_data);
                    i++;
                }
                /* Skip invalid fields (NULL, false, empty string) */
            }
        }
        ZEND_HASH_FOREACH_END();

        /* Execute the HMGET command with valid fields */
        int result = execute_hmget_command(redis->glide_client, key, key_len, field_array, i, return_value);
        /* Free the field array regardless of result */
        for (int j = 0; j < i; j++)
        {
            zval_ptr_dtor(&field_array[j]);
        }
        efree(field_array);

        if (!result)
        {
            zval_dtor(return_value);
            RETURN_FALSE;
        }

        /* Command succeeded, return_value is already set */
        return;
    }
}
/* }}} */

/* {{{ proto boolean Redis::hMset(string key, array key_values) */
PHP_METHOD(Redis, hMset)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *arr_keyvals;
    HashTable *keyvals_hash;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osa",
                                     &object, redis_ce, &key, &key_len,
                                     &arr_keyvals) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the HMSET command using the Glide client */
        keyvals_hash = Z_ARRVAL_P(arr_keyvals);
        int keyvals_count = zend_hash_num_elements(keyvals_hash) * 2; /* Each key-value pair is 2 elements */

        if (keyvals_count > 0)
        {
            if (execute_hmset_command(redis->glide_client, key, key_len, arr_keyvals, keyvals_count))
            {
                /* Command succeeded */
                RETURN_TRUE;
            }
            else
            {
                /* Command failed */
                RETURN_FALSE;
            }
        }
        else
        {
            /* No key-value pairs provided */
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array|string Redis::hRandField(string key [, array options]) */
PHP_METHOD(Redis, hRandField)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *z_opts = NULL;
    zend_long count = 1;
    zend_bool withvalues = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|a",
                                     &object, redis_ce, &key, &key_len,
                                     &z_opts) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Process options if provided */
    if (z_opts)
    {
        HashTable *htopts = Z_ARRVAL_P(z_opts);
        zval *z_count = zend_hash_str_find(htopts, "count", sizeof("count") - 1);
        zval *z_withvalues = zend_hash_str_find(htopts, "withvalues", sizeof("withvalues") - 1);

        if (z_count)
        {
            count = zval_get_long(z_count);
        }

        if (z_withvalues)
        {
            withvalues = zval_is_true(z_withvalues);
        }
    }

    if (redis->glide_client)
    {
        /* Execute the HRANDFIELD command using the Glide client */
        array_init(return_value);

        if (execute_hrandfield_command(redis->glide_client, key, key_len, count, withvalues, return_value))
        {
            /* Command succeeded, return_value is already set */
            /* If count is 1 and not withvalues, return the single string value */

            if (count == 1 && !withvalues && zend_hash_num_elements(Z_ARRVAL_P(return_value)) == 1)
            {
                zval *z_ele, z_copy;
                zend_hash_internal_pointer_reset(Z_ARRVAL_P(return_value));
                z_ele = zend_hash_get_current_data(Z_ARRVAL_P(return_value));
                if (z_ele)
                {
                    ZVAL_COPY(&z_copy, z_ele);
                    zval_dtor(return_value);
                    ZVAL_COPY_VALUE(return_value, &z_copy);
                }
            }
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

/* {{{ proto long Redis::hStrLen(string key, string field) */
PHP_METHOD(Redis, hStrLen)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *field = NULL;
    size_t key_len, field_len;
    long result_value;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &key, &key_len,
                                     &field, &field_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the HSTRLEN command using the Glide client */
        if (execute_hstrlen_command(redis->glide_client, key, key_len, field, field_len, &result_value))
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
