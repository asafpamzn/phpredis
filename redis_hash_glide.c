
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
int execute_hset_command(const void *glide_client, const char *key, size_t key_len, zval *z_args, int argc, long *output_value, int is_array_arg)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !z_args)
    {
        return 0;
    }

    /* Handle the case when we get a single associative array argument */
    if (is_array_arg)
    {
        if (argc != 1 || Z_TYPE(z_args[0]) != IS_ARRAY)
        {
            return 0;
        }

        zval *z_array = &z_args[0];
        HashTable *ht = Z_ARRVAL_P(z_array);
        int pairs_count = zend_hash_num_elements(ht);

        if (pairs_count == 0)
        {
            return 0;
        }

        /* Prepare command arguments - key + field-value pairs */
        unsigned long arg_count = 1 + (pairs_count * 2);
        uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
        unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));
        char **allocated_strings = (char **)emalloc((pairs_count * 2) * sizeof(char *));
        int allocated_count = 0;

        if (!args || !args_len || !allocated_strings)
        {
            if (args)
                efree(args);
            if (args_len)
                efree(args_len);
            if (allocated_strings)
                efree(allocated_strings);
            return 0;
        }

        /* First argument: key */
        args[0] = (uintptr_t)key;
        args_len[0] = key_len;

        /* Add field-value pairs as arguments */
        zval *data;
        zend_string *hash_key;
        zend_ulong num_idx;
        int arg_idx = 1;

        ZEND_HASH_FOREACH_KEY_VAL(ht, num_idx, hash_key, data)
        {
            /* Add field */
            if (hash_key)
            {
                /* Associative array: key is the field */
                args[arg_idx] = (uintptr_t)ZSTR_VAL(hash_key);
                args_len[arg_idx] = ZSTR_LEN(hash_key);
            }
            else
            {
                /* Numeric index - convert to string */
                char *field_str = long_to_string(num_idx, &args_len[arg_idx]);
                args[arg_idx] = (uintptr_t)field_str;
                allocated_strings[allocated_count++] = field_str;
            }
            arg_idx++;

            /* Add value */
            if (Z_TYPE_P(data) == IS_STRING)
            {
                args[arg_idx] = (uintptr_t)Z_STRVAL_P(data);
                args_len[arg_idx] = Z_STRLEN_P(data);
            }
            else
            {
                /* Convert non-string values to string */
                char *str_val = NULL;
                size_t str_len = 0;

                if (Z_TYPE_P(data) == IS_LONG)
                {
                    str_val = long_to_string(Z_LVAL_P(data), &str_len);
                }
                else if (Z_TYPE_P(data) == IS_DOUBLE)
                {
                    str_val = double_to_string(Z_DVAL_P(data), &str_len);
                }
                else if (Z_TYPE_P(data) == IS_TRUE)
                {
                    str_val = estrdup("1");
                    str_len = 1;
                }
                else if (Z_TYPE_P(data) == IS_FALSE)
                {
                    str_val = estrdup("0");
                    str_len = 1;
                }
                else if (Z_TYPE_P(data) == IS_NULL)
                {
                    str_val = estrdup("");
                    str_len = 0;
                }
                else
                {
                    /* Handle other types as empty string */
                    str_val = estrdup("");
                    str_len = 0;
                }

                if (str_val)
                {
                    args[arg_idx] = (uintptr_t)str_val;
                    args_len[arg_idx] = str_len;
                    allocated_strings[allocated_count++] = str_val;
                }
                else
                {
                    /* Free already allocated strings */
                    for (int i = 0; i < allocated_count; i++)
                    {
                        efree(allocated_strings[i]);
                    }
                    efree(allocated_strings);
                    efree(args);
                    efree(args_len);
                    return 0;
                }
            }
            arg_idx++;
        }
        ZEND_HASH_FOREACH_END();

        /* Execute the command */
        CommandResult *result = execute_command(
            glide_client,
            HSet,      /* command type */
            arg_count, /* number of arguments */
            args,      /* arguments */
            args_len   /* argument lengths */
        );

        /* Free allocated strings */
        for (int i = 0; i < allocated_count; i++)
        {
            efree(allocated_strings[i]);
        }
        efree(allocated_strings);
        efree(args);
        efree(args_len);

        /* Use the generic handler to process the result */
        return handle_int_response(result, output_value);
    }
    else
    {
        /* Original variadic usage - check if arguments are valid */
        if (argc < 2 || argc % 2 != 0)
        {
            return 0;
        }

        /* Prepare command arguments */
        unsigned long arg_count = 1 + argc; /* key + field/value pairs */
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
                    str_val = estrdup("1");
                    str_len = 1;
                }
                else if (Z_TYPE_P(value) == IS_FALSE)
                {
                    str_val = estrdup("0");
                    str_len = 1;
                }
                else
                {
                    /* Handle other types or error */
                    efree(args);
                    efree(args_len);
                    return 0;
                }

                if (str_val)
                {
                    args[i + 1] = (uintptr_t)str_val;
                    args_len[i + 1] = str_len;
                }
                else
                {
                    efree(args);
                    efree(args_len);
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
                efree((void *)args[i + 1]);
            }
        }

        /* Free the argument arrays */
        efree(args);
        efree(args_len);

        /* Use the generic handler to process the result */
        return handle_int_response(result, output_value);
    }
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
        HSetNX,    /* command type */
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
    return handle_bool_response(result);
}

