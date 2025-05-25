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

/* Execute a WAIT command using the Valkey Glide client */
int execute_wait_command(const void *glide_client, long numreplicas, long timeout, long *output_value)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2; /* numreplicas, timeout */
    uintptr_t args[2];
    unsigned long args_len[2];

    /* Convert numeric arguments to strings */
    char numreplicas_str[32], timeout_str[32];
    snprintf(numreplicas_str, sizeof(numreplicas_str), "%ld", numreplicas);
    snprintf(timeout_str, sizeof(timeout_str), "%ld", timeout);

    /* Set up arguments */
    args[0] = (uintptr_t)numreplicas_str;
    args_len[0] = strlen(numreplicas_str);
    args[1] = (uintptr_t)timeout_str;
    args_len[1] = strlen(timeout_str);

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Wait,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the proper handler for integer response */
    return handle_int_response(result, output_value);
}

/* Execute a FUNCTION command using the Valkey Glide client */
int execute_function_command(const void *glide_client, zval *args, int args_count, zval *return_value)
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

    /* Set the first argument as "FUNCTION" */
    const char *function_cmd = "FUNCTION";
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

    final_args[0] = (uintptr_t)function_cmd;
    final_args_len[0] = strlen(function_cmd);

    /* Copy the rest of the arguments */
    for (i = 0; i < arg_count; i++)
    {
        final_args[i + 1] = cmd_args[i];
        final_args_len[i + 1] = args_len[i];
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        CustomCommand, /* FUNCTION commands use custom command type */
        arg_count + 1, /* FUNCTION command + args */
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
            /* FUNCTION can return various types based on subcommand */
            status = command_response_to_zval(result->response, return_value, 0);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute a MULTI command using the Valkey Glide client */
int execute_multi_command(const void *glide_client)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
    }

    /* No arguments for MULTI */
    unsigned long arg_count = 0;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Multi,     /* command type */
        arg_count, /* number of arguments */
        NULL,      /* no arguments */
        NULL       /* no argument lengths */
    );

    /* Use the proper handler for OK response */
    int status = handle_ok_response(result);

    /* Convert response status to boolean */
    return (status == 1) ? 1 : 0;
}

/* Execute a DISCARD command using the Valkey Glide client */
int execute_discard_command(const void *glide_client)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
    }

    /* No arguments for DISCARD */
    unsigned long arg_count = 0;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Discard,   /* command type */
        arg_count, /* number of arguments */
        NULL,      /* no arguments */
        NULL       /* no argument lengths */
    );

    /* Use the proper handler for OK response */
    int status = handle_ok_response(result);

    /* Convert response status to boolean */
    return (status == 1) ? 1 : 0;
}

/* Execute an EXEC command using the Valkey Glide client */
int execute_exec_command(const void *glide_client, zval *return_value)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
    }

    /* No arguments for EXEC */
    unsigned long arg_count = 0;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Exec,      /* command type */
        arg_count, /* number of arguments */
        NULL,      /* no arguments */
        NULL       /* no argument lengths */
    );

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
            /* EXEC returns an array of results */
            status = command_response_to_zval(result->response, return_value, 0);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute an FCALL command using the Valkey Glide client */
