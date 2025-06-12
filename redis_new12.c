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
extern int execute_sadd_command(const void *glide_client, const char *key, size_t key_len,
                                zval *members, int members_count, long *output_value);
extern int execute_sadd_array_command(const void *glide_client, const char *key, size_t key_len,
                                      HashTable *members_ht, long *output_value);
extern int execute_scard_command(const void *glide_client, const char *key, size_t key_len,
                                 long *output_value);
extern int execute_srem_command(const void *glide_client, const char *key, size_t key_len,
                                zval *members, int members_count, long *output_value);
extern int execute_smove_command(const void *glide_client, const char *src, size_t src_len,
                                 const char *dst, size_t dst_len, const char *member,
                                 size_t member_len, int *output_value);
extern int execute_spop_command(const void *glide_client, const char *key, size_t key_len,
                                long count, zval *return_value);
extern int execute_srandmember_command(const void *glide_client, const char *key, size_t key_len,
                                       long count, zval *return_value);
extern int execute_sismember_command(const void *glide_client, const char *key, size_t key_len,
                                     const char *member, size_t member_len, int *output_value);
extern int execute_smembers_command(const void *glide_client, const char *key, size_t key_len,
                                    zval *return_value);
extern int execute_smismember_command(const void *glide_client, const char *key, size_t key_len,
                                      zval *members, int members_count, zval *return_value);
extern int execute_sinter_command(const void *glide_client, zval *keys, int keys_count,
                                  zval *return_value);
extern int execute_sintercard_command(const void *glide_client, zval *keys, int keys_count,
                                      long limit, zval *return_value);
extern int execute_sinterstore_command(const void *glide_client, const char *dst, size_t dst_len,
                                       zval *keys, int keys_count, long *output_value);
extern int execute_sunion_command(const void *glide_client, zval *keys, int keys_count,
                                  zval *return_value);
extern int execute_sunionstore_command(const void *glide_client, const char *dst, size_t dst_len,
                                       zval *keys, int keys_count, long *output_value);
extern int execute_sdiff_command(const void *glide_client, zval *keys, int keys_count,
                                 zval *return_value);
extern int execute_sdiffstore_command(const void *glide_client, const char *dst, size_t dst_len,
                                      zval *keys, int keys_count, long *output_value);
extern int execute_servername_command(const void *glide_client, char **output, size_t *output_len);
extern int execute_serverversion_command(const void *glide_client, char **output, size_t *output_len);
extern int execute_scan_command(const void *glide_client, long *it, const char *pattern, size_t pattern_len,
                                long count, zval *return_value);
extern int execute_sscan_command(const void *glide_client, const char *key, size_t key_len,
                                 long *it, const char *pattern, size_t pattern_len,
                                 long count, zval *return_value);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto long Redis::sAdd(string key, string member, ...) */
