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

/* Execute getReadTimeout command using the Valkey Glide client */
int execute_get_read_timeout_command(const void *glide_client, double *output_value)
{
    /* Check if client is valid */
    if (!glide_client || !output_value)
    {
        return 0;
    }

    /* Since this is a client configuration getter rather than a Redis command,
       we'll use a default value as this isn't directly supported by Glide */
    *output_value = 0.0; /* Default read timeout */

    /* Here we'd ideally access the Glide client's configuration, but since
       we don't have direct access to it through the FFI interface, we just
       return success and the default value */

    return 1;
}

/* Execute getPersistentID command using the Valkey Glide client */
int execute_get_persistent_id_command(const void *glide_client, char **result, size_t *result_len)
{
    /* Check if client is valid */
    if (!glide_client || !result || !result_len)
    {
        return 0;
    }

    /* Since this is a client connection property rather than a Redis command,
       we return a NULL value since we don't have access to this information */
    *result = NULL;
    *result_len = 0;

    return 1;
}

/* Execute getAuth command using the Valkey Glide client */
int execute_get_auth_command(const void *glide_client, char **result, size_t *result_len)
{
    /* Check if client is valid */
    if (!glide_client || !result || !result_len)
    {
        return 0;
    }

    /* Since this is a client connection property rather than a Redis command,
       we return a NULL value since we don't have access to this information */
    *result = NULL;
    *result_len = 0;

    return 1;
}

/* Execute a CLIENT command using the Valkey Glide client */
int execute_client_command(const void *glide_client, zval *args, int args_count, zval *return_value)
{
    /* Check if client and args are valid */
    if (!glide_client || !args || args_count <= 0 || !return_value)
    {
        return 0;
    }

    /* Create argument arrays */
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

    /* Keep track of allocated strings for cleanup */
    char **allocated = (char **)emalloc(args_count * sizeof(char *));
    int allocated_idx = 0;

    /* Convert arguments to strings if needed */
    int i;
    for (i = 0; i < args_count; i++)
    {
        zval *arg = &args[i];

        /* If string, use directly */
        if (Z_TYPE_P(arg) == IS_STRING)
        {
            cmd_args[i] = (uintptr_t)Z_STRVAL_P(arg);
            args_len[i] = Z_STRLEN_P(arg);
        }
        else
        {
            /* Convert non-string types to string */
            zval copy;
            size_t str_len;
            char *str;

            ZVAL_DUP(&copy, arg);
            convert_to_string(&copy);

            str_len = Z_STRLEN(copy);
            str = emalloc(str_len + 1);
            memcpy(str, Z_STRVAL(copy), str_len);
            str[str_len] = '\0';

            cmd_args[i] = (uintptr_t)str;
            args_len[i] = str_len;

            /* Track allocated string for cleanup */
            allocated[allocated_idx++] = str;

            zval_dtor(&copy);
        }
    }

    /* Determine the appropriate client command type based on the first argument */
    enum RequestType command_type = ClientInfo; /* Default to ClientInfo */

    if (args_count > 0 && Z_TYPE(args[0]) == IS_STRING)
    {
        const char *subcmd = Z_STRVAL(args[0]);
        if (strcasecmp(subcmd, "KILL") == 0)
        {
            if (args_count > 1)
                command_type = ClientKill;
            else
                command_type = ClientKillSimple;
        }
        else if (strcasecmp(subcmd, "LIST") == 0)
            command_type = ClientList;
        else if (strcasecmp(subcmd, "GETNAME") == 0)
            command_type = ClientGetName;
        else if (strcasecmp(subcmd, "ID") == 0)
            command_type = ClientId;
        else if (strcasecmp(subcmd, "SETNAME") == 0)
            command_type = ClientSetName;
        else if (strcasecmp(subcmd, "PAUSE") == 0)
            command_type = ClientPause;
        else if (strcasecmp(subcmd, "UNPAUSE") == 0)
            command_type = ClientUnpause;
        else if (strcasecmp(subcmd, "REPLY") == 0)
            command_type = ClientReply;
        else
            command_type = CustomCommand; /* Use custom command for other subcommands */
    }

    /* Set additional client prefix for custom commands */
    uintptr_t *final_args = cmd_args;
    unsigned long *final_args_len = args_len;
    unsigned long final_arg_count = arg_count;

    /* If using CustomCommand type, prepend "CLIENT" to the argument list */
    if (command_type == CustomCommand)
    {
        final_arg_count = arg_count + 1;
        final_args = (uintptr_t *)emalloc(final_arg_count * sizeof(uintptr_t));
        final_args_len = (unsigned long *)emalloc(final_arg_count * sizeof(unsigned long));

        if (!final_args || !final_args_len)
        {
            if (final_args)
                efree(final_args);
            if (final_args_len)
                efree(final_args_len);

            /* Free allocated strings */
            for (i = 0; i < allocated_idx; i++)
                efree(allocated[i]);
            efree(allocated);
            efree(cmd_args);
            efree(args_len);
            return 0;
        }

        /* Add "CLIENT" as first argument */
        final_args[0] = (uintptr_t)"CLIENT";
        final_args_len[0] = 6; /* strlen("CLIENT") */

        /* Copy the rest of the arguments */
        for (i = 0; i < arg_count; i++)
        {
            final_args[i + 1] = cmd_args[i];
            final_args_len[i + 1] = args_len[i];
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        command_type,    /* command type */
        final_arg_count, /* number of arguments */
        final_args,      /* arguments */
        final_args_len   /* argument lengths */
    );

    /* Free allocated memory */
    for (i = 0; i < allocated_idx; i++)
        efree(allocated[i]);
    efree(allocated);

    /* If we created a new args array for CustomCommand, free it */
    if (command_type == CustomCommand)
    {
        efree(final_args);
        efree(final_args_len);
    }

    efree(cmd_args);
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

        if (result->response)
        {
            /* Convert the response to PHP value */
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
        }
        free_command_result(result);
    }

    return status;
}

