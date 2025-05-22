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

/* Execute an LPUSH command using the Valkey Glide client */
long execute_lpush_command(const void *glide_client, const char *key, size_t key_len, zval *values, int values_count)
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
        LPush,     /* command type */
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

/* Execute an LPUSHX command using the Valkey Glide client */
long execute_lpushx_command(const void *glide_client, const char *key, size_t key_len, zval *values, int values_count)
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
        LPushX,    /* command type */
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

/* Execute an RPUSHX command using the Valkey Glide client */
long execute_rpushx_command(const void *glide_client, const char *key, size_t key_len, zval *values, int values_count)
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
        RPushX,    /* command type */
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

/* Execute an LPOP command using the Valkey Glide client */
int execute_lpop_command(const void *glide_client, const char *key, size_t key_len, zend_long count, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1; /* key */
    if (count > 0)
    {
        arg_count = 2; /* key + count */
    }

    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));
    char *count_str = NULL;

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

    /* Add count if provided */
    if (count > 0)
    {
        /* Convert count to string */
        size_t count_len;
        count_str = long_to_string(count, &count_len);
        if (!count_str)
        {
            free(args);
            free(args_len);
            return 0;
        }
        args[1] = (uintptr_t)count_str;
        args_len[1] = count_len;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        LPop,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated memory */
    if (count_str)
    {
        free(count_str);
    }
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Process the result */
    int ret_val = 0;
    if (result->response)
    {
        if (result->response->response_type == String)
        {
            /* Single value returned */

            ZVAL_STRINGL(return_value, result->response->string_value, result->response->string_value_len);
            ret_val = 1;
        }
        else if (result->response->response_type == Array)
        {
            /* Multiple values returned (when count > 1) */

            ret_val = command_response_to_zval(result->response, return_value, false);
        }
        else if (result->response->response_type == Null)
        {
            /* No elements in the list */

            ZVAL_FALSE(return_value);
            ret_val = 1;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute an RPOP command using the Valkey Glide client */
int execute_rpop_command(const void *glide_client, const char *key, size_t key_len, zend_long count, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1; /* key */
    if (count > 0)
    {
        arg_count = 2; /* key + count */
    }

    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));
    char *count_str = NULL;

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

    /* Add count if provided */
    if (count > 0)
    {
        /* Convert count to string */
        size_t count_len;
        count_str = long_to_string(count, &count_len);
        if (!count_str)
        {
            free(args);
            free(args_len);
            return 0;
        }
        args[1] = (uintptr_t)count_str;
        args_len[1] = count_len;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        RPop,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated memory */
    if (count_str)
    {
        free(count_str);
    }
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Process the result */
    int ret_val = 0;
    if (result->response)
    {
        if (result->response->response_type == String)
        {
            /* Single value returned */
            ZVAL_STRINGL(return_value, result->response->string_value, result->response->string_value_len);
            ret_val = 1;
        }
        else if (result->response->response_type == Array)
        {
            /* Multiple values returned (when count > 1) */
            ret_val = command_response_to_zval(result->response, return_value, false);
        }
        else if (result->response->response_type == Null)
        {
            /* No elements in the list */
            ZVAL_FALSE(return_value);
            ret_val = 1;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Helper function to prepare arguments for BPOP commands */
static int prepare_bpop_arguments(
    zval *keys,
    double timeout,
    unsigned long *arg_count_ptr,
    uintptr_t **args_ptr,
    unsigned long **args_len_ptr,
    char **timeout_str_ptr)
{
    int keys_count = 0;

    /* Check if keys is an array */
    if (Z_TYPE_P(keys) == IS_ARRAY)
    {
        /* Get the number of elements in the array */
        keys_count = zend_hash_num_elements(Z_ARRVAL_P(keys));
    }
    else if (Z_TYPE_P(keys) == IS_STRING)
    {
        /* Single key provided */
        keys_count = 1;
    }
    else
    {
        /* Invalid keys parameter */
        return 0;
    }

    /* Check if we have at least one key */
    if (keys_count <= 0)
    {
        return 0;
    }

    /* Calculate the number of arguments */
    unsigned long arg_count = keys_count + 1; /* keys + timeout */

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

    int arg_idx = 0;

    /* Add keys */
    if (Z_TYPE_P(keys) == IS_ARRAY)
    {
        /* Add all keys from the array */
        HashTable *ht = Z_ARRVAL_P(keys);
        zval *z_key;
        ZEND_HASH_FOREACH_VAL(ht, z_key)
        {
            if (Z_TYPE_P(z_key) != IS_STRING)
            {
                free(args);
                free(args_len);
                return 0;
            }
            args[arg_idx] = (uintptr_t)Z_STRVAL_P(z_key);
            args_len[arg_idx] = Z_STRLEN_P(z_key);
            arg_idx++;
        }
        ZEND_HASH_FOREACH_END();
    }
    else
    {
        /* Add the single key */
        args[arg_idx] = (uintptr_t)Z_STRVAL_P(keys);
        args_len[arg_idx] = Z_STRLEN_P(keys);
        arg_idx++;
    }

    /* Add timeout */
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

    /* Set output parameters */
    *arg_count_ptr = arg_count;
    *args_ptr = args;
    *args_len_ptr = args_len;

    return keys_count;
}

/* Execute a BLPOP command using the Valkey Glide client */
int execute_blpop_command(const void *glide_client, zval *keys, double timeout, zval *return_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys)
    {
        return 0;
    }

    /* Prepare arguments for the command */
    unsigned long arg_count = 0;
    uintptr_t *args = NULL;
    unsigned long *args_len = NULL;
    char *timeout_str = NULL;

    /* Prepare arguments */
    int keys_count = prepare_bpop_arguments(keys, timeout, &arg_count, &args, &args_len, &timeout_str);
    if (keys_count <= 0)
    {
        return 0;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        BLPop,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated memory */
    free(timeout_str);
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Process the result */
    int ret_val = 0;
    if (result->response)
    {
        if (result->response->response_type == Array)
        {
            /* Got a result, convert to PHP array [key, value] */
            array_init(return_value);
            ret_val = command_response_to_zval(result->response, return_value, false);
        }
        else if (result->response->response_type == Null)
        {
            /* Timeout reached, no elements available */
            ZVAL_NULL(return_value);
            ret_val = 1;
        }
    }

    /* Free the result */
    free_command_result(result);
    return ret_val;
}

/* Execute a BRPOP command using the Valkey Glide client */
int execute_brpop_command(const void *glide_client, zval *keys, double timeout, zval *return_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys)
    {
        return 0;
    }

    /* Prepare arguments for the command */
    unsigned long arg_count = 0;
    uintptr_t *args = NULL;
    unsigned long *args_len = NULL;
    char *timeout_str = NULL;

    /* Prepare arguments */
    int keys_count = prepare_bpop_arguments(keys, timeout, &arg_count, &args, &args_len, &timeout_str);
    if (keys_count <= 0)
    {
        return 0;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        BRPop,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated memory */
    free(timeout_str);
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Process the result */
    int ret_val = 0;
    if (result->response)
    {
        if (result->response->response_type == Array)
        {
            /* Got a result, convert to PHP array [key, value] */
            array_init(return_value);
            ret_val = command_response_to_zval(result->response, return_value, false);
        }
        else if (result->response->response_type == Null)
        {
            /* Timeout reached, no elements available */
            ZVAL_NULL(return_value);
            ret_val = 1;
        }
    }

    /* Free the result */
    free_command_result(result);
    return ret_val;
}
