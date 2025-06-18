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

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* Execute an MSET command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_mset_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    zval *z_arr;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Oa",
                                     &object, redis_ce, &z_arr) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        core_command_args_t args = {0};
        args.glide_client = redis->glide_client;
        args.cmd_type = MSet;

        /* Set up array argument for key-value pairs */
        args.args[0].type = CORE_ARG_TYPE_ARRAY;
        args.args[0].data.array_arg.array = z_arr;
        args.args[0].data.array_arg.count = zend_hash_num_elements(Z_ARRVAL_P(z_arr));
        args.arg_count = 1;

        if (execute_core_command(&args, NULL, process_core_bool_result))
        {
            ZVAL_TRUE(return_value);
            return 1;
        }
    }

    return 0;
}

/* Execute an MSETNX command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_msetnx_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    zval *z_arr;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Oa",
                                     &object, redis_ce, &z_arr) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        core_command_args_t args = {0};
        args.glide_client = redis->glide_client;
        args.cmd_type = MSetNX;

        /* Set up array argument for key-value pairs */
        args.args[0].type = CORE_ARG_TYPE_ARRAY;
        args.args[0].data.array_arg.array = z_arr;
        args.args[0].data.array_arg.count = zend_hash_num_elements(Z_ARRVAL_P(z_arr));
        args.arg_count = 1;

        if (execute_core_command(&args, NULL, process_core_bool_result))
        {
            ZVAL_TRUE(return_value);
            return 1;
        }
    }

    return 0;
}

/* Execute a FLUSHDB command using the Valkey Glide client - UNIFIED IMPLEMENTATION */
int execute_flushdb_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    zend_bool async = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "O|b",
                                     &object, redis_ce, &async) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    if (!redis || !redis->glide_client)
    {
        return 0;
    }

    /* Execute using core framework */
    core_command_args_t args = {0};
    args.glide_client = redis->glide_client;
    args.cmd_type = FlushDB;

    /* Add ASYNC option if requested */
    if (async)
    {
        args.args[0].type = CORE_ARG_TYPE_STRING;
        args.args[0].data.string_arg.value = "ASYNC";
        args.args[0].data.string_arg.len = 5;
        args.arg_count = 1;
    }

    if (execute_core_command(&args, NULL, process_core_bool_result))
    {
        ZVAL_TRUE(return_value);
        return 1;
    }
    else
    {
        ZVAL_FALSE(return_value);
        return 0;
    }
}

/* Execute a FLUSHALL command using the Valkey Glide client - UNIFIED IMPLEMENTATION */
int execute_flushall_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    zend_bool async = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "O|b",
                                     &object, redis_ce, &async) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    if (!redis || !redis->glide_client)
    {
        return 0;
    }

    /* Execute using core framework */
    core_command_args_t args = {0};
    args.glide_client = redis->glide_client;
    args.cmd_type = FlushAll;

    /* Add ASYNC option if requested */
    if (async)
    {
        args.args[0].type = CORE_ARG_TYPE_STRING;
        args.args[0].data.string_arg.value = "ASYNC";
        args.args[0].data.string_arg.len = 5;
        args.arg_count = 1;
    }

    if (execute_core_command(&args, NULL, process_core_bool_result))
    {
        ZVAL_TRUE(return_value);
        return 1;
    }
    else
    {
        ZVAL_FALSE(return_value);
        return 0;
    }
}

/* Execute a TIME command using the Valkey Glide client - UNIFIED IMPLEMENTATION */
int execute_time_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "O",
                                     &object, redis_ce) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    if (!redis || !redis->glide_client)
    {
        return 0;
    }

    /* Execute using core framework */
    core_command_args_t args = {0};
    args.glide_client = redis->glide_client;
    args.cmd_type = Time;

    if (execute_core_command(&args, return_value, process_core_array_result))
    {
        return 1;
    }
    else
    {
        ZVAL_FALSE(return_value);
        return 0;
    }
}

/* Execute a ROLE command using the Valkey Glide client - UNIFIED IMPLEMENTATION */
int execute_role_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "O",
                                     &object, redis_ce) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    if (!redis || !redis->glide_client)
    {
        return 0;
    }

    /* Execute using core framework */
    core_command_args_t args = {0};
    args.glide_client = redis->glide_client;
    args.cmd_type = Role;

    if (execute_core_command(&args, return_value, process_core_array_result))
    {
        return 1;
    }
    else
    {
        ZVAL_FALSE(return_value);
        return 0;
    }
}

/* Execute a KEYS command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_keys_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    char *pattern = NULL;
    size_t pattern_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Os",
                                     &object, redis_ce, &pattern, &pattern_len) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        core_command_args_t args = {0};
        args.glide_client = redis->glide_client;
        args.cmd_type = Keys;

        /* Add pattern argument */
        args.args[0].type = CORE_ARG_TYPE_STRING;
        args.args[0].data.string_arg.value = pattern;
        args.args[0].data.string_arg.len = pattern_len;
        args.arg_count = 1;

        if (execute_core_command(&args, return_value, process_core_array_result))
        {
            return 1;
        }
    }

    return 0;
}

