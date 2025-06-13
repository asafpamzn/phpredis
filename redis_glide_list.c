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
#include "command_response.h"
#include "include/glide_bindings.h"
#include "redis_glide_list_common.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Execute an LPUSH command using the Valkey Glide client */
long execute_lpush_command(const void *glide_client, const char *key, size_t key_len, zval *values, int values_count)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !values || values_count <= 0)
    {
        return 0;
    }

    /* Call the common framework function */
    long output_value = 0;
    if (execute_list_push_command(glide_client, LPush, key, key_len, values, values_count, &output_value))
    {
        return output_value;
    }
    else
    {
        return 0;
    }
}

/* Execute an LPUSHX command using the Valkey Glide client */
long execute_lpushx_command(const void *glide_client, const char *key, size_t key_len, zval *values, int values_count)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !values || values_count <= 0)
    {
        return 0;
    }

    /* Call the common framework function */
    long output_value = 0;
    if (execute_list_push_command(glide_client, LPushX, key, key_len, values, values_count, &output_value))
    {
        return output_value;
    }
    else
    {
        return 0;
    }
}

/* Execute an RPUSHX command using the Valkey Glide client */
long execute_rpushx_command(const void *glide_client, const char *key, size_t key_len, zval *values, int values_count)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !values || values_count <= 0)
    {
        return 0;
    }

    /* Call the common framework function */
    long output_value = 0;
    if (execute_list_push_command(glide_client, RPushX, key, key_len, values, values_count, &output_value))
    {
        return output_value;
    }
    else
    {
        return 0;
    }
}

/* Execute an LPOP command using the Valkey Glide client */
int execute_lpop_command(const void *glide_client, const char *key, size_t key_len, zend_long count, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Call the common framework function */
    return execute_list_pop_command(glide_client, LPop, key, key_len, count, return_value);
}

/* Execute an RPOP command using the Valkey Glide client */
int execute_rpop_command(const void *glide_client, const char *key, size_t key_len, zend_long count, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Call the common framework function */
    return execute_list_pop_command(glide_client, RPop, key, key_len, count, return_value);
}

/* Execute a BLPOP command using the Valkey Glide client */
int execute_blpop_command(const void *glide_client, zval *keys, double timeout, zval *return_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys)
    {
        return 0;
    }

    /* Call the common framework function */
    return execute_list_blocking_pop_command(glide_client, BLPop, keys, timeout, return_value);
}

/* Execute a BRPOP command using the Valkey Glide client */
int execute_brpop_command(const void *glide_client, zval *keys, double timeout, zval *return_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys)
    {
        return 0;
    }

    /* Call the common framework function */
    return execute_list_blocking_pop_command(glide_client, BRPop, keys, timeout, return_value);
}
/* Execute a BLMOVE command using the Valkey Glide client */
int execute_blmove_command(const void *glide_client, const char *src, size_t src_len,
                           const char *dst, size_t dst_len, const char *wherefrom,
                           size_t wherefrom_len, const char *whereto, size_t whereto_len,
                           double timeout, char **output_value, size_t *output_len)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !src || !dst || !wherefrom || !whereto || !output_value || !output_len)
    {
        return -1;
    }

    /* Call the common framework function */
    return execute_list_move_command(
        glide_client,
        BLMove,
        src, src_len,
        dst, dst_len,
        wherefrom, wherefrom_len,
        whereto, whereto_len,
        timeout,
        output_value, output_len);
}

/* Execute an LMOVE command using the Valkey Glide client */
int execute_lmove_command(const void *glide_client, const char *src, size_t src_len,
                          const char *dst, size_t dst_len, const char *wherefrom,
                          size_t wherefrom_len, const char *whereto, size_t whereto_len,
                          char **output_value, size_t *output_len)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !src || !dst || !wherefrom || !whereto || !output_value || !output_len)
    {
        return -1;
    }

    /* Call the common framework function with timeout -1.0 to indicate non-blocking */
    return execute_list_move_command(
        glide_client,
        LMove,
        src, src_len,
        dst, dst_len,
        wherefrom, wherefrom_len,
        whereto, whereto_len,
        -1.0,
        output_value, output_len);
}

/* Execute an LREM command using the Valkey Glide client */
int execute_lrem_command(const void *glide_client, const char *key, size_t key_len,
                         long count, const char *value, size_t value_len,
                         long *output_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || !value || !output_value)
    {
        return 0;
    }

    /* Call the common framework function */
    return execute_list_rem_command(glide_client, key, key_len, count, value, value_len, output_value);
}

/* Execute an LTRIM command using the Valkey Glide client */
int execute_ltrim_command(const void *glide_client, const char *key, size_t key_len,
                          long start, long end)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Call the common framework function */
    return execute_list_trim_command(glide_client, key, key_len, start, end);
}

/* Execute an LINDEX command using the Valkey Glide client */
int execute_lindex_command(const void *glide_client, const char *key, size_t key_len,
                           long index, char **output_value, size_t *output_len)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || !output_value || !output_len)
    {
        return -1;
    }

    /* Call the common framework function */
    return execute_list_index_command(glide_client, key, key_len, index, output_value, output_len);
}

/* Execute an LSET command using the Valkey Glide client */
int execute_lset_command(const void *glide_client, const char *key, size_t key_len,
                         long index, const char *value, size_t value_len)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || !value)
    {
        return 0;
    }

    /* Call the common framework function */
    return execute_list_set_command(glide_client, key, key_len, index, value, value_len);
}
