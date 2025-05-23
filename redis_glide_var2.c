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

/* Execute a BLMOVE command using the Valkey Glide client */
int execute_blmove_command(const void *glide_client, const char *src, size_t src_len,
                           const char *dst, size_t dst_len, const char *wherefrom,
                           size_t wherefrom_len, const char *whereto, size_t whereto_len,
                           long timeout, char **output_value, size_t *output_len)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !src || !dst || !wherefrom || !whereto || !output_value || !output_len)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 5; /* src, dst, wherefrom, whereto, timeout */
    uintptr_t args[5];
    unsigned long args_len[5];

    /* Set up arguments */
    args[0] = (uintptr_t)src;
    args_len[0] = src_len;

    args[1] = (uintptr_t)dst;
    args_len[1] = dst_len;

    args[2] = (uintptr_t)wherefrom;
    args_len[2] = wherefrom_len;

    args[3] = (uintptr_t)whereto;
    args_len[3] = whereto_len;

    /* Convert timeout to string */
    size_t timeout_len;
    char *timeout_str = long_to_string(timeout, &timeout_len);
    if (!timeout_str)
    {
        return -1;
    }
    args[4] = (uintptr_t)timeout_str;
    args_len[4] = timeout_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        BLMove,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the timeout string */
    free(timeout_str);

    /* Use the generic handler to process the result */
    return handle_string_response(result, output_value, output_len);
}

/* Execute an LMOVE command using the Valkey Glide client */
int execute_lmove_command(const void *glide_client, const char *src, size_t src_len,
                          const char *dst, size_t dst_len, const char *wherefrom,
                          size_t wherefrom_len, const char *whereto, size_t whereto_len,
                          char **output_value, size_t *output_len)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !src || !dst || !wherefrom || !whereto || !output_value || !output_len)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 4; /* src, dst, wherefrom, whereto */
    uintptr_t args[4];
    unsigned long args_len[4];

    /* Set up arguments */
    args[0] = (uintptr_t)src;
    args_len[0] = src_len;

    args[1] = (uintptr_t)dst;
    args_len[1] = dst_len;

    args[2] = (uintptr_t)wherefrom;
    args_len[2] = wherefrom_len;

    args[3] = (uintptr_t)whereto;
    args_len[3] = whereto_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        LMove,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_string_response(result, output_value, output_len);
}

/* Execute an LREM command using the Valkey Glide client */
int execute_lrem_command(const void *glide_client, const char *key, size_t key_len,
                         long count, const char *value, size_t value_len,
                         long *output_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || !value || !output_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* key, count, value */
    uintptr_t args[3];
    unsigned long args_len[3];

    /* Set up arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Convert count to string */
    size_t count_len;
    char *count_str = long_to_string(count, &count_len);
    if (!count_str)
    {
        return 0;
    }
    args[1] = (uintptr_t)count_str;
    args_len[1] = count_len;

    args[2] = (uintptr_t)value;
    args_len[2] = value_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        LRem,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the count string */
    free(count_str);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute an LTRIM command using the Valkey Glide client */
int execute_ltrim_command(const void *glide_client, const char *key, size_t key_len,
                          long start, long end)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* key, start, end */
    uintptr_t args[3];
    unsigned long args_len[3];

    /* Set up arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Convert start to string */
    size_t start_len;
    char *start_str = long_to_string(start, &start_len);
    if (!start_str)
    {
        return 0;
    }
    args[1] = (uintptr_t)start_str;
    args_len[1] = start_len;

    /* Convert end to string */
    size_t end_len;
    char *end_str = long_to_string(end, &end_len);
    if (!end_str)
    {
        free(start_str);
        return 0;
    }
    args[2] = (uintptr_t)end_str;
    args_len[2] = end_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        LTrim,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the start and end strings */
    free(start_str);
    free(end_str);

    /* Use the proper handler for OK response */
    int status = handle_ok_response(result);

    /* Convert response status to boolean */
    return (status == 1) ? 1 : 0;
}

