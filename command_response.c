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

/* Execute a command and handle common error checking */
CommandResult *execute_command(
    const void *glide_client,
    enum RequestType command_type,
    unsigned long arg_count,
    const uintptr_t *args,
    const unsigned long *args_len)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return NULL;
    }

    /* Execute the command */
    CommandResult *result = command(
        glide_client,
        0,            /* channel */
        command_type, /* command type */
        arg_count,    /* number of arguments */
        args,         /* arguments */
        args_len,     /* argument lengths */
        NULL,         /* route bytes */
        0             /* route bytes length */
    );

    return result;
}

/* Handle an integer response */
long handle_int_response(CommandResult *result, long *output_value)
{
    /* Check if the command was successful */
    if (!result)
    {
        return 0; /* False - failure */
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0; /* False - failure */
    }

    /* Get the result value */
    printf("result->response->response_type: %d\n", result->response->response_type);
    if (result->response && result->response->response_type == Int)
    {
        *output_value = result->response->int_value;

        /* Free the result */
        free_command_result(result);
        return 1; /* True - success */
    }

    /* Unexpected response type */
    free_command_result(result);
    return 0; /* False - failure */
}

/* Handle a string response */
int handle_string_response(CommandResult *result, char **output, size_t *output_len)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (result->response)
    {
        switch (result->response->response_type)
        {
        case String:
            /* Command returns a string/binary data */
            if (result->response->string_value_len == 0)
            {
                *output = malloc(1); // Allocate at least one byte
                if (*output)
                {
                    (*output)[0] = '\0'; // Empty string is still null-terminated
                }
                *output_len = 0;
            }
            else
            {
                // Allocate exact size needed for binary data
                *output = malloc(result->response->string_value_len);
                if (*output)
                {
                    // Copy binary data without assuming null-termination
                    memcpy(*output, result->response->string_value, result->response->string_value_len);
                }
                *output_len = result->response->string_value_len;
            }
            // Check if allocation failed
            if (!*output)
            {
                ret_val = -1; // Memory allocation failed
            }
            else
            {
                ret_val = 1; // Success
            }
            break;
        case Null:
            /* Key didn't exist, return NULL */
            *output = NULL;
            *output_len = 0;
            ret_val = 0;
            break;
        default:
            ret_val = -1;
            break;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Handle a boolean response */
int handle_bool_response(CommandResult *result)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Get the result value */
    int ret_val = -1;
    if (result->response && result->response->response_type == Bool)
    {
        ret_val = result->response->bool_value ? 1 : 0;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Handle an OK response */
int handle_ok_response(CommandResult *result)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Get the result value */
    int ret_val = -1;
    if (result->response && result->response->response_type == Ok)
    {
        ret_val = 1;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Handle a null or string response */
int handle_null_or_string_response(CommandResult *result, char **output, size_t *output_len)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (result->response)
    {
        switch (result->response->response_type)
        {
        case String:
            /* Command returns a string/binary data */
            if (result->response->string_value_len == 0)
            {
                *output = malloc(1); // Allocate at least one byte
                if (*output)
                {
                    (*output)[0] = '\0'; // Empty string is still null-terminated
                }
                *output_len = 0;
            }
            else
            {
                // Allocate exact size needed for binary data
                *output = malloc(result->response->string_value_len);
                if (*output)
                {
                    // Copy binary data without assuming null-termination
                    memcpy(*output, result->response->string_value, result->response->string_value_len);
                }
                *output_len = result->response->string_value_len;
            }
            // Check if allocation failed
            if (!*output)
            {
                ret_val = -1; // Memory allocation failed
            }
            else
            {
                ret_val = 1; // Success
            }
            break;
        case Null:
            /* Key didn't exist, return NULL */
            *output = NULL;
            *output_len = 0;
            ret_val = 0;
            break;
        default:
            ret_val = -1;
            break;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Handle a double response */
int handle_double_response(CommandResult *result, double *output)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Get the result value */
    int ret_val = -1;
    if (result->response && result->response->response_type == Float)
    {
        *output = result->response->float_value;
        ret_val = 1;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Helper function to convert a CommandResponse to a PHP value */
int command_response_to_zval(CommandResponse *response, zval *output)
{
    if (!response)
    {
        ZVAL_NULL(output);
        return 0;
    }

    switch (response->response_type)
    {
    case Null:
        ZVAL_NULL(output);
        return 0;
    case Int:
        ZVAL_LONG(output, response->int_value);
        return 1;
    case Float:
        ZVAL_DOUBLE(output, response->float_value);
        return 1;
    case Bool:
        ZVAL_BOOL(output, response->bool_value);
        return 1;
    case String:
        ZVAL_STRINGL(output, response->string_value, response->string_value_len);
        return 1;
    case Array:
        array_init(output);
        for (int i = 0; i < response->array_value_len; i++)
        {
            zval value;
            command_response_to_zval(&response->array_value[i], &value);
            add_next_index_zval(output, &value);
        }
        return 1;
#if 1
    case Map:
        array_init(output);
        for (int i = 0; i < response->array_value_len; i++)
        {
            zval key, value;
            CommandResponse *element = &response->array_value[i];

            // Process the key
            if (element->map_key != NULL)
            {
                command_response_to_zval(element->map_key, &key);
            }
            else
            {
                ZVAL_NULL(&key);
            }

            // Process the value
            if (element->map_value != NULL)
            {
                command_response_to_zval(element->map_value, &value);
            }
            else
            {
                ZVAL_NULL(&value);
            }

            // Add the key as a separate array element
            add_next_index_zval(output, &key);

            // Add the value as the next array element
            add_next_index_zval(output, &value);
        }
        return 1;
#endif
    case Sets:
        array_init(output);
        for (int i = 0; i < response->sets_value_len; i++)
        {
            zval value;
            CommandResponse *set_item = &response->sets_value[i];

            if (set_item->response_type == String)
            {
                ZVAL_STRINGL(&value, set_item->string_value, set_item->string_value_len);
                add_next_index_zval(output, &value);
            }
        }
        return 1;
    case Ok:
        ZVAL_STRING(output, "OK");
        return 1;
    default:
        ZVAL_NULL(output);
        return -1;
    }
}

/* Handle an array response */
int handle_array_response(CommandResult *result, zval *output)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (result->response)
    {
        if (result->response->response_type == Null)
        {
            ZVAL_NULL(output);
            ret_val = 0;
        }
        else if (result->response->response_type == Array)
        {
            ret_val = command_response_to_zval(result->response, output);
        }
        else
        {
            ret_val = -1;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Handle a map response */
int handle_map_response(CommandResult *result, zval *output)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (result->response)
    {
        if (result->response->response_type == Null)
        {
            ZVAL_NULL(output);
            ret_val = 0;
        }
        else if (result->response->response_type == Map)
        {
            ret_val = command_response_to_zval(result->response, output);
        }
        else
        {
            ret_val = -1;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Handle a set response */
int handle_set_response(CommandResult *result, zval *output)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (result->response)
    {
        if (result->response->response_type == Null)
        {
            ZVAL_NULL(output);
            ret_val = 0;
        }
        else if (result->response->response_type == Sets)
        {
            ret_val = command_response_to_zval(result->response, output);
        }
        else
        {
            ret_val = -1;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Convert a long value to a string */
char *long_to_string(long value, size_t *len)
{
    char buffer[32];
    *len = snprintf(buffer, sizeof(buffer), "%ld", value);
    char *str = (char *)malloc(*len + 1);
    if (str)
    {
        memcpy(str, buffer, *len);
        str[*len] = '\0';
    }
    return str;
}

/* Convert a double value to a string */
char *double_to_string(double value, size_t *len)
{
    char buffer[64];
    /* Use %.6g format to get a more user-friendly representation */
    *len = snprintf(buffer, sizeof(buffer), "%.6g", value);
    char *str = (char *)malloc(*len + 1);
    if (str)
    {
        memcpy(str, buffer, *len);
        str[*len] = '\0';
    }
    return str;
}
