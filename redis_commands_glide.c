/*
  +----------------------------------------------------------------------+
  | Redis Glide FFI integration for phpredis - Command implementations    |
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

/* Implementation of BITOP command using Valkey Glide FFI */
PHP_REDIS_API int
redis_bitop_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                      char *op, size_t op_len, char *dst, size_t dst_len,
                      zval *keys, int keys_count)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the BITOP command using the Glide client */
    long result = execute_bitop_command(redis->glide_client, op, op_len, dst, dst_len, keys, keys_count);

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

/* Implementation of BITPOS command using Valkey Glide FFI */
PHP_REDIS_API int
redis_bitpos_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                       char *key, size_t key_len, zend_long bit,
                       zend_long start, zend_long end, zend_bool bybit)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the BITPOS command using the Glide client */
    long result = execute_bitpos_command(redis->glide_client, key, key_len, bit, start, end, bybit);

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

/* Implementation of SET command using Valkey Glide FFI */
PHP_REDIS_API int
redis_set_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                    char *key, size_t key_len, char *val, size_t val_len,
                    zend_long expire, zval *opts)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the SET command using the Glide client */
    int result = execute_set_command(redis->glide_client, key, key_len, val, val_len, expire, opts);

    /* Process the result based on the return code */
    switch (result)
    {
    case 1: /* OK */
        RETVAL_TRUE;
        return SUCCESS;
    case 0: /* Not OK (e.g., NX/XX condition not met) */
        RETVAL_FALSE;
        return SUCCESS;
    case 2: /* Value returned (GET option) */
        /* This case is handled by the execute_set_command function */
        return SUCCESS;
    default: /* Error */
        RETVAL_FALSE;
        return SUCCESS;
    }
}

/* Implementation of SETEX command using Valkey Glide FFI */
PHP_REDIS_API int
redis_setex_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                      char *key, size_t key_len, zend_long expire, char *val, size_t val_len)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the SETEX command using the Glide client */
    int result = execute_setex_command(redis->glide_client, key, key_len, expire, val, val_len);

    /* If the result is 1, the command was successful */
    if (result == 1)
    {
        RETVAL_TRUE;
        return SUCCESS;
    }

    /* Otherwise, there was an error */
    RETVAL_FALSE;
    return SUCCESS;
}

/* Implementation of PSETEX command using Valkey Glide FFI */
PHP_REDIS_API int
redis_psetex_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                       char *key, size_t key_len, zend_long expire, char *val, size_t val_len)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the PSETEX command using the Glide client */
    int result = execute_psetex_command(redis->glide_client, key, key_len, expire, val, val_len);

    /* If the result is 1, the command was successful */
    if (result == 1)
    {
        RETVAL_TRUE;
        return SUCCESS;
    }

    /* Otherwise, there was an error */
    RETVAL_FALSE;
    return SUCCESS;
}

/* Implementation of SETNX command using Valkey Glide FFI */
PHP_REDIS_API int
redis_setnx_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                      char *key, size_t key_len, char *val, size_t val_len)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the SETNX command using the Glide client */
    int result = execute_setnx_command(redis->glide_client, key, key_len, val, val_len);

    /* If the result is 1, the key was set */
    if (result == 1)
    {
        RETVAL_TRUE;
        return SUCCESS;
    }

    /* Otherwise, the key already exists */
    RETVAL_FALSE;
    return SUCCESS;
}

/* Implementation of GETSET command using Valkey Glide FFI */
PHP_REDIS_API int
redis_getset_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                       char *key, size_t key_len, char *val, size_t val_len)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the GETSET command using the Glide client */
    char *result;
    size_t result_len;
    int status = execute_getset_command(redis->glide_client, key, key_len, val, val_len, &result, &result_len);

    /* If the status is -1, there was an error */
    if (status == -1)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* If the result is NULL, return NULL */
    if (result == NULL)
    {
        RETVAL_NULL();
        return SUCCESS;
    }

    /* Otherwise, return the result string */
    RETVAL_STRINGL(result, result_len);
    free(result); /* Free the result string allocated by execute_getset_command */
    return SUCCESS;
}

