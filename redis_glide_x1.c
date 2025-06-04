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

/* Execute an XLEN command using the Valkey Glide client */
int execute_xlen_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || key_len <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1; /* Just the key */
    uintptr_t args[1];
    unsigned long args_len[1];

    /* Set up arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        XLen,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the proper handler for integer response */
    return handle_int_response(result, output_value);
}

/* Execute an XDEL command using the Valkey Glide client */
int execute_xdel_command(const void *glide_client, const char *key, size_t key_len,
                         zval *ids, int id_count, long *output_value)
{
    /* Check if client and arguments are valid */
    if (!glide_client || !key || key_len <= 0 || !ids || id_count <= 0)
    {
        return 0;
    }

    /* Prepare command arguments: key + IDs */
    unsigned long arg_count = 1 + id_count;
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

    /* Set key as first argument */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add all stream IDs */
    zval *z_id;
    int i = 1;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(ids), z_id)
    {
        if (Z_TYPE_P(z_id) != IS_STRING)
        {
            convert_to_string(z_id);
        }
        args[i] = (uintptr_t)Z_STRVAL_P(z_id);
        args_len[i] = Z_STRLEN_P(z_id);
        i++;
    }
    ZEND_HASH_FOREACH_END();

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        XDel,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free resources */
    efree(args);
    efree(args_len);

    /* Use the proper handler for integer response */
    return handle_int_response(result, output_value);
}

/* Execute an XACK command using the Valkey Glide client */
int execute_xack_command(const void *glide_client, const char *key, size_t key_len,
                         const char *group, size_t group_len, zval *ids, int id_count,
                         long *output_value)
{

    /* Set a default value for output in case of early return */
    *output_value = 0;

    /* Check if client and arguments are valid */
    if (!glide_client || !key || key_len <= 0 || !group || group_len <= 0)
    {
        printf("DEBUG: XACK validation failed for client/key/group\n");
        return 0;
    }

    /* Special case: empty IDs array should return false as per phpredis behavior */
    if (!ids || id_count <= 0)
    {
        printf("DEBUG: XACK empty IDs array - returning false as expected\n");
        return 0;
    }

    /* Prepare command arguments: key + group + ids */
    unsigned long arg_count = 2 + id_count;
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        printf("DEBUG: XACK memory allocation failed\n");
        return 0;
    }

    /* Set key and group as first two arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;
    args[1] = (uintptr_t)group;
    args_len[1] = group_len;

    /* Add all stream IDs */
    zval *z_id;
    int i = 2;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(ids), z_id)
    {
        if (Z_TYPE_P(z_id) != IS_STRING)
        {
            convert_to_string(z_id);
        }
        args[i] = (uintptr_t)Z_STRVAL_P(z_id);
        args_len[i] = Z_STRLEN_P(z_id);
        i++;
    }
    ZEND_HASH_FOREACH_END();

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        XAck,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free resources */
    efree(args);
    efree(args_len);

    /* Check the result directly for debugging */
    if (result)
    {

        if (result->command_error)
        {
            printf("DEBUG: XACK command error: %s\n", result->command_error->command_error_message);
            free_command_result(result);
            return 0;
        }

        if (result->response)
        {

            /* For Redis XACK command, integer response is the number of messages successfully acknowledged */
            if (result->response->response_type == Int)
            {
                /* Store the count in output_value */
                *output_value = result->response->int_value;

                /* Free the result - we don't need it anymore */
                free_command_result(result);

                /* Always return success (1) even if count is 0 - let PHP code decide how to handle it */
                return 1;
            }
        }

        free_command_result(result);
    }

    return 0;
}

