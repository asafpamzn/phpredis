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
extern int execute_xack_command(const void *glide_client, const char *key, size_t key_len,
                                const char *group, size_t group_len, zval *ids, int id_count,
                                long *output_value);
extern int execute_xadd_command(const void *glide_client, const char *key, size_t key_len,
                                const char *id, size_t id_len, zval *field_values, int fv_count,
                                zval *options, zval *return_value);
extern int execute_xautoclaim_command(const void *glide_client, const char *key, size_t key_len,
                                      const char *group, size_t group_len, const char *consumer,
                                      size_t consumer_len, long min_idle_time, const char *start,
                                      size_t start_len, zval *options, zval *return_value);
extern int execute_xclaim_command(const void *glide_client, const char *key, size_t key_len,
                                  const char *group, size_t group_len, const char *consumer,
                                  size_t consumer_len, long min_idle_time, zval *ids, int id_count,
                                  zval *options, zval *return_value);
extern int execute_xdel_command(const void *glide_client, const char *key, size_t key_len,
                                zval *ids, int id_count, long *output_value);
extern int execute_xgroup_command(const void *glide_client, const char *op, size_t op_len,
                                  zval *args, int args_count, zval *return_value);
extern int execute_xinfo_command(const void *glide_client, const char *op, size_t op_len,
                                 zval *args, int args_count, zval *return_value);
extern int execute_xlen_command(const void *glide_client, const char *key, size_t key_len,
                                long *output_value);
extern int execute_xpending_command(const void *glide_client, const char *key, size_t key_len,
                                    const char *group, size_t group_len, zval *options,
                                    zval *return_value);
extern int execute_xrange_command(const void *glide_client, const char *key, size_t key_len,
                                  const char *start, size_t start_len, const char *end, size_t end_len,
                                  zval *options, zval *return_value);
extern int execute_xread_command(const void *glide_client, zval *streams, zval *ids,
                                 zval *options, zval *return_value);
extern int execute_xreadgroup_command(const void *glide_client, const char *group, size_t group_len,
                                      const char *consumer, size_t consumer_len, zval *streams,
                                      zval *ids, zval *options, zval *return_value);
extern int execute_xrevrange_command(const void *glide_client, const char *key, size_t key_len,
                                     const char *end, size_t end_len, const char *start, size_t start_len,
                                     zval *options, zval *return_value);
extern int execute_xtrim_command(const void *glide_client, const char *key, size_t key_len,
                                 const char *strategy, size_t strategy_len, long threshold,
                                 zval *options, long *output_value);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto long Redis::xack(string key, string group, array ids) */
