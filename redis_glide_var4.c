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

/* Execute a COPY command using the Valkey Glide client */
int execute_copy_command(const void *glide_client, const char *src, size_t src_len,
                         const char *dst, size_t dst_len, int replace)
{
    /* Check if client and keys are valid */
    if (!glide_client || !src || !dst)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = replace ? 3 : 2;
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

    /* First argument: source key */
    args[0] = (uintptr_t)src;
    args_len[0] = src_len;

    /* Second argument: destination key */
    args[1] = (uintptr_t)dst;
    args_len[1] = dst_len;

    /* Third argument (optional): REPLACE */
    if (replace)
    {
        args[2] = (uintptr_t)"REPLACE";
        args_len[2] = 7; /* strlen("REPLACE") */
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Copy,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Process the result */
    int status = 0;
    long output_value = 0;

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
            if (result->response->response_type == Int)
            {
                output_value = result->response->int_value;
                status = (output_value == 1);
            }
            else if (result->response->response_type == Ok)
            {
                /* Some Redis versions return OK instead of 1 */
                status = 1;
            }
        }
        free_command_result(result);
    }

    return status;
}

/* Execute an HSCAN command using the Valkey Glide client */
int execute_hscan_command(const void *glide_client, const char *key, size_t key_len,
                          long cursor, char *pattern, size_t pattern_len,
                          long count, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !return_value)
    {
        return 0;
    }

    /* Calculate the number of arguments */
    unsigned long arg_count = 2; /* key + cursor */
    if (pattern && pattern_len > 0)
        arg_count += 2; /* MATCH + pattern */
    if (count > 0)
        arg_count += 2; /* COUNT + count */

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

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: cursor (convert to string) */
    size_t cursor_len;
    char *cursor_str = long_to_string(cursor, &cursor_len);
    if (!cursor_str)
    {
        efree(args);
        efree(args_len);
        return 0;
    }
    args[1] = (uintptr_t)cursor_str;
    args_len[1] = cursor_len;

    /* Track current argument index */
    int arg_idx = 2;

    /* Add pattern if provided */
    if (pattern && pattern_len > 0)
    {
        args[arg_idx] = (uintptr_t)"MATCH";
        args_len[arg_idx] = 5; /* strlen("MATCH") */
        arg_idx++;

        args[arg_idx] = (uintptr_t)pattern;
        args_len[arg_idx] = pattern_len;
        arg_idx++;
    }

    /* Add count if provided */
    if (count > 0)
    {
        args[arg_idx] = (uintptr_t)"COUNT";
        args_len[arg_idx] = 5; /* strlen("COUNT") */
        arg_idx++;

        size_t count_len;
        char *count_str = long_to_string(count, &count_len);
        if (!count_str)
        {
            efree(cursor_str);
            efree(args);
            efree(args_len);
            return 0;
        }
        args[arg_idx] = (uintptr_t)count_str;
        args_len[arg_idx] = count_len;
        arg_idx++;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        HScan,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the cursor string */
    efree(cursor_str);

    /* Free the count string if used */
    if (count > 0)
        efree((void *)args[arg_idx - 1]);

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Process the result */
    int status = 0;

    if (result)
    {
        if (result->command_error)
        {
            /* Command failed */
            free_command_result(result);
            return 0;
        }

        if (result->response && result->response->response_type == Array)
        {
            /* Convert the nested array result to PHP array */
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
        }
        free_command_result(result);
    }

    return status;
}

/* Execute a PFADD command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_pfadd_command(const void *glide_client, const char *key, size_t key_len,
                          zval *elements, int elements_count, int *output_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = PfAdd;
    args.key = key;
    args.key_len = key_len;

    /* Add elements array argument */
    args.args[0].type = CORE_ARG_TYPE_ARRAY;
    args.args[0].data.array_arg.array = elements;
    args.args[0].data.array_arg.count = elements_count;
    args.arg_count = 1;

    long result;
    int success = execute_core_command(&args, &result, process_core_int_result);
    if (success)
        *output_value = (int)result;
    return success;
}

/* Execute a PFCOUNT command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_pfcount_command(const void *glide_client, zval *keys, int keys_count, long *output_value)
{
    /* Use the existing multi-key handler that properly handles both single strings and arrays */
    return execute_multi_key_command(glide_client, PfCount, keys, keys_count, output_value);
}

/* Execute a PFMERGE command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_pfmerge_command(const void *glide_client, const char *dst, size_t dst_len,
                            zval *keys, int keys_count)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = PfMerge;
    args.key = dst; /* Destination key */
    args.key_len = dst_len;

    /* Add source keys array */
    args.args[0].type = CORE_ARG_TYPE_ARRAY;
    args.args[0].data.array_arg.array = keys;
    args.args[0].data.array_arg.count = keys_count;
    args.arg_count = 1;

    return execute_core_command(&args, NULL, process_core_bool_result);
}

/* Execute getTimeout command using the Valkey Glide client */
int execute_get_timeout_command(const void *glide_client, double *output_value)
{
    /* Check if client is valid */
    if (!glide_client || !output_value)
    {
        return 0;
    }

    /* Since this is a client configuration getter rather than a Redis command,
       we'll use a default value as this isn't directly supported by Glide */
    *output_value = 0.0; /* Default timeout */

    /* Here we'd ideally access the Glide client's configuration, but since
       we don't have direct access to it through the FFI interface, we just
       return success and the default value */

    return 1;
}
