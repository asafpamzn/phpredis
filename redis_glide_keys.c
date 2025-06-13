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

/* Execute a KEYS command using the Valkey Glide client */
int execute_keys_command(const void *glide_client, const char *pattern, size_t pattern_len, zval *return_value)
{
    /* Check if client and pattern are valid */
    if (!glide_client || !pattern)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1;
    uintptr_t args[1];
    unsigned long args_len[1];

    /* Pattern argument */
    args[0] = (uintptr_t)pattern;
    args_len[0] = pattern_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Keys,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing KEYS command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    int ret_val = 0;
    if (result->response && result->response->response_type == Array)
    {
        /* Initialize the return array */
        array_init(return_value);

        /* Add array elements to the result */
        for (int i = 0; i < result->response->array_value_len; i++)
        {
            CommandResponse *element = &result->response->array_value[i];

            /* Process each element based on its type */
            if (element->response_type == String)
            {
                add_next_index_stringl(return_value, element->string_value, element->string_value_len);
            }
        }

        /* Command succeeded */
        ret_val = 1;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}