PHP_METHOD(Redis, sAdd)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *z_args;
    int argc = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os+",
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
        /* Execute the SADD command using the Glide client */
        long result_value;
        if (execute_sadd_command(redis->glide_client, key, key_len,
                                 z_args, argc, &result_value))
        {
            /* Return the number of added elements */
            RETURN_LONG(result_value);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::sAddArray(string key, array values) */
PHP_METHOD(Redis, sAddArray)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *z_arr;
    HashTable *ht_arr;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osa",
                                     &object, redis_ce, &key, &key_len,
                                     &z_arr) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Get HashTable from array */
    ht_arr = Z_ARRVAL_P(z_arr);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SADD command using the Glide client */
        long result_value;
        if (execute_sadd_array_command(redis->glide_client, key, key_len,
                                       ht_arr, &result_value))
        {
            /* Return the number of added elements */
            RETURN_LONG(result_value);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::scard(string key) */
PHP_METHOD(Redis, scard)
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
        /* Execute the SCARD command using the Glide client */
        long result_value;
        if (execute_scard_command(redis->glide_client, key, key_len, &result_value))
        {
            /* Return the set cardinality */
            RETURN_LONG(result_value);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::srem(string key, string member, ...) */
PHP_METHOD(Redis, srem)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *z_args;
    int argc = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os+",
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
        /* Execute the SREM command using the Glide client */
        long result_value;
        if (execute_srem_command(redis->glide_client, key, key_len,
                                 z_args, argc, &result_value))
        {
            /* Return the number of removed elements */
            RETURN_LONG(result_value);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto bool Redis::sMove(string src, string dst, string member) */
PHP_METHOD(Redis, sMove)
{
    zval *object;
    redis_object *redis;
    char *src = NULL, *dst = NULL, *member = NULL;
    size_t src_len, dst_len, member_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osss",
                                     &object, redis_ce, &src, &src_len,
                                     &dst, &dst_len, &member, &member_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SMOVE command using the Glide client */
        int result_value;
        if (execute_smove_command(redis->glide_client, src, src_len, dst, dst_len,
                                  member, member_len, &result_value))
        {
            /* Return whether the member was moved */
            RETURN_BOOL(result_value);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto string|array Redis::sPop(string key, [long count]) */
PHP_METHOD(Redis, sPop)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long count = 0;
    int has_count = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|l",
                                     &object, redis_ce, &key, &key_len,
                                     &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Check if count parameter was provided */
    has_count = (ZEND_NUM_ARGS() > 1);

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Use count=1 if not specified, otherwise use the provided count */
        long pop_count = has_count ? count : 1;

        /* Execute the SPOP command using the Glide client */
        if (execute_spop_command(redis->glide_client, key, key_len,
                                 pop_count, return_value))
        {
            /* Return value already set in execute_spop_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto string|array Redis::sRandMember(string key, [long count]) */
PHP_METHOD(Redis, sRandMember)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long count = 0;
    int has_count = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|l",
                                     &object, redis_ce, &key, &key_len,
                                     &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Check if count parameter was provided */
    has_count = (ZEND_NUM_ARGS() > 1);

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* If no count is specified, use count=1 */
        long rand_count = has_count ? count : 1;

        /* Execute the SRANDMEMBER command using the Glide client */
        if (execute_srandmember_command(redis->glide_client, key, key_len,
                                        rand_count, return_value))
        {
            /* Return value already set in execute_srandmember_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto bool Redis::sismember(string key, string member) */
PHP_METHOD(Redis, sismember)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *member = NULL;
    size_t key_len, member_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &key, &key_len,
                                     &member, &member_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SISMEMBER command using the Glide client */
        int result_value;
        if (execute_sismember_command(redis->glide_client, key, key_len,
                                      member, member_len, &result_value))
        {
            /* Return whether the member exists in the set */
            RETURN_BOOL(result_value);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::sMembers(string key) */
PHP_METHOD(Redis, sMembers)
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
        /* Execute the SMEMBERS command using the Glide client */
        if (execute_smembers_command(redis->glide_client, key, key_len,
                                     return_value))
        {
            /* Return value already set in execute_smembers_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::sMisMember(string key, array members) */
PHP_METHOD(Redis, sMisMember)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *z_args;
    int argc = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os+",
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
        /* Execute the SMISMEMBER command using the Glide client */
        if (execute_smismember_command(redis->glide_client, key, key_len,
                                       z_args, argc, return_value))
        {
            /* Return value already set in execute_smismember_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::sInter(string key, ...) */
PHP_METHOD(Redis, sInter)
{
    zval *object;
    redis_object *redis;
    zval *z_args = NULL;
    int argc = 0;
    zval *z_keys_arr = NULL;
    HashTable *ht_keys = NULL;
    int keys_count = 0;
    zval *z_extracted_keys = NULL;

    /* Check if we have a single array argument or variadic string arguments */
    if (ZEND_NUM_ARGS() == 1)
    {
        /* Try to parse as a single array argument */
        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oa",
                                         &object, redis_ce, &z_keys_arr) == SUCCESS)
        {
            /* We have an array of keys */
            ht_keys = Z_ARRVAL_P(z_keys_arr);
            keys_count = zend_hash_num_elements(ht_keys);

            /* If array is empty, return FALSE */
            if (keys_count == 0)
            {
                RETURN_FALSE;
            }

            /* Allocate memory for array of zvals */
            z_extracted_keys = ecalloc(keys_count, sizeof(zval));

            /* Copy array values to sequential array */
            zval *data;
            int idx = 0;
            ZEND_HASH_FOREACH_VAL(ht_keys, data)
            {
                ZVAL_COPY(&z_extracted_keys[idx], data);
                idx++;
            }
            ZEND_HASH_FOREACH_END();

            /* Set for later use */
            z_args = z_extracted_keys;
            argc = keys_count;
        }
    }

    /* If we didn't get an array, parse as variadic arguments */
    if (!z_args)
    {
        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O+",
                                         &object, redis_ce, &z_args, &argc) == FAILURE)
        {
            RETURN_FALSE;
        }
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SINTER command using the Glide client */
        if (execute_sinter_command(redis->glide_client, z_args, argc,
                                   return_value))
        {
            /* Clean up if we allocated memory for the array keys */
            if (z_extracted_keys)
            {
                for (int i = 0; i < keys_count; i++)
                {
                    zval_dtor(&z_extracted_keys[i]);
                }
                efree(z_extracted_keys);
            }

            /* Return value already set in execute_sinter_command */
            return;
        }
        else
        {
            /* Clean up if we allocated memory for the array keys */
            if (z_extracted_keys)
            {
                for (int i = 0; i < keys_count; i++)
                {
                    zval_dtor(&z_extracted_keys[i]);
                }
                efree(z_extracted_keys);
            }

            RETURN_FALSE;
        }
    }

    /* Clean up if we allocated memory for the array keys but didn't execute the command */
    if (z_extracted_keys)
    {
        for (int i = 0; i < keys_count; i++)
        {
            zval_dtor(&z_extracted_keys[i]);
        }
        efree(z_extracted_keys);
    }
}
/* }}} */

/* {{{ proto long Redis::sintercard(array keys, [long limit]) */
PHP_METHOD(Redis, sintercard)
{
    zval *object;
    redis_object *redis;
    zval *z_keys;
    zend_long limit = 0;
    int has_limit = 0;
    HashTable *ht_keys;
    zval *z_args;
    int keys_count;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oa|l",
                                     &object, redis_ce, &z_keys, &limit) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Check if limit parameter was provided */
    has_limit = (ZEND_NUM_ARGS() > 1);
    if (has_limit && limit < 0)
    {
        php_error_docref(NULL, E_WARNING, "Limit must be a non-negative integer");
        RETURN_FALSE;
    }
    /* Check if keys parameter is an array */

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Get keys count and convert HashTable to zval array */
    ht_keys = Z_ARRVAL_P(z_keys);
    keys_count = zend_hash_num_elements(ht_keys);

    /* If we have no keys, return false */
    if (keys_count == 0)
    {
        RETURN_FALSE;
    }

    /* Allocate memory for array of zvals */
    z_args = ecalloc(keys_count, sizeof(zval));

    /* Copy array values to sequential array */
    zval *data;
    zend_string *key;
    int idx = 0;
    ZEND_HASH_FOREACH_KEY_VAL(ht_keys, idx, key, data)
    {
        ZVAL_COPY(&z_args[idx], data);
        idx++;
    }
    ZEND_HASH_FOREACH_END();

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Use default limit if not specified */
        long card_limit = has_limit ? limit : 0;

        /* Execute the SINTERCARD command using the Glide client */
        if (execute_sintercard_command(redis->glide_client, z_args, keys_count,
                                       card_limit, return_value))
        {
            /* Clean up allocated array */
            for (int i = 0; i < keys_count; i++)
            {
                zval_dtor(&z_args[i]);
            }
            efree(z_args);

            /* Return value already set in execute_sintercard_command */
            return;
        }
        else
        {
            /* Clean up allocated array */
            for (int i = 0; i < keys_count; i++)
            {
                zval_dtor(&z_args[i]);
            }
            efree(z_args);

            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto long Redis::sInterStore(string dst, string key1, ...) */
PHP_METHOD(Redis, sInterStore)
{
    zval *object;
    redis_object *redis;
    char *dst = NULL;
    size_t dst_len;
    zval *z_args = NULL;
    int argc = 0;
    zval *z_keys_arr = NULL;
    HashTable *ht_keys = NULL;
    int keys_count = 0;
    zval *z_extracted_keys = NULL;
    zval *data;
    int idx = 0;
    int has_destination = 0;

    /* Check if we have a single array argument */
    if (ZEND_NUM_ARGS() == 1)
    {
        /* Try to parse it as an array */
        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oa",
                                         &object, redis_ce, &z_keys_arr) == SUCCESS)
        {
            /* We have an array which will contain both destination and source keys */
            ht_keys = Z_ARRVAL_P(z_keys_arr);
            keys_count = zend_hash_num_elements(ht_keys);

            /* We need at least one element (destination key) */
            if (keys_count == 0)
            {
                RETURN_FALSE;
            }

            /* Extract the first element as the destination key */
            HashPosition pointer;
            zend_hash_internal_pointer_reset_ex(ht_keys, &pointer);
            data = zend_hash_get_current_data_ex(ht_keys, &pointer);
            if (data == NULL || Z_TYPE_P(data) != IS_STRING)
            {
                php_error_docref(NULL, E_WARNING, "Destination key must be a string");
                RETURN_FALSE;
            }

            /* Set the destination key */
            dst = Z_STRVAL_P(data);
            dst_len = Z_STRLEN_P(data);
            has_destination = 1;

            /* If there's only the destination key, return false */
            if (keys_count == 1)
            {
                RETURN_FALSE;
            }

            /* Move past the destination key */
            zend_hash_move_forward_ex(ht_keys, &pointer);

            /* Allocate memory for array of source keys (excluding destination) */
            z_extracted_keys = ecalloc(keys_count - 1, sizeof(zval));

            /* Copy all remaining values (source keys) to sequential array */
            idx = 0;
            while ((data = zend_hash_get_current_data_ex(ht_keys, &pointer)))
            {
                ZVAL_COPY(&z_extracted_keys[idx], data);
                idx++;
                zend_hash_move_forward_ex(ht_keys, &pointer);
            }

            /* Set for later use */
            z_args = z_extracted_keys;
            argc = keys_count - 1;
        }
    }

    /* If we didn't get a single array, try other parameter formats */
    if (!has_destination)
    {
        /* First argument is always the destination key */
        if (zend_parse_method_parameters(1, getThis(), "Os",
                                         &object, redis_ce, &dst, &dst_len) == FAILURE)
        {
            RETURN_FALSE;
        }

        /* Check if we have exactly two parameters (destination + array) */
        if (ZEND_NUM_ARGS() == 2)
        {
            /* Try to parse second parameter as an array */
            if (zend_parse_parameters(1, "a", &z_keys_arr) == SUCCESS)
            {
                /* We have an array of source keys */
                ht_keys = Z_ARRVAL_P(z_keys_arr);
                keys_count = zend_hash_num_elements(ht_keys);

                /* If array is empty, return FALSE */
                if (keys_count == 0)
                {
                    RETURN_FALSE;
                }

                /* Allocate memory for array of zvals */
                z_extracted_keys = ecalloc(keys_count, sizeof(zval));

                /* Copy array values to sequential array */
                idx = 0;
                ZEND_HASH_FOREACH_VAL(ht_keys, data)
                {
                    ZVAL_COPY(&z_extracted_keys[idx], data);
                    idx++;
                }
                ZEND_HASH_FOREACH_END();

                /* Set for later use */
                z_args = z_extracted_keys;
                argc = keys_count;
            }
        }

        /* If we didn't get an array as the second parameter, parse remaining args as variadic */
        if (!z_args)
        {
            /* Parse all parameters including destination key */
            if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os+",
                                             &object, redis_ce, &dst, &dst_len,
                                             &z_args, &argc) == FAILURE)
            {
                RETURN_FALSE;
            }
        }
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SINTERSTORE command using the Glide client */
        long result_value;
        if (execute_sinterstore_command(redis->glide_client, dst, dst_len,
                                        z_args, argc, &result_value))
        {
            /* Clean up if we allocated memory for the array keys */
            if (z_extracted_keys)
            {
                for (int i = 0; i < (has_destination ? keys_count - 1 : keys_count); i++)
                {
                    zval_dtor(&z_extracted_keys[i]);
                }
                efree(z_extracted_keys);
            }

            /* Return the cardinality of the resulting set */
            RETURN_LONG(result_value);
        }
        else
        {
            /* Clean up if we allocated memory for the array keys */
            if (z_extracted_keys)
            {
                for (int i = 0; i < (has_destination ? keys_count - 1 : keys_count); i++)
                {
                    zval_dtor(&z_extracted_keys[i]);
                }
                efree(z_extracted_keys);
            }

            RETURN_FALSE;
        }
    }

    /* Clean up if we allocated memory for the array keys but didn't execute the command */
    if (z_extracted_keys)
    {
        for (int i = 0; i < (has_destination ? keys_count - 1 : keys_count); i++)
        {
            zval_dtor(&z_extracted_keys[i]);
        }
        efree(z_extracted_keys);
    }
}
/* }}} */

/* {{{ proto array Redis::sUnion(string key, ...) */
PHP_METHOD(Redis, sUnion)
{
    zval *object;
    redis_object *redis;
    zval *z_args = NULL;
    int argc = 0;
    zval *z_keys_arr = NULL;
    HashTable *ht_keys = NULL;
    int keys_count = 0;
    zval *z_extracted_keys = NULL;

    /* Check if we have a single array argument or variadic string arguments */
    if (ZEND_NUM_ARGS() == 1)
    {
        /* Try to parse as a single array argument */
        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oa",
                                         &object, redis_ce, &z_keys_arr) == SUCCESS)
        {
            /* We have an array of keys */
            ht_keys = Z_ARRVAL_P(z_keys_arr);
            keys_count = zend_hash_num_elements(ht_keys);

            /* If array is empty, return FALSE */
            if (keys_count == 0)
            {
                RETURN_FALSE;
            }

            /* Allocate memory for array of zvals */
            z_extracted_keys = ecalloc(keys_count, sizeof(zval));

            /* Copy array values to sequential array */
            zval *data;
            int idx = 0;
            ZEND_HASH_FOREACH_VAL(ht_keys, data)
            {
                ZVAL_COPY(&z_extracted_keys[idx], data);
                idx++;
            }
            ZEND_HASH_FOREACH_END();

            /* Set for later use */
            z_args = z_extracted_keys;
            argc = keys_count;
        }
    }

    /* If we didn't get an array, parse as variadic arguments */
    if (!z_args)
    {
        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O+",
                                         &object, redis_ce, &z_args, &argc) == FAILURE)
        {
            RETURN_FALSE;
        }
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SUNION command using the Glide client */
        if (execute_sunion_command(redis->glide_client, z_args, argc,
                                   return_value))
        {
            /* Clean up if we allocated memory for the array keys */
            if (z_extracted_keys)
            {
                for (int i = 0; i < keys_count; i++)
                {
                    zval_dtor(&z_extracted_keys[i]);
                }
                efree(z_extracted_keys);
            }

            /* Return value already set in execute_sunion_command */
            return;
        }
        else
        {
            /* Clean up if we allocated memory for the array keys */
            if (z_extracted_keys)
            {
                for (int i = 0; i < keys_count; i++)
                {
                    zval_dtor(&z_extracted_keys[i]);
                }
                efree(z_extracted_keys);
            }

            RETURN_FALSE;
        }
    }

    /* Clean up if we allocated memory for the array keys but didn't execute the command */
    if (z_extracted_keys)
    {
        for (int i = 0; i < keys_count; i++)
        {
            zval_dtor(&z_extracted_keys[i]);
        }
        efree(z_extracted_keys);
    }
}
/* }}} */