PHP_METHOD(Redis, xack)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *group = NULL;
    size_t key_len = 0, group_len = 0;
    zval *z_ids;
    long count = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ossa",
                                     &object, redis_ce, &key, &key_len,
                                     &group, &group_len, &z_ids) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the XACK command using the Glide client */
        if (execute_xack_command(redis->glide_client, key, key_len, group, group_len,
                                 z_ids, zend_hash_num_elements(Z_ARRVAL_P(z_ids)), &count))
        {
            RETURN_LONG(count);
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

/* {{{ proto string Redis::xadd(string key, string id, array field_values [, int maxlen [, bool approximate]]) */
PHP_METHOD(Redis, xadd)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *id = NULL;
    size_t key_len = 0, id_len = 0;
    zval *z_field_values, *z_options = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ossa|a",
                                     &object, redis_ce, &key, &key_len,
                                     &id, &id_len, &z_field_values, &z_options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the XADD command using the Glide client */
        if (execute_xadd_command(redis->glide_client, key, key_len, id, id_len,
                                 z_field_values, zend_hash_num_elements(Z_ARRVAL_P(z_field_values)),
                                 z_options, return_value))
        {
            /* Return value already set in execute_xadd_command */
            return;
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

/* {{{ proto array Redis::xautoclaim(string key, string group, string consumer, int min_idle_time, string start [, array options]) */
PHP_METHOD(Redis, xautoclaim)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *group = NULL, *consumer = NULL, *start = NULL;
    size_t key_len = 0, group_len = 0, consumer_len = 0, start_len = 0;
    long min_idle_time = 0;
    zval *z_options = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osssls|a",
                                     &object, redis_ce, &key, &key_len,
                                     &group, &group_len, &consumer, &consumer_len,
                                     &min_idle_time, &start, &start_len, &z_options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the XAUTOCLAIM command using the Glide client */
        if (execute_xautoclaim_command(redis->glide_client, key, key_len, group, group_len,
                                       consumer, consumer_len, min_idle_time, start, start_len,
                                       z_options, return_value))
        {
            /* Return value already set in execute_xautoclaim_command */
            return;
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

/* {{{ proto array Redis::xclaim(string key, string group, string consumer, int min_idle_time, array ids [, array options]) */
PHP_METHOD(Redis, xclaim)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *group = NULL, *consumer = NULL;
    size_t key_len = 0, group_len = 0, consumer_len = 0;
    long min_idle_time = 0;
    zval *z_ids, *z_options = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osssla|a",
                                     &object, redis_ce, &key, &key_len,
                                     &group, &group_len, &consumer, &consumer_len,
                                     &min_idle_time, &z_ids, &z_options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the XCLAIM command using the Glide client */
        if (execute_xclaim_command(redis->glide_client, key, key_len, group, group_len,
                                   consumer, consumer_len, min_idle_time, z_ids,
                                   zend_hash_num_elements(Z_ARRVAL_P(z_ids)), z_options, return_value))
        {
            /* Return value already set in execute_xclaim_command */
            return;
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

/* {{{ proto long Redis::xdel(string key, array ids) */
PHP_METHOD(Redis, xdel)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len = 0;
    zval *z_ids;
    long count = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osa",
                                     &object, redis_ce, &key, &key_len, &z_ids) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the XDEL command using the Glide client */
        if (execute_xdel_command(redis->glide_client, key, key_len,
                                 z_ids, zend_hash_num_elements(Z_ARRVAL_P(z_ids)), &count))
        {
            RETURN_LONG(count);
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

/* {{{ proto mixed Redis::xgroup(string op, [string key, string group, ...]) */
PHP_METHOD(Redis, xgroup)
{
    zval *object;
    redis_object *redis;
    char *op = NULL;
    size_t op_len = 0;
    zval *z_args;
    int argc;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os*",
                                     &object, redis_ce, &op, &op_len, &z_args, &argc) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the XGROUP command using the Glide client */
        if (execute_xgroup_command(redis->glide_client, op, op_len, z_args, argc, return_value))
        {
            /* Return value already set in execute_xgroup_command */
            return;
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

/* {{{ proto mixed Redis::xinfo(string op, [string key, string group, ...]) */
PHP_METHOD(Redis, xinfo)
{
    zval *object;
    redis_object *redis;
    char *op = NULL;
    size_t op_len = 0;
    zval *z_args;
    int argc;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os*",
                                     &object, redis_ce, &op, &op_len, &z_args, &argc) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the XINFO command using the Glide client */
        if (execute_xinfo_command(redis->glide_client, op, op_len, z_args, argc, return_value))
        {
            /* Return value already set in execute_xinfo_command */
            return;
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

/* {{{ proto long Redis::xlen(string key) */
PHP_METHOD(Redis, xlen)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len = 0;
    long length = 0;

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
        /* Execute the XLEN command using the Glide client */
        if (execute_xlen_command(redis->glide_client, key, key_len, &length))
        {
            RETURN_LONG(length);
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

/* {{{ proto array Redis::xpending(string key, string group [, array options]) */
PHP_METHOD(Redis, xpending)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *group = NULL;
    size_t key_len = 0, group_len = 0;
    zval *z_options = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss|a",
                                     &object, redis_ce, &key, &key_len,
                                     &group, &group_len, &z_options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the XPENDING command using the Glide client */
        if (execute_xpending_command(redis->glide_client, key, key_len,
                                     group, group_len, z_options, return_value))
        {
            /* Return value already set in execute_xpending_command */
            return;
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

/* {{{ proto array Redis::xrange(string key, string start, string end [, array options]) */
PHP_METHOD(Redis, xrange)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *start = NULL, *end = NULL;
    size_t key_len = 0, start_len = 0, end_len = 0;
    zval *z_options = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osss|a",
                                     &object, redis_ce, &key, &key_len,
                                     &start, &start_len, &end, &end_len, &z_options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the XRANGE command using the Glide client */
        if (execute_xrange_command(redis->glide_client, key, key_len, start, start_len,
                                   end, end_len, z_options, return_value))
        {
            /* Return value already set in execute_xrange_command */
            return;
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

/* {{{ proto array Redis::xread([array options, ]array streams) */
PHP_METHOD(Redis, xread)
{
    zval *object;
    redis_object *redis;
    zval *z_streams, *z_ids, *z_options = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oaa|a",
                                     &object, redis_ce, &z_streams, &z_ids, &z_options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the XREAD command using the Glide client */
        if (execute_xread_command(redis->glide_client, z_streams, z_ids, z_options, return_value))
        {
            /* Return value already set in execute_xread_command */
            return;
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

/* {{{ proto array Redis::xreadgroup(string group, string consumer, array streams, array ids [, array options]) */
PHP_METHOD(Redis, xreadgroup)
{
    zval *object;
    redis_object *redis;
    char *group = NULL, *consumer = NULL;
    size_t group_len = 0, consumer_len = 0;
    zval *z_streams, *z_ids, *z_options = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ossaa|a",
                                     &object, redis_ce, &group, &group_len,
                                     &consumer, &consumer_len, &z_streams, &z_ids, &z_options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the XREADGROUP command using the Glide client */
        if (execute_xreadgroup_command(redis->glide_client, group, group_len, consumer, consumer_len,
                                       z_streams, z_ids, z_options, return_value))
        {
            /* Return value already set in execute_xreadgroup_command */
            return;
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

/* {{{ proto array Redis::xrevrange(string key, string end, string start [, array options]) */
PHP_METHOD(Redis, xrevrange)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *start = NULL, *end = NULL;
    size_t key_len = 0, start_len = 0, end_len = 0;
    zval *z_options = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osss|a",
                                     &object, redis_ce, &key, &key_len,
                                     &end, &end_len, &start, &start_len, &z_options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the XREVRANGE command using the Glide client */
        if (execute_xrevrange_command(redis->glide_client, key, key_len, end, end_len,
                                      start, start_len, z_options, return_value))
        {
            /* Return value already set in execute_xrevrange_command */
            return;
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

/* {{{ proto long Redis::xtrim(string key, string strategy, int threshold [, array options]) */
PHP_METHOD(Redis, xtrim)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *strategy = NULL;
    size_t key_len = 0, strategy_len = 0;
    long threshold = 0, count = 0;
    zval *z_options = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ossl|a",
                                     &object, redis_ce, &key, &key_len,
                                     &strategy, &strategy_len, &threshold, &z_options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the XTRIM command using the Glide client */
        if (execute_xtrim_command(redis->glide_client, key, key_len, strategy, strategy_len,
                                  threshold, z_options, &count))
        {
            RETURN_LONG(count);
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
