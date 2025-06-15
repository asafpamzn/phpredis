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

/* Execute a setOption command using the Valkey Glide client */
int execute_setOption_command(const void *glide_client, zend_long option, zval *value)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
    }

    /* Currently, Glide client doesn't directly support setting client options,
     * so we store them in the local Redis object. We'll return success for now. */
    return 1;
}

/* Execute an MSET command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_mset_command(const void *glide_client, zval *arr)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = MSet;

    /* Set up array argument for key-value pairs */
    args.args[0].type = CORE_ARG_TYPE_ARRAY;
    args.args[0].data.array_arg.array = arr;
    args.args[0].data.array_arg.count = zend_hash_num_elements(Z_ARRVAL_P(arr));
    args.arg_count = 1;

    return execute_core_command(&args, NULL, process_core_bool_result);
}

/* Execute an MSETNX command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_msetnx_command(const void *glide_client, zval *arr, long *output_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = MSetNX;

    /* Set up array argument for key-value pairs */
    args.args[0].type = CORE_ARG_TYPE_ARRAY;
    args.args[0].data.array_arg.array = arr;
    args.args[0].data.array_arg.count = zend_hash_num_elements(Z_ARRVAL_P(arr));
    args.arg_count = 1;

    /* Convert output to long for compatibility */

    int result = execute_core_command(&args, output_value, process_core_bool_result);

    return result;
}
