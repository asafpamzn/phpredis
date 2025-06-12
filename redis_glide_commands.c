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

/* Execute a setOption command using the Valkey Glide client */
int execute_setOption_command(const void *glide_client, zend_long option, zval *value)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
    }

    /* Currently, Glide client doesn't directly support setting client options,
     * so we store them in the local Redis object. We'll return success for now. */
    return 1;
}

/* Execute an MSET command using the Valkey Glide client */
int execute_mset_command(const void *glide_client, zval *arr)
{
    /* Check if client and array are valid */
    if (!glide_client || !arr || Z_TYPE_P(arr) != IS_ARRAY)
    {
        return 0;
    }

    /* Get the hash table from the array */
    HashTable *ht = Z_ARRVAL_P(arr);
    int key_count = zend_hash_num_elements(ht);

    if (key_count == 0)
    {
        return 0;
    }

    /* Prepare command arguments - each key-value pair requires 2 arguments */
    unsigned long arg_count = key_count * 2;
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

    /* Track allocated strings that need to be freed after command execution */
    char **allocated_strings = (char **)ecalloc(arg_count, sizeof(char *));
    int allocated_count = 0;

    if (!allocated_strings)
    {
        efree(args);
        efree(args_len);
        return 0;
    }

    /* Add keys and values as arguments */
    zval *data;
    zend_string *key;
    zend_ulong num_key;
    int arg_idx = 0;

    ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, key, data)
    {
        if (!key)
        {
            /* Not a string key - convert numeric key to string */
            size_t key_len;
            char *key_str = long_to_string((long)num_key, &key_len);
            if (!key_str)
            {
                /* Free any previously allocated strings */
                int i;
                for (i = 0; i < allocated_count; i++)
                {
                    efree(allocated_strings[i]);
                }
                efree(allocated_strings);
                efree(args);
                efree(args_len);
                return 0;
            }

            /* Add key */
            args[arg_idx] = (uintptr_t)key_str;
            args_len[arg_idx] = key_len;

            /* Track this allocated string */
            allocated_strings[allocated_count++] = key_str;

            arg_idx++;

            /* Add value */
            if (Z_TYPE_P(data) == IS_STRING)
            {
                args[arg_idx] = (uintptr_t)Z_STRVAL_P(data);
                args_len[arg_idx] = Z_STRLEN_P(data);
                arg_idx++;
            }
            else
            {
                /* Convert non-string value to string */
                zval copy;
                size_t value_len;
                char *value_str = NULL;

                /* Make a copy of the zval as a string */
                ZVAL_COPY(&copy, data);
                convert_to_string(&copy);

                /* Add value */
                args[arg_idx] = (uintptr_t)Z_STRVAL(copy);
                args_len[arg_idx] = Z_STRLEN(copy);
                arg_idx++;

                /* We will free this when we free the arguments */
                zval_dtor(&copy);
            }
        }
        else
        {
            /* Add key */
            args[arg_idx] = (uintptr_t)ZSTR_VAL(key);
            args_len[arg_idx] = ZSTR_LEN(key);
            arg_idx++;

            /* Add value */
            if (Z_TYPE_P(data) == IS_STRING)
            {
                args[arg_idx] = (uintptr_t)Z_STRVAL_P(data);
                args_len[arg_idx] = Z_STRLEN_P(data);
                arg_idx++;
            }
            else
            {
                /* Convert non-string value to string */
                zval copy;
                size_t value_len;
                char *value_str = NULL;

                /* Make a copy of the zval as a string */
                ZVAL_COPY(&copy, data);
                convert_to_string(&copy);

                /* Add value */
                args[arg_idx] = (uintptr_t)Z_STRVAL(copy);
                args_len[arg_idx] = Z_STRLEN(copy);
                arg_idx++;

                /* We will free this when we free the arguments */
                zval_dtor(&copy);
            }
        }
    }
    ZEND_HASH_FOREACH_END();

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        MSet,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free all allocated key strings */
    int i;
    for (i = 0; i < allocated_count; i++)
    {
        efree(allocated_strings[i]);
    }
    efree(allocated_strings);

    /* Free the allocated arguments */
    efree(args);
    efree(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing MSET command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Success is indicated by an OK response */
    int success = 0;
    if (result->response && result->response->response_type == Ok)
    {
        success = 1;
    }

    /* Free the result */
    free_command_result(result);

    return success;
}

