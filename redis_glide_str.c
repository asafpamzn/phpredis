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

/* Execute a TYPE command using the Valkey Glide client */
int execute_type_command(const void *glide_client, const char *key, size_t key_len, long *result)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1; /* key */
    uintptr_t args[1];
    unsigned long args_len[1];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Execute the command */
    CommandResult *cmd_result = execute_command(
        glide_client,
        Type,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Get the string result first */
    char *type_str = NULL;
    size_t type_len = 0;
    int ret = handle_string_response(cmd_result, &type_str, &type_len);

    /* If we have a valid string response, map it to the appropriate constant */
    if (ret == 1 && type_str != NULL)
    {
        if (strcmp(type_str, "string") == 0)
        {
            *result = 1; /* REDIS_STRING */
        }
        else if (strcmp(type_str, "list") == 0)
        {
            *result = 3; /* REDIS_LIST */
        }
        else if (strcmp(type_str, "set") == 0)
        {
            *result = 2; /* REDIS_SET */
        }
        else if (strcmp(type_str, "zset") == 0)
        {
            *result = 4; /* REDIS_ZSET */
        }
        else if (strcmp(type_str, "hash") == 0)
        {
            *result = 5; /* REDIS_HASH */
        }
        else if (strcmp(type_str, "stream") == 0)
        {
            *result = 6; /* REDIS_STREAM */
        }
        else if (strcmp(type_str, "none") == 0)
        {
            *result = 0; /* REDIS_NOT_FOUND */
        }
        else
        {
            /* Unknown type, default to NOT_FOUND */
            *result = 0;
        }

        /* Free the string response */
        if (type_str)
        {
            efree(type_str);
        }

        return 1; /* Success */
    }
    else if (ret == 0)
    {
        /* Key doesn't exist */
        *result = 0; /* REDIS_NOT_FOUND */
        return 1;
    }

    /* Error occurred */
    return ret;
}

