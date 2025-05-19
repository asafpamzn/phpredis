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

/* Execute an EXPIRE command using the Valkey Glide client */
int execute_expire_command(const void *glide_client, const char *key, size_t key_len,
                           long seconds, const char *mode, size_t mode_len)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = mode ? 3 : 2;
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return 0;
    }

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: seconds */
    size_t seconds_len;
    char *seconds_str = long_to_string(seconds, &seconds_len);
    if (!seconds_str)
    {
        free(args);
        free(args_len);
        return 0;
    }
    args[1] = (uintptr_t)seconds_str;
    args_len[1] = seconds_len;

    /* Third argument (optional): mode (NX, XX, GT, LT) */
    if (mode)
    {
        args[2] = (uintptr_t)mode;
        args_len[2] = mode_len;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Expire,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated memory */
    free(seconds_str);
    free(args);
    free(args_len);
    /* Check if the command was successful */
    if (!result)
    {

        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing EXPIRE command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Get the result value */
    return handle_bool_response(result);
}

/* Execute an EXPIREAT command using the Valkey Glide client */
int execute_expireat_command(const void *glide_client, const char *key, size_t key_len,
                             long timestamp, const char *mode, size_t mode_len)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = mode ? 3 : 2;
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return 0;
    }

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: timestamp */
    size_t timestamp_len;
    char *timestamp_str = long_to_string(timestamp, &timestamp_len);
    if (!timestamp_str)
    {
        free(args);
        free(args_len);
        return 0;
    }
    args[1] = (uintptr_t)timestamp_str;
    args_len[1] = timestamp_len;

    /* Third argument (optional): mode (NX, XX, GT, LT) */
    if (mode)
    {
        args[2] = (uintptr_t)mode;
        args_len[2] = mode_len;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ExpireAt,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated memory */
    free(timestamp_str);
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing EXPIREAT command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Get the result value */
    return handle_bool_response(result);
}

/* Execute a PEXPIRE command using the Valkey Glide client */
int execute_pexpire_command(const void *glide_client, const char *key, size_t key_len,
                            long milliseconds, const char *mode, size_t mode_len)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = mode ? 3 : 2;
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return 0;
    }

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: milliseconds */
    size_t ms_len;
    char *ms_str = long_to_string(milliseconds, &ms_len);
    if (!ms_str)
    {
        free(args);
        free(args_len);
        return 0;
    }
    args[1] = (uintptr_t)ms_str;
    args_len[1] = ms_len;

    /* Third argument (optional): mode (NX, XX, GT, LT) */
    if (mode)
    {
        args[2] = (uintptr_t)mode;
        args_len[2] = mode_len;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        PExpire,   /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated memory */
    free(ms_str);
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing PEXPIRE command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Get the result value */
    return handle_bool_response(result);
}

/* Execute a PEXPIREAT command using the Valkey Glide client */
int execute_pexpireat_command(const void *glide_client, const char *key, size_t key_len,
                              long timestamp_ms, const char *mode, size_t mode_len)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = mode ? 3 : 2;
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return 0;
    }

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: timestamp in milliseconds */
    size_t timestamp_len;
    char *timestamp_str = long_to_string(timestamp_ms, &timestamp_len);
    if (!timestamp_str)
    {
        free(args);
        free(args_len);
        return 0;
    }
    args[1] = (uintptr_t)timestamp_str;
    args_len[1] = timestamp_len;

    /* Third argument (optional): mode (NX, XX, GT, LT) */
    if (mode)
    {
        args[2] = (uintptr_t)mode;
        args_len[2] = mode_len;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        PExpireAt, /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated memory */
    free(timestamp_str);
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing PEXPIREAT command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Get the result value */
    return handle_bool_response(result);
}

/* Execute a PERSIST command using the Valkey Glide client */
int execute_persist_command(const void *glide_client, const char *key, size_t key_len)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1;
    uintptr_t args[1];
    unsigned long args_len[1];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Persist,   /* command type */
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
        printf("Error executing PERSIST command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Get the result value */
    return handle_bool_response(result);
}

/* Execute an EXPIRETIME command using the Valkey Glide client */
int execute_expiretime_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !output_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1;
    uintptr_t args[1];
    unsigned long args_len[1];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ExpireTime, /* command type */
        arg_count,  /* number of arguments */
        args,       /* arguments */
        args_len    /* argument lengths */
    );

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing EXPIRETIME command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Get the result value */
    return handle_int_response(result, output_value);
}

/* Execute a PEXPIRETIME command using the Valkey Glide client */
int execute_pexpiretime_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !output_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1;
    uintptr_t args[1];
    unsigned long args_len[1];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        PExpireTime, /* command type */
        arg_count,   /* number of arguments */
        args,        /* arguments */
        args_len     /* argument lengths */
    );

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing PEXPIRETIME command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Get the result value */
    return handle_int_response(result, output_value);
}