/* Execute a WATCH command using the Valkey Glide client - UNIFIED IMPLEMENTATION */
int execute_watch_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    zval *z_args;
    int arg_count;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "O*",
                                     &object, redis_ce, &z_args, &arg_count) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    if (!redis || !redis->glide_client)
    {
        return 0;
    }

    /* Execute using core framework */
    core_command_args_t args = {0};
    args.glide_client = redis->glide_client;
    args.cmd_type = Watch;

    /* Set up array argument for keys */
    args.args[0].type = CORE_ARG_TYPE_ARRAY;
    args.args[0].data.array_arg.array = z_args;
    args.args[0].data.array_arg.count = arg_count;
    args.arg_count = 1;

    if (execute_core_command(&args, NULL, process_core_bool_result))
    {
        ZVAL_TRUE(return_value);
        return 1;
    }
    else
    {
        ZVAL_FALSE(return_value);
        return 0;
    }
}

/* Execute an UNWATCH command using the Valkey Glide client - UNIFIED IMPLEMENTATION */
int execute_unwatch_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "O",
                                     &object, redis_ce) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    if (!redis || !redis->glide_client)
    {
        return 0;
    }

    /* Execute using core framework */
    core_command_args_t args = {0};
    args.glide_client = redis->glide_client;
    args.cmd_type = UnWatch;

    if (execute_core_command(&args, NULL, process_core_bool_result))
    {
        ZVAL_TRUE(return_value);
        return 1;
    }
    else
    {
        ZVAL_FALSE(return_value);
        return 0;
    }
}

/* Execute an ACL command using the Valkey Glide client - UNIFIED IMPLEMENTATION */
int execute_acl_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    zval *z_args;
    int arg_count;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "O*",
                                     &object, redis_ce, &z_args, &arg_count) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    if (!redis || !redis->glide_client || !z_args || arg_count <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long cmd_arg_count = arg_count;
    uintptr_t *cmd_args = (uintptr_t *)emalloc(cmd_arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(cmd_arg_count * sizeof(unsigned long));

    if (!cmd_args || !args_len)
    {
        if (cmd_args)
            efree(cmd_args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Convert arguments to strings if needed */
    int i;
    for (i = 0; i < arg_count; i++)
    {
        zval *arg = &z_args[i];

        /* If not string, convert to one */
        if (Z_TYPE_P(arg) != IS_STRING)
        {
            zval temp;
            ZVAL_COPY(&temp, arg);
            convert_to_string(&temp);
            cmd_args[i] = (uintptr_t)Z_STRVAL(temp);
            args_len[i] = Z_STRLEN(temp);
            zval_dtor(&temp);
        }
        else
        {
            cmd_args[i] = (uintptr_t)Z_STRVAL_P(arg);
            args_len[i] = Z_STRLEN_P(arg);
        }
    }

    /* Set the first argument as "ACL" */
    const char *acl_cmd = "ACL";
    uintptr_t *final_args = (uintptr_t *)emalloc((cmd_arg_count + 1) * sizeof(uintptr_t));
    unsigned long *final_args_len = (unsigned long *)emalloc((cmd_arg_count + 1) * sizeof(unsigned long));

    if (!final_args || !final_args_len)
    {
        if (cmd_args)
            efree(cmd_args);
        if (args_len)
            efree(args_len);
        if (final_args)
            efree(final_args);
        if (final_args_len)
            efree(final_args_len);
        return 0;
    }

    final_args[0] = (uintptr_t)acl_cmd;
    final_args_len[0] = strlen(acl_cmd);

    /* Copy the rest of the arguments */
    for (i = 0; i < cmd_arg_count; i++)
    {
        final_args[i + 1] = cmd_args[i];
        final_args_len[i + 1] = args_len[i];
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        redis->glide_client,
        CustomCommand,     /* ACL commands use custom command type */
        cmd_arg_count + 1, /* ACL command + args */
        final_args,        /* arguments */
        final_args_len     /* argument lengths */
    );

    /* Free the argument arrays */
    efree(cmd_args);
    efree(args_len);
    efree(final_args);
    efree(final_args_len);

    /* Handle the result directly */
    int status = 0;
    if (result)
    {
        if (result->command_error)
        {
            /* Command failed */
            free_command_result(result);
            return 0;
        }

        if (result->response)
        {
            /* ACL can return various types based on subcommand */
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
            free_command_result(result);
            return status ? 1 : 0;
        }
        free_command_result(result);
    }

    return 0;
}

/* Implementation of the OBJECT command with the original signature */
int execute_object_command_impl(const void *glide_client,
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

/* New execute_object_command function with standardized signature that follows the pattern */
int execute_object_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    char *key = NULL, *subcommand = NULL;
    size_t key_len, subcommand_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Oss",
                                     &object, redis_ce, &subcommand, &subcommand_len,
                                     &key, &key_len) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the OBJECT command using the Glide client via the implementation function */
        if (execute_object_command_impl(redis->glide_client, subcommand, subcommand_len, key, key_len, return_value) >= 0)
        {
            return 1;
        }
    }

    return 0;
}