/* Execute an XADD command using the Valkey Glide client */
int execute_xadd_command(const void *glide_client, const char *key, size_t key_len,
                         const char *id, size_t id_len, zval *field_values, int fv_count,
                         zval *options, zval *return_value)
{
    /* Check if client and arguments are valid */
    if (!glide_client || !key || key_len <= 0 || !id || id_len <= 0 || !field_values || fv_count <= 0)
    {
        return 0;
    }

    /* Count options (MAXLEN, etc.) */
    unsigned long extra_args = 0;
    long maxlen = 0;
    zend_bool approximate = 0;
    zend_bool nomkstream = 0;
    zend_bool minid_strategy = 0;
    char maxlen_str[64];
    unsigned long maxlen_str_len = 0;

    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(options);
        zval *z_maxlen, *z_approx, *z_nomkstream, *z_minid_strategy;

        /* Check for MAXLEN option */
        if ((z_maxlen = zend_hash_str_find(ht, "MAXLEN", sizeof("MAXLEN") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_maxlen) == IS_LONG)
            {
                maxlen = Z_LVAL_P(z_maxlen);
                maxlen_str_len = snprintf(maxlen_str, sizeof(maxlen_str), "%ld", maxlen);
                extra_args += 2; /* MAXLEN + value */
            }
        }

        /* Check for approximate option */
        if ((z_approx = zend_hash_str_find(ht, "APPROXIMATE", sizeof("APPROXIMATE") - 1)) != NULL)
        {
            approximate = zval_is_true(z_approx);
            if (approximate)
                extra_args += 1; /* ~ (tilde) */
        }

        /* Check for NOMKSTREAM option */
        if ((z_nomkstream = zend_hash_str_find(ht, "NOMKSTREAM", sizeof("NOMKSTREAM") - 1)) != NULL)
        {
            nomkstream = zval_is_true(z_nomkstream);
            if (nomkstream)
                extra_args += 1; /* NOMKSTREAM */
        }

        /* Check for MINID strategy option */
        if ((z_minid_strategy = zend_hash_str_find(ht, "MINID", sizeof("MINID") - 1)) != NULL)
        {
            minid_strategy = zval_is_true(z_minid_strategy);
            if (minid_strategy && maxlen > 0)
            {
                /* Replace MAXLEN with MINID */
                extra_args -= 1; /* Already counted MAXLEN, just reuse that slot */
            }
        }
    }

    /* Calculate total args: key + options + ID + field/value pairs (each entry is a pair) */
    unsigned long arg_count = 1 + extra_args + 1 + (fv_count * 2); /* Each field-value is 2 args */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    /* Allocate array to track temporary string allocations */
    char **allocated_strings = (char **)ecalloc(fv_count, sizeof(char *));
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

    /* Set key as first argument */
    unsigned int arg_idx = 0;
    args[arg_idx] = (uintptr_t)key;
    args_len[arg_idx] = key_len;
    arg_idx++;

    /* Add NOMKSTREAM if specified */
    if (nomkstream)
    {
        args[arg_idx] = (uintptr_t)"NOMKSTREAM";
        args_len[arg_idx] = sizeof("NOMKSTREAM") - 1;
        arg_idx++;
    }

    /* Add MAXLEN/MINID if specified */
    if (maxlen > 0)
    {
        if (minid_strategy)
        {
            args[arg_idx] = (uintptr_t)"MINID";
            args_len[arg_idx] = sizeof("MINID") - 1;
        }
        else
        {
            args[arg_idx] = (uintptr_t)"MAXLEN";
            args_len[arg_idx] = sizeof("MAXLEN") - 1;
        }
        arg_idx++;

        /* Add ~ for approximate trimming */
        if (approximate)
        {
            args[arg_idx] = (uintptr_t)"~";
            args_len[arg_idx] = 1;
            arg_idx++;
        }

        /* Add the threshold value */
        args[arg_idx] = (uintptr_t)maxlen_str;
        args_len[arg_idx] = maxlen_str_len;
        arg_idx++;
    }

    /* Add stream ID */
    args[arg_idx] = (uintptr_t)id;
    args_len[arg_idx] = id_len;
    arg_idx++;

    /* Add field-value pairs */
    HashTable *ht = Z_ARRVAL_P(field_values);
    zend_string *field_str;
    zval *z_value;

    ZEND_HASH_FOREACH_STR_KEY_VAL(ht, field_str, z_value)
    {
        /* Add field name */
        if (field_str)
        {
            args[arg_idx] = (uintptr_t)ZSTR_VAL(field_str);
            args_len[arg_idx] = ZSTR_LEN(field_str);
            arg_idx++;

            /* Add field value, convert to string if needed */
            if (Z_TYPE_P(z_value) != IS_STRING)
            {
                zval temp;
                ZVAL_COPY(&temp, z_value);
                convert_to_string(&temp);

                /* Create persistent copy of the string */
                char *str_copy = estrndup(Z_STRVAL(temp), Z_STRLEN(temp));
                if (str_copy)
                {
                    allocated_strings[allocated_count++] = str_copy;
                    args[arg_idx] = (uintptr_t)str_copy;
                    args_len[arg_idx] = Z_STRLEN(temp);
                    arg_idx++;
                }

                zval_dtor(&temp);
            }
            else
            {
                args[arg_idx] = (uintptr_t)Z_STRVAL_P(z_value);
                args_len[arg_idx] = Z_STRLEN_P(z_value);
                arg_idx++;
            }
        }
    }
    ZEND_HASH_FOREACH_END();

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        XAdd,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free resources */
    for (int i = 0; i < allocated_count; i++)
    {
        if (allocated_strings[i])
        {
            efree(allocated_strings[i]);
        }
    }
    efree(allocated_strings);
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
            /* XADD returns the ID string, convert to proper output */
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute an XTRIM command using the Valkey Glide client */
int execute_xtrim_command(const void *glide_client, const char *key, size_t key_len,
                          const char *strategy, size_t strategy_len, long threshold,
                          zval *options, long *output_value)
{
    /* Check if client and arguments are valid */
    if (!glide_client || !key || key_len <= 0 || !strategy || strategy_len <= 0)
    {
        return 0;
    }

    /* Check for options */
    unsigned long extra_args = 0;
    zend_bool approximate = 0;
    zend_bool limit = 0;
    long limit_value = 0;
    char limit_str[32];
    unsigned long limit_str_len = 0;

    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(options);
        zval *z_approx, *z_limit;

        /* Check for approximate option */
        if ((z_approx = zend_hash_str_find(ht, "APPROXIMATE", sizeof("APPROXIMATE") - 1)) != NULL)
        {
            approximate = zval_is_true(z_approx);
            if (approximate)
                extra_args += 1; /* ~ (tilde) */
        }

        /* Check for LIMIT option */
        if ((z_limit = zend_hash_str_find(ht, "LIMIT", sizeof("LIMIT") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_limit) == IS_LONG)
            {
                limit_value = Z_LVAL_P(z_limit);
                limit = 1;
                limit_str_len = snprintf(limit_str, sizeof(limit_str), "%ld", limit_value);
                extra_args += 2; /* LIMIT + value */
            }
        }
    }

    /* Calculate total args: key + strategy + (~ if approximate) + threshold + (LIMIT + limit_value) */
    unsigned long arg_count = 1 + 1 + (approximate ? 1 : 0) + 1 + (limit ? 2 : 0);
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

    /* Set key as first argument */
    unsigned int arg_idx = 0;
    args[arg_idx] = (uintptr_t)key;
    args_len[arg_idx] = key_len;
    arg_idx++;

    /* Add strategy */
    args[arg_idx] = (uintptr_t)strategy;
    args_len[arg_idx] = strategy_len;
    arg_idx++;

    /* Add ~ for approximate trimming */
    if (approximate)
    {
        args[arg_idx] = (uintptr_t)"~";
        args_len[arg_idx] = 1;
        arg_idx++;
    }

    /* Add threshold value as string */
    char threshold_str[32];
    unsigned long threshold_str_len = snprintf(threshold_str, sizeof(threshold_str), "%ld", threshold);
    args[arg_idx] = (uintptr_t)threshold_str;
    args_len[arg_idx] = threshold_str_len;
    arg_idx++;

    /* Add LIMIT if specified */
    if (limit)
    {
        args[arg_idx] = (uintptr_t)"LIMIT";
        args_len[arg_idx] = sizeof("LIMIT") - 1;
        arg_idx++;

        args[arg_idx] = (uintptr_t)limit_str;
        args_len[arg_idx] = limit_str_len;
        arg_idx++;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        XTrim,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free resources */
    efree(args);
    efree(args_len);

    /* Use the proper handler for integer response */
    return handle_int_response(result, output_value);
}

