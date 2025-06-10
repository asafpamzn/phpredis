/*
  +----------------------------------------------------------------------+
  | Valkey Glide X-Commands Implementation                               |
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
#include "valkey_glide_x_common.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ====================================================================
 * COMMAND IMPLEMENTATION FUNCTIONS
 * ==================================================================== */

/**
 * Execute an XLEN command
 */
int execute_xlen_command(const void *glide_client, const char *key, size_t key_len,
                         long *output_value)
{
    /* Initialize the arguments structure */
    x_command_args_t args = {0};
    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;

    /* Use the generic command execution framework */
    return execute_x_generic_command(glide_client, XLen, &args, output_value, process_x_int_result);
}

/**
 * Execute an XDEL command
 */
int execute_xdel_command(const void *glide_client, const char *key, size_t key_len,
                         zval *ids, int id_count, long *output_value)
{
    /* Initialize the arguments structure */
    x_command_args_t args = {0};
    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.ids = ids;
    args.id_count = id_count;

    /* Use the generic command execution framework */
    return execute_x_generic_command(glide_client, XDel, &args, output_value, process_x_int_result);
}

/**
 * Execute an XACK command
 */
int execute_xack_command(const void *glide_client, const char *key, size_t key_len,
                         const char *group, size_t group_len, zval *ids, int id_count,
                         long *output_value)
{
    /* Set a default value for output in case of early return */
    *output_value = 0;

    /* Initialize the arguments structure */
    x_command_args_t args = {0};
    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.group = group;
    args.group_len = group_len;
    args.ids = ids;
    args.id_count = id_count;

    /* Use the generic command execution framework */
    return execute_x_generic_command(glide_client, XAck, &args, output_value, process_x_int_result);
}

/**
 * Execute an XADD command
 */
int execute_xadd_command(const void *glide_client, const char *key, size_t key_len,
                         const char *id, size_t id_len, zval *field_values, int fv_count,
                         zval *options, zval *return_value)
{
    /* Initialize the arguments structure */
    x_command_args_t args = {0};
    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.id = id;
    args.id_len = id_len;
    args.field_values = field_values;
    args.fv_count = fv_count;
    args.options = options;

    /* Parse options */
    parse_x_add_options(options, &args.add_opts);

    /* Use the generic command execution framework */
    return execute_x_generic_command(glide_client, XAdd, &args, return_value, process_x_add_result);
}

/**
 * Execute an XTRIM command
 */
int execute_xtrim_command(const void *glide_client, const char *key, size_t key_len,
                          const char *strategy, size_t strategy_len, const char *threshold,
                          size_t threshold_len, zval *options, long *output_value)
{
    /* Initialize the arguments structure */
    x_command_args_t args = {0};
    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.strategy = strategy;
    args.strategy_len = strategy_len;
    args.threshold = threshold;
    args.threshold_len = threshold_len;
    args.options = options;

    /* Parse options */
    parse_x_trim_options(options, &args.trim_opts);

    /* Use the generic command execution framework */
    return execute_x_generic_command(glide_client, XTrim, &args, output_value, process_x_int_result);
}

/**
 * Execute an XRANGE command
 */
int execute_xrange_command(const void *glide_client, const char *key, size_t key_len,
                           const char *start, size_t start_len, const char *end, size_t end_len,
                           zval *options, zval *return_value)
{
    /* Initialize the arguments structure */
    x_command_args_t args = {0};
    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.start = start;
    args.start_len = start_len;
    args.end = end;
    args.end_len = end_len;
    args.options = options;

    /* Parse options */
    parse_x_count_options(options, &args.range_opts);

    /* Use the generic command execution framework */
    return execute_x_generic_command(glide_client, XRange, &args, return_value, process_x_stream_result);
}

/**
 * Execute an XREVRANGE command
 */