/* Execute an HINCRBY command using the Valkey Glide client */
int execute_hincrby_command(const void *glide_client, const char *key, size_t key_len,
                            char *field, size_t field_len,
                            long increment, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !field)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3;
    uintptr_t args[3];
    unsigned long args_len[3];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: field */
    args[1] = (uintptr_t)field;
    args_len[1] = field_len;

    /* Third argument: increment */
    char increment_str[32];
    int increment_len = snprintf(increment_str, sizeof(increment_str), "%ld", increment);
    args[2] = (uintptr_t)increment_str;
    args_len[2] = increment_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        HIncrBy,   /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute an HINCRBYFLOAT command using the Valkey Glide client */
int execute_hincrbyfloat_command(const void *glide_client, const char *key, size_t key_len,
                                 char *field, size_t field_len,
                                 double increment, double *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !field)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3;
    uintptr_t args[3];
    unsigned long args_len[3];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: field */
    args[1] = (uintptr_t)field;
    args_len[1] = field_len;

    /* Third argument: increment */
    char increment_str[64];
    int increment_len = snprintf(increment_str, sizeof(increment_str), "%.17g", increment);
    args[2] = (uintptr_t)increment_str;
    args_len[2] = increment_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        HIncrByFloat, /* command type */
        arg_count,    /* number of arguments */
        args,         /* arguments */
        args_len      /* argument lengths */
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

    /* Process the result (a string representing a double) */
    int ret_val = command_response_to_zval(result->response, output_value, 0);

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute an HMGET command using the Valkey Glide client */
int execute_hmget_command(const void *glide_client, const char *key, size_t key_len,
                          zval *fields, int fields_count, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || fields_count <= 0)
    {
        return 0;
    }

    /* Count valid fields (excluding NULL, false, and empty strings) */
    int valid_fields_count = 0;
    for (int i = 0; i < fields_count; i++)
    {
        zval *field = &fields[i];
        if (Z_TYPE_P(field) == IS_STRING && Z_STRLEN_P(field) > 0)
        {
            valid_fields_count++;
        }
        else if (Z_TYPE_P(field) == IS_LONG || Z_TYPE_P(field) == IS_DOUBLE || Z_TYPE_P(field) == IS_TRUE)
        {
            valid_fields_count++;
        }
        /* Skip NULL, false, empty string */
    }

    /* If no valid fields, return failure */
    if (valid_fields_count == 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1 + valid_fields_count; /* key + valid fields */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));
    char **allocated_strings = NULL;
    int allocated_count = 0;

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Allocate memory for string conversions if needed */
    allocated_strings = (char **)emalloc(valid_fields_count * sizeof(char *));
    if (!allocated_strings)
    {
        efree(args);
        efree(args_len);
        return 0;
    }

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add only valid fields as arguments */
    int arg_idx = 1;
    zval *original_fields = emalloc(valid_fields_count * sizeof(zval));
    int original_idx = 0;

    for (int i = 0; i < fields_count; i++)
    {
        zval *field = &fields[i];

        /* Skip invalid field types */
        if ((Z_TYPE_P(field) == IS_STRING && Z_STRLEN_P(field) == 0) ||
            Z_TYPE_P(field) == IS_FALSE || Z_TYPE_P(field) == IS_NULL)
        {
            continue;
        }

        /* Keep track of original field for result mapping */
        ZVAL_COPY_VALUE(&original_fields[original_idx++], field);

        if (Z_TYPE_P(field) == IS_STRING)
        {
            args[arg_idx] = (uintptr_t)Z_STRVAL_P(field);
            args_len[arg_idx] = Z_STRLEN_P(field);
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
                str_val = estrdup("1");
                str_len = 1;
            }
            else
            {
                /* This shouldn't happen due to our filtering above */
                str_val = estrdup("");
                str_len = 0;
            }

            if (str_val)
            {
                args[arg_idx] = (uintptr_t)str_val;
                args_len[arg_idx] = str_len;
                allocated_strings[allocated_count++] = str_val;
            }
            else
            {
                /* Clean up on failure */
                for (int j = 0; j < allocated_count; j++)
                {
                    efree(allocated_strings[j]);
                }
                efree(allocated_strings);
                efree(args);
                efree(args_len);
                efree(original_fields);
                return 0;
            }
        }

        arg_idx++;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        HMGet,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Clean up allocated strings */
    for (int i = 0; i < allocated_count; i++)
    {
        efree(allocated_strings[i]);
    }
    efree(allocated_strings);
    efree(args);
    efree(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        efree(original_fields);
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        efree(original_fields);
        return 0;
    }

    /* Process the result - map back to original field names */
    int ret_val = 0;
    if (result->response && result->response->response_type == Array)
    {
        for (int i = 0; i < valid_fields_count && i < result->response->array_value_len; i++)
        {
            zval *field = &original_fields[i];
            zval field_value;
            char *field_str = NULL;
            size_t field_len = 0;
            int need_to_free = 0;

            /* Convert field to string for associative array key */
            if (Z_TYPE_P(field) == IS_STRING)
            {
                field_str = Z_STRVAL_P(field);
                field_len = Z_STRLEN_P(field);
            }
            else
            {
                /* Convert other types to string */
                if (Z_TYPE_P(field) == IS_LONG)
                {
                    field_str = long_to_string(Z_LVAL_P(field), &field_len);
                    need_to_free = 1;
                }
                else if (Z_TYPE_P(field) == IS_DOUBLE)
                {
                    field_str = double_to_string(Z_DVAL_P(field), &field_len);
                    need_to_free = 1;
                }
                else if (Z_TYPE_P(field) == IS_TRUE)
                {
                    field_str = estrdup("1");
                    field_len = 1;
                    need_to_free = 1;
                }
                else
                {
                    /* This shouldn't happen with our valid field filtering */
                    field_str = estrdup("");
                    field_len = 0;
                    need_to_free = 1;
                }
            }

            /* Set value in result array */
            struct CommandResponse *element = &result->response->array_value[i];

            if (element->response_type == String)
            {
                ZVAL_STRINGL(&field_value, element->string_value, element->string_value_len);
            }
            else if (element->response_type == Null)
            {
                ZVAL_FALSE(&field_value);
            }
            else
            {
                ZVAL_NULL(&field_value);
            }

            if (field_str != NULL)
            {
                add_assoc_zval_ex(return_value, field_str, field_len, &field_value);

                /* Free the field string if we allocated it */
                if (need_to_free)
                {
                    efree(field_str);
                }
            }
            else
            {
                add_next_index_zval(return_value, &field_value);
            }
        }
        ret_val = 1;
    }

    /* Free the result and temporary array */
    free_command_result(result);
    efree(original_fields);

    return ret_val;
}