/* Implementation of RANDOMKEY command using Valkey Glide FFI */
PHP_REDIS_API int
redis_randomkey_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the RANDOMKEY command using the Glide client */
    char *result;
    size_t result_len;
    int status = execute_randomkey_command(redis->glide_client, &result, &result_len);

    /* If the status is -1, there was an error */
    if (status == -1)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* If the result is NULL, return FALSE (empty database) */
    if (result == NULL)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* Otherwise, return the result string */
    RETVAL_STRINGL(result, result_len);
    free(result); /* Free the result string allocated by execute_randomkey_command */
    return SUCCESS;
}

/* Implementation of ECHO command using Valkey Glide FFI */
PHP_REDIS_API int
redis_echo_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                     char *msg, size_t msg_len)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the ECHO command using the Glide client */
    char *result;
    size_t result_len;
    int status = execute_echo_command(redis->glide_client, msg, msg_len, &result, &result_len);

    /* If the status is -1, there was an error */
    if (status == -1)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* Return the echoed message */
    RETVAL_STRINGL(result, result_len);
    free(result); /* Free the result string allocated by execute_echo_command */
    return SUCCESS;
}

/* Implementation of RENAME command using Valkey Glide FFI */
PHP_REDIS_API int
redis_rename_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                       char *src, size_t src_len, char *dst, size_t dst_len)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the RENAME command using the Glide client */
    int result = execute_rename_command(redis->glide_client, src, src_len, dst, dst_len);

    /* If the result is 1, the command was successful */
    if (result == 1)
    {
        RETVAL_TRUE;
        return SUCCESS;
    }

    /* Otherwise, there was an error */
    RETVAL_FALSE;
    return SUCCESS;
}

/* Implementation of RENAMENX command using Valkey Glide FFI */
PHP_REDIS_API int
redis_renamenx_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                         char *src, size_t src_len, char *dst, size_t dst_len)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the RENAMENX command using the Glide client */
    int result = execute_renamenx_command(redis->glide_client, src, src_len, dst, dst_len);

    /* If the result is 1, the key was renamed */
    if (result == 1)
    {
        RETVAL_TRUE;
        return SUCCESS;
    }

    /* Otherwise, the destination key already exists */
    RETVAL_FALSE;
    return SUCCESS;
}

/* Implementation of RESET command using Valkey Glide FFI */
PHP_REDIS_API int
redis_reset_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the RESET command using the Glide client */
    int result = execute_reset_command(redis->glide_client);

    /* If the result is 1, the command was successful */
    if (result == 1)
    {
        RETVAL_TRUE;
        return SUCCESS;
    }

    /* Otherwise, there was an error */
    RETVAL_FALSE;
    return SUCCESS;
}

/* Implementation of GET command using Valkey Glide FFI */
PHP_REDIS_API int
redis_get_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                    char *key, size_t key_len)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the GET command using the Glide client */
    char *result;
    size_t result_len;
    int status = execute_get_command(redis->glide_client, key, key_len, &result, &result_len);

    /* If the status is -1, there was an error */
    if (status == -1)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* If the result is NULL, return FALSE (key doesn't exist) */
    if (result == NULL)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* Otherwise, return the result string */
    RETVAL_STRINGL(result, result_len);
    free(result); /* Free the result string allocated by execute_get_command */
    return SUCCESS;
}

/* Implementation of GETWITHMETA command using Valkey Glide FFI */
PHP_REDIS_API int
redis_getwithmeta_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                            char *key, size_t key_len)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the GET command with metadata using the Glide client */
    char *result;
    size_t result_len;
    int status = execute_getwithmeta_command(redis->glide_client, key, key_len, &result, &result_len);

    /* If the status is -1, there was an error */
    if (status == -1)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* If the result is NULL, return FALSE (key doesn't exist) */
    if (result == NULL)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* Otherwise, return the result string */
    RETVAL_STRINGL(result, result_len);
    free(result); /* Free the result string allocated by execute_getwithmeta_command */
    return SUCCESS;
}

