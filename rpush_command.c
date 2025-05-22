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

/* Helper function to prepare arguments for MPOP commands */
static int prepare_mpop_arguments(
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
            free(args);
            free(args_len);
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
        free(args);
        free(args_len);
        if (is_blocking)
        {
            free(*timeout_str_ptr);
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
            free(args);
            free(args_len);
            free(numkeys_str);
            *numkeys_str_ptr = NULL;
            if (is_blocking)
            {
                free(*timeout_str_ptr);
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
        uintptr_t *new_args = (uintptr_t *)realloc(args, arg_count * sizeof(uintptr_t));
        unsigned long *new_args_len = (unsigned long *)realloc(args_len, arg_count * sizeof(unsigned long));

        if (!new_args || !new_args_len)
        {
            free(args);
            free(args_len);
            free(numkeys_str);
            *numkeys_str_ptr = NULL;
            if (is_blocking)
            {
                free(*timeout_str_ptr);
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
            free(args);
            free(args_len);
            free(numkeys_str);
            *numkeys_str_ptr = NULL;
            if (is_blocking)
            {
                free(*timeout_str_ptr);
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

/* Execute a ZMPOP or BZMPOP command (for sorted set operations) using the Valkey Glide client */
int execute_zmpop_command(const void *glide_client, const char *cmd, double timeout, zval *keys, const char *from, size_t from_len, long count, zval *result)
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
    enum RequestType cmd_type = is_blocking ? BZMPop : ZMPop;

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
        free(numkeys_str);
    if (timeout_str)
        free(timeout_str);
    if (count_str)
        free(count_str);
    free(args);
    free(args_len);

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
    /* For ZMPOP, use associative array format for the values */
    int use_assoc = 1; /* Always use associative arrays for sorted set responses */
    int ret_val = command_response_to_zval(cmd_result->response, result, use_assoc);

    /* Free the result */
    free_command_result(cmd_result);

    return ret_val;
}
/* Execute an RPUSH command using the Valkey Glide client */
long execute_rpush_command(const void *glide_client, const char *key, size_t key_len, zval *values, int values_count)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !values || values_count <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1 + values_count; /* key + values */
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

    /* Remaining arguments: values */
    int i;
    for (i = 0; i < values_count; i++)
    {
        zval *value = &values[i];
        if (Z_TYPE_P(value) != IS_STRING)
        {
            free(args);
            free(args_len);
            return 0;
        }
        args[1 + i] = (uintptr_t)Z_STRVAL_P(value);
        args_len[1 + i] = Z_STRLEN_P(value);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        RPush,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    free(args);
    free(args_len);

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
        free(numkeys_str);
    if (timeout_str)
        free(timeout_str);
    if (count_str)
        free(count_str);
    free(args);
    free(args_len);

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
    int use_assoc = (strncmp(cmd, "ZMPOP", 5) == 0 || strncmp(cmd, "BZMPOP", 6) == 0) ? 1 : 0;
    ret_val = command_response_to_zval(cmd_result->response, result, use_assoc);

    /* Free the result */
    free_command_result(cmd_result);

    return ret_val;
}
