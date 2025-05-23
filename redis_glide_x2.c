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

/* Execute an XPENDING command using the Valkey Glide client */
int execute_xpending_command(const void *glide_client, const char *key, size_t key_len,
                             const char *group, size_t group_len, zval *options,
                             zval *return_value)
{
    /* Check if client and arguments are valid */
    if (!glide_client || !key || key_len <= 0 || !group || group_len <= 0)
    {
        return 0;
    }

    /* Check for options: start, end, count, consumer */
    zend_bool has_start = 0, has_end = 0, has_count = 0, has_consumer = 0;
    const char *start = NULL, *end = NULL, *consumer = NULL;
    size_t start_len = 0, end_len = 0, consumer_len = 0;
    long count_value = 0;
    char count_str[32];
    unsigned long count_str_len = 0;
    unsigned long extra_args = 0;

    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(options);
        zval *z_start, *z_end, *z_count, *z_consumer;

        /* Check for start ID */
        if ((z_start = zend_hash_str_find(ht, "START", sizeof("START") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_start) == IS_STRING)
            {
                has_start = 1;
                start = Z_STRVAL_P(z_start);
                start_len = Z_STRLEN_P(z_start);
                extra_args += 1;
            }
        }

        /* Check for end ID */
        if ((z_end = zend_hash_str_find(ht, "END", sizeof("END") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_end) == IS_STRING)
            {
                has_end = 1;
                end = Z_STRVAL_P(z_end);
                end_len = Z_STRLEN_P(z_end);
                extra_args += 1;
            }
        }

        /* Check for COUNT option */
        if ((z_count = zend_hash_str_find(ht, "COUNT", sizeof("COUNT") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_count) == IS_LONG)
            {
                count_value = Z_LVAL_P(z_count);
                has_count = 1;
                count_str_len = snprintf(count_str, sizeof(count_str), "%ld", count_value);
                extra_args += 1;
            }
        }

        /* Check for consumer */
        if ((z_consumer = zend_hash_str_find(ht, "CONSUMER", sizeof("CONSUMER") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_consumer) == IS_STRING)
            {
                has_consumer = 1;
                consumer = Z_STRVAL_P(z_consumer);
                consumer_len = Z_STRLEN_P(z_consumer);
                extra_args += 1;
            }
        }
    }

    /* Calculate total args: key + group + (start + end + count + consumer) */
    unsigned long arg_count = 2 + extra_args;
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

    /* Set arguments */
    unsigned int arg_idx = 0;
    args[arg_idx] = (uintptr_t)key;
    args_len[arg_idx] = key_len;
    arg_idx++;

    args[arg_idx] = (uintptr_t)group;
    args_len[arg_idx] = group_len;
    arg_idx++;

    /* Add additional options */

    /* Add start if provided */
    if (has_start)
    {
        args[arg_idx] = (uintptr_t)start;
        args_len[arg_idx] = start_len;
        arg_idx++;
    }

    /* Add end if provided */
    if (has_end)
    {
        args[arg_idx] = (uintptr_t)end;
        args_len[arg_idx] = end_len;
        arg_idx++;
    }

    /* Add count if provided */
    if (has_count)
    {
        args[arg_idx] = (uintptr_t)count_str;
        args_len[arg_idx] = count_str_len;
        arg_idx++;
    }

    /* Add consumer if provided */
    if (has_consumer)
    {
        args[arg_idx] = (uintptr_t)consumer;
        args_len[arg_idx] = consumer_len;
        arg_idx++;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        XPending,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free resources */
    efree(args);
    efree(args_len);

    /* Handle the response directly */
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
            /* XPENDING returns pending entries info */
            status = command_response_to_zval(result->response, return_value, 0);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute an XREAD command using the Valkey Glide client */
int execute_xread_command(const void *glide_client, zval *streams, zval *ids,
                          zval *options, zval *return_value)
{
    /* Check if client and arguments are valid */
    if (!glide_client || !streams || !ids)
    {
        return 0;
    }

    /* Get the number of streams */
    HashTable *streams_ht = Z_ARRVAL_P(streams);
    HashTable *ids_ht = Z_ARRVAL_P(ids);
    int streams_count = zend_hash_num_elements(streams_ht);
    int ids_count = zend_hash_num_elements(ids_ht);

    /* Check counts match */
    if (streams_count <= 0 || streams_count != ids_count)
    {
        return 0;
    }

    /* Check for options */
    zend_bool has_block = 0, has_count = 0;
    long block_ms = 0, count_value = 0;
    char block_str[32], count_str[32];
    unsigned long block_str_len = 0, count_str_len = 0;
    unsigned long extra_args = 0;

    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(options);
        zval *z_block, *z_count;

        /* Check for BLOCK option */
        if ((z_block = zend_hash_str_find(ht, "BLOCK", sizeof("BLOCK") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_block) == IS_LONG)
            {
                block_ms = Z_LVAL_P(z_block);
                has_block = 1;
                block_str_len = snprintf(block_str, sizeof(block_str), "%ld", block_ms);
                extra_args += 2; /* BLOCK + value */
            }
        }

        /* Check for COUNT option */
        if ((z_count = zend_hash_str_find(ht, "COUNT", sizeof("COUNT") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_count) == IS_LONG)
            {
                count_value = Z_LVAL_P(z_count);
                has_count = 1;
                count_str_len = snprintf(count_str, sizeof(count_str), "%ld", count_value);
                extra_args += 2; /* COUNT + value */
            }
        }
    }

    /* Calculate total args: options + STREAMS + streams + ids */
    unsigned long arg_count = extra_args + 1 + streams_count + ids_count;
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

    /* Add options */
    unsigned int arg_idx = 0;

    /* Add COUNT if specified */
    if (has_count)
    {
        args[arg_idx] = (uintptr_t)"COUNT";
        args_len[arg_idx] = sizeof("COUNT") - 1;
        arg_idx++;

        args[arg_idx] = (uintptr_t)count_str;
        args_len[arg_idx] = count_str_len;
        arg_idx++;
    }

    /* Add BLOCK if specified */
    if (has_block)
    {
        args[arg_idx] = (uintptr_t)"BLOCK";
        args_len[arg_idx] = sizeof("BLOCK") - 1;
        arg_idx++;

        args[arg_idx] = (uintptr_t)block_str;
        args_len[arg_idx] = block_str_len;
        arg_idx++;
    }

    /* Add STREAMS keyword */
    args[arg_idx] = (uintptr_t)"STREAMS";
    args_len[arg_idx] = sizeof("STREAMS") - 1;
    arg_idx++;

    /* Add all stream keys */
    zval *z_stream;
    ZEND_HASH_FOREACH_VAL(streams_ht, z_stream)
    {
        if (Z_TYPE_P(z_stream) != IS_STRING)
        {
            convert_to_string(z_stream);
        }
        args[arg_idx] = (uintptr_t)Z_STRVAL_P(z_stream);
        args_len[arg_idx] = Z_STRLEN_P(z_stream);
        arg_idx++;
    }
    ZEND_HASH_FOREACH_END();

    /* Add all stream IDs */
    zval *z_id;
    ZEND_HASH_FOREACH_VAL(ids_ht, z_id)
    {
        if (Z_TYPE_P(z_id) != IS_STRING)
        {
            convert_to_string(z_id);
        }
        args[arg_idx] = (uintptr_t)Z_STRVAL_P(z_id);
        args_len[arg_idx] = Z_STRLEN_P(z_id);
        arg_idx++;
    }
    ZEND_HASH_FOREACH_END();

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        XRead,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free resources */
    efree(args);
    efree(args_len);

    /* Handle the response directly */
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
            /* XREAD returns streams data or NULL if timeout */
            status = command_response_to_zval(result->response, return_value, 0);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute an XREADGROUP command using the Valkey Glide client */
int execute_xreadgroup_command(const void *glide_client, const char *group, size_t group_len,
                               const char *consumer, size_t consumer_len, zval *streams, zval *ids,
                               zval *options, zval *return_value)
{
    /* Check if client and arguments are valid */
    if (!glide_client || !group || group_len <= 0 || !consumer || consumer_len <= 0 || !streams || !ids)
    {
        return 0;
    }

    /* Get the number of streams */
    HashTable *streams_ht = Z_ARRVAL_P(streams);
    HashTable *ids_ht = Z_ARRVAL_P(ids);
    int streams_count = zend_hash_num_elements(streams_ht);
    int ids_count = zend_hash_num_elements(ids_ht);

    /* Check counts match */
    if (streams_count <= 0 || streams_count != ids_count)
    {
        return 0;
    }

    /* Check for options */
    zend_bool has_block = 0, has_count = 0, has_noack = 0;
    long block_ms = 0, count_value = 0;
    char block_str[32], count_str[32];
    unsigned long block_str_len = 0, count_str_len = 0;
    unsigned long extra_args = 0;

    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(options);
        zval *z_block, *z_count, *z_noack;

        /* Check for BLOCK option */
        if ((z_block = zend_hash_str_find(ht, "BLOCK", sizeof("BLOCK") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_block) == IS_LONG)
            {
                block_ms = Z_LVAL_P(z_block);
                has_block = 1;
                block_str_len = snprintf(block_str, sizeof(block_str), "%ld", block_ms);
                extra_args += 2; /* BLOCK + value */
            }
        }

        /* Check for COUNT option */
        if ((z_count = zend_hash_str_find(ht, "COUNT", sizeof("COUNT") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_count) == IS_LONG)
            {
                count_value = Z_LVAL_P(z_count);
                has_count = 1;
                count_str_len = snprintf(count_str, sizeof(count_str), "%ld", count_value);
                extra_args += 2; /* COUNT + value */
            }
        }

        /* Check for NOACK option */
        if ((z_noack = zend_hash_str_find(ht, "NOACK", sizeof("NOACK") - 1)) != NULL)
        {
            has_noack = zval_is_true(z_noack);
            if (has_noack)
                extra_args += 1; /* NOACK */
        }
    }

    /* Calculate total args: GROUP + group + consumer + options + STREAMS + streams + ids */
    unsigned long arg_count = 3 + extra_args + 1 + streams_count + ids_count;
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

    /* Add GROUP, group, consumer */
    unsigned int arg_idx = 0;
    args[arg_idx] = (uintptr_t)"GROUP";
    args_len[arg_idx] = sizeof("GROUP") - 1;
    arg_idx++;

    args[arg_idx] = (uintptr_t)group;
    args_len[arg_idx] = group_len;
    arg_idx++;

    args[arg_idx] = (uintptr_t)consumer;
    args_len[arg_idx] = consumer_len;
    arg_idx++;

    /* Add COUNT if specified */
    if (has_count)
    {
        args[arg_idx] = (uintptr_t)"COUNT";
        args_len[arg_idx] = sizeof("COUNT") - 1;
        arg_idx++;

        args[arg_idx] = (uintptr_t)count_str;
        args_len[arg_idx] = count_str_len;
        arg_idx++;
    }

    /* Add BLOCK if specified */
    if (has_block)
    {
        args[arg_idx] = (uintptr_t)"BLOCK";
        args_len[arg_idx] = sizeof("BLOCK") - 1;
        arg_idx++;

        args[arg_idx] = (uintptr_t)block_str;
        args_len[arg_idx] = block_str_len;
        arg_idx++;
    }

    /* Add NOACK if specified */
    if (has_noack)
    {
        args[arg_idx] = (uintptr_t)"NOACK";
        args_len[arg_idx] = sizeof("NOACK") - 1;
        arg_idx++;
    }

    /* Add STREAMS keyword */
    args[arg_idx] = (uintptr_t)"STREAMS";
    args_len[arg_idx] = sizeof("STREAMS") - 1;
    arg_idx++;

    /* Add all stream keys */
    zval *z_stream;
    ZEND_HASH_FOREACH_VAL(streams_ht, z_stream)
    {
        if (Z_TYPE_P(z_stream) != IS_STRING)
        {
            convert_to_string(z_stream);
        }
        args[arg_idx] = (uintptr_t)Z_STRVAL_P(z_stream);
        args_len[arg_idx] = Z_STRLEN_P(z_stream);
        arg_idx++;
    }
    ZEND_HASH_FOREACH_END();

    /* Add all stream IDs */
    zval *z_id;
    ZEND_HASH_FOREACH_VAL(ids_ht, z_id)
    {
        if (Z_TYPE_P(z_id) != IS_STRING)
        {
            convert_to_string(z_id);
        }
        args[arg_idx] = (uintptr_t)Z_STRVAL_P(z_id);
        args_len[arg_idx] = Z_STRLEN_P(z_id);
        arg_idx++;
    }
    ZEND_HASH_FOREACH_END();

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        XReadGroup, /* command type */
        arg_count,  /* number of arguments */
        args,       /* arguments */
        args_len    /* argument lengths */
    );

    /* Free resources */
    efree(args);
    efree(args_len);

    /* Handle the response directly */
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
            /* XREADGROUP returns streams data or NULL if timeout */
            status = command_response_to_zval(result->response, return_value, 0);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}
