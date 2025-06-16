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

/* Execute a FLUSHDB command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_flushdb_command(const void *glide_client, int async)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = FlushDB;

    if (async)
    {
        args.args[0].type = CORE_ARG_TYPE_STRING;
        args.args[0].data.string_arg.value = "ASYNC";
        args.args[0].data.string_arg.len = 5;
        args.arg_count = 1;
    }

    return execute_core_command(&args, NULL, process_core_bool_result);
}

/* Execute a FLUSHALL command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_flushall_command(const void *glide_client, int async)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = FlushAll;

    if (async)
    {
        args.args[0].type = CORE_ARG_TYPE_STRING;
        args.args[0].data.string_arg.value = "ASYNC";
        args.args[0].data.string_arg.len = 5;
        args.arg_count = 1;
    }

    return execute_core_command(&args, NULL, process_core_bool_result);
}

/* Execute a TIME command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_time_command(const void *glide_client, zval *return_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Time;

    return execute_core_command(&args, return_value, process_core_array_result);
}

/* Execute a ROLE command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_role_command(const void *glide_client, zval *return_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Role;

    return execute_core_command(&args, return_value, process_core_array_result);
}
