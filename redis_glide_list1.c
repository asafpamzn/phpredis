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

/* Execute a WATCH command using the Valkey Glide client */
int execute_watch_command(const void *glide_client, zval *keys, int keys_count)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys || keys_count <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = keys_count;
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

    /* Populate arguments array */
    int i;
    for (i = 0; i < keys_count; i++)
    {
        zval *key = &keys[i];
        if (Z_TYPE_P(key) != IS_STRING)
        {
            efree(args);
            efree(args_len);
            return 0;
        }
        args[i] = (uintptr_t)Z_STRVAL_P(key);
        args_len[i] = Z_STRLEN_P(key);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Watch,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Use the proper handler for OK response */
    int status = handle_ok_response(result);

    /* Convert response status to boolean */
    return (status == 1) ? 1 : 0;
}

/* Execute an UNWATCH command using the Valkey Glide client */
int execute_unwatch_command(const void *glide_client)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
    }

    /* No arguments for UNWATCH */
    unsigned long arg_count = 0;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        UnWatch,   /* command type */
        arg_count, /* number of arguments */
        NULL,      /* no arguments */
        NULL       /* no argument lengths */
    );

    /* Use the proper handler for OK response */
    int status = handle_ok_response(result);

    /* Convert response status to boolean */
    return (status == 1) ? 1 : 0;
}

/* Execute an ACL command using the Valkey Glide client */
int execute_acl_command(const void *glide_client, zval *args, int args_count, zval *return_value)
{
    /* Check if client and args are valid */
    if (!glide_client || !args || args_count <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = args_count;
    uintptr_t *cmd_args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

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
    for (i = 0; i < args_count; i++)
    {
        zval *arg = &args[i];

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
    uintptr_t *final_args = (uintptr_t *)emalloc((arg_count + 1) * sizeof(uintptr_t));
    unsigned long *final_args_len = (unsigned long *)emalloc((arg_count + 1) * sizeof(unsigned long));

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
    for (i = 0; i < arg_count; i++)
    {
        final_args[i + 1] = cmd_args[i];
        final_args_len[i + 1] = args_len[i];
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        CustomCommand, /* ACL commands use custom command type */
        arg_count + 1, /* ACL command + args */
        final_args,    /* arguments */
        final_args_len /* argument lengths */
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
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute an LINSERT command using the Valkey Glide client */
int execute_linsert_command(const void *glide_client, const char *key, size_t key_len,
                            const char *position, size_t position_len,
                            const char *pivot, size_t pivot_len,
                            const char *value, size_t value_len,
                            long *output_value)
{
    /* Use the common framework function directly */
    return execute_list_insert_command(glide_client, key, key_len, position, position_len,
                                       pivot, pivot_len, value, value_len, output_value);
}

/* Execute an LPOS command using the Valkey Glide client */
int execute_lpos_command(const void *glide_client, const char *key, size_t key_len,
                         const char *element, size_t element_len, zval *options,
                         zval *return_value)
{
    /* Use the common framework function directly */
    return execute_list_position_command(glide_client, key, key_len, element, element_len,
                                         options, return_value);
}

/* Execute an LLEN command using the Valkey Glide client */
int execute_llen_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    /* Use the common framework function directly */
    return execute_list_len_command(glide_client, key, key_len, output_value);
}

/* Execute an RPOPLPUSH command using the Valkey Glide client */
int execute_rpoplpush_command(const void *glide_client, const char *src, size_t src_len, const char *dst, size_t dst_len, char **result, size_t *result_len)
{
    /* Check if client and keys are valid */
    if (!glide_client || !src || !dst || !result || !result_len)
    {
        return -1;
    }

    /* Use the common framework function directly */
    /* For RPOPLPUSH, we use "RIGHT" as source direction and "LEFT" as destination direction */
    return execute_list_move_command(
        glide_client,
        RPopLPush,
        src, src_len,
        dst, dst_len,
        "RIGHT", 5,
        "LEFT", 4,
        -1.0, /* No timeout for non-blocking version */
        result, result_len);
}

/* Execute a BRPOPLPUSH command using the Valkey Glide client */
int execute_brpoplpush_command(const void *glide_client, const char *src, size_t src_len, const char *dst, size_t dst_len, zend_long timeout, char **result, size_t *result_len)
{
    /* Check if client and keys are valid */
    if (!glide_client || !src || !dst || !result || !result_len)
    {
        return -1;
    }

    /* Use the common framework function directly */
    /* For BRPOPLPUSH, we use "RIGHT" as source direction and "LEFT" as destination direction */
    return execute_list_move_command(
        glide_client,
        BRPopLPush,
        src, src_len,
        dst, dst_len,
        "RIGHT", 5,
        "LEFT", 4,
        (double)timeout, /* Convert timeout to double for the framework */
        result, result_len);
}

/*
 * Note: The prepare_mpop_arguments helper function has been removed as it's
 * no longer needed after refactoring to use the common framework functions
 * provided by redis_glide_list_common.c
 */

/* Execute an RPUSH command using the Valkey Glide client */
long execute_rpush_command(const void *glide_client, const char *key, size_t key_len, zval *values, int values_count)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !values || values_count <= 0)
    {
        return 0;
    }

    /* Call the common framework function - pass values_count to ensure all values are sent */
    long output_value = 0;
    if (execute_list_push_command(glide_client, RPush, key, key_len, values, values_count, &output_value))
    {
        return output_value;
    }
    else
    {
        return 0;
    }
}

/* Execute an LMPOP or BLMPOP command (for list operations) using the Valkey Glide client */
int execute_lmpop_command(const void *glide_client, const char *cmd, double timeout, zval *keys, const char *from, size_t from_len, long count, zval *result)
{
    /* Check if client, keys, and from are valid */
    if (!glide_client || !keys || !from)
    {
        return 0;
    }

    /* Determine if this is a blocking command */
    int is_blocking = (strncmp(cmd, "B", 1) == 0);
    enum RequestType cmd_type = is_blocking ? BLMPop : LMPop;

    /* Only pass timeout for blocking commands, and only pass count if it's greater than 0 */
    double actual_timeout = is_blocking ? timeout : -1.0;
    long actual_count = count > 0 ? count : 0;

    /* Use the common framework function directly */
    return execute_list_mpop_command(
        glide_client,
        cmd_type,
        keys,
        from, from_len,
        actual_count,   /* Only pass non-zero count */
        actual_timeout, /* Only pass timeout for blocking version */
        result);
}
