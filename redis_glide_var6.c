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
#include "valkey_glide_core_common.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Execute an AUTH command using the Valkey Glide client */
int execute_auth_command(const void *glide_client, const char *password, size_t password_len,
                         const char *username, size_t username_len)
{
    /* Check if client is valid */
    if (!glide_client || !password)
    {
        return 0;
    }

    /* Calculate the number of arguments */
    unsigned long arg_count = username && username_len > 0 ? 2 : 1;

    /* Allocate argument arrays */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Set up arguments */
    int arg_idx = 0;

    /* Add username if provided */
    if (username && username_len > 0)
    {
        args[arg_idx] = (uintptr_t)username;
        args_len[arg_idx] = username_len;
        arg_idx++;
    }

    /* Add password (required) */
    args[arg_idx] = (uintptr_t)password;
    args_len[arg_idx] = password_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Auth,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Check if the command was successful */
    int status = 0;

    if (result)
    {
        if (result->command_error)
        {
            /* Command failed */
            free_command_result(result);
            return 0;
        }

        if (result->response && result->response->response_type == Ok)
        {
            /* Success */
            status = 1;
        }
        free_command_result(result);
    }

    return status;
}

/* Execute a SELECT command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_select_command(const void *glide_client, long database)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Select;

    args.args[0].type = CORE_ARG_TYPE_LONG;
    args.args[0].data.long_arg.value = database;
    args.arg_count = 1;

    return execute_core_command(&args, NULL, process_core_bool_result);
}

/* Execute a SWAPDB command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_swapdb_command(const void *glide_client, long db1, long db2)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = SwapDb;

    args.args[0].type = CORE_ARG_TYPE_LONG;
    args.args[0].data.long_arg.value = db1;
    args.args[1].type = CORE_ARG_TYPE_LONG;
    args.args[1].data.long_arg.value = db2;
    args.arg_count = 2;

    return execute_core_command(&args, NULL, process_core_bool_result);
}

/* Execute a MOVE command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_move_command(const void *glide_client, const char *key, size_t key_len, long db, int *output_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Move;
    args.key = key;
    args.key_len = key_len;

    args.args[0].type = CORE_ARG_TYPE_LONG;
    args.args[0].data.long_arg.value = db;
    args.arg_count = 1;

    long result;
    int success = execute_core_command(&args, &result, process_core_int_result);
    if (success)
        *output_value = (int)result;
    return success;
}
