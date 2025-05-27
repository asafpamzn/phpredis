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

/* Execute an AUTH command using the Valkey Glide client */
int execute_auth_command(const void *glide_client, const char *password, size_t password_len,
                         const char *username, size_t username_len)
{
    /* Check if client is valid */
    if (!glide_client || !password)
    {
        return 0;
    }

    /* Calculate the number of arguments */
    unsigned long arg_count = username && username_len > 0 ? 2 : 1;

    /* Allocate argument arrays */
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

    /* Set up arguments */
    int arg_idx = 0;

    /* Add username if provided */
    if (username && username_len > 0)
    {
        args[arg_idx] = (uintptr_t)username;
        args_len[arg_idx] = username_len;
        arg_idx++;
    }

    /* Add password (required) */
    args[arg_idx] = (uintptr_t)password;
    args_len[arg_idx] = password_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Auth,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Check if the command was successful */
    int status = 0;

    if (result)
    {
        if (result->command_error)
        {
            /* Command failed */
            free_command_result(result);
            return 0;
        }

        if (result->response && result->response->response_type == Ok)
        {
            /* Success */
            status = 1;
        }
        free_command_result(result);
    }

    return status;
}

/* Execute a SELECT command using the Valkey Glide client */
int execute_select_command(const void *glide_client, long database)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
    }

    /* Convert database index to string */
    size_t db_len;
    char *db_str = long_to_string(database, &db_len);
    if (!db_str)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1;
    uintptr_t args[1];
    unsigned long args_len[1];

    /* Set database index */
    args[0] = (uintptr_t)db_str;
    args_len[0] = db_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Select,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the database string */
    efree(db_str);

    /* Check if the command was successful */
    int status = 0;

    if (result)
    {
        if (result->command_error)
        {
            /* Command failed */
            free_command_result(result);
            return 0;
        }

        if (result->response && result->response->response_type == Ok)
        {
            /* Success */
            status = 1;
        }
        free_command_result(result);
    }

    return status;
}

/* Execute a SWAPDB command using the Valkey Glide client */
int execute_swapdb_command(const void *glide_client, long db1, long db2)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
    }

    /* Convert database indices to strings */
    size_t db1_len, db2_len;
    char *db1_str = long_to_string(db1, &db1_len);
    char *db2_str = NULL;

    if (!db1_str)
    {
        return 0;
    }

    db2_str = long_to_string(db2, &db2_len);
    if (!db2_str)
    {
        efree(db1_str);
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2;
    uintptr_t args[2];
    unsigned long args_len[2];

    /* Set database indices */
    args[0] = (uintptr_t)db1_str;
    args_len[0] = db1_len;
    args[1] = (uintptr_t)db2_str;
    args_len[1] = db2_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SwapDb,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the database strings */
    efree(db1_str);
    efree(db2_str);

    /* Check if the command was successful */
    int status = 0;

    if (result)
    {
        if (result->command_error)
        {
            /* Command failed */
            free_command_result(result);
            return 0;
        }

        if (result->response && result->response->response_type == Ok)
        {
            /* Success */
            status = 1;
        }
        free_command_result(result);
    }

    return status;
}

/* Execute a MOVE command using the Valkey Glide client */
int execute_move_command(const void *glide_client, const char *key, size_t key_len, long db, int *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !output_value)
    {
        return 0;
    }

    /* Convert database index to string */
    size_t db_len;
    char *db_str = long_to_string(db, &db_len);
    if (!db_str)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2;
    uintptr_t args[2];
    unsigned long args_len[2];

    /* Set arguments: key, database index */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;
    args[1] = (uintptr_t)db_str;
    args_len[1] = db_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Move,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the database string */
    efree(db_str);

    /* Check if the command was successful */
    int status = 0;
    long value = 0;

    if (result)
    {
        if (result->command_error)
        {
            /* Command failed */
            free_command_result(result);
            return 0;
        }

        if (result->response && result->response->response_type == Int)
        {
            /* Success, set output value */
            value = result->response->int_value;
            *output_value = (int)value;
            status = 1;
        }
        free_command_result(result);
    }

    return status;
}