/* Execute an HMSET command using the Valkey Glide client */
int execute_hmset_command(const void *glide_client, const char *key, size_t key_len,
                          zval *keyvals, int keyvals_count)
{
    /* Check if client, key, and keyvals are valid */
    if (!glide_client || !key || !keyvals || keyvals_count <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    HashTable *keyvals_hash = Z_ARRVAL_P(keyvals);
    int pairs_count = zend_hash_num_elements(keyvals_hash);
    unsigned long arg_count = 1 + (pairs_count * 2); /* key + (field, value) pairs */

    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));
    char **allocated_strings = (char **)emalloc((pairs_count * 2) * sizeof(char *));
    int allocated_count = 0;

    if (!args || !args_len || !allocated_strings)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        if (allocated_strings)
            efree(allocated_strings);
        return 0;
    }

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add field-value pairs as arguments */
    zval *data;
    zend_string *hash_key;
    zend_ulong num_idx;
    int arg_idx = 1;

    ZEND_HASH_FOREACH_KEY_VAL(keyvals_hash, num_idx, hash_key, data)
    {
        /* Add field */
        if (hash_key)
        {
            /* Associative array: key is the field */
            args[arg_idx] = (uintptr_t)ZSTR_VAL(hash_key);
            args_len[arg_idx] = ZSTR_LEN(hash_key);
        }
        else
        {
            /* Numeric index - this shouldn't happen for HMSET, but handle it anyway */
            char *field_str = long_to_string(num_idx, &args_len[arg_idx]);
            args[arg_idx] = (uintptr_t)field_str;
            allocated_strings[allocated_count++] = field_str;
        }
        arg_idx++;

        /* Add value */
        if (Z_TYPE_P(data) == IS_STRING)
        {
            args[arg_idx] = (uintptr_t)Z_STRVAL_P(data);
            args_len[arg_idx] = Z_STRLEN_P(data);
        }
        else
        {
            /* Convert non-string values to string */
            char *str_val = NULL;
            size_t str_len = 0;

            if (Z_TYPE_P(data) == IS_LONG)
            {
                str_val = long_to_string(Z_LVAL_P(data), &str_len);
            }
            else if (Z_TYPE_P(data) == IS_DOUBLE)
            {
                str_val = double_to_string(Z_DVAL_P(data), &str_len);
            }
            else if (Z_TYPE_P(data) == IS_TRUE)
            {
                str_val = estrdup("1");
                str_len = 1;
            }
            else if (Z_TYPE_P(data) == IS_FALSE)
            {
                str_val = estrdup("0");
                str_len = 1;
            }
            else if (Z_TYPE_P(data) == IS_NULL)
            {
                str_val = estrdup("");
                str_len = 0;
            }
            else
            {
                /* Handle other types as empty string */
                str_val = estrdup("");
                str_len = 0;
            }

            if (str_val)
            {
                args[arg_idx] = (uintptr_t)str_val;
                args_len[arg_idx] = str_len;
                allocated_strings[allocated_count++] = str_val;
            }
            else
            {
                /* Free already allocated strings */
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
        }
        arg_idx++;
    }
    ZEND_HASH_FOREACH_END();

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        HMSet,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated strings */
    int i;
    for (i = 0; i < allocated_count; i++)
    {
        efree(allocated_strings[i]);
    }
    efree(allocated_strings);
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
        free_command_result(result);
        return 0;
    }

    /* Check for OK response */
    int ret_val = 0;
    if (result->response)
    {
        if (result->response->response_type == Ok)
        {
            ret_val = 1;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute an HRANDFIELD command using the Valkey Glide client */
int execute_hrandfield_command(const void *glide_client, const char *key, size_t key_len,
                               long count, int withvalues, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1;
    char count_str[32];
    int count_str_len = 0;

    if (count != 1)
    {
        arg_count = 2;
        count_str_len = snprintf(count_str, sizeof(count_str), "%ld", count);
    }

    if (withvalues && count != 1)
    {
        arg_count = 3;
    }

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

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument (optional): count */
    if (count != 1)
    {
        args[1] = (uintptr_t)count_str;
        args_len[1] = count_str_len;
    }

    /* Third argument (optional): WITHVALUES */
    if (withvalues && count != 1)
    {
        const char *withvalues_str = "WITHVALUES";
        args[2] = (uintptr_t)withvalues_str;
        args_len[2] = strlen(withvalues_str);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        HRandField, /* command type */
        arg_count,  /* number of arguments */
        args,       /* arguments */
        args_len    /* argument lengths */
    );
    printf("file = %s, line = %d\n", __FILE__, __LINE__);
    /* Free the argument arrays */
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
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    int ret_val = 0;
    if (result->response)
    {
        /* Single field case */
        if (count == 1 && !withvalues)
        {
            if (result->response->response_type == String)
            {
                printf("file = %s, line = %d\n", __FILE__, __LINE__);
                add_next_index_stringl(return_value, result->response->string_value, result->response->string_value_len);
                ret_val = 1;
            }
            else if (result->response->response_type == Null)
            {
                printf("file = %s, line = %d\n", __FILE__, __LINE__);
                add_next_index_null(return_value);
                ret_val = 1;
            }
        }
        /* Multiple fields without values */
        else if (count != 1 && !withvalues && result->response->response_type == Array)
        {
            printf("file = %s, line = %d\n", __FILE__, __LINE__);
            size_t i;
            for (i = 0; i < result->response->array_value_len; i++)
            {
                struct CommandResponse *element = &result->response->array_value[i];
                if (element->response_type == String)
                {
                    add_next_index_stringl(return_value, element->string_value, element->string_value_len);
                }
                else if (element->response_type == Null)
                {
                    add_next_index_null(return_value);
                }
            }
            ret_val = 1;
        }
        /* Multiple fields with values (field-value pairs) */
        else if (count != 1 && withvalues && result->response->response_type == Array)
        {
            size_t i;
            for (i = 0; i < result->response->array_value_len; i++)
            {
                struct CommandResponse *element = &result->response->array_value[i];

                // Each element should be an array with a field and value
                if (element->response_type == Array && element->array_value_len == 2)
                {
                    struct CommandResponse *field = &element->array_value[0];
                    struct CommandResponse *value = &element->array_value[1];

                    if (field->response_type == String)
                    {
                        if (value->response_type == String)
                        {
                            add_assoc_stringl_ex(return_value, field->string_value, field->string_value_len,
                                                 value->string_value, value->string_value_len);
                        }
                        else if (value->response_type == Null)
                        {
                            add_assoc_null_ex(return_value, field->string_value, field->string_value_len);
                        }
                        else if (value->response_type == Int)
                        {
                            add_assoc_long_ex(return_value, field->string_value, field->string_value_len,
                                              value->int_value);
                        }
                        else if (value->response_type == Float)
                        {
                            add_assoc_double_ex(return_value, field->string_value, field->string_value_len,
                                                value->float_value);
                        }
                        else if (value->response_type == Bool)
                        {
                            add_assoc_bool_ex(return_value, field->string_value, field->string_value_len,
                                              value->bool_value);
                        }
                    }
                }
            }
            ret_val = 1;
        }
    }
    printf("file = %s, line = %d\n", __FILE__, __LINE__);
    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute an HSTRLEN command using the Valkey Glide client */
int execute_hstrlen_command(const void *glide_client, const char *key, size_t key_len,
                            char *field, size_t field_len, long *output_value)
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
        HStrlen,   /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
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
                str_val = estrdup("1");
                str_len = 1;
            }
            else if (Z_TYPE_P(field) == IS_FALSE)
            {
                str_val = estrdup("0");
                str_len = 1;
            }
            else
            {
                /* Handle other types or error */
                efree(args);
                efree(args_len);
                return 0;
            }

            if (str_val)
            {
                args[i + 1] = (uintptr_t)str_val;
                args_len[i + 1] = str_len;
            }
            else
            {
                efree(args);
                efree(args_len);
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
            efree((void *)args[i + 1]);
        }
    }

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

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

    return handle_bool_response(result);
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

    int ret_val = command_response_to_zval(result->response, return_value, 1);

    /* Free the result */
    free_command_result(result);

    return ret_val;
}