int execute_xrevrange_command(const void *glide_client, const char *key, size_t key_len,
                              const char *end, size_t end_len, const char *start, size_t start_len,
                              zval *options, zval *return_value)
{
    /* Initialize the arguments structure */
    x_command_args_t args = {0};
    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    /* Note: For XREVRANGE, the function parameters are already swapped (end comes before start)
     * But in the command arguments, we need to preserve the order expected by prepare_x_range_args
     * So we assign them in the correct mapping for command construction */
    args.start = end; /* end is actually the 'start' argument for XREVRANGE */
    args.start_len = end_len;
    args.end = start; /* start is actually the 'end' argument for XREVRANGE */
    args.end_len = start_len;
    args.options = options;

    /* Parse options */
    parse_x_count_options(options, &args.range_opts);

    /* Use the generic command execution framework */
    return execute_x_generic_command(glide_client, XRevRange, &args, return_value, process_x_stream_result);
}

/**
 * Execute an XINFO command
 */
int execute_xinfo_command(const void *glide_client, const char *subcommand, size_t subcommand_len,
                          zval *args, int args_count, zval *return_value)
{
    enum RequestType command_type;

    /* Determine which XINFO command to use based on subcommand */
    if (strcasecmp(subcommand, "CONSUMERS") == 0)
    {
        command_type = XInfoConsumers;
    }
    else if (strcasecmp(subcommand, "GROUPS") == 0)
    {
        command_type = XInfoGroups;
    }
    else if (strcasecmp(subcommand, "STREAM") == 0)
    {
        command_type = XInfoStream;
    }
    else
    {
        /* Unknown subcommand */
        return 0;
    }

    /* We need to handle the arguments based on the subcommand */
    if (strcasecmp(subcommand, "STREAM") == 0)
    {
        /* We need at least the key argument */
        if (!args || args_count < 1 || Z_TYPE(args[0]) == IS_NULL)
        {
            return 0;
        }

        /* Create arguments based on args array */
        uintptr_t *cmd_args = NULL;
        unsigned long *args_len = NULL;
        char **allocated_strings = NULL;
        int allocated_count = 0;
        int arg_count = 0;

        /* Initialize x_command_args_t, but we'll handle arguments manually */
        x_command_args_t x_args = {0};
        x_args.glide_client = glide_client;
        x_args.subcommand = subcommand;
        x_args.subcommand_len = subcommand_len;
        x_args.args = args;
        x_args.args_count = args_count;

        /* Allocate for arguments */
        cmd_args = (uintptr_t *)emalloc(5 * sizeof(uintptr_t)); /* Maximum 5 args: key, FULL, COUNT, value */
        args_len = (unsigned long *)emalloc(5 * sizeof(unsigned long));
        allocated_strings = (char **)ecalloc(2, sizeof(char *));
        allocated_count = 0;

        if (!cmd_args || !args_len || !allocated_strings)
        {
            if (cmd_args)
                efree(cmd_args);
            if (args_len)
                efree(args_len);
            if (allocated_strings)
                efree(allocated_strings);
            return 0;
        }

        /* Add key */
        if (Z_TYPE(args[0]) == IS_STRING)
        {
            cmd_args[arg_count] = (uintptr_t)Z_STRVAL(args[0]);
            args_len[arg_count] = Z_STRLEN(args[0]);
            arg_count++;
        }
        else
        {
            zval temp;
            ZVAL_COPY(&temp, &args[0]);
            convert_to_string(&temp);
            char *key_copy = estrndup(Z_STRVAL(temp), Z_STRLEN(temp));
            if (key_copy)
            {
                allocated_strings[allocated_count++] = key_copy;
                cmd_args[arg_count] = (uintptr_t)key_copy;
                args_len[arg_count] = Z_STRLEN(temp);
                arg_count++;
            }
            zval_dtor(&temp);
        }

        /* Check for FULL option */
        zend_bool has_full = 0;
        if (args_count >= 2 && Z_TYPE(args[1]) != IS_NULL)
        {
            if (Z_TYPE(args[1]) == IS_STRING && strcasecmp(Z_STRVAL(args[1]), "FULL") == 0)
            {
                has_full = 1;
                cmd_args[arg_count] = (uintptr_t)"FULL";
                args_len[arg_count] = sizeof("FULL") - 1;
                arg_count++;
            }
        }

        /* Check for COUNT (only if FULL is specified) */
        if (has_full && args_count >= 3 && Z_TYPE(args[2]) != IS_NULL)
        {
            zend_bool has_count = 0;
            long count_value = 0;

            if (Z_TYPE(args[2]) == IS_LONG)
            {
                count_value = Z_LVAL(args[2]);
                if (count_value != -1)
                {
                    has_count = 1;
                }
            }
            else if (Z_TYPE(args[2]) == IS_STRING)
            {
                if (Z_STRLEN(args[2]) != 2 || strcmp(Z_STRVAL(args[2]), "-1") != 0)
                {
                    has_count = 1;
                    count_value = atol(Z_STRVAL(args[2]));
                }
            }

            if (has_count)
            {
                cmd_args[arg_count] = (uintptr_t)"COUNT";
                args_len[arg_count] = sizeof("COUNT") - 1;
                arg_count++;

                char count_str[32];
                size_t count_str_len = snprintf(count_str, sizeof(count_str), "%ld", count_value);
                char *count_copy = estrndup(count_str, count_str_len);
                if (count_copy)
                {
                    allocated_strings[allocated_count++] = count_copy;
                    cmd_args[arg_count] = (uintptr_t)count_copy;
                    args_len[arg_count] = count_str_len;
                    arg_count++;
                }
            }
        }

        /* Execute command */
        CommandResult *result = execute_command(
            glide_client,
            command_type, /* Use specific XINFO command type */
            arg_count,    /* total arguments */
            cmd_args,     /* arguments */
            args_len      /* argument lengths */
        );

        /* Free allocated strings */
        for (int i = 0; i < allocated_count; i++)
        {
            if (allocated_strings[i])
                efree(allocated_strings[i]);
        }
        efree(allocated_strings);
        efree(cmd_args);
        efree(args_len);

        /* Process result */
        int status = 0;
        if (result)
        {
            if (!result->command_error && result->response)
            {
                /* XINFO returns information about the stream */
                status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_ASSOSIATIVE_ARRAY, false);
            }
            free_command_result(result);
        }

        return status;
    }
    else
    {
        /* For CONSUMERS and GROUPS */
        uintptr_t *cmd_args = NULL;
        unsigned long *args_len = NULL;
        int arg_count = 0;

        /* Count valid args (not NULL) */
        int valid_arg_count = 0;
        if (args && args_count > 0)
        {
            for (int i = 0; i < args_count; i++)
            {
                if (Z_TYPE(args[i]) != IS_NULL)
                {
                    valid_arg_count++;
                }
            }
        }

        if (valid_arg_count > 0)
        {
            cmd_args = (uintptr_t *)emalloc(valid_arg_count * sizeof(uintptr_t));
            args_len = (unsigned long *)emalloc(valid_arg_count * sizeof(unsigned long));

            if (!cmd_args || !args_len)
            {
                if (cmd_args)
                    efree(cmd_args);
                if (args_len)
                    efree(args_len);
                return 0;
            }

            /* Add non-NULL arguments */
            arg_count = 0;
            for (int i = 0; i < args_count; i++)
            {
                /* Skip NULL arguments */
                if (Z_TYPE(args[i]) == IS_NULL)
                {
                    continue;
                }

                /* Convert to string if not already */
                if (Z_TYPE(args[i]) != IS_STRING)
                {
                    convert_to_string(&args[i]);
                }

                cmd_args[arg_count] = (uintptr_t)Z_STRVAL(args[i]);
                args_len[arg_count] = Z_STRLEN(args[i]);
                arg_count++;
            }
        }

        /* Execute command */
        CommandResult *result;
        if (valid_arg_count > 0)
        {
            result = execute_command(
                glide_client,
                command_type, /* Use specific XINFO command type */
                arg_count,    /* total arguments */
                cmd_args,     /* arguments */
                args_len      /* argument lengths */
            );
        }
        else
        {
            /* No arguments case */
            result = execute_command(
                glide_client,
                command_type, /* Use specific XINFO command type */
                0,            /* no arguments */
                NULL,         /* no arguments */
                NULL          /* no argument lengths */
            );
        }

        /* Free arguments if allocated */
        if (cmd_args)
            efree(cmd_args);
        if (args_len)
            efree(args_len);

        /* Process result */
        int status = 0;
        if (result)
        {
            if (!result->command_error && result->response)
            {
                /* XINFO returns information about the stream or consumers */
                status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_ASSOSIATIVE_ARRAY, false);
            }
            free_command_result(result);
        }

        return status;
    }

    return 0;
}

