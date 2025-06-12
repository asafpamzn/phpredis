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
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
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

/* Execute an RPOPLPUSH command using the Valkey Glide client */
int execute_rpoplpush_command(const void *glide_client, const char *src, size_t src_len, const char *dst, size_t dst_len, char **result, size_t *result_len)
{
    /* Check if client and keys are valid */
    if (!glide_client || !src || !dst || !result || !result_len)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2;
    uintptr_t args[2];
    unsigned long args_len[2];

    /* First argument: source key */
    args[0] = (uintptr_t)src;
    args_len[0] = src_len;

    /* Second argument: destination key */
    args[1] = (uintptr_t)dst;
    args_len[1] = dst_len;

    /* Execute the command */
    CommandResult *cmd_result = execute_command(
        glide_client,
        RPopLPush, /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_string_response(cmd_result, result, result_len);
}

/* Execute a BRPOPLPUSH command using the Valkey Glide client */
int execute_brpoplpush_command(const void *glide_client, const char *src, size_t src_len, const char *dst, size_t dst_len, zend_long timeout, char **result, size_t *result_len)
{
    /* Check if client and keys are valid */
    if (!glide_client || !src || !dst || !result || !result_len)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3;
    uintptr_t args[3];
    unsigned long args_len[3];

    /* First argument: source key */
    args[0] = (uintptr_t)src;
    args_len[0] = src_len;

    /* Second argument: destination key */
    args[1] = (uintptr_t)dst;
    args_len[1] = dst_len;

    /* Third argument: timeout */
    size_t timeout_len;
    char *timeout_str = long_to_string(timeout, &timeout_len);
    if (!timeout_str)
    {
        return -1;
    }
    args[2] = (uintptr_t)timeout_str;
    args_len[2] = timeout_len;

    /* Execute the command */
    CommandResult *cmd_result = execute_command(
        glide_client,
        BRPopLPush, /* command type */
        arg_count,  /* number of arguments */
        args,       /* arguments */
        args_len    /* argument lengths */
    );

    /* Free the timeout string */
    efree(timeout_str);

    /* Use the generic handler to process the result */
    return handle_string_response(cmd_result, result, result_len);
}

/* Helper function to prepare arguments for MPOP commands */
int prepare_mpop_arguments(
    const void *glide_client,
    int is_blocking,
    double timeout,
    zval *keys,
    const char *from,
    size_t from_len,
    long count,
    unsigned long *arg_count_ptr,
    uintptr_t **args_ptr,
    unsigned long **args_len_ptr,
    char **numkeys_str_ptr,
    char **timeout_str_ptr,
    char **count_str_ptr)
{
    /* Get the number of keys */
    int keys_count = 0;
    if (Z_TYPE_P(keys) == IS_ARRAY)
    {
        keys_count = zend_hash_num_elements(Z_ARRVAL_P(keys));
    }
    else
    {
        return 0; /* Keys must be an array */
    }

    /* Check if we have at least one key */
    if (keys_count <= 0)
    {
        return 0;
    }

    /* Calculate the number of arguments */
    unsigned long arg_count = keys_count + 2; /* numkeys + keys + direction */
    if (is_blocking)
    {
        arg_count++; /* Add timeout for blocking commands */
    }

    /* Allocate memory for arguments */
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

    /* Current argument index */
    int arg_idx = 0;

    /* Add timeout for blocking commands */
    if (is_blocking)
    {
        /* Convert timeout to string */
        size_t timeout_len;
        char *timeout_str = double_to_string(timeout, &timeout_len);
        if (!timeout_str)
        {
            efree(args);
            efree(args_len);
            return 0;
        }
        args[arg_idx] = (uintptr_t)timeout_str;
        args_len[arg_idx] = timeout_len;
        *timeout_str_ptr = timeout_str;
        arg_idx++;
    }

    /* Add numkeys first (this should be the first argument after timeout for blocking commands) */
    size_t numkeys_len;
    char *numkeys_str = long_to_string(keys_count, &numkeys_len);
    if (!numkeys_str)
    {
        efree(args);
        efree(args_len);
        if (is_blocking)
        {
            efree(*timeout_str_ptr);
            *timeout_str_ptr = NULL;
        }
        return 0;
    }
    /* Debug output to see the value being passed */

    args[arg_idx] = (uintptr_t)numkeys_str;
    args_len[arg_idx] = numkeys_len;
    *numkeys_str_ptr = numkeys_str;
    arg_idx++;

    /* Add keys */
    HashTable *ht = Z_ARRVAL_P(keys);
    zval *z_key;
    ZEND_HASH_FOREACH_VAL(ht, z_key)
    {
        if (Z_TYPE_P(z_key) != IS_STRING)
        {
            efree(args);
            efree(args_len);
            efree(numkeys_str);
            *numkeys_str_ptr = NULL;
            if (is_blocking)
            {
                efree(*timeout_str_ptr);
                *timeout_str_ptr = NULL;
            }
            return 0;
        }
        args[arg_idx] = (uintptr_t)Z_STRVAL_P(z_key);
        args_len[arg_idx] = Z_STRLEN_P(z_key);
        arg_idx++;
    }
    ZEND_HASH_FOREACH_END();

    /* Add direction (LEFT or RIGHT) directly */
    args[arg_idx] = (uintptr_t)from;
    args_len[arg_idx] = from_len;
    arg_idx++;

    /* Add COUNT if count > 1 */
    if (count > 1)
    {
        /* Increase arg_count for COUNT and its value */
        arg_count += 2;

        /* Reallocate args and args_len arrays */
        uintptr_t *new_args = (uintptr_t *)erealloc(args, arg_count * sizeof(uintptr_t));
        unsigned long *new_args_len = (unsigned long *)erealloc(args_len, arg_count * sizeof(unsigned long));

        if (!new_args || !new_args_len)
        {
            efree(args);
            efree(args_len);
            efree(numkeys_str);
            *numkeys_str_ptr = NULL;
            if (is_blocking)
            {
                efree(*timeout_str_ptr);
                *timeout_str_ptr = NULL;
            }
            return 0;
        }

        args = new_args;
        args_len = new_args_len;

        /* Add COUNT keyword */
        args[arg_idx] = (uintptr_t)"COUNT";
        args_len[arg_idx] = 5;
        arg_idx++;

        /* Add count value */
        size_t count_len;
        char *count_str = long_to_string(count, &count_len);
        if (!count_str)
        {
            efree(args);
            efree(args_len);
            efree(numkeys_str);
            *numkeys_str_ptr = NULL;
            if (is_blocking)
            {
                efree(*timeout_str_ptr);
                *timeout_str_ptr = NULL;
            }
            return 0;
        }
        args[arg_idx] = (uintptr_t)count_str;
        args_len[arg_idx] = count_len;
        *count_str_ptr = count_str;
        arg_idx++;
    }

    /* Set output parameters */
    *arg_count_ptr = arg_count;
    *args_ptr = args;
    *args_len_ptr = args_len;

    return keys_count;
}

/* Execute an RPUSH command using the Valkey Glide client */
long execute_rpush_command(const void *glide_client, const char *key, size_t key_len, zval *values, int values_count)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !values || values_count <= 0)
    {
        return 0;
    }

    /* First, count the total number of arguments including expanded arrays */
    unsigned long total_args = 1; /* Start with 1 for the key */
    int i, j;

    /* Count all items, including those in nested arrays */
    for (i = 0; i < values_count; i++)
    {
        zval *value = &values[i];
        if (Z_TYPE_P(value) == IS_STRING)
        {
            /* Direct string argument */
            total_args++;
        }
        else if (Z_TYPE_P(value) == IS_ARRAY)
        {
            /* Array argument - count all elements in the array */
            HashTable *ht = Z_ARRVAL_P(value);
            total_args += zend_hash_num_elements(ht);
        }
        else
        {
            /* Unsupported type */
            return 0;
        }
    }

    /* Prepare command arguments */
    uintptr_t *args = (uintptr_t *)emalloc(total_args * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(total_args * sizeof(unsigned long));

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

    /* Process all values and add to args array */
    unsigned long arg_idx = 1; /* Start after the key */

    for (i = 0; i < values_count; i++)
    {
        zval *value = &values[i];

        if (Z_TYPE_P(value) == IS_STRING)
        {
            /* Process string directly */
            args[arg_idx] = (uintptr_t)Z_STRVAL_P(value);
            args_len[arg_idx] = Z_STRLEN_P(value);
            arg_idx++;
        }
        else if (Z_TYPE_P(value) == IS_ARRAY)
        {
            /* Process each element in the array */
            HashTable *ht = Z_ARRVAL_P(value);
            zval *z_item;

            ZEND_HASH_FOREACH_VAL(ht, z_item)
            {
                /* Each array element must be a string */
                if (Z_TYPE_P(z_item) != IS_STRING)
                {
                    efree(args);
                    efree(args_len);
                    return 0;
                }

                args[arg_idx] = (uintptr_t)Z_STRVAL_P(z_item);
                args_len[arg_idx] = Z_STRLEN_P(z_item);
                arg_idx++;
            }
            ZEND_HASH_FOREACH_END();
        }
        else
        {
            /* Unexpected type (already checked above, but for safety) */
            efree(args);
            efree(args_len);
            return 0;
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        RPush,      /* command type */
        total_args, /* number of arguments including expanded arrays */
        args,       /* arguments */
        args_len    /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Use the generic handler to process the result */
    long output_value = -1;
    if (handle_int_response(result, &output_value))
    {
        /* Command succeeded */
        return output_value;
    }
    else
    {
        /* Command failed */
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

    /* Prepare for argument construction */
    unsigned long arg_count = 0;
    uintptr_t *args = NULL;
    unsigned long *args_len = NULL;
    char *numkeys_str = NULL;
    char *timeout_str = NULL;
    char *count_str = NULL;

    /* Prepare the arguments */
    int keys_count = prepare_mpop_arguments(
        glide_client, is_blocking, timeout, keys, from, from_len, count,
        &arg_count, &args, &args_len,
        &numkeys_str, &timeout_str, &count_str);

    if (keys_count < 0)
    {
        return 0;
    }

    /* Determine the command type */
    enum RequestType cmd_type = is_blocking ? BLMPop : LMPop;

    /* Execute the command */
    CommandResult *cmd_result = command(
        glide_client,
        0,         /* channel */
        cmd_type,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len,  /* argument lengths */
        NULL,      /* route bytes */
        0          /* route bytes length */
    );

    /* Free the argument strings */
    if (numkeys_str)
        efree(numkeys_str);
    if (timeout_str)
        efree(timeout_str);
    if (count_str)
        efree(count_str);
    efree(args);
    efree(args_len);

    /* Check if the command was successful */
    if (!cmd_result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (cmd_result->command_error)
    {
        printf("Error executing %s command: %s\n", cmd, cmd_result->command_error->command_error_message);
        free_command_result(cmd_result);
        return 0;
    }

    /* Process the result */
    int ret_val = 0;
    /* For ZMPOP, use associative array format for the values */
    int use_assoc = (strncmp(cmd, "ZMPOP", 5) == 0 || strncmp(cmd, "BZMPOP", 6) == 0) ? COMMAND_RESPONSE_ASSOSIATIVE_ARRAY : COMMAND_RESPONSE_NOT_ASSOSIATIVE;
    ret_val = command_response_to_zval(cmd_result->response, result, use_assoc, false);

    /* Free the result */
    free_command_result(cmd_result);

    return ret_val;
}
