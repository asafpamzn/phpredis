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

/*
 * Function to execute the OBJECT command using Glide client
 * Returns:
 *  1 on success
 *  0 when key doesn't exist
 * -1 on error
 */
int execute_object_command(const void *glide_client,
                           const char *subcommand, size_t subcommand_len,
                           const char *key, size_t key_len,
                           zval *return_value)
{
    CommandResult *result = NULL;
    int ret_val = -1; /* Default to error */

    /* Create command array: ["OBJECT", subcommand, key] */
    uintptr_t args[1];
    unsigned long args_len[1];

    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Select appropriate request type based on subcommand */
    enum RequestType req_type = CustomCommand; /* Default to CustomCommand */

    if (strncasecmp(subcommand, "REFCOUNT", subcommand_len) == 0)
    {
        req_type = ObjectRefCount;
    }
    else if (strncasecmp(subcommand, "IDLETIME", subcommand_len) == 0)
    {
        req_type = ObjectIdleTime;
    }
    else if (strncasecmp(subcommand, "FREQ", subcommand_len) == 0)
    {
        req_type = ObjectFreq;
    }
    else if (strncasecmp(subcommand, "ENCODING", subcommand_len) == 0)
    {
        req_type = ObjectEncoding;
    }
    /* For HELP and other subcommands, use CustomCommand (default) */

    /* Execute the command */
    result = execute_command(glide_client, req_type, 1, args, args_len);
    if (result == NULL)
    {
        return -1;
    }

    /* Handle different result types based on the subcommand */
    if (strncasecmp(subcommand, "REFCOUNT", subcommand_len) == 0 ||
        strncasecmp(subcommand, "IDLETIME", subcommand_len) == 0 ||
        strncasecmp(subcommand, "FREQ", subcommand_len) == 0)
    {
        /* These subcommands return integers */
        if (result->response && result->response->response_type == Int)
        {
            /* Success, set return value */
            ZVAL_LONG(return_value, (long)result->response->int_value);
            ret_val = 1;
        }
        else if (result->response && result->response->response_type == Null)
        {
            /* Key doesn't exist */
            ZVAL_FALSE(return_value);
            ret_val = 0;
        }
    }
    else if (strncasecmp(subcommand, "ENCODING", subcommand_len) == 0)
    {
        /* ENCODING returns a string */
        if (result->response && result->response->response_type == String)
        {
            /* Success, set return value */
            ZVAL_STRINGL(return_value, result->response->string_value, result->response->string_value_len);
            ret_val = 1;
        }
        else if (result->response && result->response->response_type == Null)
        {
            /* Key doesn't exist */
            ZVAL_FALSE(return_value);
            ret_val = 0;
        }
    }
    else if (strncasecmp(subcommand, "HELP", subcommand_len) == 0)
    {
        /* HELP returns an array of strings */
        if (result->response && result->response->response_type == Array)
        {
            if (command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false) == 1)
            {
                ret_val = 1;
            }
            else
            {
                ret_val = -1;
            }
        }
        else
        {
            ret_val = -1;
        }
    }
    else
    {
        /* Unsupported subcommand */
        ret_val = -1;
    }

    /* Clean up */
    free_command_result(result);

    return ret_val;
}

/* Execute a RENAME command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_rename_command(const void *glide_client, const char *src, size_t src_len, const char *dst, size_t dst_len)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Rename;
    args.key = src;
    args.key_len = src_len;

    /* Add destination key as argument */
    args.args[0].type = CORE_ARG_TYPE_STRING;
    args.args[0].data.string_arg.value = dst;
    args.args[0].data.string_arg.len = dst_len;
    args.arg_count = 1;

    return execute_core_command(&args, NULL, process_core_bool_result);
}

/* Execute a RENAMENX command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_renamenx_command(const void *glide_client, const char *src, size_t src_len, const char *dst, size_t dst_len)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = RenameNX;
    args.key = src;
    args.key_len = src_len;

    /* Add destination key as argument */
    args.args[0].type = CORE_ARG_TYPE_STRING;
    args.args[0].data.string_arg.value = dst;
    args.args[0].data.string_arg.len = dst_len;
    args.arg_count = 1;

    return execute_core_command(&args, NULL, process_core_bool_result);
}