/**
 * Execute an XGROUP command
 */
int execute_xgroup_command(const void *glide_client, const char *subcommand, size_t subcommand_len,
                           zval *args, int args_count, zval *return_value)
{
    /* Initialize the arguments structure */
    x_command_args_t x_args = {0};
    x_args.glide_client = glide_client;
    x_args.subcommand = subcommand;
    x_args.subcommand_len = subcommand_len;
    x_args.args = args;
    x_args.args_count = args_count;

    /* Prepare arguments manually since XGROUP needs special handling */
    uintptr_t *cmd_args = NULL;
    unsigned long *args_len = NULL;
    int arg_count;

    /* Prepare arguments for the XGROUP command */
    arg_count = prepare_x_group_args(&x_args, &cmd_args, &args_len);
    if (arg_count <= 0)
    {
        return 0;
    }

    /* Prepend "XGROUP" to the command arguments */
    uintptr_t *full_args = (uintptr_t *)emalloc((arg_count + 1) * sizeof(uintptr_t));
    unsigned long *full_args_len = (unsigned long *)emalloc((arg_count + 1) * sizeof(unsigned long));

    if (!full_args || !full_args_len)
    {
        if (cmd_args)
            efree(cmd_args);
        if (args_len)
            efree(args_len);
        if (full_args)
            efree(full_args);
        if (full_args_len)
            efree(full_args_len);
        return 0;
    }

    /* Add "XGROUP" as the first argument */
    full_args[0] = (uintptr_t)"XGROUP";
    full_args_len[0] = sizeof("XGROUP") - 1;

    /* Copy the rest of the arguments */
    for (int i = 0; i < arg_count; i++)
    {
        full_args[i + 1] = cmd_args[i];
        full_args_len[i + 1] = args_len[i];
    }

    /* Free the original arguments arrays */
    efree(cmd_args);
    efree(args_len);

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        CustomCommand, /* XGROUP uses custom command type */
        arg_count + 1, /* total arguments (including XGROUP) */
        full_args,     /* arguments */
        full_args_len  /* argument lengths */
    );

    /* Free arguments */
    efree(full_args);
    efree(full_args_len);

    /* Process result */
    int status = 0;
    if (result)
    {
        if (!result->command_error && result->response)
        {
            /* Process the result with our standard function */
            status = process_x_group_result(result, return_value);
        }
        free_command_result(result);
    }

    return status;
}

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
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);

            /* Special handling for empty XPENDING response */
            if (status && Z_TYPE_P(return_value) == IS_ARRAY)
            {
                HashTable *ht = Z_ARRVAL_P(return_value);
                int num_elements = zend_hash_num_elements(ht);

                if (num_elements > 0)
                {
                    /* Get the last index for special handling (last element becomes empty array) */
                    int last_index = num_elements - 1;

                    /* Iterate through all elements */
                    zval *element;
                    zend_ulong idx;

                    ZEND_HASH_FOREACH_NUM_KEY_VAL(ht, idx, element)
                    {
                        /* If element is NULL, convert it based on position */
                        if (Z_TYPE_P(element) == IS_NULL)
                        {

                            /* All other NULL elements become bool(false) */
                            ZVAL_BOOL(element, 0);
                        }
                    }
                    ZEND_HASH_FOREACH_END();
                }
            }

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
            status = command_response_to_stream_zval(result->response, return_value);
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
        printf("DEBUG: XREADGROUP validation failed\n");
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
        printf("DEBUG: XREADGROUP stream/ID count mismatch\n");
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
            /* We need to create a specific format for XREADGROUP responses:
             * [stream_name => [id1 => [field=>value, ...], id2 => [...]], ...]
             */
            array_init(return_value);

            if (result->response->response_type == Map && result->response->array_value_len > 0)
            {
                /* First element has the stream name and entries */
                for (int jj = 0; jj < result->response->array_value_len; jj++)
                {
                    CommandResponse *element = &result->response->array_value[jj];

                    if (element->map_key && element->map_key->response_type == String && element->map_value)
                    {
                        zval stream_name;
                        command_response_to_zval(element->map_key, &stream_name, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);

                        /* Create associative array for stream entries */

                        /* Process the entries - they're in an array format with alternating ID and data */

                        zval stream_entries;
                        command_response_to_stream_zval(element->map_value, &stream_entries);

                        /* Add stream entries to output as an associative array */
                        add_assoc_zval(return_value, Z_STRVAL(stream_name), &stream_entries);
                        zval_dtor(&stream_name); // Clean up stream name after adding
                        status = 1;
                    }
                }
            }

            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute an XAUTOCLAIM command using the Valkey Glide client */
