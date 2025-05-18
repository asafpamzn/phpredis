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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Execute an RPUSH command using the Valkey Glide client */
long execute_rpush_command(const void *glide_client, const char *key, size_t key_len, zval *values, int values_count)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !values || values_count <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1 + values_count; /* key + values */
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return 0;
    }

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Remaining arguments: values */
    int i;
    for (i = 0; i < values_count; i++)
    {
        zval *value = &values[i];
        if (Z_TYPE_P(value) != IS_STRING)
        {
            free(args);
            free(args_len);
            return 0;
        }
        args[1 + i] = (uintptr_t)Z_STRVAL_P(value);
        args_len[1 + i] = Z_STRLEN_P(value);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        RPush,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    free(args);
    free(args_len);

    /* Use the generic handler to process the result */
    long output_value = -1;
    if (handle_int_response(result, &output_value))
    {
        /* Command succeeded */
        return output_value;
    }
    else
    {
        /* Command failed */
        return 0;
    }
}
