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

/* Execute a FLUSHDB command using the Valkey Glide client */
int execute_flushdb_command(const void *glide_client, int async)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = async ? 1 : 0; /* Optional ASYNC argument */
    uintptr_t args[1];
    unsigned long args_len[1];

    /* Set up arguments */
    if (async)
    {
        args[0] = (uintptr_t)"ASYNC";
        args_len[0] = 5;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        FlushDB,   /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the proper handler for OK response */
    int status = handle_ok_response(result);

    /* Convert response status to boolean */
    return (status == 1) ? 1 : 0;
}

/* Execute a FLUSHALL command using the Valkey Glide client */
int execute_flushall_command(const void *glide_client, int async)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = async ? 1 : 0; /* Optional ASYNC argument */
    uintptr_t args[1];
    unsigned long args_len[1];

    /* Set up arguments */
    if (async)
    {
        args[0] = (uintptr_t)"ASYNC";
        args_len[0] = 5;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        FlushAll,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the proper handler for OK response */
    int status = handle_ok_response(result);

    /* Convert response status to boolean */
    return (status == 1) ? 1 : 0;
}

/* Execute a TIME command using the Valkey Glide client */
int execute_time_command(const void *glide_client, zval *return_value)
{
    /* Check if client is valid */
    if (!glide_client || !return_value)
    {
        return 0;
    }

    /* No arguments for TIME command */
    unsigned long arg_count = 0;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Time,      /* command type */
        arg_count, /* number of arguments */
        NULL,      /* no arguments */
        NULL       /* no argument lengths */
    );

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing TIME command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    if (result->response && result->response->response_type == Array)
    {
        /* Initialize the return array */
        array_init(return_value);

        /* Redis TIME command returns array with [unix_timestamp, microseconds] */
        if (result->response->array_value_len >= 2)
        {
            /* Process the timestamp */
            if (result->response->array_value[0].response_type == String)
            {
                add_next_index_string(return_value, result->response->array_value[0].string_value);
            }
            /* Process the microseconds */
            if (result->response->array_value[1].response_type == String)
            {
                add_next_index_string(return_value, result->response->array_value[1].string_value);
            }
        }

        free_command_result(result);
        return 1;
    }

    /* Free the result */
    free_command_result(result);
    return 0;
}

/* Execute a ROLE command using the Valkey Glide client */
int execute_role_command(const void *glide_client, zval *return_value)
{
    /* Check if client is valid */
    if (!glide_client || !return_value)
    {
        return 0;
    }

    /* No arguments for ROLE command */
    unsigned long arg_count = 0;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Role,      /* command type */
        arg_count, /* number of arguments */
        NULL,      /* no arguments */
        NULL       /* no argument lengths */
    );

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing ROLE command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    int ret_val = 0;
    if (result->response)
    {
        /* Convert response to PHP array */
        ret_val = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
    }

    /* Free the result */
    free_command_result(result);
    return ret_val;
}