/* Execute an LINDEX command using the Valkey Glide client */
int execute_lindex_command(const void *glide_client, const char *key, size_t key_len,
                           long index, char **output_value, size_t *output_len)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || !output_value || !output_len)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2; /* key, index */
    uintptr_t args[2];
    unsigned long args_len[2];

    /* Set up arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Convert index to string */
    size_t index_len;
    char *index_str = long_to_string(index, &index_len);
    if (!index_str)
    {
        return -1;
    }
    args[1] = (uintptr_t)index_str;
    args_len[1] = index_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        LIndex,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the index string */
    free(index_str);

    /* Use the generic handler to process the result */
    return handle_string_response(result, output_value, output_len);
}

/* Execute an LSET command using the Valkey Glide client */
int execute_lset_command(const void *glide_client, const char *key, size_t key_len,
                         long index, const char *value, size_t value_len)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || !value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* key, index, value */
    uintptr_t args[3];
    unsigned long args_len[3];

    /* Set up arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Convert index to string */
    size_t index_len;
    char *index_str = long_to_string(index, &index_len);
    if (!index_str)
    {
        return 0;
    }
    args[1] = (uintptr_t)index_str;
    args_len[1] = index_len;

    args[2] = (uintptr_t)value;
    args_len[2] = value_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        LSet,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the index string */
    free(index_str);

    /* Use the proper handler for OK response */
    int status = handle_ok_response(result);

    /* Convert response status to boolean */
    return (status == 1) ? 1 : 0;
}

/* Execute a FLUSHDB command using the Valkey Glide client */
int execute_flushdb_command(const void *glide_client, int async)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = async ? 1 : 0; /* Optional ASYNC argument */
    uintptr_t args[1];
    unsigned long args_len[1];

    /* Set up arguments */
    if (async)
    {
        args[0] = (uintptr_t)"ASYNC";
        args_len[0] = 5;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        FlushDB,   /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the proper handler for OK response */
    int status = handle_ok_response(result);

    /* Convert response status to boolean */
    return (status == 1) ? 1 : 0;
}

/* Execute a FLUSHALL command using the Valkey Glide client */
int execute_flushall_command(const void *glide_client, int async)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = async ? 1 : 0; /* Optional ASYNC argument */
    uintptr_t args[1];
    unsigned long args_len[1];

    /* Set up arguments */
    if (async)
    {
        args[0] = (uintptr_t)"ASYNC";
        args_len[0] = 5;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        FlushAll,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the proper handler for OK response */
    int status = handle_ok_response(result);

    /* Convert response status to boolean */
    return (status == 1) ? 1 : 0;
}

/* Execute a TIME command using the Valkey Glide client */
int execute_time_command(const void *glide_client, zval *return_value)
{
    /* Check if client is valid */
    if (!glide_client || !return_value)
    {
        return 0;
    }

    /* No arguments for TIME command */
    unsigned long arg_count = 0;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Time,      /* command type */
        arg_count, /* number of arguments */
        NULL,      /* no arguments */
        NULL       /* no argument lengths */
    );

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing TIME command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    if (result->response && result->response->response_type == Array)
    {
        /* Initialize the return array */
        array_init(return_value);

        /* Redis TIME command returns array with [unix_timestamp, microseconds] */
        if (result->response->array_value_len >= 2)
        {
            /* Process the timestamp */
            if (result->response->array_value[0].response_type == String)
            {
                add_next_index_string(return_value, result->response->array_value[0].string_value);
            }
            /* Process the microseconds */
            if (result->response->array_value[1].response_type == String)
            {
                add_next_index_string(return_value, result->response->array_value[1].string_value);
            }
        }

        free_command_result(result);
        return 1;
    }

    /* Free the result */
    free_command_result(result);
    return 0;
}

/* Execute a ROLE command using the Valkey Glide client */
int execute_role_command(const void *glide_client, zval *return_value)
{
    /* Check if client is valid */
    if (!glide_client || !return_value)
    {
        return 0;
    }

    /* No arguments for ROLE command */
    unsigned long arg_count = 0;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Role,      /* command type */
        arg_count, /* number of arguments */
        NULL,      /* no arguments */
        NULL       /* no argument lengths */
    );

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing ROLE command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    int ret_val = 0;
    if (result->response)
    {
        /* Convert response to PHP array */
        ret_val = command_response_to_zval(result->response, return_value, 0);
    }

    /* Free the result */
    free_command_result(result);
    return ret_val;
}
