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