/* Implementation of GETDEL command using Valkey Glide FFI */
PHP_REDIS_API int
redis_getdel_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                       char *key, size_t key_len)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the GETDEL command using the Glide client */
    char *result;
    size_t result_len;
    int status = execute_getdel_command(redis->glide_client, key, key_len, &result, &result_len);

    /* If the status is -1, there was an error */
    if (status == -1)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* If the result is NULL, return FALSE (key doesn't exist) */
    if (result == NULL)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* Otherwise, return the result string */
    RETVAL_STRINGL(result, result_len);
    free(result); /* Free the result string allocated by execute_getdel_command */
    return SUCCESS;
}

/* Implementation of GETEX command using Valkey Glide FFI */
PHP_REDIS_API int
redis_getex_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                      char *key, size_t key_len, zval *opts)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the GETEX command using the Glide client */
    char *result;
    size_t result_len;
    int status = execute_getex_command(redis->glide_client, key, key_len, opts, &result, &result_len);

    /* If the status is -1, there was an error */
    if (status == -1)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* If the result is NULL, return FALSE (key doesn't exist) */
    if (result == NULL)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* Otherwise, return the result string */
    RETVAL_STRINGL(result, result_len);
    free(result); /* Free the result string allocated by execute_getex_command */
    return SUCCESS;
}

/* Implementation of PING command using Valkey Glide FFI */
PHP_REDIS_API int
redis_ping_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                     char *msg, size_t msg_len)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the PING command using the Glide client */
    char *result;
    size_t result_len;
    int status = execute_ping_command(redis->glide_client, msg, msg_len, &result, &result_len);

    /* If the status is -1, there was an error */
    if (status == -1)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* Return the result */
    RETVAL_STRINGL(result, result_len);
    free(result); /* Free the result string allocated by execute_ping_command */
    return SUCCESS;
}

/* Implementation of INCR command using Valkey Glide FFI */
PHP_REDIS_API int
redis_incr_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                     char *key, size_t key_len)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the INCR command using the Glide client */
    long result = execute_incr_command(redis->glide_client, key, key_len);

    /* If the result is LONG_MIN, there was an error */
    if (result == LONG_MIN)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* Return the incremented value */
    RETVAL_LONG(result);
    return SUCCESS;
}

/* Implementation of INCRBY command using Valkey Glide FFI */
PHP_REDIS_API int
redis_incrby_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                       char *key, size_t key_len, zend_long value)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the INCRBY command using the Glide client */
    long result = execute_incrby_command(redis->glide_client, key, key_len, value);

    /* If the result is LONG_MIN, there was an error */
    if (result == LONG_MIN)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* Return the incremented value */
    RETVAL_LONG(result);
    return SUCCESS;
}

/* Implementation of INCRBYFLOAT command using Valkey Glide FFI */
PHP_REDIS_API int
redis_incrbyfloat_glide_cmd(INTERNAL_FUNCTION_PARAMETERS, redis_object *redis,
                            char *key, size_t key_len, double value)
{
    /* If the Glide client is not available, return FAILURE to fall back to the original implementation */
    if (!redis->glide_client)
    {
        return FAILURE;
    }

    /* Execute the INCRBYFLOAT command using the Glide client */
    double result;
    int status = execute_incrbyfloat_command(redis->glide_client, key, key_len, value, &result);

    /* If the status is -1, there was an error */
    if (status == -1)
    {
        RETVAL_FALSE;
        return SUCCESS;
    }

    /* Return the incremented value */
    RETVAL_DOUBLE(result);
    return SUCCESS;
}