int execute_fcall_command(const void *glide_client, const char *name, size_t name_len,
                          int numkeys, zval *args, int args_count, zval *return_value)
{
    /* Check if client and name are valid */
    if (!glide_client || !name || name_len <= 0)
    {
        return 0;
    }

    /* Prepare numkeys as string */
    char numkeys_str[32];
    snprintf(numkeys_str, sizeof(numkeys_str), "%d", numkeys);

    /* Calculate total arguments: function_name + numkeys + all additional args */
    unsigned long arg_count = 2 + args_count; /* name + numkeys + additional args */
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

    /* Set function name and numkeys */
    cmd_args[0] = (uintptr_t)name;
    args_len[0] = name_len;
    cmd_args[1] = (uintptr_t)numkeys_str;
    args_len[1] = strlen(numkeys_str);

    /* Convert additional arguments to strings if needed */
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
            cmd_args[i + 2] = (uintptr_t)Z_STRVAL(temp);
            args_len[i + 2] = Z_STRLEN(temp);
            zval_dtor(&temp);
        }
        else
        {
            cmd_args[i + 2] = (uintptr_t)Z_STRVAL_P(arg);
            args_len[i + 2] = Z_STRLEN_P(arg);
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        FCall,     /* command type */
        arg_count, /* number of arguments */
        cmd_args,  /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(cmd_args);
    efree(args_len);

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
            /* FCALL can return various types */
            status = command_response_to_zval(result->response, return_value, 0);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute an FCALL_RO command using the Valkey Glide client */
int execute_fcall_ro_command(const void *glide_client, const char *name, size_t name_len,
                             int numkeys, zval *args, int args_count, zval *return_value)
{
    /* Check if client and name are valid */
    if (!glide_client || !name || name_len <= 0)
    {
        return 0;
    }

    /* Prepare numkeys as string */
    char numkeys_str[32];
    snprintf(numkeys_str, sizeof(numkeys_str), "%d", numkeys);

    /* Calculate total arguments: function_name + numkeys + all additional args */
    unsigned long arg_count = 2 + args_count; /* name + numkeys + additional args */
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

    /* Set function name and numkeys */
    cmd_args[0] = (uintptr_t)name;
    args_len[0] = name_len;
    cmd_args[1] = (uintptr_t)numkeys_str;
    args_len[1] = strlen(numkeys_str);

    /* Convert additional arguments to strings if needed */
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
            cmd_args[i + 2] = (uintptr_t)Z_STRVAL(temp);
            args_len[i + 2] = Z_STRLEN(temp);
            zval_dtor(&temp);
        }
        else
        {
            cmd_args[i + 2] = (uintptr_t)Z_STRVAL_P(arg);
            args_len[i + 2] = Z_STRLEN_P(arg);
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        FCallReadOnly, /* command type */
        arg_count,     /* number of arguments */
        cmd_args,      /* arguments */
        args_len       /* argument lengths */
    );

    /* Free the argument arrays */
    efree(cmd_args);
    efree(args_len);

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
            /* FCALL_RO can return various types */
            status = command_response_to_zval(result->response, return_value, 0);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute a DUMP command using the Valkey Glide client */
int execute_dump_command(const void *glide_client, const char *key, size_t key_len,
                         char **output, size_t *output_len)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || key_len <= 0)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1; /* key */
    uintptr_t args[1];
    unsigned long args_len[1];

    /* Set up arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Dump,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Handle string response with possible NULL result */
    return handle_null_or_string_response(result, output, output_len);
}

/* Execute a RESTORE command using the Valkey Glide client */
int execute_restore_command(const void *glide_client, const char *key, size_t key_len,
                            long ttl, const char *serialized, size_t serialized_len,
                            int replace)
{
    /* Check if client, key and serialized value are valid */
    if (!glide_client || !key || key_len <= 0 || !serialized || serialized_len <= 0)
    {
        return 0;
    }

    /* Convert TTL to string */
    char ttl_str[32];
    snprintf(ttl_str, sizeof(ttl_str), "%ld", ttl);

    /* Calculate command arguments: key + ttl + serialized + [replace] */
    unsigned long arg_count = replace ? 4 : 3;
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
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;
    args[1] = (uintptr_t)ttl_str;
    args_len[1] = strlen(ttl_str);
    args[2] = (uintptr_t)serialized;
    args_len[2] = serialized_len;

    /* Add REPLACE if needed */
    if (replace)
    {
        const char *replace_str = "REPLACE";
        args[3] = (uintptr_t)replace_str;
        args_len[3] = strlen(replace_str);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Restore,   /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Process the result */
    int status = handle_ok_response(result);

    /* Convert response status to boolean */
    return (status == 1) ? 1 : 0;
}