/* Execute a BZPOPMAX command using the Valkey Glide client */
int execute_bzpopmax_command(const void *glide_client, zval *keys, int keys_count,
                             double timeout, zval *return_value)
{
    /* Check if client, keys, and return_value are valid */
    if (!glide_client || !keys || keys_count <= 0 || !return_value)
    {
        return 0;
    }

    /* Calculate the number of arguments */
    unsigned long arg_count = keys_count + 1; /* keys + timeout */

    /* Allocate argument arrays */
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

    /* Add keys as arguments */
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

    /* Add timeout as the last argument */
    size_t timeout_len;
    char *timeout_str = double_to_string(timeout, &timeout_len);
    if (!timeout_str)
    {
        efree(args);
        efree(args_len);
        return 0;
    }
    args[keys_count] = (uintptr_t)timeout_str;
    args_len[keys_count] = timeout_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        BZPopMax,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the timeout string */
    efree(timeout_str);

    /* Free the argument arrays */
    efree(args);
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
            if (result->response->response_type == Null)
            {
                /* Timeout occurred, return false */
                ZVAL_FALSE(return_value);
                status = 1;
            }
            else if (result->response->response_type == Array)
            {
                /* For BZPOPMIN, need to manually ensure the score is a string */
                if (result->response->array_value_len == 3 &&
                    result->response->array_value[2].response_type != String)
                {

                    /* Convert the response array to PHP array */
                    status = command_response_to_zval(result->response, return_value, 0);

                    /* Get the score element (should be index 2) */
                    zval *score = NULL;
                    zval *arr = return_value;
                    HashTable *ht = Z_ARRVAL_P(arr);

                    /* Convert numeric score to string */
                    if (ht && zend_hash_index_exists(ht, 2))
                    {
                        score = zend_hash_index_find(ht, 2);
                        if (score && (Z_TYPE_P(score) == IS_LONG || Z_TYPE_P(score) == IS_DOUBLE))
                        {
                            convert_to_string(score);
                        }
                    }
                }
                else
                {
                    /* Regular array conversion */
                    status = command_response_to_zval(result->response, return_value, 0);
                }
            }
        }
        free_command_result(result);
    }

    return status;
}

/* Execute a BZPOPMIN command using the Valkey Glide client */
int execute_bzpopmin_command(const void *glide_client, zval *keys, int keys_count,
                             double timeout, zval *return_value)
{
    /* Check if client, keys, and return_value are valid */
    if (!glide_client || !keys || keys_count <= 0 || !return_value)
    {
        return 0;
    }

    /* Calculate the number of arguments */
    unsigned long arg_count = keys_count + 1; /* keys + timeout */

    /* Allocate argument arrays */
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

    /* Add keys as arguments */
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

    /* Add timeout as the last argument */
    size_t timeout_len;
    char *timeout_str = double_to_string(timeout, &timeout_len);
    if (!timeout_str)
    {
        efree(args);
        efree(args_len);
        return 0;
    }
    args[keys_count] = (uintptr_t)timeout_str;
    args_len[keys_count] = timeout_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        BZPopMin,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the timeout string */
    efree(timeout_str);

    /* Free the argument arrays */
    efree(args);
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
            if (result->response->response_type == Null)
            {
                /* Timeout occurred, return false */
                ZVAL_FALSE(return_value);
                status = 1;
            }
            else if (result->response->response_type == Array)
            {
                /* For BZPOPMIN, need to manually ensure the score is a string */
                if (result->response->array_value_len == 3 &&
                    result->response->array_value[2].response_type != String)
                {

                    /* Convert the response array to PHP array */
                    status = command_response_to_zval(result->response, return_value, 0);

                    /* Get the score element (should be index 2) */
                    zval *score = NULL;
                    zval *arr = return_value;
                    HashTable *ht = Z_ARRVAL_P(arr);

                    /* Convert numeric score to string */
                    if (ht && zend_hash_index_exists(ht, 2))
                    {
                        score = zend_hash_index_find(ht, 2);
                        if (score && (Z_TYPE_P(score) == IS_LONG || Z_TYPE_P(score) == IS_DOUBLE))
                        {
                            convert_to_string(score);
                        }
                    }
                }
                else
                {
                    /* Regular array conversion */
                    status = command_response_to_zval(result->response, return_value, 0);
                }
            }
        }
        free_command_result(result);
    }

    return status;
}
