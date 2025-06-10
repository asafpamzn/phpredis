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

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

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

    /* Initialize the arguments structure */
    x_command_args_t x_args = {0};
    x_args.glide_client = glide_client;
    x_args.subcommand = subcommand;
    x_args.subcommand_len = subcommand_len;
    x_args.args = args;
    x_args.args_count = args_count;

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

    /* Use the generic command execution framework */
    return execute_x_generic_command(glide_client, command_type, &x_args, return_value, process_x_info_result);
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
int execute_xreadgroup_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    char *group = NULL, *consumer = NULL;
    size_t group_len = 0, consumer_len = 0;
    zval *z_streams_and_ids, *z_options = NULL;
    long count = -1;
    int options_created = 0;

    /* Parse parameters - handle multiple calling patterns */
    if (argc == 4)
    {
        /* Try parsing as (group, consumer, streams, count) first */
        if (zend_parse_method_parameters(argc, object, "Ossal",
                                         &object, redis_ce, &group, &group_len,
                                         &consumer, &consumer_len, &z_streams_and_ids, &count) == SUCCESS)
        {
            /* Create options array with COUNT */
            z_options = emalloc(sizeof(zval));
            array_init(z_options);
            add_assoc_long(z_options, "COUNT", count);
            options_created = 1;
        }
        else
        {
            /* Try parsing as (group, consumer, streams, options) */
            if (zend_parse_method_parameters(argc, object, "Ossa",
                                             &object, redis_ce, &group, &group_len,
                                             &consumer, &consumer_len, &z_streams_and_ids, &z_options) == FAILURE)
            {
                return 0;
            }
        }
    }
    else if (argc == 5)
    {
        long block = -1;

        /* First try parsing as (group, consumer, streams, count, block) */
        if (zend_parse_method_parameters(argc, object, "Ossall",
                                         &object, redis_ce, &group, &group_len,
                                         &consumer, &consumer_len, &z_streams_and_ids, &count, &block) == SUCCESS)
        {
            /* Create options array with both COUNT and BLOCK */
            z_options = emalloc(sizeof(zval));
            array_init(z_options);
            add_assoc_long(z_options, "COUNT", count);
            add_assoc_long(z_options, "BLOCK", block);
            options_created = 1;
        }
        else
        {
            /* Fallback to parsing as (group, consumer, streams, count, options) */
            if (zend_parse_method_parameters(argc, object, "Ossala",
                                             &object, redis_ce, &group, &group_len,
                                             &consumer, &consumer_len, &z_streams_and_ids, &count, &z_options) == FAILURE)
            {
                return 0;
            }

            /* Add COUNT to existing options array or create new one */
            if (z_options && Z_TYPE_P(z_options) == IS_ARRAY)
            {
                add_assoc_long(z_options, "COUNT", count);
            }
            else
            {
                z_options = emalloc(sizeof(zval));
                array_init(z_options);
                add_assoc_long(z_options, "COUNT", count);
                options_created = 1;
            }
        }
    }
    else
    {
        /* Parse as (group, consumer, streams [, options]) - original format for backward compatibility */
        if (zend_parse_method_parameters(argc, object, "Ossa|a",
                                         &object, redis_ce, &group, &group_len,
                                         &consumer, &consumer_len, &z_streams_and_ids, &z_options) == FAILURE)
        {
            return 0;
        }
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* For the combined format, we need to separate streams and IDs */
        zval z_streams, z_ids;
        array_init(&z_streams);
        array_init(&z_ids);

        /* Extract streams and IDs from the combined array */
        zend_string *stream_key;
        zval *stream_id;
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(z_streams_and_ids), stream_key, stream_id)
        {
            if (stream_key)
            {
                add_next_index_str(&z_streams, zend_string_copy(stream_key));
                if (Z_TYPE_P(stream_id) != IS_STRING)
                {
                    convert_to_string(stream_id);
                }
                add_next_index_str(&z_ids, zend_string_copy(Z_STR_P(stream_id)));
            }
        }
        ZEND_HASH_FOREACH_END();

        /* Initialize the arguments structure */
        x_command_args_t args = {0};
        args.glide_client = redis->glide_client;
        args.group = group;
        args.group_len = group_len;
        args.consumer = consumer;
        args.consumer_len = consumer_len;
        args.streams = &z_streams;
        args.ids = &z_ids;
        args.options = z_options;

        /* Parse options for XREADGROUP command */
        parse_x_read_options(z_options, &args.read_opts);

        /* Execute the command with the generic framework */
        int result = execute_x_generic_command(
            redis->glide_client,
            XReadGroup,
            &args,
            return_value,
            process_x_readgroup_result);

        /* Clean up temporary arrays */
        zval_dtor(&z_streams);
        zval_dtor(&z_ids);

        /* Clean up if we created options array */
        if (options_created)
        {
            zval_dtor(z_options);
            efree(z_options);
        }

        return result;
    }

    return 0;
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