/* Execute an MSETNX command using the Valkey Glide client */
int execute_msetnx_command(const void *glide_client, zval *arr, int *output_value)
{
    /* Check if client and array are valid */
    if (!glide_client || !arr || Z_TYPE_P(arr) != IS_ARRAY || !output_value)
    {
        return 0;
    }

    /* Get the hash table from the array */
    HashTable *ht = Z_ARRVAL_P(arr);
    int key_count = zend_hash_num_elements(ht);

    if (key_count == 0)
    {
        return 0;
    }

    /* Prepare command arguments - each key-value pair requires 2 arguments */
    unsigned long arg_count = key_count * 2;
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

    /* Track allocated strings that need to be freed after command execution */
    char **allocated_strings = (char **)ecalloc(arg_count, sizeof(char *));
    int allocated_count = 0;

    if (!allocated_strings)
    {
        efree(args);
        efree(args_len);
        return 0;
    }

    /* Add keys and values as arguments */
    zval *data;
    zend_string *key;
    zend_ulong num_key;
    int arg_idx = 0;

    ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, key, data)
    {
        if (!key)
        {
            /* Not a string key - convert numeric key to string */
            size_t key_len;
            char *key_str = long_to_string((long)num_key, &key_len);
            if (!key_str)
            {
                /* Free any previously allocated strings */
                int i;
                for (i = 0; i < allocated_count; i++)
                {
                    efree(allocated_strings[i]);
                }
                efree(allocated_strings);
                efree(args);
                efree(args_len);
                return 0;
            }

            /* Add key */
            args[arg_idx] = (uintptr_t)key_str;
            args_len[arg_idx] = key_len;

            /* Track this allocated string */
            allocated_strings[allocated_count++] = key_str;

            arg_idx++;

            /* Add value */
            if (Z_TYPE_P(data) == IS_STRING)
            {
                args[arg_idx] = (uintptr_t)Z_STRVAL_P(data);
                args_len[arg_idx] = Z_STRLEN_P(data);
                arg_idx++;
            }
            else
            {
                /* Convert non-string value to string */
                zval copy;
                size_t value_len;
                char *value_str = NULL;

                /* Make a copy of the zval as a string */
                ZVAL_COPY(&copy, data);
                convert_to_string(&copy);

                /* Add value */
                args[arg_idx] = (uintptr_t)Z_STRVAL(copy);
                args_len[arg_idx] = Z_STRLEN(copy);
                arg_idx++;

                /* We will free this when we free the arguments */
                zval_dtor(&copy);
            }
        }
        else
        {
            /* Add key */
            args[arg_idx] = (uintptr_t)ZSTR_VAL(key);
            args_len[arg_idx] = ZSTR_LEN(key);
            arg_idx++;

            /* Add value */
            if (Z_TYPE_P(data) == IS_STRING)
            {
                args[arg_idx] = (uintptr_t)Z_STRVAL_P(data);
                args_len[arg_idx] = Z_STRLEN_P(data);
                arg_idx++;
            }
            else
            {
                /* Convert non-string value to string */
                zval copy;
                size_t value_len;
                char *value_str = NULL;

                /* Make a copy of the zval as a string */
                ZVAL_COPY(&copy, data);
                convert_to_string(&copy);

                /* Add value */
                args[arg_idx] = (uintptr_t)Z_STRVAL(copy);
                args_len[arg_idx] = Z_STRLEN(copy);
                arg_idx++;

                /* We will free this when we free the arguments */
                zval_dtor(&copy);
            }
        }
    }
    ZEND_HASH_FOREACH_END();

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        MSetNX,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free all allocated key strings */
    int i;
    for (i = 0; i < allocated_count; i++)
    {
        efree(allocated_strings[i]);
    }
    efree(allocated_strings);

    /* Free the allocated arguments */
    efree(args);
    efree(args_len);

    /* Check if the command was successful */
    int ret = 0;

    if (!result)
    {
        return 0; /* Failed to execute command */
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing MSETNX command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Process the result based on its type */
    if (result->response)
    {
        switch (result->response->response_type)
        {
        case Int:
            /* Handle integer response */
            *output_value = (int)result->response->int_value;
            ret = 1;
            break;

        case Bool:
            /* Handle boolean response */
            *output_value = result->response->bool_value ? 1 : 0;
            ret = 1;
            break;

        default:
            /* Unexpected response type */
            printf("Unexpected response type for MSETNX command: %d\n", result->response->response_type);
            ret = 0;
            break;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret;
}
