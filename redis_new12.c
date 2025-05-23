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
    zval *z_args;
    int argc = 0;

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
        /* Execute the SINTER command using the Glide client */
        if (execute_sinter_command(redis->glide_client, z_args, argc,
                                   return_value))
        {
            /* Return value already set in execute_sinter_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
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

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Get keys count and convert HashTable to zval array */
    ht_keys = Z_ARRVAL_P(z_keys);
    keys_count = zend_hash_num_elements(ht_keys);

    /* If we have no keys, return empty array */
    if (keys_count == 0)
    {
        RETURN_LONG(0);
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

    /* Clean up if we didn't execute the command */
    for (int i = 0; i < keys_count; i++)
    {
        zval_dtor(&z_args[i]);
    }
    efree(z_args);
}
/* }}} */

/* {{{ proto long Redis::sInterStore(string dst, string key1, ...) */
PHP_METHOD(Redis, sInterStore)
{
    zval *object;
    redis_object *redis;
    char *dst = NULL;
    size_t dst_len;
    zval *z_args;
    int argc = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os+",
                                     &object, redis_ce, &dst, &dst_len,
                                     &z_args, &argc) == FAILURE)
    {
        RETURN_FALSE;
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
            /* Return the cardinality of the resulting set */
            RETURN_LONG(result_value);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::sUnion(string key, ...) */
PHP_METHOD(Redis, sUnion)
{
    zval *object;
    redis_object *redis;
    zval *z_args;
    int argc = 0;

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
        /* Execute the SUNION command using the Glide client */
        if (execute_sunion_command(redis->glide_client, z_args, argc,
                                   return_value))
        {
            /* Return value already set in execute_sunion_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
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
    zval *z_args;
    int argc = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os+",
                                     &object, redis_ce, &dst, &dst_len,
                                     &z_args, &argc) == FAILURE)
    {
        RETURN_FALSE;
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
            /* Return the cardinality of the resulting set */
            RETURN_LONG(result_value);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::sDiff(string key, ...) */
PHP_METHOD(Redis, sDiff)
{
    zval *object;
    redis_object *redis;
    zval *z_args;
    int argc = 0;

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
        /* Execute the SDIFF command using the Glide client */
        if (execute_sdiff_command(redis->glide_client, z_args, argc,
                                  return_value))
        {
            /* Return value already set in execute_sdiff_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
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
    zval *z_args;
    int argc = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os+",
                                     &object, redis_ce, &dst, &dst_len,
                                     &z_args, &argc) == FAILURE)
    {
        RETURN_FALSE;
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
            /* Return the cardinality of the resulting set */
            RETURN_LONG(result_value);
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto string Redis::serverName() */
PHP_METHOD(Redis, serverName)
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
        /* Execute the INFO command and get server name */
        char *result = NULL;
        size_t result_len;

        if (execute_servername_command(redis->glide_client, &result, &result_len))
        {
            /* Check if we got a server name */
            if (result != NULL && result_len > 0)
            {
                /* Return the server name */
                RETVAL_STRINGL(result, result_len);

                /* Free allocated memory */
                efree(result);
                return;
            }
            else if (result != NULL)
            {
                efree(result);
            }
        }

        RETURN_FALSE;
    }
}
/* }}} */
