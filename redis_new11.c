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
extern int execute_blmove_command(const void *glide_client, const char *src, size_t src_len,
                                  const char *dst, size_t dst_len, const char *wherefrom,
                                  size_t wherefrom_len, const char *whereto, size_t whereto_len,
                                  long timeout, char **output_value, size_t *output_len);
extern int execute_lmove_command(const void *glide_client, const char *src, size_t src_len,
                                 const char *dst, size_t dst_len, const char *wherefrom,
                                 size_t wherefrom_len, const char *whereto, size_t whereto_len,
                                 char **output_value, size_t *output_len);
extern int execute_lrem_command(const void *glide_client, const char *key, size_t key_len,
                                long count, const char *value, size_t value_len,
                                long *output_value);
extern int execute_ltrim_command(const void *glide_client, const char *key, size_t key_len,
                                 long start, long end);
extern int execute_lindex_command(const void *glide_client, const char *key, size_t key_len,
                                  long index, char **output_value, size_t *output_len);
extern int execute_lset_command(const void *glide_client, const char *key, size_t key_len,
                                long index, const char *value, size_t value_len);
extern int execute_flushdb_command(const void *glide_client, int async);
extern int execute_flushall_command(const void *glide_client, int async);
extern int execute_time_command(const void *glide_client, zval *return_value);
extern int execute_role_command(const void *glide_client, zval *return_value);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto string Redis::blmove(string src, string dst, string wherefrom, string whereto, int timeout) */
PHP_METHOD(Redis, blmove)
{
    zval *object;
    redis_object *redis;
    char *src = NULL, *dst = NULL, *wherefrom = NULL, *whereto = NULL;
    size_t src_len, dst_len, wherefrom_len, whereto_len;
    zend_long timeout;
    char *output_value = NULL;
    size_t output_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ossssl",
                                     &object, redis_ce,
                                     &src, &src_len,
                                     &dst, &dst_len,
                                     &wherefrom, &wherefrom_len,
                                     &whereto, &whereto_len,
                                     &timeout) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        int status;

        /* Execute the BLMOVE command using the Glide client */
        status = execute_blmove_command(redis->glide_client,
                                        src, src_len,
                                        dst, dst_len,
                                        wherefrom, wherefrom_len,
                                        whereto, whereto_len,
                                        timeout, &output_value, &output_len);

        if (status > 0)
        {
            /* Success with data */
            if (output_value)
            {
                RETVAL_STRINGL(output_value, output_len);
                efree(output_value);
                return;
            }
            else
            {
                RETURN_FALSE;
            }
        }
        else if (status == 0)
        {
            /* Key didn't exist or list was empty */
            RETURN_FALSE;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }

    /* If we reach here, either we don't have a Glide client or something went wrong */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto string Redis::lMove(string src, string dst, string wherefrom, string whereto) */
PHP_METHOD(Redis, lMove)
{
    zval *object;
    redis_object *redis;
    char *src = NULL, *dst = NULL, *wherefrom = NULL, *whereto = NULL;
    size_t src_len, dst_len, wherefrom_len, whereto_len;
    char *output_value = NULL;
    size_t output_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ossss",
                                     &object, redis_ce,
                                     &src, &src_len,
                                     &dst, &dst_len,
                                     &wherefrom, &wherefrom_len,
                                     &whereto, &whereto_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        int status;

        /* Execute the LMOVE command using the Glide client */
        status = execute_lmove_command(redis->glide_client,
                                       src, src_len,
                                       dst, dst_len,
                                       wherefrom, wherefrom_len,
                                       whereto, whereto_len,
                                       &output_value, &output_len);

        if (status > 0)
        {
            /* Success with data */
            if (output_value)
            {
                RETVAL_STRINGL(output_value, output_len);
                efree(output_value);
                return;
            }
            else
            {
                RETURN_FALSE;
            }
        }
        else if (status == 0)
        {
            /* Key didn't exist or list was empty */
            RETURN_FALSE;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }

    /* If we reach here, either we don't have a Glide client or something went wrong */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto long Redis::lrem(string key, long count, string value) */
PHP_METHOD(Redis, lrem)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *value = NULL;
    size_t key_len, value_len;
    zend_long count;
    long output_value;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osls",
                                     &object, redis_ce,
                                     &key, &key_len,
                                     &count,
                                     &value, &value_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        int status;

        /* Execute the LREM command using the Glide client */
        status = execute_lrem_command(redis->glide_client,
                                      key, key_len,
                                      count,
                                      value, value_len,
                                      &output_value);

        if (status)
        {
            /* Return the number of removed elements */
            RETURN_LONG(output_value);
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }

    /* If we reach here, either we don't have a Glide client or something went wrong */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean Redis::ltrim(string key, long start, long end) */
PHP_METHOD(Redis, ltrim)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long start, end;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osll",
                                     &object, redis_ce,
                                     &key, &key_len,
                                     &start, &end) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        int status;

        /* Execute the LTRIM command using the Glide client */
        status = execute_ltrim_command(redis->glide_client,
                                       key, key_len,
                                       start, end);

        if (status)
        {
            /* Success */
            RETURN_TRUE;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }

    /* If we reach here, either we don't have a Glide client or something went wrong */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto string Redis::lindex(string key, long index) */
