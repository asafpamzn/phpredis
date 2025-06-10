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
    /* Initialize the arguments structure */
    x_command_args_t args = {0};
    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.group = group;
    args.group_len = group_len;
    args.options = options;

    /* Parse options for XPENDING command */
    parse_x_pending_options(options, &args.pending_opts);

    /* Use the generic command execution framework */
    return execute_x_generic_command(glide_client, XPending, &args, return_value, process_x_pending_result);
}

/* Execute an XREAD command using the Valkey Glide client */
int execute_xread_command(const void *glide_client, zval *streams, zval *ids,
                          zval *options, zval *return_value)
{
    /* Initialize the arguments structure */
    x_command_args_t args = {0};
    args.glide_client = glide_client;
    args.streams = streams;
    args.ids = ids;
    args.options = options;

    /* Parse options for XREAD command */
    parse_x_read_options(options, &args.read_opts);

    /* Use the generic command execution framework */
    return execute_x_generic_command(glide_client, XRead, &args, return_value, process_x_stream_result);
}

/* Execute an XREADGROUP command using the Valkey Glide client */
int execute_xreadgroup_command(const void *glide_client, const char *group, size_t group_len,
                               const char *consumer, size_t consumer_len, zval *streams, zval *ids,
                               zval *options, zval *return_value)
{
    /* Initialize the arguments structure */
    x_command_args_t args = {0};
    args.glide_client = glide_client;
    args.group = group;
    args.group_len = group_len;
    args.consumer = consumer;
    args.consumer_len = consumer_len;
    args.streams = streams;
    args.ids = ids;
    args.options = options;

    /* Parse options for XREADGROUP command */
    parse_x_read_options(options, &args.read_opts);

    /* Use the generic command execution framework */
    return execute_x_generic_command(glide_client, XReadGroup, &args, return_value, process_x_readgroup_result);
}

/* Execute an XAUTOCLAIM command using the Valkey Glide client */
int execute_xautoclaim_command(const void *glide_client, const char *key, size_t key_len,
                               const char *group, size_t group_len, const char *consumer,
                               size_t consumer_len, long min_idle_time, const char *start,
                               size_t start_len, zval *options, zval *return_value)
{
    /* Initialize the arguments structure */
    x_command_args_t args = {0};
    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.group = group;
    args.group_len = group_len;
    args.consumer = consumer;
    args.consumer_len = consumer_len;
    args.min_idle_time = min_idle_time;
    args.start = start;
    args.start_len = start_len;
    args.options = options;

    /* Parse options for XAUTOCLAIM command */
    parse_x_claim_options(options, &args.claim_opts);

    /* Check if COUNT is specified (not in standard claim_opts) */
    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(options);
        zval *z_count;

        /* Check for COUNT option */
        if ((z_count = zend_hash_str_find(ht, "COUNT", sizeof("COUNT") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_count) == IS_LONG)
            {
                args.claim_opts.count = Z_LVAL_P(z_count);
                args.claim_opts.has_count = 1;
            }
        }
    }

    /* Use the generic command execution framework */
    return execute_x_generic_command(glide_client, XAutoClaim, &args, return_value, process_x_autoclaim_result);
}

/**
 * Helper function to process XCLAIM results
 */
static int process_xclaim_result_adapter(CommandResult *result, void *output, int justid)
{
    return process_x_claim_result(result, output, justid);
}

/* Execute an XCLAIM command using the Valkey Glide client */
int execute_xclaim_command(const void *glide_client, const char *key, size_t key_len,
                           const char *group, size_t group_len, const char *consumer,
                           size_t consumer_len, long min_idle_time, zval *ids, int id_count,
                           zval *options, zval *return_value)
{
    /* Initialize the arguments structure */
    x_command_args_t args = {0};
    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.group = group;
    args.group_len = group_len;
    args.consumer = consumer;
    args.consumer_len = consumer_len;
    args.min_idle_time = min_idle_time;
    args.ids = ids;
    args.id_count = id_count;
    args.options = options;

    /* Parse options for XCLAIM command */
    parse_x_claim_options(options, &args.claim_opts);

    /* For XCLAIM, we need to implement a custom result processor */
    int status = 0;

    /* Use direct command execution so we can extract the JUSTID flag */
    uintptr_t *cmd_args = NULL;
    unsigned long *args_len = NULL;
    int arg_count = 0;

    /* Prepare arguments */
    arg_count = prepare_x_claim_args(&args, &cmd_args, &args_len);
    if (arg_count <= 0)
    {
        return 0;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        XClaim,
        arg_count,
        cmd_args,
        args_len);

    /* Free command args */
    free_command_args(cmd_args, args_len);

    /* Process result */
    if (result)
    {
        if (!result->command_error && result->response)
        {
            status = process_xclaim_result_adapter(result, return_value, args.claim_opts.justid);
        }
        free_command_result(result);
    }

    return status;
}
