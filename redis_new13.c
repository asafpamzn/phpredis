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
extern int execute_servername_command(const void *glide_client, char **output, size_t *output_len);
extern int execute_serverversion_command(const void *glide_client, char **output, size_t *output_len);
extern int execute_scan_command(const void *glide_client, long *it, const char *pattern, size_t pattern_len,
                                long count, zval *return_value);
extern int execute_sscan_command(const void *glide_client, const char *key, size_t key_len,
                                 long *it, const char *pattern, size_t pattern_len,
                                 long count, zval *return_value);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

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

/* {{{ proto string Redis::serverVersion() */
PHP_METHOD(Redis, serverVersion)
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
        /* Execute the INFO command and get server version */
        char *result = NULL;
        size_t result_len;

        if (execute_serverversion_command(redis->glide_client, &result, &result_len))
        {
            /* Check if we got a server version */
            if (result != NULL && result_len > 0)
            {
                /* Return the server version */
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

/* {{{ proto array Redis::scan(long &iterator [, string pattern, long count]) */
PHP_METHOD(Redis, scan)
{
    zval *object;
    redis_object *redis;
    zval *z_iter;
    char *pattern = NULL;
    size_t pattern_len = 0;
    int has_pattern = 0;
    zend_long count = 0;
    int has_count = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oz|sl",
                                     &object, redis_ce, &z_iter, &pattern, &pattern_len,
                                     &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Check if optional parameters are provided */
    has_pattern = (pattern != NULL && pattern_len > 0);
    has_count = (ZEND_NUM_ARGS() > 2);

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Dereference if it's a reference */
    ZVAL_DEREF(z_iter);

    /* Convert iterator */
    convert_to_long(z_iter);
    long iter = Z_LVAL_P(z_iter);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Use empty pattern if not specified */
        const char *scan_pattern = has_pattern ? pattern : "";
        size_t scan_pattern_len = has_pattern ? pattern_len : 0;

        /* Use default count if not specified */
        long scan_count = has_count ? count : 10;

        /* Execute the SCAN command using the Glide client */
        if (execute_scan_command(redis->glide_client, &iter, scan_pattern,
                                 scan_pattern_len, scan_count, return_value))
        {
            /* Update iterator value */
            ZVAL_LONG(z_iter, iter);

            /* Return value already set in execute_scan_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */

/* {{{ proto array Redis::sscan(string key, long &iterator [, string pattern, long count]) */
PHP_METHOD(Redis, sscan)
{
    zval *object;
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zval *z_iter;
    char *pattern = NULL;
    size_t pattern_len = 0;
    int has_pattern = 0;
    zend_long count = 0;
    int has_count = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osz|sl",
                                     &object, redis_ce, &key, &key_len, &z_iter,
                                     &pattern, &pattern_len, &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Check if optional parameters are provided */
    has_pattern = (pattern != NULL && pattern_len > 0);
    has_count = (ZEND_NUM_ARGS() > 3);

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Dereference if it's a reference */
    ZVAL_DEREF(z_iter);

    /* Convert iterator */
    convert_to_long(z_iter);
    long iter = Z_LVAL_P(z_iter);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Use empty pattern if not specified */
        const char *scan_pattern = has_pattern ? pattern : "";
        size_t scan_pattern_len = has_pattern ? pattern_len : 0;

        /* Use default count if not specified */
        long scan_count = has_count ? count : 10;

        /* Execute the SSCAN command using the Glide client */
        if (execute_sscan_command(redis->glide_client, key, key_len, &iter,
                                  scan_pattern, scan_pattern_len,
                                  scan_count, return_value))
        {
            /* Update iterator value */
            ZVAL_LONG(z_iter, iter);

            /* Return value already set in execute_sscan_command */
            return;
        }
        else
        {
            RETURN_FALSE;
        }
    }
}
/* }}} */
