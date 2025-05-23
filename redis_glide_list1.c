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
            status = command_response_to_zval(result->response, return_value, 0);
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
    /* Check if client and key are valid */
    if (!glide_client || !key || !position || !pivot || !value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 4; /* key, position, pivot, value */
    uintptr_t args[4];
    unsigned long args_len[4];

    /* Set up arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;
    args[1] = (uintptr_t)position;
    args_len[1] = position_len;
    args[2] = (uintptr_t)pivot;
    args_len[2] = pivot_len;
    args[3] = (uintptr_t)value;
    args_len[3] = value_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        LInsert,   /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the proper handler for integer response */
    return handle_int_response(result, output_value);
}

/* Execute an LPOS command using the Valkey Glide client */
int execute_lpos_command(const void *glide_client, const char *key, size_t key_len,
                         const char *element, size_t element_len, zval *options,
                         zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !element)
    {
        return 0;
    }

    /* Count the number of options we need to include */
    int opt_count = 0;
    zend_bool has_rank = 0, has_count = 0, has_maxlen = 0;
    zval *rank = NULL, *count = NULL, *maxlen = NULL;

    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(options);

        /* Check for RANK option */
        rank = zend_hash_str_find(ht, "rank", sizeof("rank") - 1);
        if (rank && Z_TYPE_P(rank) == IS_LONG)
        {
            has_rank = 1;
            opt_count += 2; /* RANK + value */
        }

        /* Check for COUNT option */
        count = zend_hash_str_find(ht, "count", sizeof("count") - 1);
        if (count && Z_TYPE_P(count) == IS_LONG)
        {
            has_count = 1;
            opt_count += 2; /* COUNT + value */
        }

        /* Check for MAXLEN option */
        maxlen = zend_hash_str_find(ht, "maxlen", sizeof("maxlen") - 1);
        if (maxlen && Z_TYPE_P(maxlen) == IS_LONG)
        {
            has_maxlen = 1;
            opt_count += 2; /* MAXLEN + value */
        }
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2 + opt_count; /* key + element + options */
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

    /* Key and element are first two arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;
    args[1] = (uintptr_t)element;
    args_len[1] = element_len;

    char rank_str[32], count_str[32], maxlen_str[32];
    unsigned int arg_idx = 2;

    /* Add optional arguments */
    if (has_rank)
    {
        args[arg_idx] = (uintptr_t)"RANK";
        args_len[arg_idx] = 4;
        arg_idx++;

        snprintf(rank_str, sizeof(rank_str), "%ld", Z_LVAL_P(rank));
        args[arg_idx] = (uintptr_t)rank_str;
        args_len[arg_idx] = strlen(rank_str);
        arg_idx++;
    }

    if (has_count)
    {
        args[arg_idx] = (uintptr_t)"COUNT";
        args_len[arg_idx] = 5;
        arg_idx++;

        snprintf(count_str, sizeof(count_str), "%ld", Z_LVAL_P(count));
        args[arg_idx] = (uintptr_t)count_str;
        args_len[arg_idx] = strlen(count_str);
        arg_idx++;
    }

    if (has_maxlen)
    {
        args[arg_idx] = (uintptr_t)"MAXLEN";
        args_len[arg_idx] = 6;
        arg_idx++;

        snprintf(maxlen_str, sizeof(maxlen_str), "%ld", Z_LVAL_P(maxlen));
        args[arg_idx] = (uintptr_t)maxlen_str;
        args_len[arg_idx] = strlen(maxlen_str);
        arg_idx++;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        LPos,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Handle the response directly */
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
            /* LPOS returns position(s) of the element */
            status = command_response_to_zval(result->response, return_value, 0);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute an LLEN command using the Valkey Glide client */
int execute_llen_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || key_len <= 0)
    {
        return 0;
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
        LLen,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    int status = handle_int_response(result, output_value);

    return status;
}