/* {{{ proto long Redis::sUnionStore(string dst, string key1, ...) */
PHP_METHOD(Redis, sUnionStore)
{
    zval *object;
    redis_object *redis;
    char *dst = NULL;
    size_t dst_len;
    zval *z_args = NULL;
    int argc = 0;
    zval *z_keys_arr = NULL;
    HashTable *ht_keys = NULL;
    int keys_count = 0;
    zval *z_extracted_keys = NULL;
    zval *data;
    int idx = 0;
    int has_destination = 0;

    /* Check if we have a single array argument */
    if (ZEND_NUM_ARGS() == 1)
    {
        /* Try to parse it as an array */
        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oa",
                                         &object, redis_ce, &z_keys_arr) == SUCCESS)
        {
            /* We have an array which will contain both destination and source keys */
            ht_keys = Z_ARRVAL_P(z_keys_arr);
            keys_count = zend_hash_num_elements(ht_keys);

            /* We need at least one element (destination key) */
            if (keys_count == 0)
            {
                RETURN_FALSE;
            }

            /* Extract the first element as the destination key */
            HashPosition pointer;
            zend_hash_internal_pointer_reset_ex(ht_keys, &pointer);
            data = zend_hash_get_current_data_ex(ht_keys, &pointer);
            if (data == NULL || Z_TYPE_P(data) != IS_STRING)
            {
                php_error_docref(NULL, E_WARNING, "Destination key must be a string");
                RETURN_FALSE;
            }

            /* Set the destination key */
            dst = Z_STRVAL_P(data);
            dst_len = Z_STRLEN_P(data);
            has_destination = 1;

            /* If there's only the destination key, return false */
            if (keys_count == 1)
            {
                RETURN_FALSE;
            }

            /* Move past the destination key */
            zend_hash_move_forward_ex(ht_keys, &pointer);

            /* Allocate memory for array of source keys (excluding destination) */
            z_extracted_keys = ecalloc(keys_count - 1, sizeof(zval));

            /* Copy all remaining values (source keys) to sequential array */
            idx = 0;
            while ((data = zend_hash_get_current_data_ex(ht_keys, &pointer)))
            {
                ZVAL_COPY(&z_extracted_keys[idx], data);
                idx++;
                zend_hash_move_forward_ex(ht_keys, &pointer);
            }

            /* Set for later use */
            z_args = z_extracted_keys;
            argc = keys_count - 1;
        }
    }

    /* If we didn't get a single array, try other parameter formats */
    if (!has_destination)
    {
        /* First argument is always the destination key */
        if (zend_parse_method_parameters(1, getThis(), "Os",
                                         &object, redis_ce, &dst, &dst_len) == FAILURE)
        {
            RETURN_FALSE;
        }

        /* Check if we have exactly two parameters (destination + array) */
        if (ZEND_NUM_ARGS() == 2)
        {
            /* Try to parse second parameter as an array */
            if (zend_parse_parameters(1, "a", &z_keys_arr) == SUCCESS)
            {
                /* We have an array of source keys */
                ht_keys = Z_ARRVAL_P(z_keys_arr);
                keys_count = zend_hash_num_elements(ht_keys);

                /* If array is empty, return FALSE */
                if (keys_count == 0)
                {
                    RETURN_FALSE;
                }

                /* Allocate memory for array of zvals */
                z_extracted_keys = ecalloc(keys_count, sizeof(zval));

                /* Copy array values to sequential array */
                idx = 0;
                ZEND_HASH_FOREACH_VAL(ht_keys, data)
                {
                    ZVAL_COPY(&z_extracted_keys[idx], data);
                    idx++;
                }
                ZEND_HASH_FOREACH_END();

                /* Set for later use */
                z_args = z_extracted_keys;
                argc = keys_count;
            }
        }

        /* If we didn't get an array as the second parameter, parse remaining args as variadic */
        if (!z_args)
        {
            /* Parse all parameters including destination key */
            if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os+",
                                             &object, redis_ce, &dst, &dst_len,
                                             &z_args, &argc) == FAILURE)
            {
                RETURN_FALSE;
            }
        }
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SUNIONSTORE command using the Glide client */
        long result_value;
        if (execute_sunionstore_command(redis->glide_client, dst, dst_len,
                                        z_args, argc, &result_value))
        {
            /* Clean up if we allocated memory for the array keys */
            if (z_extracted_keys)
            {
                for (int i = 0; i < (has_destination ? keys_count - 1 : keys_count); i++)
                {
                    zval_dtor(&z_extracted_keys[i]);
                }
                efree(z_extracted_keys);
            }

            /* Return the cardinality of the resulting set */
            RETURN_LONG(result_value);
        }
        else
        {
            /* Clean up if we allocated memory for the array keys */
            if (z_extracted_keys)
            {
                for (int i = 0; i < (has_destination ? keys_count - 1 : keys_count); i++)
                {
                    zval_dtor(&z_extracted_keys[i]);
                }
                efree(z_extracted_keys);
            }

            RETURN_FALSE;
        }
    }

    /* Clean up if we allocated memory for the array keys but didn't execute the command */
    if (z_extracted_keys)
    {
        for (int i = 0; i < (has_destination ? keys_count - 1 : keys_count); i++)
        {
            zval_dtor(&z_extracted_keys[i]);
        }
        efree(z_extracted_keys);
    }
}
/* }}} */