/* Execute an XRANGE command using the Valkey Glide client */
int execute_xrange_command(const void *glide_client, const char *key, size_t key_len,
                           const char *start, size_t start_len, const char *end, size_t end_len,
                           zval *options, zval *return_value)
{
    /* Check if client and arguments are valid */
    if (!glide_client || !key || key_len <= 0 || !start || start_len <= 0 || !end || end_len <= 0)
    {
        return 0;
    }

    /* Check for COUNT option */
    unsigned long extra_args = 0;
    zend_bool has_count = 0;
    long count_value = 0;
    char count_str[32];
    unsigned long count_str_len = 0;

    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(options);
        zval *z_count;

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

    /* Calculate total args: key + start + end + (COUNT + count_value) */
    unsigned long arg_count = 1 + 1 + 1 + (has_count ? 2 : 0);
    uintptr_t args[5]; /* Max 5 arguments: key + start + end + COUNT + count_value */
    unsigned long args_len[5];

    /* Set arguments */
    unsigned int arg_idx = 0;
    args[arg_idx] = (uintptr_t)key;
    args_len[arg_idx] = key_len;
    arg_idx++;

    args[arg_idx] = (uintptr_t)start;
    args_len[arg_idx] = start_len;
    arg_idx++;

    args[arg_idx] = (uintptr_t)end;
    args_len[arg_idx] = end_len;
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

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        XRange,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

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
            /* XRANGE returns array of entries, use stream-specific handler */
            status = command_response_to_stream_zval(result->response, return_value);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute an XREVRANGE command using the Valkey Glide client */
int execute_xrevrange_command(const void *glide_client, const char *key, size_t key_len,
                              const char *end, size_t end_len, const char *start, size_t start_len,
                              zval *options, zval *return_value)
{
    /* Check if client and arguments are valid */
    if (!glide_client || !key || key_len <= 0 || !end || end_len <= 0 || !start || start_len <= 0)
    {
        return 0;
    }

    /* Check for COUNT option */
    unsigned long extra_args = 0;
    zend_bool has_count = 0;
    long count_value = 0;
    char count_str[32];
    unsigned long count_str_len = 0;

    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(options);
        zval *z_count;

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

    /* Calculate total args: key + end + start + (COUNT + count_value) */
    unsigned long arg_count = 1 + 1 + 1 + (has_count ? 2 : 0);
    uintptr_t args[5]; /* Max 5 arguments: key + end + start + COUNT + count_value */
    unsigned long args_len[5];

    /* Set arguments */
    unsigned int arg_idx = 0;
    args[arg_idx] = (uintptr_t)key;
    args_len[arg_idx] = key_len;
    arg_idx++;

    args[arg_idx] = (uintptr_t)end;
    args_len[arg_idx] = end_len;
    arg_idx++;

    args[arg_idx] = (uintptr_t)start;
    args_len[arg_idx] = start_len;
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

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        XRevRange, /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

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
            /* XREVRANGE returns array of entries, use stream-specific handler */
            status = command_response_to_stream_zval(result->response, return_value);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}