PHP_METHOD(Redis, lindex)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long index;
    char *output_value = NULL;
    size_t output_len;
    printf("file = %s, line = %d\n", __FILE__, __LINE__);
    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osl",
                                     &object, redis_ce,
                                     &key, &key_len,
                                     &index) == FAILURE)
    {
        RETURN_FALSE;
    }
    printf("file = %s, line = %d\n", __FILE__, __LINE__);
    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        int status;

        /* Execute the LINDEX command using the Glide client */
        printf("file = %s, line = %d\n", __FILE__, __LINE__);
        status = execute_lindex_command(redis->glide_client,
                                        key, key_len,
                                        index,
                                        &output_value, &output_len);

        printf("file = %s, line = %d\n", __FILE__, __LINE__);

        if (status > 0)
        {
            /* Success with data */
            if (output_value)
            {
                RETVAL_STRINGL(output_value, output_len);
                efree(output_value);
                return;
            }
            else
            {
                RETURN_FALSE;
            }
        }
        else if (status == 0)
        {
            /* Index is out of range or key doesn't exist */
            RETURN_FALSE;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }

    /* If we reach here, either we don't have a Glide client or something went wrong */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean Redis::lSet(string key, long index, string value) */
PHP_METHOD(Redis, lSet)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *val = NULL;
    size_t key_len, val_len;
    zend_long index;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osls",
                                     &object, redis_ce,
                                     &key, &key_len,
                                     &index,
                                     &val, &val_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        int status;

        /* Execute the LSET command using the Glide client */
        status = execute_lset_command(redis->glide_client,
                                      key, key_len,
                                      index,
                                      val, val_len);

        if (status)
        {
            /* Success */
            RETURN_TRUE;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }

    /* If we reach here, either we don't have a Glide client or something went wrong */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean Redis::flushDB([boolean async]) */
PHP_METHOD(Redis, flushDB)
{
    zval *object;
    redis_object *redis;
    zend_bool async = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|b",
                                     &object, redis_ce, &async) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        int status;

        /* Execute the FLUSHDB command using the Glide client */
        status = execute_flushdb_command(redis->glide_client, async);

        if (status)
        {
            /* Success */
            RETURN_TRUE;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }

    /* If we reach here, either we don't have a Glide client or something went wrong */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean Redis::flushAll([boolean async]) */
PHP_METHOD(Redis, flushAll)
{
    zval *object;
    redis_object *redis;
    zend_bool async = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|b",
                                     &object, redis_ce, &async) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        int status;

        /* Execute the FLUSHALL command using the Glide client */
        status = execute_flushall_command(redis->glide_client, async);

        if (status)
        {
            /* Success */
            RETURN_TRUE;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }

    /* If we reach here, either we don't have a Glide client or something went wrong */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Redis::time() */
PHP_METHOD(Redis, time)
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
        /* Execute the TIME command using the Glide client */
        if (execute_time_command(redis->glide_client, return_value))
        {
            /* Return value already set in execute_time_command */
            return;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }

    /* If we reach here, either we don't have a Glide client or something went wrong */
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Redis::role() */
PHP_METHOD(Redis, role)
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
        /* Execute the ROLE command using the Glide client */
        if (execute_role_command(redis->glide_client, return_value))
        {
            /* Return value already set in execute_role_command */
            return;
        }
        else
        {
            /* Error */
            RETURN_FALSE;
        }
    }

    /* If we reach here, either we don't have a Glide client or something went wrong */
    RETURN_FALSE;
}
/* }}} */
