/*
  +----------------------------------------------------------------------+
  | Redis Glide FFI integration for phpredis - Hash Commands             |
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

/* Execute an HSET command using the Valkey Glide client */
int execute_hset_command(const void *glide_client, const char *key, size_t key_len, zval *z_args, int argc, long *output_value)
{
    /* Check if client, key, and args are valid */
    if (!glide_client || !key || !z_args || argc < 2 || argc % 2 != 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1 + argc; /* key + field/value pairs */
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

    /* Add field/value pairs */
    int i;
    for (i = 0; i < argc; i++)
    {
        zval *value = &z_args[i];

        if (Z_TYPE_P(value) == IS_STRING)
        {
            args[i + 1] = (uintptr_t)Z_STRVAL_P(value);
            args_len[i + 1] = Z_STRLEN_P(value);
        }
        else
        {
            /* Convert non-string values to string */
            char *str_val = NULL;
            size_t str_len = 0;

            if (Z_TYPE_P(value) == IS_LONG)
            {
                str_val = long_to_string(Z_LVAL_P(value), &str_len);
            }
            else if (Z_TYPE_P(value) == IS_DOUBLE)
            {
                str_val = double_to_string(Z_DVAL_P(value), &str_len);
            }
            else if (Z_TYPE_P(value) == IS_TRUE)
            {
                str_val = strdup("1");
                str_len = 1;
            }
            else if (Z_TYPE_P(value) == IS_FALSE)
            {
                str_val = strdup("0");
                str_len = 1;
            }
            else
            {
                /* Handle other types or error */
                free(args);
                free(args_len);
                return 0;
            }

            if (str_val)
            {
                args[i + 1] = (uintptr_t)str_val;
                args_len[i + 1] = str_len;
            }
            else
            {
                free(args);
                free(args_len);
                return 0;
            }
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        HSet,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated strings for non-string arguments */
    for (i = 0; i < argc; i++)
    {
        zval *value = &z_args[i];
        if (Z_TYPE_P(value) != IS_STRING)
        {
            free((void *)args[i + 1]);
        }
    }

    /* Free the argument arrays */
    free(args);
    free(args_len);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute an HSETNX command using the Valkey Glide client */
int execute_hsetnx_command(const void *glide_client, const char *key, size_t key_len,
                           char *field, size_t field_len,
                           char *value, size_t value_len,
                           int *output_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || !field || !value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* key + field + value */
    uintptr_t args[3];
    unsigned long args_len[3];

    /* Set arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    args[1] = (uintptr_t)field;
    args_len[1] = field_len;

    args[2] = (uintptr_t)value;
    args_len[2] = value_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        HSetNx,    /* command type */
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
        free_command_result(result);
        return 0;
    }

    /* Process the result (0 or 1) */
    int ret_val = 0;
    if (result->response)
    {
        if (result->response->response_type == Integer)
        {
            *output_value = result->response->integer_value;
            ret_val = 1;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute an HGET command using the Valkey Glide client */
int execute_hget_command(const void *glide_client, const char *key, size_t key_len,
                         char *field, size_t field_len,
                         char **result, size_t *result_len)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !field)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2; /* key + field */
    uintptr_t args[2];
    unsigned long args_len[2];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: field */
    args[1] = (uintptr_t)field;
    args_len[1] = field_len;

    /* Execute the command */
    CommandResult *cmd_result = execute_command(
        glide_client,
        HGet,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_string_response(cmd_result, result, result_len);
}

/* Execute an HLEN command using the Valkey Glide client */
int execute_hlen_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
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
        HLen,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute an HDEL command using the Valkey Glide client */
int execute_hdel_command(const void *glide_client, const char *key, size_t key_len, zval *fields, int fields_count, long *output_value)
{
    /* Check if client, key, and fields are valid */
    if (!glide_client || !key || !fields || fields_count <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1 + fields_count; /* key + fields */
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

    /* Add fields as arguments */
    int i;
    for (i = 0; i < fields_count; i++)
    {
        zval *field = &fields[i];

        if (Z_TYPE_P(field) == IS_STRING)
        {
            args[i + 1] = (uintptr_t)Z_STRVAL_P(field);
            args_len[i + 1] = Z_STRLEN_P(field);
        }
        else
        {
            /* Convert non-string values to string */
            char *str_val = NULL;
            size_t str_len = 0;

            if (Z_TYPE_P(field) == IS_LONG)
            {
                str_val = long_to_string(Z_LVAL_P(field), &str_len);
            }
            else if (Z_TYPE_P(field) == IS_DOUBLE)
            {
                str_val = double_to_string(Z_DVAL_P(field), &str_len);
            }
            else if (Z_TYPE_P(field) == IS_TRUE)
            {
                str_val = strdup("1");
                str_len = 1;
            }
            else if (Z_TYPE_P(field) == IS_FALSE)
            {
                str_val = strdup("0");
                str_len = 1;
            }
            else
            {
                /* Handle other types or error */
                free(args);
                free(args_len);
                return 0;
            }

            if (str_val)
            {
                args[i + 1] = (uintptr_t)str_val;
                args_len[i + 1] = str_len;
            }
            else
            {
                free(args);
                free(args_len);
                return 0;
            }
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        HDel,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated strings for non-string arguments */
    for (i = 0; i < fields_count; i++)
    {
        zval *field = &fields[i];
        if (Z_TYPE_P(field) != IS_STRING)
        {
            free((void *)args[i + 1]);
        }
    }

    /* Free the argument arrays */
    free(args);
    free(args_len);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute an HEXISTS command using the Valkey Glide client */
int execute_hexists_command(const void *glide_client, const char *key, size_t key_len,
                            char *field, size_t field_len,
                            int *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !field)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2;
    uintptr_t args[2];
    unsigned long args_len[2];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: field */
    args[1] = (uintptr_t)field;
    args_len[1] = field_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        HExists,   /* command type */
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
        free_command_result(result);
        return 0;
    }

    /* Process the result (0 or 1) */
    int ret_val = 0;
    if (result->response)
    {
        if (result->response->response_type == Integer)
        {
            *output_value = result->response->integer_value;
            ret_val = 1;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute an HKEYS command using the Valkey Glide client */
int execute_hkeys_command(const void *glide_client, const char *key, size_t key_len, zval *return_value)
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
        HKeys,     /* command type */
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
        free_command_result(result);
        return 0;
    }

    /* Process the result (array of keys) */
    int ret_val = 0;
    if (result->response && result->response->response_type == Array)
    {
        size_t i;
        for (i = 0; i < result->response->array_value_len; i++)
        {
            struct CommandResponse *element = &result->response->array_value[i];
            if (element->response_type == String)
            {
                add_next_index_stringl(return_value, element->string_value, element->string_value_len);
            }
        }
        ret_val = 1;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute an HVALS command using the Valkey Glide client */
int execute_hvals_command(const void *glide_client, const char *key, size_t key_len, zval *return_value)
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
        HVals,     /* command type */
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
        free_command_result(result);
        return 0;
    }

    /* Process the result (array of values) */
    int ret_val = 0;
    if (result->response && result->response->response_type == Array)
    {
        size_t i;
        for (i = 0; i < result->response->array_value_len; i++)
        {
            struct CommandResponse *element = &result->response->array_value[i];
            if (element->response_type == String)
            {
                add_next_index_stringl(return_value, element->string_value, element->string_value_len);
            }
        }
        ret_val = 1;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute an HGETALL command using the Valkey Glide client */
int execute_hgetall_command(const void *glide_client, const char *key, size_t key_len, zval *return_value)
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
        HGetAll,   /* command type */
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
        free_command_result(result);
        return 0;
    }

    /* Process the result (array of field/value pairs) */
    int ret_val = 0;
    if (result->response && result->response->response_type == Array)
    {
        size_t i;
        for (i = 0; i < result->response->array_value_len; i += 2)
        {
            if (i + 1 < result->response->array_value_len)
            {
                struct CommandResponse *field = &result->response->array_value[i];
                struct CommandResponse *value = &result->response->array_value[i + 1];

                if (field->response_type == String && value->response_type == String)
                {
                    add_assoc_stringl_ex(return_value, field->string_value, field->string_value_len, value->string_value, value->string_value_len);
                }
            }
        }
        ret_val = 1;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}