/* Execute a GETWITHMETA command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_getwithmeta_command(const void *glide_client, const char *key, size_t key_len, char **result, size_t *result_len)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Get; /* Using GET for now, replace with GETWITHMETA when available */
    args.key = key;
    args.key_len = key_len;

    /* Use string result processor */
    struct
    {
        char **result;
        size_t *result_len;
    } output = {result, result_len};

    return execute_core_command(&args, &output, process_core_string_result);
}

/* Execute a GETDEL command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_getdel_command(const void *glide_client, const char *key, size_t key_len, char **result, size_t *result_len)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = GetDel;
    args.key = key;
    args.key_len = key_len;

    /* Use string result processor */
    struct
    {
        char **result;
        size_t *result_len;
    } output = {result, result_len};

    return execute_core_command(&args, &output, process_core_string_result);
}

/* Execute a GETEX command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_getex_command(const void *glide_client, const char *key, size_t key_len, zval *opts, char **result, size_t *result_len)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = GetEx;
    args.key = key;
    args.key_len = key_len;
    args.raw_options = opts;

    /* Parse options using existing core framework option parsing */
    if (opts)
    {
        parse_core_options(opts, &args.options);
    }

    /* Use string result processor */
    struct
    {
        char **result;
        size_t *result_len;
    } output = {result, result_len};

    return execute_core_command(&args, &output, process_core_string_result);
}

/* Execute an INCR command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
long execute_incr_command(const void *glide_client, const char *key, size_t key_len)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Incr;
    args.key = key;
    args.key_len = key_len;

    long result;
    return execute_core_command(&args, &result, process_core_int_result) ? result : 0;
}

/* Execute an INCRBY command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
long execute_incrby_command(const void *glide_client, const char *key, size_t key_len, long increment)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = IncrBy;
    args.key = key;
    args.key_len = key_len;

    /* Add increment argument */
    args.args[0].type = CORE_ARG_TYPE_LONG;
    args.args[0].data.long_arg.value = increment;
    args.arg_count = 1;

    long result;
    return execute_core_command(&args, &result, process_core_int_result) ? result : 0;
}

/* Execute an INCRBYFLOAT command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_incrbyfloat_command(const void *glide_client, const char *key, size_t key_len, double increment, double *result_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = IncrByFloat;
    args.key = key;
    args.key_len = key_len;

    /* Add increment argument */
    args.args[0].type = CORE_ARG_TYPE_DOUBLE;
    args.args[0].data.double_arg.value = increment;
    args.arg_count = 1;

    return execute_core_command(&args, result_value, process_core_double_result);
}

/* Execute a DECR command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_decr_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Decr;
    args.key = key;
    args.key_len = key_len;

    return execute_core_command(&args, output_value, process_core_int_result);
}

/* Execute a DECRBY command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_decrby_command(const void *glide_client, const char *key, size_t key_len, long decrement, long *output_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = DecrBy;
    args.key = key;
    args.key_len = key_len;

    /* Add decrement argument */
    args.args[0].type = CORE_ARG_TYPE_LONG;
    args.args[0].data.long_arg.value = decrement;
    args.arg_count = 1;

    return execute_core_command(&args, output_value, process_core_int_result);
}

/* Execute an MGET command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_mget_command(const void *glide_client, zval *keys, zval *return_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = MGet;

    /* Set up array argument for keys */
    args.args[0].type = CORE_ARG_TYPE_ARRAY;
    args.args[0].data.array_arg.array = keys;
    args.args[0].data.array_arg.count = zend_hash_num_elements(Z_ARRVAL_P(keys));
    args.arg_count = 1;

    return execute_core_command(&args, return_value, process_core_array_result);
}

/* Execute an EXISTS command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_exists_command(const void *glide_client, zval *keys, int keys_count, long *output_value)
{
    return execute_multi_key_command(glide_client, Exists, keys, keys_count, output_value);
}

/* Execute a TOUCH command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_touch_command(const void *glide_client, zval *keys, int keys_count, long *output_value)
{
    return execute_multi_key_command(glide_client, Touch, keys, keys_count, output_value);
}

/* Execute an UNLINK command using the Valkey Glide client - UNIFIED IMPLEMENTATION */
int execute_unlink_command(const void *glide_client, zval *keys, int keys_count, long *output_value)
{
    return execute_multi_key_command(glide_client, Unlink, keys, keys_count, output_value);
}