/* {{{ proto array Redis::sDiff(string key, ...) */
PHP_METHOD(Redis, sDiff)
{
    zval *object;
    redis_object *redis;
    zval *z_args = NULL;
    int argc = 0;
    zval *z_keys_arr = NULL;
    HashTable *ht_keys = NULL;
    int keys_count = 0;
    zval *z_extracted_keys = NULL;

    /* Check if we have a single array argument or variadic string arguments */
    if (ZEND_NUM_ARGS() == 1)
    {
        /* Try to parse as a single array argument */
        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oa",
                                         &object, redis_ce, &z_keys_arr) == SUCCESS)
        {
            /* We have an array of keys */
            ht_keys = Z_ARRVAL_P(z_keys_arr);
            keys_count = zend_hash_num_elements(ht_keys);

            /* If array is empty, return FALSE */
            if (keys_count == 0)
            {
                RETURN_FALSE;
            }

            /* Allocate memory for array of zvals */
            z_extracted_keys = ecalloc(keys_count, sizeof(zval));

            /* Copy array values to sequential array */
            zval *data;
            int idx = 0;
            ZEND_HASH_FOREACH_VAL(ht_keys, data)
            {
                ZVAL_COPY(&z_extracted_keys[idx], data);
                idx++;
            }
            ZEND_HASH_FOREACH_END();

            /* Set for later use */
            z_args = z_extracted_keys;
            argc = keys_count;
        }
    }

    /* If we didn't get an array, parse as variadic arguments */
    if (!z_args)
    {
        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O+",
                                         &object, redis_ce, &z_args, &argc) == FAILURE)
        {
            RETURN_FALSE;
        }
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SDIFF command using the Glide client */
        if (execute_sdiff_command(redis->glide_client, z_args, argc,
                                  return_value))
        {
            /* Clean up if we allocated memory for the array keys */
            if (z_extracted_keys)
            {
                for (int i = 0; i < keys_count; i++)
                {
                    zval_dtor(&z_extracted_keys[i]);
                }
                efree(z_extracted_keys);
            }

            /* Return value already set in execute_sdiff_command */
            return;
        }
        else
        {
            /* Clean up if we allocated memory for the array keys */
            if (z_extracted_keys)
            {
                for (int i = 0; i < keys_count; i++)
                {
                    zval_dtor(&z_extracted_keys[i]);
                }
                efree(z_extracted_keys);
            }

            RETURN_FALSE;
        }
    }

    /* Clean up if we allocated memory for the array keys but didn't execute the command */
    if (z_extracted_keys)
    {
        for (int i = 0; i < keys_count; i++)
        {
            zval_dtor(&z_extracted_keys[i]);
        }
        efree(z_extracted_keys);
    }
}
/* }}} */