/* Execute a RAWCOMMAND command using the Valkey Glide client */
int execute_rawcommand_command(const void *glide_client, zval *args, int args_count, zval *return_value)
{
    /* Check if client and args are valid */
    if (!glide_client || !args || args_count <= 0 || !return_value)
    {
        return 0;
    }

    /* Create argument arrays */
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

    /* Keep track of allocated strings for cleanup */
    char **allocated = (char **)emalloc(args_count * sizeof(char *));
    int allocated_idx = 0;

    /* Convert arguments to strings if needed */
    int i;
    for (i = 0; i < args_count; i++)
    {
        zval *arg = &args[i];

        /* If string, use directly */
        if (Z_TYPE_P(arg) == IS_STRING)
        {
            cmd_args[i] = (uintptr_t)Z_STRVAL_P(arg);
            args_len[i] = Z_STRLEN_P(arg);
        }
        else
        {
            /* Convert non-string types to string */
            zval copy;
            size_t str_len;
            char *str;

            ZVAL_DUP(&copy, arg);
            convert_to_string(&copy);

            str_len = Z_STRLEN(copy);
            str = emalloc(str_len + 1);
            memcpy(str, Z_STRVAL(copy), str_len);
            str[str_len] = '\0';

            cmd_args[i] = (uintptr_t)str;
            args_len[i] = str_len;

            /* Track allocated string for cleanup */
            allocated[allocated_idx++] = str;

            zval_dtor(&copy);
        }
    }

    /* Execute the command using CustomCommand type */
    CommandResult *result = execute_command(
        glide_client,
        CustomCommand, /* command type for raw commands */
        arg_count,     /* number of arguments */
        cmd_args,      /* arguments */
        args_len       /* argument lengths */
    );

    /* Free allocated memory */
    for (i = 0; i < allocated_idx; i++)
        efree(allocated[i]);
    efree(allocated);
    efree(cmd_args);
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

        if (result->response)
        {
            /* Convert the response to PHP value */
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
        }
        free_command_result(result);
    }

    return status;
}

