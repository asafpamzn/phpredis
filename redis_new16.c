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
    zend_long maxlen = 0;
    zend_bool approximate = 0;
    int argc = ZEND_NUM_ARGS();
    int options_created = 0;

    /* First, try parsing as (key, id, fields, maxlen, approximate) */
    if (argc >= 4 && argc <= 5)
    {
        zend_bool parse_success = 0;

        if (argc == 4)
        {
            if (zend_parse_method_parameters(argc, getThis(), "Ossal",
                                             &object, redis_ce, &key, &key_len,
                                             &id, &id_len, &z_field_values, &maxlen) == SUCCESS)
            {
                parse_success = 1;
            }
        }
        else if (argc == 5)
        {
            if (zend_parse_method_parameters(argc, getThis(), "Ossalb",
                                             &object, redis_ce, &key, &key_len,
                                             &id, &id_len, &z_field_values, &maxlen, &approximate) == SUCCESS)
            {
                parse_success = 1;
            }
        }

        if (parse_success)
        {
            /* Create options array with MAXLEN */
            z_options = emalloc(sizeof(zval));
            array_init(z_options);

            /* Add MAXLEN option */
            add_assoc_long(z_options, "MAXLEN", maxlen);

            /* Add APPROXIMATE option if true */
            if (approximate)
            {
                add_assoc_bool(z_options, "APPROXIMATE", 1);
            }

            /* Flag that we created this and will need to free it later */
            options_created = 1;
        }
    }

    /* If above parsing failed or was not attempted, try the standard way */
    if (!z_options)
    {
        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ossa|a",
                                         &object, redis_ce, &key, &key_len,
                                         &id, &id_len, &z_field_values, &z_options) == FAILURE)
        {
            RETURN_FALSE;
        }
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
            /* Clean up if we created options array */
            if (options_created)
            {
                zval_dtor(z_options);
                efree(z_options);
            }

            /* Return value already set in execute_xadd_command */
            return;
        }
        else
        {
            /* Clean up if we created options array */
            if (options_created)
            {
                zval_dtor(z_options);
                efree(z_options);
            }

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
        /* Validate arguments based on subcommand before any processing */
        if (op_len == 6 && strncasecmp(op, "CREATE", 6) == 0)
        {
            /* CREATE subcommand expects: key, group, id, [mkstream_bool], [entries_read] */
            if (argc < 3 || argc > 5)
            {
                RETURN_FALSE;
            }

            /* Calculate the maximum number of processed arguments we might need */
            int max_processed_args = 3 + 2 + 1; /* key, group, id + MKSTREAM + ENTRIESREAD + value */
            zval *processed_args = (zval *)emalloc(sizeof(zval) * max_processed_args);
            int processed_argc = 0;

            /* Copy the first 3 arguments (key, group, id) */
            for (int i = 0; i < 3; i++)
            {
                processed_args[processed_argc++] = z_args[i];
            }

            /* Process optional parameters */
            if (argc > 3)
            {
                /* 4th argument: MKSTREAM boolean */
                zval *mkstream_arg = &z_args[3];
                if (zval_is_true(mkstream_arg))
                {
                    /* Add MKSTREAM keyword */
                    ZVAL_STRING(&processed_args[processed_argc], "MKSTREAM");
                    processed_argc++;
                }
            }

            if (argc > 4)
            {
                /* 5th argument: ENTRIESREAD value */
                zval *entries_read_arg = &z_args[4];

                /* Add ENTRIESREAD keyword */
                ZVAL_STRING(&processed_args[processed_argc], "ENTRIESREAD");
                processed_argc++;

                /* Add the entries read value */
                processed_args[processed_argc++] = *entries_read_arg;
            }

            /* Execute the XGROUP CREATE command */
            int result = execute_xgroup_command(redis->glide_client, op, op_len, processed_args, processed_argc, return_value);

            /* Clean up allocated ZVAL strings */
            for (int i = 3; i < processed_argc; i++)
            {
                if (Z_TYPE(processed_args[i]) == IS_STRING &&
                    (strcmp(Z_STRVAL(processed_args[i]), "MKSTREAM") == 0 ||
                     strcmp(Z_STRVAL(processed_args[i]), "ENTRIESREAD") == 0))
                {
                    zval_ptr_dtor(&processed_args[i]);
                }
            }
            efree(processed_args);

            if (result)
            {
                return;
            }
            else
            {
                RETURN_FALSE;
            }
        }
        else if (op_len == 14 && strncasecmp(op, "CREATECONSUMER", 14) == 0)
        {
            /* CREATECONSUMER expects exactly: key, group, consumer */
            if (argc != 3)
            {
                RETURN_FALSE;
            }

            /* Use the arguments as-is for CREATECONSUMER */
            if (execute_xgroup_command(redis->glide_client, op, op_len, z_args, argc, return_value))
            {
                return;
            }
            else
            {
                RETURN_FALSE;
            }
        }
        else if (op_len == 7 && strncasecmp(op, "DESTROY", 7) == 0)
        {
            /* DESTROY expects exactly: key, group */
            if (argc != 2)
            {
                RETURN_FALSE;
            }

            /* Use the arguments as-is for DESTROY */
            if (execute_xgroup_command(redis->glide_client, op, op_len, z_args, argc, return_value))
            {
                return;
            }
            else
            {
                RETURN_FALSE;
            }
        }
        else if (op_len == 5 && strncasecmp(op, "SETID", 5) == 0)
        {
            /* SETID expects exactly: key, group, id */
            if (argc != 3)
            {
                RETURN_FALSE;
            }

            /* Use the arguments as-is for SETID */
            if (execute_xgroup_command(redis->glide_client, op, op_len, z_args, argc, return_value))
            {
                return;
            }
            else
            {
                RETURN_FALSE;
            }
        }
        else if (op_len == 11 && strncasecmp(op, "DELCONSUMER", 11) == 0)
        {
            /* DELCONSUMER expects exactly: key, group, consumer */
            if (argc != 3)
            {
                RETURN_FALSE;
            }

            /* Use the arguments as-is for DELCONSUMER */
            if (execute_xgroup_command(redis->glide_client, op, op_len, z_args, argc, return_value))
            {
                return;
            }
            else
            {
                RETURN_FALSE;
            }
        }
        else
        {
            /* Unknown subcommand - don't send to Redis */
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

/* {{{ proto array Redis::xpending(string key, string group [, array options OR string start, string end, int count [, string consumer]]) */
PHP_METHOD(Redis, xpending)
{
    zval *object, *z_options = NULL;
    redis_object *redis;
    char *key = NULL, *group = NULL;
    char *start = NULL, *end = NULL, *consumer = NULL;
    size_t key_len = 0, group_len = 0;
    size_t start_len = 0, end_len = 0, consumer_len = 0;
    zend_long count = 0;
    zend_bool options_created = 0;
    int argc = ZEND_NUM_ARGS();

    /* Handle different parameter formats based on argument count */
    if (argc == 3 || argc == 2)
    {
        printf("file = %s, line = %d\n", __FILE__, __LINE__);
        /* Format: xpending(key, group, options_array) */
        if (zend_parse_method_parameters(argc, getThis(), "Oss|a",
                                         &object, redis_ce, &key, &key_len,
                                         &group, &group_len, &z_options) == FAILURE)
        {
            RETURN_FALSE;
        }
    }
    else if (argc == 5)
    {
        printf("file = %s, line = %d\n", __FILE__, __LINE__);
        /* Format: xpending(key, group, start, end, count) */
        if (zend_parse_method_parameters(argc, getThis(), "Osssl",
                                         &object, redis_ce, &key, &key_len,
                                         &group, &group_len, &start, &start_len,
                                         &end, &end_len, &count) == FAILURE)
        {
            RETURN_FALSE;
        }
    }
    else if (argc == 6)
    {
        printf("file = %s, line = %d\n", __FILE__, __LINE__);
        /* Format: xpending(key, group, start, end, count, consumer) */
        if (zend_parse_method_parameters(argc, getThis(), "Ossssls",
                                         &object, redis_ce, &key, &key_len,
                                         &group, &group_len, &start, &start_len,
                                         &end, &end_len, &count, &consumer, &consumer_len) == FAILURE)
        {
            RETURN_FALSE;
        }
    }
    else
    {
        printf("file = %s, line = %d\n", __FILE__, __LINE__);
        /* Invalid number of arguments */
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* If we got the extended format (start, end, count), convert to options array */
        if (z_options == NULL && start != NULL)
        {
            options_created = 1;
            z_options = emalloc(sizeof(zval));
            array_init(z_options);

            /* Add START, END to options array */
            add_assoc_stringl(z_options, "START", start, start_len);
            add_assoc_stringl(z_options, "END", end, end_len);
            add_assoc_long(z_options, "COUNT", count);

            /* Add CONSUMER to options array if provided */
            if (consumer)
            {
                add_assoc_stringl(z_options, "CONSUMER", consumer, consumer_len);
            }
        }
        printf("file = %s, line = %d\n", __FILE__, __LINE__);
        /* Execute the XPENDING command using the Glide client */
        int result = execute_xpending_command(redis->glide_client, key, key_len,
                                              group, group_len, z_options, return_value);

        /* Clean up if we created options array */
        if (options_created && z_options)
        {
            zval_dtor(z_options);
            efree(z_options);
        }

        if (result)
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

/* {{{ proto array Redis::xrange(string key, string start, string end [, int count [, array options]]) */
PHP_METHOD(Redis, xrange)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *start = NULL, *end = NULL;
    size_t key_len = 0, start_len = 0, end_len = 0;
    zval *z_options = NULL;
    long count = 0;
    int argc = ZEND_NUM_ARGS();
    int options_created = 0;

    /* Parse parameters - try different combinations based on argument count */
    if (argc == 4)
    {
        /* xrange(key, start, end, count) */
        if (zend_parse_method_parameters(argc, getThis(), "Osssl",
                                         &object, redis_ce, &key, &key_len,
                                         &start, &start_len, &end, &end_len, &count) == FAILURE)
        {
            RETURN_FALSE;
        }

        /* Create options array with COUNT */
        z_options = emalloc(sizeof(zval));
        array_init(z_options);
        add_assoc_long(z_options, "COUNT", count);
        options_created = 1;
    }
    else if (argc == 5)
    {
        /* xrange(key, start, end, count, options) */
        if (zend_parse_method_parameters(argc, getThis(), "Ossla",
                                         &object, redis_ce, &key, &key_len,
                                         &start, &start_len, &end, &end_len, &count, &z_options) == FAILURE)
        {
            RETURN_FALSE;
        }

        /* Add COUNT to existing options array or create new one */
        if (z_options && Z_TYPE_P(z_options) == IS_ARRAY)
        {
            add_assoc_long(z_options, "COUNT", count);
        }
        else
        {
            z_options = emalloc(sizeof(zval));
            array_init(z_options);
            add_assoc_long(z_options, "COUNT", count);
            options_created = 1;
        }
    }
    else
    {
        /* xrange(key, start, end [, options]) - original format for backward compatibility */
        if (zend_parse_method_parameters(argc, getThis(), "Osss|a",
                                         &object, redis_ce, &key, &key_len,
                                         &start, &start_len, &end, &end_len, &z_options) == FAILURE)
        {
            RETURN_FALSE;
        }
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
            /* Clean up if we created options array */
            if (options_created)
            {
                zval_dtor(z_options);
                efree(z_options);
            }

            /* Return value already set in execute_xrange_command */
            return;
        }
        else
        {
            /* Clean up if we created options array */
            if (options_created)
            {
                zval_dtor(z_options);
                efree(z_options);
            }

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

/* {{{ proto array Redis::xread(array streams_and_ids [, int count [, int block]]) */
PHP_METHOD(Redis, xread)
{
    zval *object;
    redis_object *redis;
    zval *z_streams_and_ids, *z_options = NULL;
    long count = -1, block = -1;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oa|ll",
                                     &object, redis_ce, &z_streams_and_ids, &count, &block) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Convert associative array to separate streams and ids arrays */
        zval z_streams, z_ids;
        array_init(&z_streams);
        array_init(&z_ids);

        zend_string *stream_key;
        zval *stream_id;
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(z_streams_and_ids), stream_key, stream_id)
        {
            if (stream_key)
            {
                add_next_index_str(&z_streams, zend_string_copy(stream_key));
                if (Z_TYPE_P(stream_id) != IS_STRING)
                {
                    convert_to_string(stream_id);
                }
                add_next_index_str(&z_ids, zend_string_copy(Z_STR_P(stream_id)));
            }
        }
        ZEND_HASH_FOREACH_END();

        /* Create options array if count or block were specified */
        if (count >= 0 || block >= 0)
        {
            z_options = emalloc(sizeof(zval));
            array_init(z_options);

            if (count >= 0)
            {
                add_assoc_long(z_options, "COUNT", count);
            }
            if (block >= 0)
            {
                add_assoc_long(z_options, "BLOCK", block);
            }
        }

        /* Execute the XREAD command using the Glide client */
        int result = execute_xread_command(redis->glide_client, &z_streams, &z_ids, z_options, return_value);

        /* Clean up */
        zval_dtor(&z_streams);
        zval_dtor(&z_ids);
        if (z_options)
        {
            zval_dtor(z_options);
            efree(z_options);
        }

        if (result)
        {
            /* Return value already set in execute_xread_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::xreadgroup(string group, string consumer, array streams [, int count [, array options]]) */
PHP_METHOD(Redis, xreadgroup)
{
    zval *object;
    redis_object *redis;
    char *group = NULL, *consumer = NULL;
    size_t group_len = 0, consumer_len = 0;
    zval *z_streams_and_ids, *z_options = NULL;
    long count = -1;
    int argc = ZEND_NUM_ARGS();
    int options_created = 0;

    /* Parse parameters - handle multiple calling patterns */
    if (argc == 4)
    {
        /* Try parsing as (group, consumer, streams, count) first */
        if (zend_parse_method_parameters(argc, getThis(), "Ossal",
                                         &object, redis_ce, &group, &group_len,
                                         &consumer, &consumer_len, &z_streams_and_ids, &count) == SUCCESS)
        {
            /* Create options array with COUNT */
            z_options = emalloc(sizeof(zval));
            array_init(z_options);
            add_assoc_long(z_options, "COUNT", count);
            options_created = 1;
        }
        else
        {
            /* Try parsing as (group, consumer, streams, options) */
            if (zend_parse_method_parameters(argc, getThis(), "Ossa",
                                             &object, redis_ce, &group, &group_len,
                                             &consumer, &consumer_len, &z_streams_and_ids, &z_options) == FAILURE)
            {
                RETURN_FALSE;
            }
        }
    }
    else if (argc == 5)
    {
        long block = -1;

        /* First try parsing as (group, consumer, streams, count, block) */
        if (zend_parse_method_parameters(argc, getThis(), "Ossall",
                                         &object, redis_ce, &group, &group_len,
                                         &consumer, &consumer_len, &z_streams_and_ids, &count, &block) == SUCCESS)
        {
            /* Create options array with both COUNT and BLOCK */
            z_options = emalloc(sizeof(zval));
            array_init(z_options);
            add_assoc_long(z_options, "COUNT", count);
            add_assoc_long(z_options, "BLOCK", block);
            options_created = 1;
        }
        else
        {
            /* Fallback to parsing as (group, consumer, streams, count, options) */
            if (zend_parse_method_parameters(argc, getThis(), "Ossala",
                                             &object, redis_ce, &group, &group_len,
                                             &consumer, &consumer_len, &z_streams_and_ids, &count, &z_options) == FAILURE)
            {
                RETURN_FALSE;
            }

            /* Add COUNT to existing options array or create new one */
            if (z_options && Z_TYPE_P(z_options) == IS_ARRAY)
            {
                add_assoc_long(z_options, "COUNT", count);
            }
            else
            {
                z_options = emalloc(sizeof(zval));
                array_init(z_options);
                add_assoc_long(z_options, "COUNT", count);
                options_created = 1;
            }
        }
    }
    else
    {
        /* Parse as (group, consumer, streams [, options]) - original format for backward compatibility */
        if (zend_parse_method_parameters(argc, getThis(), "Ossa|a",
                                         &object, redis_ce, &group, &group_len,
                                         &consumer, &consumer_len, &z_streams_and_ids, &z_options) == FAILURE)
        {
            RETURN_FALSE;
        }
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* For the combined format, we need to separate streams and IDs */
        zval z_streams, z_ids;
        array_init(&z_streams);
        array_init(&z_ids);

        /* Extract streams and IDs from the combined array */
        zend_string *stream_key;
        zval *stream_id;
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(z_streams_and_ids), stream_key, stream_id)
        {
            if (stream_key)
            {
                add_next_index_str(&z_streams, zend_string_copy(stream_key));
                if (Z_TYPE_P(stream_id) != IS_STRING)
                {
                    convert_to_string(stream_id);
                }
                add_next_index_str(&z_ids, zend_string_copy(Z_STR_P(stream_id)));
            }
        }
        ZEND_HASH_FOREACH_END();

        /* Execute the XREADGROUP command using the Glide client */
        int result = execute_xreadgroup_command(redis->glide_client, group, group_len, consumer, consumer_len,
                                                &z_streams, &z_ids, z_options, return_value);

        /* Clean up temporary arrays */
        zval_dtor(&z_streams);
        zval_dtor(&z_ids);

        /* Clean up if we created options array */
        if (options_created)
        {
            zval_dtor(z_options);
            efree(z_options);
        }

        if (result)
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

/* {{{ proto array Redis::xrevrange(string key, string end, string start [, int count [, array options]]) */
PHP_METHOD(Redis, xrevrange)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *start = NULL, *end = NULL;
    size_t key_len = 0, start_len = 0, end_len = 0;
    zval *z_options = NULL;
    long count = 0;
    int argc = ZEND_NUM_ARGS();
    int options_created = 0;

    /* Parse parameters - try different combinations based on argument count */
    if (argc == 4)
    {
        /* xrevrange(key, end, start, count) */
        if (zend_parse_method_parameters(argc, getThis(), "Osssl",
                                         &object, redis_ce, &key, &key_len,
                                         &end, &end_len, &start, &start_len, &count) == FAILURE)
        {
            RETURN_FALSE;
        }

        /* Create options array with COUNT */
        z_options = emalloc(sizeof(zval));
        array_init(z_options);
        add_assoc_long(z_options, "COUNT", count);
        options_created = 1;
    }
    else if (argc == 5)
    {
        /* xrevrange(key, end, start, count, options) */
        if (zend_parse_method_parameters(argc, getThis(), "Ossla",
                                         &object, redis_ce, &key, &key_len,
                                         &end, &end_len, &start, &start_len, &count, &z_options) == FAILURE)
        {
            RETURN_FALSE;
        }

        /* Add COUNT to existing options array or create new one */
        if (z_options && Z_TYPE_P(z_options) == IS_ARRAY)
        {
            add_assoc_long(z_options, "COUNT", count);
        }
        else
        {
            z_options = emalloc(sizeof(zval));
            array_init(z_options);
            add_assoc_long(z_options, "COUNT", count);
            options_created = 1;
        }
    }
    else
    {
        /* xrevrange(key, end, start [, options]) - original format for backward compatibility */
        if (zend_parse_method_parameters(argc, getThis(), "Osss|a",
                                         &object, redis_ce, &key, &key_len,
                                         &end, &end_len, &start, &start_len, &z_options) == FAILURE)
        {
            RETURN_FALSE;
        }
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
            /* Clean up if we created options array */
            if (options_created)
            {
                zval_dtor(z_options);
                efree(z_options);
            }

            /* Return value already set in execute_xrevrange_command */
            return;
        }
        else
        {
            /* Clean up if we created options array */
            if (options_created)
            {
                zval_dtor(z_options);
                efree(z_options);
            }

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