/* {{{ proto long Redis::sDiffStore(string dst, string key1, ...) */
PHP_METHOD(Redis, sDiffStore)
{
    zval *object;
    redis_object *redis;
    char *dst = NULL;
    size_t dst_len;
    zval *z_args = NULL;
    int argc = 0;
    zval *z_keys_arr = NULL;
    HashTable *ht_keys = NULL;
    int keys_count = 0;
    zval *z_extracted_keys = NULL;
    zval *data;
    int idx = 0;
    int has_destination = 0;

    /* Check if we have a single array argument */
    if (ZEND_NUM_ARGS() == 1)
    {
        /* Try to parse it as an array */
        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oa",
                                         &object, redis_ce, &z_keys_arr) == SUCCESS)
        {
            /* We have an array which will contain both destination and source keys */
            ht_keys = Z_ARRVAL_P(z_keys_arr);
            keys_count = zend_hash_num_elements(ht_keys);

            /* We need at least one element (destination key) */
            if (keys_count == 0)
            {
                RETURN_FALSE;
            }

            /* Extract the first element as the destination key */
            HashPosition pointer;
            zend_hash_internal_pointer_reset_ex(ht_keys, &pointer);
            data = zend_hash_get_current_data_ex(ht_keys, &pointer);
            if (data == NULL || Z_TYPE_P(data) != IS_STRING)
            {
                php_error_docref(NULL, E_WARNING, "Destination key must be a string");
                RETURN_FALSE;
            }

            /* Set the destination key */
            dst = Z_STRVAL_P(data);
            dst_len = Z_STRLEN_P(data);
            has_destination = 1;

            /* If there's only the destination key, return false */
            if (keys_count == 1)
            {
                RETURN_FALSE;
            }

            /* Move past the destination key */
            zend_hash_move_forward_ex(ht_keys, &pointer);

            /* Allocate memory for array of source keys (excluding destination) */
            z_extracted_keys = ecalloc(keys_count - 1, sizeof(zval));

            /* Copy all remaining values (source keys) to sequential array */
            idx = 0;
            while ((data = zend_hash_get_current_data_ex(ht_keys, &pointer)))
            {
                ZVAL_COPY(&z_extracted_keys[idx], data);
                idx++;
                zend_hash_move_forward_ex(ht_keys, &pointer);
            }

            /* Set for later use */
            z_args = z_extracted_keys;
            argc = keys_count - 1;
        }
    }

    /* If we didn't get a single array, try other parameter formats */
    if (!has_destination)
    {
        /* First argument is always the destination key */
        if (zend_parse_method_parameters(1, getThis(), "Os",
                                         &object, redis_ce, &dst, &dst_len) == FAILURE)
        {
            RETURN_FALSE;
        }

        /* Check if we have exactly two parameters (destination + array) */
        if (ZEND_NUM_ARGS() == 2)
        {
            /* Try to parse second parameter as an array */
            if (zend_parse_parameters(1, "a", &z_keys_arr) == SUCCESS)
            {
                /* We have an array of source keys */
                ht_keys = Z_ARRVAL_P(z_keys_arr);
                keys_count = zend_hash_num_elements(ht_keys);

                /* If array is empty, return FALSE */
                if (keys_count == 0)
                {
                    RETURN_FALSE;
                }

                /* Allocate memory for array of zvals */
                z_extracted_keys = ecalloc(keys_count, sizeof(zval));

                /* Copy array values to sequential array */
                idx = 0;
                ZEND_HASH_FOREACH_VAL(ht_keys, data)
                {
                    ZVAL_COPY(&z_extracted_keys[idx], data);
                    idx++;
                }
                ZEND_HASH_FOREACH_END();

                /* Set for later use */
                z_args = z_extracted_keys;
                argc = keys_count;
            }
        }

        /* If we didn't get an array as the second parameter, parse remaining args as variadic */
        if (!z_args)
        {
            /* Parse all parameters including destination key */
            if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os+",
                                             &object, redis_ce, &dst, &dst_len,
                                             &z_args, &argc) == FAILURE)
            {
                RETURN_FALSE;
            }
        }
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the SDIFFSTORE command using the Glide client */
        long result_value;
        if (execute_sdiffstore_command(redis->glide_client, dst, dst_len,
                                       z_args, argc, &result_value))
        {
            /* Clean up if we allocated memory for the array keys */
            if (z_extracted_keys)
            {
                for (int i = 0; i < (has_destination ? keys_count - 1 : keys_count); i++)
                {
                    zval_dtor(&z_extracted_keys[i]);
                }
                efree(z_extracted_keys);
            }

            /* Return the cardinality of the resulting set */
            RETURN_LONG(result_value);
        }
        else
        {
            /* Clean up if we allocated memory for the array keys */
            if (z_extracted_keys)
            {
                for (int i = 0; i < (has_destination ? keys_count - 1 : keys_count); i++)
                {
                    zval_dtor(&z_extracted_keys[i]);
                }
                efree(z_extracted_keys);
            }

            RETURN_FALSE;
        }
    }

    /* Clean up if we allocated memory for the array keys but didn't execute the command */
    if (z_extracted_keys)
    {
        for (int i = 0; i < (has_destination ? keys_count - 1 : keys_count); i++)
        {
            zval_dtor(&z_extracted_keys[i]);
        }
        efree(z_extracted_keys);
    }
}
/* }}} */
