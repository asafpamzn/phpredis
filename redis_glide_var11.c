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

/* Execute a CONFIG command using the Valkey Glide client */
int execute_config_command(const void *glide_client, const char *operation, size_t operation_len,
                           zval *key, zval *value, zval *return_value)
{
    /* Check if client is valid */
    if (!glide_client || !operation)
    {
        return 0;
    }

    /* Determine the CONFIG operation type */
    enum RequestType command_type;

    if (strncasecmp(operation, "GET", operation_len) == 0)
    {
        command_type = ConfigGet;
    }
    else if (strncasecmp(operation, "SET", operation_len) == 0)
    {
        command_type = ConfigSet;
    }
    else if (strncasecmp(operation, "RESETSTAT", operation_len) == 0)
    {
        command_type = ConfigResetStat;
    }
    else if (strncasecmp(operation, "REWRITE", operation_len) == 0)
    {
        command_type = ConfigRewrite;
    }
    else
    {
        php_error_docref(NULL, E_WARNING, "Unknown CONFIG operation '%s'", operation);
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 0;
    uintptr_t *args = NULL;
    unsigned long *args_len = NULL;
    char **temp_strings = NULL;
    int temp_string_count = 0;

    /* For CONFIG GET */
    if (command_type == ConfigGet)
    {
        if (!key)
        {
            php_error_docref(NULL, E_WARNING, "CONFIG GET requires a parameter");
            return 0;
        }

        /* Handle string or array parameter */
        if (Z_TYPE_P(key) == IS_STRING)
        {
            arg_count = 1;
            args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
            args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

            args[0] = (uintptr_t)Z_STRVAL_P(key);
            args_len[0] = Z_STRLEN_P(key);
        }
        else if (Z_TYPE_P(key) == IS_ARRAY)
        {
            HashTable *ht = Z_ARRVAL_P(key);
            arg_count = zend_hash_num_elements(ht);

            if (arg_count == 0)
            {
                php_error_docref(NULL, E_WARNING, "CONFIG GET array cannot be empty");
                return 0;
            }

            args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
            args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));
            temp_strings = (char **)ecalloc(arg_count, sizeof(char *));

            zval *z_param;
            int i = 0;
            ZEND_HASH_FOREACH_VAL(ht, z_param)
            {
                zval temp;
                ZVAL_COPY(&temp, z_param);
                convert_to_string(&temp);

                temp_strings[temp_string_count] = estrdup(Z_STRVAL(temp));
                args[i] = (uintptr_t)temp_strings[temp_string_count];
                args_len[i] = Z_STRLEN(temp);
                temp_string_count++;
                i++;

                zval_dtor(&temp);
            }
            ZEND_HASH_FOREACH_END();
        }
        else
        {
            php_error_docref(NULL, E_WARNING, "CONFIG GET parameter must be a string or array");
            return 0;
        }
    }
    /* For CONFIG SET */
    else if (command_type == ConfigSet)
    {
        if (!key || (Z_TYPE_P(key) != IS_ARRAY && !value))
        {
            php_error_docref(NULL, E_WARNING, "CONFIG SET requires key and value parameters");
            return 0;
        }

        /* Handle two strings or an array */
        if (Z_TYPE_P(key) == IS_STRING && Z_TYPE_P(value) != IS_NULL)
        {
            /* CONFIG SET key value */
            arg_count = 2;
            args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
            args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));
            temp_strings = (char **)ecalloc(2, sizeof(char *));

            /* Key */
            args[0] = (uintptr_t)Z_STRVAL_P(key);
            args_len[0] = Z_STRLEN_P(key);

            /* Value */
            zval temp;
            ZVAL_COPY(&temp, value);
            convert_to_string(&temp);

            temp_strings[0] = estrdup(Z_STRVAL(temp));
            args[1] = (uintptr_t)temp_strings[0];
            args_len[1] = Z_STRLEN(temp);
            temp_string_count = 1;

            zval_dtor(&temp);
        }
        else if (Z_TYPE_P(key) == IS_ARRAY && value == NULL)
        {
            /* CONFIG SET from array */
            HashTable *ht = Z_ARRVAL_P(key);
            arg_count = zend_hash_num_elements(ht) * 2;

            if (arg_count == 0)
            {
                php_error_docref(NULL, E_WARNING, "CONFIG SET array cannot be empty");
                return 0;
            }

            args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
            args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));
            temp_strings = (char **)ecalloc(zend_hash_num_elements(ht), sizeof(char *));

            zend_string *zkey;
            zval *zvalue;
            int i = 0;
            ZEND_HASH_FOREACH_STR_KEY_VAL(ht, zkey, zvalue)
            {
                if (!zkey)
                {
                    php_error_docref(NULL, E_WARNING, "CONFIG SET array must be associative");
                    goto cleanup;
                }

                /* Add key */
                args[i] = (uintptr_t)ZSTR_VAL(zkey);
                args_len[i] = ZSTR_LEN(zkey);
                i++;

                /* Add value */
                zval temp;
                ZVAL_COPY(&temp, zvalue);
                convert_to_string(&temp);

                temp_strings[temp_string_count] = estrdup(Z_STRVAL(temp));
                args[i] = (uintptr_t)temp_strings[temp_string_count];
                args_len[i] = Z_STRLEN(temp);
                temp_string_count++;
                i++;

                zval_dtor(&temp);
            }
            ZEND_HASH_FOREACH_END();
        }
        else
        {
            php_error_docref(NULL, E_WARNING, "CONFIG SET requires two strings or an array");
            return 0;
        }
    }
    /* CONFIG RESETSTAT and CONFIG REWRITE have no additional arguments */

    else if (command_type == ConfigResetStat || command_type == ConfigRewrite)
    {
        arg_count = 0; /* No arguments needed */
        args = NULL;
        args_len = NULL;
    }
    else
    {
        php_error_docref(NULL, E_WARNING, "Unknown CONFIG operation '%s'", operation);
        return 0;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        command_type,
        arg_count,
        args,
        args_len);

    /* Free temporary strings */
    if (temp_strings)
    {
        for (int i = 0; i < temp_string_count; i++)
        {
            if (temp_strings[i])
                efree(temp_strings[i]);
        }
        efree(temp_strings);
    }

    /* Free the argument arrays */
    if (args)
        efree(args);
    if (args_len)
        efree(args_len);

    /* Handle the result */
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
            if (command_type == ConfigGet)
            {
                /* CONFIG GET returns a Map - convert to associative array */
                status = command_response_to_zval(result->response, return_value, 1);
            }
            else
            {
                /* CONFIG SET/RESETSTAT/REWRITE return OK */
                if (result->response->response_type == Ok)
                {
                    ZVAL_TRUE(return_value);

                    status = 1;
                }
                else
                {
                    ZVAL_FALSE(return_value);
                    status = 0;
                }
            }
        }
        free_command_result(result);
    }

    return status;

cleanup:
    /* Cleanup on error */
    if (temp_strings)
    {
        for (int i = 0; i < temp_string_count; i++)
        {
            if (temp_strings[i])
                efree(temp_strings[i]);
        }
        efree(temp_strings);
    }
    if (args)
        efree(args);
    if (args_len)
        efree(args_len);
    return 0;
}
