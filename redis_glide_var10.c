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

/* Execute a WAIT command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_wait_command(const void *glide_client, long numreplicas, long timeout, long *output_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Wait;

    /* WAIT is a server-level command (not key-based) with 2 arguments: numreplicas, timeout */
    args.args[0].type = CORE_ARG_TYPE_LONG;
    args.args[0].data.long_arg.value = numreplicas;
    args.args[1].type = CORE_ARG_TYPE_LONG;
    args.args[1].data.long_arg.value = timeout;
    args.arg_count = 2;

    return execute_core_command(&args, output_value, process_core_int_result);
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
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute a MULTI command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_multi_command(const void *glide_client)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Multi;

    return execute_core_command(&args, NULL, process_core_bool_result);
}

/* Execute a DISCARD command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_discard_command(const void *glide_client)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Discard;

    return execute_core_command(&args, NULL, process_core_bool_result);
}

/* Execute an EXEC command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_exec_command(const void *glide_client, zval *return_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Exec;

    return execute_core_command(&args, return_value, process_core_array_result);
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
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
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
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute a DUMP command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_dump_command(const void *glide_client, const char *key, size_t key_len,
                         char **output, size_t *output_len)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Dump;
    args.key = key;
    args.key_len = key_len;

    /* Use string result processor that handles null */
    struct
    {
        char **result;
        size_t *result_len;
    } out = {output, output_len};
    return execute_core_command(&args, &out, process_core_string_result);
}

/* Execute a RESTORE command using the Valkey Glide client */
int execute_restore_command(const void *glide_client, const char *key, size_t key_len,
                            long ttl, const char *serialized, size_t serialized_len,
                            zval *options)
{
    /* Check if client, key and serialized value are valid */
    if (!glide_client || !key || key_len <= 0 || !serialized || serialized_len <= 0)
    {
        return 0;
    }

    /* Convert TTL to string */
    char ttl_str[32];
    snprintf(ttl_str, sizeof(ttl_str), "%ld", ttl);

    /* Start with basic arguments: key + ttl + serialized */
    unsigned long base_arg_count = 3;
    unsigned long max_args = 10; /* Maximum possible arguments */
    uintptr_t *args = (uintptr_t *)emalloc(max_args * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(max_args * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Set up base arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;
    args[1] = (uintptr_t)ttl_str;
    args_len[1] = strlen(ttl_str);
    args[2] = (uintptr_t)serialized;
    args_len[2] = serialized_len;

    unsigned long arg_count = base_arg_count;

    /* Process options if provided */
    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(options);
        zval *val;
        zend_string *key_str;
        zend_ulong num_key;

        /* Variables for option values */
        zend_bool has_replace = 0;
        zend_bool has_absttl = 0;
        long idletime = -1;
        long freq = -1;

        /* Parse the array */
        ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, key_str, val)
        {
            /* Handle indexed array elements (like ['REPLACE', 'ABSTTL']) */
            if (!key_str && Z_TYPE_P(val) == IS_STRING)
            {
                const char *flag = Z_STRVAL_P(val);
                if (strcmp(flag, "REPLACE") == 0)
                {
                    has_replace = 1;
                }
                else if (strcmp(flag, "ABSTTL") == 0)
                {
                    has_absttl = 1;
                }
            }
            /* Handle associative array elements (like ['IDLETIME' => 200]) */
            else if (key_str)
            {
                const char *opt_name = ZSTR_VAL(key_str);
                if (strcmp(opt_name, "REPLACE") == 0)
                {
                    has_replace = 1;
                }
                else if (strcmp(opt_name, "ABSTTL") == 0)
                {
                    has_absttl = 1;
                }
                else if (strcmp(opt_name, "IDLETIME") == 0 && Z_TYPE_P(val) == IS_LONG)
                {
                    idletime = Z_LVAL_P(val);
                }
                else if (strcmp(opt_name, "FREQ") == 0 && Z_TYPE_P(val) == IS_LONG)
                {
                    freq = Z_LVAL_P(val);
                }
            }
        }
        ZEND_HASH_FOREACH_END();

        /* Add REPLACE if needed */
        if (has_replace && arg_count < max_args)
        {
            const char *replace_str = "REPLACE";
            args[arg_count] = (uintptr_t)replace_str;
            args_len[arg_count] = strlen(replace_str);
            arg_count++;
        }

        /* Add ABSTTL if needed */
        if (has_absttl && arg_count < max_args)
        {
            const char *absttl_str = "ABSTTL";
            args[arg_count] = (uintptr_t)absttl_str;
            args_len[arg_count] = strlen(absttl_str);
            arg_count++;
        }

        /* Add IDLETIME if provided */
        if (idletime >= 0 && arg_count + 1 < max_args)
        {
            const char *idletime_str = "IDLETIME";
            args[arg_count] = (uintptr_t)idletime_str;
            args_len[arg_count] = strlen(idletime_str);
            arg_count++;

            /* Convert idletime to string */
            char *idletime_val = (char *)emalloc(32);
            snprintf(idletime_val, 32, "%ld", idletime);
            args[arg_count] = (uintptr_t)idletime_val;
            args_len[arg_count] = strlen(idletime_val);
            arg_count++;
        }

        /* Add FREQ if provided */
        if (freq >= 0 && arg_count + 1 < max_args)
        {
            const char *freq_str = "FREQ";
            args[arg_count] = (uintptr_t)freq_str;
            args_len[arg_count] = strlen(freq_str);
            arg_count++;

            /* Convert freq to string */
            char *freq_val = (char *)emalloc(32);
            snprintf(freq_val, 32, "%ld", freq);
            args[arg_count] = (uintptr_t)freq_val;
            args_len[arg_count] = strlen(freq_val);
            arg_count++;
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Restore,   /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free any dynamically allocated option values */
    int i;
    for (i = base_arg_count; i < arg_count; i++)
    {
        /* Check if this is a dynamically allocated string (IDLETIME/FREQ values) */
        char *str = (char *)args[i];
        if (str && str[0] >= '0' && str[0] <= '9')
        {
            efree(str);
        }
    }

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Process the result */
    int status = handle_ok_response(result);

    /* Convert response status to boolean */
    return (status == 1) ? 1 : 0;
}