int execute_xautoclaim_command(const void *glide_client, const char *key, size_t key_len,
                               const char *group, size_t group_len, const char *consumer,
                               size_t consumer_len, long min_idle_time, const char *start,
                               size_t start_len, zval *options, zval *return_value)
{
    /* Check if client and arguments are valid */
    if (!glide_client || !key || key_len <= 0 || !group || group_len <= 0 ||
        !consumer || consumer_len <= 0 || !start)
    {
        return 0;
    }

    /* Check for options */
    zend_bool has_count = 0, has_justid = 0;
    long count_value = 0;
    char count_str[32];
    unsigned long count_str_len = 0;
    unsigned long extra_args = 0;

    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(options);
        zval *z_count, *z_justid;

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

        /* Check for JUSTID option - first check associative key */
        if ((z_justid = zend_hash_str_find(ht, "JUSTID", sizeof("JUSTID") - 1)) != NULL)
        {
            has_justid = zval_is_true(z_justid);
            if (has_justid)
                extra_args += 1; /* JUSTID */
        }
        /* If not found as associative key, check array values */
        else
        {
            zval *z_val;
            ZEND_HASH_FOREACH_VAL(ht, z_val)
            {
                if (Z_TYPE_P(z_val) == IS_STRING)
                {
                    if (Z_STRLEN_P(z_val) == 6 && strcasecmp(Z_STRVAL_P(z_val), "JUSTID") == 0)
                    {
                        has_justid = 1;
                        extra_args += 1; /* JUSTID */
                        break;
                    }
                }
            }
            ZEND_HASH_FOREACH_END();
        }
    }

    /* Calculate total args: key + group + consumer + min_idle_time + start + options */
    unsigned long arg_count = 5 + extra_args;
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

    /* Set key, group, consumer, min_idle_time, start */
    unsigned int arg_idx = 0;
    args[arg_idx] = (uintptr_t)key;
    args_len[arg_idx] = key_len;
    arg_idx++;

    args[arg_idx] = (uintptr_t)group;
    args_len[arg_idx] = group_len;
    arg_idx++;

    args[arg_idx] = (uintptr_t)consumer;
    args_len[arg_idx] = consumer_len;
    arg_idx++;

    /* Convert min_idle_time to string */
    char min_idle_str[32];
    unsigned long min_idle_str_len = snprintf(min_idle_str, sizeof(min_idle_str), "%ld", min_idle_time);
    args[arg_idx] = (uintptr_t)min_idle_str;
    args_len[arg_idx] = min_idle_str_len;
    arg_idx++;

    /* Add start ID */
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

    /* Add JUSTID if specified */
    if (has_justid)
    {
        args[arg_idx] = (uintptr_t)"JUSTID";
        args_len[arg_idx] = sizeof("JUSTID") - 1;
        arg_idx++;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        XAutoClaim, /* command type */
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
            /* XAUTOCLAIM returns array with [next-id, claimed-entries] */
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_STREAM_ARRAY_ASSOCIATIVE, false);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute an XCLAIM command using the Valkey Glide client */
int execute_xclaim_command(const void *glide_client, const char *key, size_t key_len,
                           const char *group, size_t group_len, const char *consumer,
                           size_t consumer_len, long min_idle_time, zval *ids, int id_count,
                           zval *options, zval *return_value)
{
    /* Check if client and arguments are valid */
    if (!glide_client || !key || key_len <= 0 || !group || group_len <= 0 ||
        !consumer || consumer_len <= 0 || !ids || id_count <= 0)
    {
        return 0;
    }

    /* Check for options */
    zend_bool has_idle = 0, has_time = 0, has_retrycount = 0, has_force = 0, has_justid = 0;
    long idle_time = 0, time_stamp = 0, retry_count = 0;
    char idle_str[32], time_str[32], retry_str[32];
    unsigned long idle_str_len = 0, time_str_len = 0, retry_str_len = 0;
    unsigned long extra_args = 0;

    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(options);
        zval *z_idle, *z_time, *z_retry, *z_force, *z_justid;
        // php_var_dump(options, 2);

        /* Check for IDLE option */
        if ((z_idle = zend_hash_str_find(ht, "IDLE", sizeof("IDLE") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_idle) == IS_LONG)
            {
                idle_time = Z_LVAL_P(z_idle);
                has_idle = 1;
                idle_str_len = snprintf(idle_str, sizeof(idle_str), "%ld", idle_time);
                extra_args += 2; /* IDLE + value */
            }
        }

        /* Check for TIME option */
        if ((z_time = zend_hash_str_find(ht, "TIME", sizeof("TIME") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_time) == IS_LONG)
            {
                time_stamp = Z_LVAL_P(z_time);
                has_time = 1;
                time_str_len = snprintf(time_str, sizeof(time_str), "%ld", time_stamp);
                extra_args += 2; /* TIME + value */
            }
        }

        /* Check for RETRYCOUNT option */
        if ((z_retry = zend_hash_str_find(ht, "RETRYCOUNT", sizeof("RETRYCOUNT") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_retry) == IS_LONG)
            {
                retry_count = Z_LVAL_P(z_retry);
                has_retrycount = 1;
                retry_str_len = snprintf(retry_str, sizeof(retry_str), "%ld", retry_count);
                extra_args += 2; /* RETRYCOUNT + value */
            }
        }

        /* Check for FORCE option - first check associative key */
        // printf("DEBUG: Checking FORCE option\n");
        if ((z_force = zend_hash_str_find(ht, "FORCE", sizeof("FORCE") - 1)) != NULL)
        {
            // printf("DEBUG: Found FORCE option\n");
            has_force = zval_is_true(z_force);
            if (has_force)
                extra_args += 1; /* FORCE */
        }
        /* If not found as associative key, check array values */
        else
        {
            zval *z_val;
            ZEND_HASH_FOREACH_VAL(ht, z_val)
            {
                if (Z_TYPE_P(z_val) == IS_STRING)
                {
                    if (Z_STRLEN_P(z_val) == 5 && strcasecmp(Z_STRVAL_P(z_val), "FORCE") == 0)
                    {
                        has_force = 1;
                        extra_args += 1; /* FORCE */
                        break;
                    }
                }
            }
            ZEND_HASH_FOREACH_END();
        }

        /* Check for JUSTID option - first check associative key */
        if ((z_justid = zend_hash_str_find(ht, "JUSTID", sizeof("JUSTID") - 1)) != NULL)
        {
            has_justid = zval_is_true(z_justid);
            if (has_justid)
                extra_args += 1; /* JUSTID */
        }
        /* If not found as associative key, check array values */
        else
        {
            zval *z_val;
            ZEND_HASH_FOREACH_VAL(ht, z_val)
            {
                if (Z_TYPE_P(z_val) == IS_STRING)
                {
                    if (Z_STRLEN_P(z_val) == 6 && strcasecmp(Z_STRVAL_P(z_val), "JUSTID") == 0)
                    {
                        has_justid = 1;
                        extra_args += 1; /* JUSTID */
                        break;
                    }
                }
            }
            ZEND_HASH_FOREACH_END();
        }
    }
    //("DEBUG: Extra args count: %lu has_justid=%d\n", extra_args, has_justid);
    /* Calculate total args: key + group + consumer + min_idle_time + options + ids */
    unsigned long arg_count = 4 + extra_args + id_count;
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

    /* Set key, group, consumer, min_idle_time */
    unsigned int arg_idx = 0;
    args[arg_idx] = (uintptr_t)key;
    args_len[arg_idx] = key_len;
    arg_idx++;

    args[arg_idx] = (uintptr_t)group;
    args_len[arg_idx] = group_len;
    arg_idx++;

    args[arg_idx] = (uintptr_t)consumer;
    args_len[arg_idx] = consumer_len;
    arg_idx++;

    char min_idle_str[32];
    unsigned long min_idle_str_len = snprintf(min_idle_str, sizeof(min_idle_str), "%ld", min_idle_time);
    args[arg_idx] = (uintptr_t)min_idle_str;
    args_len[arg_idx] = min_idle_str_len;
    arg_idx++;

    /* Add all message IDs */
    zval *z_id;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(ids), z_id)
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

    /* Add options */
    if (has_idle)
    {
        args[arg_idx] = (uintptr_t)"IDLE";
        args_len[arg_idx] = sizeof("IDLE") - 1;
        arg_idx++;

        args[arg_idx] = (uintptr_t)idle_str;
        args_len[arg_idx] = idle_str_len;
        arg_idx++;
    }

    if (has_time)
    {
        args[arg_idx] = (uintptr_t)"TIME";
        args_len[arg_idx] = sizeof("TIME") - 1;
        arg_idx++;

        args[arg_idx] = (uintptr_t)time_str;
        args_len[arg_idx] = time_str_len;
        arg_idx++;
    }

    if (has_retrycount)
    {
        args[arg_idx] = (uintptr_t)"RETRYCOUNT";
        args_len[arg_idx] = sizeof("RETRYCOUNT") - 1;
        arg_idx++;

        args[arg_idx] = (uintptr_t)retry_str;
        args_len[arg_idx] = retry_str_len;
        arg_idx++;
    }

    if (has_force)
    {
        args[arg_idx] = (uintptr_t)"FORCE";
        args_len[arg_idx] = sizeof("FORCE") - 1;
        arg_idx++;
    }

    if (has_justid)
    {
        args[arg_idx] = (uintptr_t)"JUSTID";
        args_len[arg_idx] = sizeof("JUSTID") - 1;
        arg_idx++;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        XClaim,    /* command type */
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
            /* XCLAIM returns the claimed entries */
            // printf("file = %s, line = %d, XCLAIM response received\n", __FILE__, __LINE__);
            if (has_justid)
            {
                // printf("file = %s, line = %d, JUSTID was specified\n", __FILE__, __LINE__);
                /* If JUSTID was specified, we return an array of IDs */
                status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
            }
            else
            {
                /* Otherwise, we return the full entries */
                status = command_response_to_stream_zval(result->response, return_value);
            }
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}