/* Execute the COMMAND command/method using the Valkey Glide client */
int execute_command_method(const void *glide_client, zval *args, int args_count, zval *return_value)
{
    /* Check if client is valid */
    if (!glide_client || !return_value)
    {
        return 0;
    }

    /* Calculate the number of arguments */
    unsigned long arg_count = args_count;

    /* If no args, execute COMMAND without arguments */
    if (arg_count == 0)
    {
        /* Execute the command */
        CommandResult *result = execute_command(
            glide_client,
            Command_, /* command type */
            0,        /* number of arguments */
            NULL,     /* arguments */
            NULL      /* argument lengths */
        );

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

            if (result->response)
            {
                /* Convert the response to PHP value */
                status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
            }
            free_command_result(result);
        }

        return status;
    }

    /* Has arguments, create argument arrays */
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

    /* Keep track of allocated strings for cleanup */
    char **allocated = (char **)emalloc(args_count * sizeof(char *));
    int allocated_idx = 0;

    /* Convert arguments to strings if needed */
    int i;
    for (i = 0; i < args_count; i++)
    {
        zval *arg = &args[i];

        /* If string, use directly */
        if (Z_TYPE_P(arg) == IS_STRING)
        {
            cmd_args[i] = (uintptr_t)Z_STRVAL_P(arg);
            args_len[i] = Z_STRLEN_P(arg);
        }
        else
        {
            /* Convert non-string types to string */
            zval copy;
            size_t str_len;
            char *str;

            ZVAL_DUP(&copy, arg);
            convert_to_string(&copy);

            str_len = Z_STRLEN(copy);
            str = emalloc(str_len + 1);
            memcpy(str, Z_STRVAL(copy), str_len);
            str[str_len] = '\0';

            cmd_args[i] = (uintptr_t)str;
            args_len[i] = str_len;

            /* Track allocated string for cleanup */
            allocated[allocated_idx++] = str;

            zval_dtor(&copy);
        }
    }

    /* Determine the COMMAND subcommand type */
    enum RequestType command_type = Command_; /* Default */

    if (args_count > 0 && Z_TYPE(args[0]) == IS_STRING)
    {
        const char *subcmd = Z_STRVAL(args[0]);
        if (strcasecmp(subcmd, "COUNT") == 0)
            command_type = CommandCount;
        else if (strcasecmp(subcmd, "LIST") == 0)
            command_type = CommandList;
        else if (strcasecmp(subcmd, "INFO") == 0)
            command_type = CommandInfo;
        else if (strcasecmp(subcmd, "GETKEYS") == 0)
            command_type = CommandGetKeys;
        else
            command_type = CustomCommand; /* Use custom command for other subcommands */
    }

    /* If using CustomCommand type, prepend "COMMAND" to the argument list */
    uintptr_t *final_args = cmd_args;
    unsigned long *final_args_len = args_len;
    unsigned long final_arg_count = arg_count;

    if (command_type == CustomCommand)
    {
        final_arg_count = arg_count + 1;
        final_args = (uintptr_t *)emalloc(final_arg_count * sizeof(uintptr_t));
        final_args_len = (unsigned long *)emalloc(final_arg_count * sizeof(unsigned long));

        if (!final_args || !final_args_len)
        {
            if (final_args)
                efree(final_args);
            if (final_args_len)
                efree(final_args_len);

            /* Free allocated strings */
            for (i = 0; i < allocated_idx; i++)
                efree(allocated[i]);
            efree(allocated);
            efree(cmd_args);
            efree(args_len);
            return 0;
        }

        /* Add "COMMAND" as first argument */
        final_args[0] = (uintptr_t)"COMMAND";
        final_args_len[0] = 7; /* strlen("COMMAND") */

        /* Copy the rest of the arguments */
        for (i = 0; i < arg_count; i++)
        {
            final_args[i + 1] = cmd_args[i];
            final_args_len[i + 1] = args_len[i];
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        command_type,    /* command type */
        final_arg_count, /* number of arguments */
        final_args,      /* arguments */
        final_args_len   /* argument lengths */
    );

    /* Free allocated memory */
    for (i = 0; i < allocated_idx; i++)
        efree(allocated[i]);
    efree(allocated);

    /* If we created a new args array for CustomCommand, free it */
    if (command_type == CustomCommand)
    {
        efree(final_args);
        efree(final_args_len);
    }

    efree(cmd_args);
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

        if (result->response)
        {
            /* Convert the response to PHP value */
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
        }
        free_command_result(result);
    }

    return status;
}

/* Execute a DBSIZE command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_dbsize_command(const void *glide_client, long *output_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = DBSize;

    return execute_core_command(&args, output_value, process_core_int_result);
}
