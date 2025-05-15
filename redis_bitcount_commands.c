/*
  +----------------------------------------------------------------------+
  | Redis Glide FFI integration for phpredis - BITCOUNT command          |
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

/* External declarations */
extern zend_class_entry *redis_ce;

/* Implementation of BITCOUNT command using Valkey Glide FFI */
PHP_REDIS_API int
redis_bitcount_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                         char *key, size_t key_len, zend_long start, zend_long end,
                         zend_bool bybit)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }
    printf("file = %s, line = %d\n", __FILE__, __LINE__);
    /* Execute the BITCOUNT command using the Glide client */
    long result = execute_bitcount_command(redis->glide_client, key, key_len, start, end, bybit);

    /* If the result is -1, there was an error */
    if (result == -1)
    {
        /* Set PHP return value to FALSE */
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* Set PHP return value to the result */
    RETVAL_LONG(result);
    return SUCCESS;
}

/* Override for the BITCOUNT command handler in Redis class */
PHP_METHOD(Redis, bitcount_glide)
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

    /* Try to use the Glide implementation */
    if (redis_bitcount_glide_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, redis, key, key_len,
                                 start, end, bybit) == SUCCESS)
    {
        return;
    }

    /* Fall back to the original implementation */
    REDIS_PROCESS_CMD(bitcount, redis_long_response);
}