/* Execute an APPEND command using the Valkey Glide client */
int execute_append_command(const void *glide_client, const char *key, size_t key_len, const char *value, size_t value_len, long *output_value)
{
    /* Check if client, key, and value are valid */
    if (!glide_client || !key || !value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2; /* key + value */
    uintptr_t args[2];
    unsigned long args_len[2];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: value */
    args[1] = (uintptr_t)value;
    args_len[1] = value_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Append,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute a GETRANGE command using the Valkey Glide client */
int execute_getrange_command(const void *glide_client, const char *key, size_t key_len, long start, long end, char **result, size_t *result_len)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* key + start + end */
    uintptr_t args[3];
    unsigned long args_len[3];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: start */
    size_t start_len;
    char *start_str = long_to_string(start, &start_len);
    if (!start_str)
    {
        return -1;
    }
    args[1] = (uintptr_t)start_str;
    args_len[1] = start_len;

    /* Third argument: end */
    size_t end_len;
    char *end_str = long_to_string(end, &end_len);
    if (!end_str)
    {
        efree(start_str);
        return -1;
    }
    args[2] = (uintptr_t)end_str;
    args_len[2] = end_len;

    /* Execute the command */
    CommandResult *cmd_result = execute_command(
        glide_client,
        GetRange,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument strings */
    efree(start_str);
    efree(end_str);

    /* Use the generic handler to process the result */
    return handle_string_response(cmd_result, result, result_len);
}

/* Helper function to build SORT command arguments */
static void build_sort_args(
    const char *key, size_t key_len,
    zval *sort_pattern,
    zend_bool alpha, zend_bool desc,
    uintptr_t **args_ptr, unsigned long **args_len_ptr,
    unsigned long *arg_count_ptr)
{
    /* Calculate the maximum number of arguments */
    unsigned long max_args = 1; /* key */
    if (sort_pattern && Z_TYPE_P(sort_pattern) == IS_ARRAY)
    {
        /* Patterns array can have: BY, LIMIT, GET, STORE */
        HashTable *ht = Z_ARRVAL_P(sort_pattern);
        max_args += 2 * zend_hash_num_elements(ht) + 2; /* Extra space for possible LIMIT offset count */
    }
    if (alpha)
        max_args++; /* ALPHA */
    if (desc)
        max_args++; /* DESC */

    /* Allocate arrays for arguments */
    uintptr_t *args = (uintptr_t *)emalloc(max_args * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(max_args * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        *args_ptr = NULL;
        *args_len_ptr = NULL;
        *arg_count_ptr = 0;
        return;
    }

    /* Current argument index */
    unsigned long arg_idx = 0;

    /* First argument: key */
    args[arg_idx] = (uintptr_t)key;
    args_len[arg_idx] = key_len;
    arg_idx++;

    /* Add sort patterns if provided */
    if (sort_pattern && Z_TYPE_P(sort_pattern) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(sort_pattern);
        zval *z_ele;
        zend_string *z_key;
        zend_ulong num_key;

        /* Check for BY pattern */
        if ((z_ele = zend_hash_str_find(ht, "by", sizeof("by") - 1)) != NULL && Z_TYPE_P(z_ele) == IS_STRING)
        {
            /* Add BY keyword */
            args[arg_idx] = (uintptr_t)"BY";
            args_len[arg_idx] = 2;
            arg_idx++;

            /* Add BY pattern */
            args[arg_idx] = (uintptr_t)Z_STRVAL_P(z_ele);
            args_len[arg_idx] = Z_STRLEN_P(z_ele);
            arg_idx++;
        }

        /* Check for LIMIT offset count */
        zval *z_offset, *z_count;
        if ((z_offset = zend_hash_str_find(ht, "limit_offset", sizeof("limit_offset") - 1)) != NULL &&
            (z_count = zend_hash_str_find(ht, "limit_count", sizeof("limit_count") - 1)) != NULL)
        {
            /* Add LIMIT keyword */
            args[arg_idx] = (uintptr_t)"LIMIT";
            args_len[arg_idx] = 5;
            arg_idx++;

            /* Add offset */
            char *offset_str;
            size_t offset_len;
            long offset_val = zval_get_long(z_offset);
            offset_str = long_to_string(offset_val, &offset_len);
            if (offset_str)
            {
                args[arg_idx] = (uintptr_t)offset_str;
                args_len[arg_idx] = offset_len;
                arg_idx++;

                /* Add count */
                char *count_str;
                size_t count_len;
                long count_val = zval_get_long(z_count);
                count_str = long_to_string(count_val, &count_len);
                if (count_str)
                {
                    args[arg_idx] = (uintptr_t)count_str;
                    args_len[arg_idx] = count_len;
                    arg_idx++;
                }
                else
                {
                    efree(offset_str);
                }
            }
        }

        /* Add GET patterns */
        ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, z_key, z_ele)
        {
            if (z_key && strncasecmp(ZSTR_VAL(z_key), "get", 3) == 0 && Z_TYPE_P(z_ele) == IS_STRING)
            {
                /* Add GET keyword */
                args[arg_idx] = (uintptr_t)"GET";
                args_len[arg_idx] = 3;
                arg_idx++;

                /* Add GET pattern */
                args[arg_idx] = (uintptr_t)Z_STRVAL_P(z_ele);
                args_len[arg_idx] = Z_STRLEN_P(z_ele);
                arg_idx++;
            }
        }
        ZEND_HASH_FOREACH_END();

        /* Check for STORE destination */
        if ((z_ele = zend_hash_str_find(ht, "store", sizeof("store") - 1)) != NULL && Z_TYPE_P(z_ele) == IS_STRING)
        {
            /* Add STORE keyword */
            args[arg_idx] = (uintptr_t)"STORE";
            args_len[arg_idx] = 5;
            arg_idx++;

            /* Add STORE destination key */
            args[arg_idx] = (uintptr_t)Z_STRVAL_P(z_ele);
            args_len[arg_idx] = Z_STRLEN_P(z_ele);
            arg_idx++;
        }
    }

    /* Add sorting options */
    if (alpha)
    {
        args[arg_idx] = (uintptr_t)"ALPHA";
        args_len[arg_idx] = 5;
        arg_idx++;
    }

    if (desc)
    {
        args[arg_idx] = (uintptr_t)"DESC";
        args_len[arg_idx] = 4;
        arg_idx++;
    }

    /* Set output parameters */
    *args_ptr = args;
    *args_len_ptr = args_len;
    *arg_count_ptr = arg_idx;
}

/* Free memory allocated for SORT command arguments */
static void free_sort_args(uintptr_t *args, unsigned long *args_len, unsigned long arg_count)
{
    if (args && args_len)
    {
        /* Free any dynamically allocated argument strings (offset and count) */
        for (unsigned long i = 0; i < arg_count; i++)
        {
            /* Skip key, BY, GET, LIMIT, STORE, ALPHA, DESC and any string that was
               directly extracted from a zval (not allocated) */
            if (strcmp((const char *)args[i], "BY") != 0 &&
                strcmp((const char *)args[i], "GET") != 0 &&
                strcmp((const char *)args[i], "LIMIT") != 0 &&
                strcmp((const char *)args[i], "STORE") != 0 &&
                strcmp((const char *)args[i], "ALPHA") != 0 &&
                strcmp((const char *)args[i], "DESC") != 0 &&
                i > 0 &&                                                       /* Skip key */
                ((i > 1 && strcmp((const char *)args[i - 1], "LIMIT") == 0) || /* Only free offset and count */
                 (i > 2 && strcmp((const char *)args[i - 2], "LIMIT") == 0)))
            {
                efree((void *)args[i]);
            }
        }

        efree(args);
        efree(args_len);
    }
}

/* Execute a SORT command using the Valkey Glide client */
int execute_sort_command(const void *glide_client, const char *key, size_t key_len,
                         zval *sort_pattern, zend_bool alpha, zend_bool desc, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Build command arguments */
    uintptr_t *args = NULL;
    unsigned long *args_len = NULL;
    unsigned long arg_count = 0;
    build_sort_args(key, key_len, sort_pattern, alpha, desc, &args, &args_len, &arg_count);

    if (!args || !args_len || arg_count == 0)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Execute the command */
    CommandResult *cmd_result = execute_command(
        glide_client,
        Sort,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    free_sort_args(args, args_len, arg_count);

    /* Check if we have a valid result */
    if (!cmd_result || !cmd_result->response)
    {
        if (cmd_result)
            free_command_result(cmd_result);
        return 0;
    }

    /* Process the result */
    int ret_val = 0;

    /* Check for STORE option */
    if (sort_pattern && Z_TYPE_P(sort_pattern) == IS_ARRAY)
    {
        zval *z_store = zend_hash_str_find(Z_ARRVAL_P(sort_pattern), "store", sizeof("store") - 1);
        if (z_store && Z_TYPE_P(z_store) == IS_STRING)
        {
            /* With STORE option, we get the number of stored elements */
            long result_value = 0;
            if (handle_int_response(cmd_result, &result_value))
            {
                ZVAL_LONG(return_value, result_value);
                return 1;
            }
            return 0;
        }
    }

    /* Without STORE option, we get an array of sorted values */
    if (cmd_result->response->response_type == Array)
    {
        array_init(return_value);
        ret_val = command_response_to_zval(cmd_result->response, return_value, false);
    }
    else if (cmd_result->response->response_type == Null)
    {
        /* Empty array */
        array_init(return_value);
        ret_val = 1;
    }

    free_command_result(cmd_result);
    return ret_val;
}

/* Execute a SORT_RO command using the Valkey Glide client */
int execute_sort_ro_command(const void *glide_client, const char *key, size_t key_len,
                            zval *sort_pattern, zend_bool alpha, zend_bool desc, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Build command arguments */
    uintptr_t *args = NULL;
    unsigned long *args_len = NULL;
    unsigned long arg_count = 0;
    build_sort_args(key, key_len, sort_pattern, alpha, desc, &args, &args_len, &arg_count);

    if (!args || !args_len || arg_count == 0)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Execute the command */
    CommandResult *cmd_result = execute_command(
        glide_client,
        SortReadOnly, /* command type */
        arg_count,    /* number of arguments */
        args,         /* arguments */
        args_len      /* argument lengths */
    );

    /* Free the argument arrays */
    free_sort_args(args, args_len, arg_count);

    /* Check if we have a valid result */
    if (!cmd_result || !cmd_result->response)
    {
        if (cmd_result)
            free_command_result(cmd_result);
        return 0;
    }

    /* Process the result */
    int ret_val = 0;

    /* SORT_RO doesn't support STORE option, so we always get an array of values */
    if (cmd_result->response->response_type == Array)
    {
        array_init(return_value);
        ret_val = command_response_to_zval(cmd_result->response, return_value, false);
    }
    else if (cmd_result->response->response_type == Null)
    {
        /* Empty array */
        array_init(return_value);
        ret_val = 1;
    }

    free_command_result(cmd_result);
    return ret_val;
}

/* Execute an EXPIREMEMBER command using the Valkey Glide client (custom command) */
int execute_expiremember_command(const void *glide_client, const char *key, size_t key_len,
                                 const char *member, size_t member_len, long seconds, long *output_value)
{
    /* Check if client, key and member are valid */
    if (!glide_client || !key || !member)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* key + member + seconds */
    uintptr_t args[3];
    unsigned long args_len[3];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: member */
    args[1] = (uintptr_t)member;
    args_len[1] = member_len;

    /* Third argument: seconds */
    size_t seconds_len;
    char *seconds_str = long_to_string(seconds, &seconds_len);
    if (!seconds_str)
    {
        return 0;
    }
    args[2] = (uintptr_t)seconds_str;
    args_len[2] = seconds_len;

    /* Execute the custom command */
    CommandResult *result = command(
        glide_client,
        0,                               /* channel */
        CustomCommand,                   /* command type */
        arg_count,                       /* number of arguments */
        args,                            /* arguments */
        args_len,                        /* argument lengths */
        (unsigned char *)"EXPIREMEMBER", /* command name */
        11                               /* command name length */
    );

    /* Free the argument strings */
    efree(seconds_str);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute an EXPIREMEMBERAT command using the Valkey Glide client (custom command) */
int execute_expirememberat_command(const void *glide_client, const char *key, size_t key_len,
                                   const char *member, size_t member_len, long timestamp, long *output_value)
{
    /* Check if client, key and member are valid */
    if (!glide_client || !key || !member)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* key + member + timestamp */
    uintptr_t args[3];
    unsigned long args_len[3];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: member */
    args[1] = (uintptr_t)member;
    args_len[1] = member_len;

    /* Third argument: timestamp */
    size_t timestamp_len;
    char *timestamp_str = long_to_string(timestamp, &timestamp_len);
    if (!timestamp_str)
    {
        return 0;
    }
    args[2] = (uintptr_t)timestamp_str;
    args_len[2] = timestamp_len;

    /* Execute the custom command */
    CommandResult *result = command(
        glide_client,
        0,                                 /* channel */
        CustomCommand,                     /* command type */
        arg_count,                         /* number of arguments */
        args,                              /* arguments */
        args_len,                          /* argument lengths */
        (unsigned char *)"EXPIREMEMBERAT", /* command name */
        14                                 /* command name length */
    );

    /* Free the argument strings */
    efree(timestamp_str);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}
