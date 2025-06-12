/*
  +----------------------------------------------------------------------+
  | Redis Glide S-Commands Common Utilities                              |
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

#include "redis_glide_s_common.h"

/* ====================================================================
 * UTILITY FUNCTIONS
 * ==================================================================== */

/**
 * Allocate command arguments arrays
 */
int allocate_s_command_args(int count, uintptr_t **args_out, unsigned long **args_len_out)
{
    *args_out = (uintptr_t *)emalloc(count * sizeof(uintptr_t));
    *args_len_out = (unsigned long *)emalloc(count * sizeof(unsigned long));

    if (!*args_out || !*args_len_out)
    {
        if (*args_out)
            efree(*args_out);
        if (*args_len_out)
            efree(*args_len_out);
        return 0;
    }

    return 1;
}

/**
 * Free command arguments arrays
 */
void cleanup_s_command_args(uintptr_t *args, unsigned long *args_len)
{
    if (args)
        efree(args);
    if (args_len)
        efree(args_len);
}

/**
 * Convert a single zval to string with proper cleanup handling
 */
int convert_single_zval_to_string(zval *input, const char **str_out, size_t *len_out, zval *temp_storage)
{
    if (!input || !str_out || !len_out)
    {
        return 0;
    }

    if (Z_TYPE_P(input) == IS_STRING)
    {
        *str_out = Z_STRVAL_P(input);
        *len_out = Z_STRLEN_P(input);
        return 1;
    }

    /* Handle object conversion specially */
    if (Z_TYPE_P(input) == IS_OBJECT)
    {
        ZVAL_COPY(temp_storage, input);
        zend_std_cast_object_tostring(temp_storage, temp_storage, IS_STRING);
        *str_out = Z_STRVAL_P(temp_storage);
        *len_out = Z_STRLEN_P(temp_storage);
        return 1;
    }

    /* Handle other types */
    ZVAL_COPY(temp_storage, input);
    convert_to_string(temp_storage);
    *str_out = Z_STRVAL_P(temp_storage);
    *len_out = Z_STRLEN_P(temp_storage);
    return 1;
}

/**
 * Convert array of zvals to string arguments
 */
int convert_zval_to_string_args(zval *input, int count, uintptr_t **args_out, unsigned long **args_len_out, int offset)
{
    int i;
    zval temp;

    for (i = 0; i < count; i++)
    {
        zval *element = &input[i];

        if (Z_TYPE_P(element) == IS_STRING)
        {
            (*args_out)[offset + i] = (uintptr_t)Z_STRVAL_P(element);
            (*args_len_out)[offset + i] = Z_STRLEN_P(element);
        }
        else if (Z_TYPE_P(element) == IS_OBJECT)
        {
            /* Handle object conversion specially */
            zval tmp_zval;
            ZVAL_COPY(&tmp_zval, element);
            zend_std_cast_object_tostring(&tmp_zval, &tmp_zval, IS_STRING);
            (*args_out)[offset + i] = (uintptr_t)Z_STRVAL(tmp_zval);
            (*args_len_out)[offset + i] = Z_STRLEN(tmp_zval);
            zval_dtor(&tmp_zval);
        }
        else
        {
            /* Convert other types to string */
            ZVAL_COPY(&temp, element);
            convert_to_string(&temp);
            (*args_out)[offset + i] = (uintptr_t)Z_STRVAL(temp);
            (*args_len_out)[offset + i] = Z_STRLEN(temp);
            zval_dtor(&temp);
        }
    }

    return 1;
}

/**
 * Allocate a string representation of a long value
 */
char *alloc_long_string(long value, size_t *len_out)
{
    char temp[32];
    size_t len = snprintf(temp, sizeof(temp), "%ld", value);
    char *result = emalloc(len + 1);
    if (result)
    {
        memcpy(result, temp, len);
        result[len] = '\0';
        if (len_out)
            *len_out = len;
    }
    return result;
}

/* ====================================================================
 * ARGUMENT PREPARATION FUNCTIONS
 * ==================================================================== */

/**
 * Prepare arguments for key + members commands (SADD, SREM, SMISMEMBER)
 */
int prepare_s_key_members_args(s_command_args_t *args, uintptr_t **args_out, unsigned long **args_len_out)
{
    if (!args->glide_client || !args->key || args->key_len == 0 ||
        !args->members || args->members_count <= 0)
    {
        return 0;
    }

    unsigned long arg_count = 1 + args->members_count; /* key + members */

    if (!allocate_s_command_args(arg_count, args_out, args_len_out))
    {
        return 0;
    }

    /* Set key as first argument */
    (*args_out)[0] = (uintptr_t)args->key;
    (*args_len_out)[0] = args->key_len;

    /* Convert and set member arguments */
    convert_zval_to_string_args(args->members, args->members_count, args_out, args_len_out, 1);

    return arg_count;
}

/**
 * Prepare arguments for key-only commands (SCARD, SMEMBERS)
 */
int prepare_s_key_only_args(s_command_args_t *args, uintptr_t **args_out, unsigned long **args_len_out)
{
    if (!args->glide_client || !args->key || args->key_len == 0)
    {
        return 0;
    }

    if (!allocate_s_command_args(1, args_out, args_len_out))
    {
        return 0;
    }

    (*args_out)[0] = (uintptr_t)args->key;
    (*args_len_out)[0] = args->key_len;

    return 1;
}

/**
 * Prepare arguments for key + member commands (SISMEMBER)
 */
int prepare_s_key_member_args(s_command_args_t *args, uintptr_t **args_out, unsigned long **args_len_out)
{
    if (!args->glide_client || !args->key || args->key_len == 0 ||
        !args->member || args->member_len == 0)
    {
        return 0;
    }

    if (!allocate_s_command_args(2, args_out, args_len_out))
    {
        return 0;
    }

    (*args_out)[0] = (uintptr_t)args->key;
    (*args_len_out)[0] = args->key_len;
    (*args_out)[1] = (uintptr_t)args->member;
    (*args_len_out)[1] = args->member_len;

    return 2;
}

/**
 * Prepare arguments for key + count commands (SPOP, SRANDMEMBER)
 */
int prepare_s_key_count_args(s_command_args_t *args, uintptr_t **args_out, unsigned long **args_len_out)
{
    if (!args->glide_client || !args->key || args->key_len == 0)
    {
        return 0;
    }

    unsigned long arg_count = args->has_count ? 2 : 1;

    if (!allocate_s_command_args(arg_count, args_out, args_len_out))
    {
        return 0;
    }

    (*args_out)[0] = (uintptr_t)args->key;
    (*args_len_out)[0] = args->key_len;

    if (args->has_count)
    {
        char *count_str = alloc_long_string(args->count, NULL);
        if (!count_str)
        {
            cleanup_s_command_args(*args_out, *args_len_out);
            return 0;
        }
        (*args_out)[1] = (uintptr_t)count_str;
        (*args_len_out)[1] = strlen(count_str);
    }

    return arg_count;
}

/**
 * Prepare arguments for multi-key commands (SINTER, SUNION, SDIFF)
 */
int prepare_s_multi_key_args(s_command_args_t *args, uintptr_t **args_out, unsigned long **args_len_out)
{
    if (!args->glide_client || !args->keys || args->keys_count <= 0)
    {
        return 0;
    }

    if (!allocate_s_command_args(args->keys_count, args_out, args_len_out))
    {
        return 0;
    }

    convert_zval_to_string_args(args->keys, args->keys_count, args_out, args_len_out, 0);

    return args->keys_count;
}

/**
 * Prepare arguments for multi-key + limit commands (SINTERCARD)
 */
int prepare_s_multi_key_limit_args(s_command_args_t *args, uintptr_t **args_out, unsigned long **args_len_out)
{
    if (!args->glide_client || !args->keys || args->keys_count <= 0)
    {
        return 0;
    }

    unsigned long arg_count = 1 + args->keys_count + (args->has_limit ? 2 : 0); /* numkeys + keys + [LIMIT value] */

    if (!allocate_s_command_args(arg_count, args_out, args_len_out))
    {
        return 0;
    }

    /* First argument is the number of keys */
    char *numkeys_str = alloc_long_string(args->keys_count, NULL);
    if (!numkeys_str)
    {
        cleanup_s_command_args(*args_out, *args_len_out);
        return 0;
    }
    (*args_out)[0] = (uintptr_t)numkeys_str;
    (*args_len_out)[0] = strlen(numkeys_str);

    /* Add keys */
    convert_zval_to_string_args(args->keys, args->keys_count, args_out, args_len_out, 1);

    /* Add LIMIT if specified */
    if (args->has_limit)
    {
        (*args_out)[1 + args->keys_count] = (uintptr_t)"LIMIT";
        (*args_len_out)[1 + args->keys_count] = 5;

        char *limit_str = alloc_long_string(args->limit, NULL);
        if (!limit_str)
        {
            efree((void *)(*args_out)[0]); /* Free numkeys_str */
            cleanup_s_command_args(*args_out, *args_len_out);
            return 0;
        }
        (*args_out)[2 + args->keys_count] = (uintptr_t)limit_str;
        (*args_len_out)[2 + args->keys_count] = strlen(limit_str);
    }

    return arg_count;
}

/**
 * Prepare arguments for destination + multi-key commands (SINTERSTORE, SUNIONSTORE, SDIFFSTORE)
 */
int prepare_s_dst_multi_key_args(s_command_args_t *args, uintptr_t **args_out, unsigned long **args_len_out)
{
    if (!args->glide_client || !args->dst_key || args->dst_key_len == 0 ||
        !args->keys || args->keys_count <= 0)
    {
        return 0;
    }

    unsigned long arg_count = 1 + args->keys_count; /* destination + keys */

    if (!allocate_s_command_args(arg_count, args_out, args_len_out))
    {
        return 0;
    }

    /* Set destination key */
    (*args_out)[0] = (uintptr_t)args->dst_key;
    (*args_len_out)[0] = args->dst_key_len;

    /* Add source keys */
    convert_zval_to_string_args(args->keys, args->keys_count, args_out, args_len_out, 1);

    return arg_count;
}

/**
 * Prepare arguments for two-key + member commands (SMOVE)
 */
int prepare_s_two_key_member_args(s_command_args_t *args, uintptr_t **args_out, unsigned long **args_len_out)
{
    if (!args->glide_client || !args->src_key || args->src_key_len == 0 ||
        !args->dst_key || args->dst_key_len == 0 ||
        !args->member || args->member_len == 0)
    {
        return 0;
    }

    if (!allocate_s_command_args(3, args_out, args_len_out))
    {
        return 0;
    }

    (*args_out)[0] = (uintptr_t)args->src_key;
    (*args_len_out)[0] = args->src_key_len;
    (*args_out)[1] = (uintptr_t)args->dst_key;
    (*args_len_out)[1] = args->dst_key_len;
    (*args_out)[2] = (uintptr_t)args->member;
    (*args_len_out)[2] = args->member_len;

    return 3;
}

/**
 * Prepare arguments for scan commands (SCAN, SSCAN)
 */
int prepare_s_scan_args(s_command_args_t *args, uintptr_t **args_out, unsigned long **args_len_out)
{
    if (!args->glide_client || !args->cursor)
    {
        return 0;
    }

    int has_pattern = (args->pattern && args->pattern_len > 0);
    int has_count = args->has_count;
    int has_key = (args->key && args->key_len > 0); /* For SSCAN */

    unsigned long arg_count = (has_key ? 1 : 0) + 1 + (has_pattern ? 2 : 0) + (has_count ? 2 : 0);

    if (!allocate_s_command_args(arg_count, args_out, args_len_out))
    {
        return 0;
    }

    int arg_idx = 0;

    /* Add key if this is SSCAN */
    if (has_key)
    {
        (*args_out)[arg_idx] = (uintptr_t)args->key;
        (*args_len_out)[arg_idx] = args->key_len;
        arg_idx++;
    }

    /* Add cursor */
    char *cursor_str = alloc_long_string(*args->cursor, NULL);
    if (!cursor_str)
    {
        cleanup_s_command_args(*args_out, *args_len_out);
        return 0;
    }
    (*args_out)[arg_idx] = (uintptr_t)cursor_str;
    (*args_len_out)[arg_idx] = strlen(cursor_str);
    arg_idx++;

    /* Add MATCH pattern if provided */
    if (has_pattern)
    {
        (*args_out)[arg_idx] = (uintptr_t)"MATCH";
        (*args_len_out)[arg_idx] = 5;
        arg_idx++;
        (*args_out)[arg_idx] = (uintptr_t)args->pattern;
        (*args_len_out)[arg_idx] = args->pattern_len;
        arg_idx++;
    }

    /* Add COUNT if provided */
    if (has_count)
    {
        (*args_out)[arg_idx] = (uintptr_t)"COUNT";
        (*args_len_out)[arg_idx] = 5;
        arg_idx++;

        char *count_str = alloc_long_string(args->count, NULL);
        if (!count_str)
        {
            efree((void *)(*args_out)[has_key ? 1 : 0]); /* Free cursor_str */
            cleanup_s_command_args(*args_out, *args_len_out);
            return 0;
        }
        (*args_out)[arg_idx] = (uintptr_t)count_str;
        (*args_len_out)[arg_idx] = strlen(count_str);
        arg_idx++;
    }

    return arg_count;
}

/**
 * Prepare arguments for server commands
 */
int prepare_s_server_args(s_command_args_t *args, uintptr_t **args_out, unsigned long **args_len_out)
{
    if (!args->glide_client)
    {
        return 0;
    }

    if (!allocate_s_command_args(1, args_out, args_len_out))
    {
        return 0;
    }

    /* Server commands typically use "server" section for INFO */
    (*args_out)[0] = (uintptr_t)"server";
    (*args_len_out)[0] = 6;

    return 1;
}

/* ====================================================================
 * RESPONSE PROCESSING FUNCTIONS
 * ==================================================================== */

/**
 * Process integer response
 */
int process_s_int_response(CommandResult *result, s_command_args_t *args, zval *return_value)
{
    if (result && result->response && !result->command_error)
    {
        if (result->response->response_type == Int)
        {
            if (args->output_long)
            {
                *args->output_long = result->response->int_value;
                return 1;
            }
            if (return_value)
            {
                ZVAL_LONG(return_value, result->response->int_value);
                return 1;
            }
        }
    }
    return 0;
}

/**
 * Process boolean response
 */
int process_s_bool_response(CommandResult *result, s_command_args_t *args, zval *return_value)
{
    if (result && result->response && !result->command_error)
    {
        if (result->response->response_type == Bool)
        {
            int bool_val = result->response->bool_value ? 1 : 0;
            if (args->output_int)
            {
                *args->output_int = bool_val;
                return 1;
            }
            if (return_value)
            {
                ZVAL_BOOL(return_value, bool_val);
                return 1;
            }
        }
    }
    return 0;
}

/**
 * Process set/array response
 */
int process_s_set_response(CommandResult *result, s_command_args_t *args, zval *return_value)
{
    if (result && result->response && !result->command_error && return_value)
    {
        return handle_set_response(result, return_value);
    }
    return 0;
}

/**
 * Process mixed response (string or array)
 */
int process_s_mixed_response(CommandResult *result, s_command_args_t *args, zval *return_value)
{
    if (result && result->response && !result->command_error && return_value)
    {
        return command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
    }
    return 0;
}

/**
 * Process string response
 */
int process_s_string_response(CommandResult *result, s_command_args_t *args, zval *return_value)
{
    if (result && result->response && !result->command_error)
    {
        if (result->response->response_type == String)
        {
            if (args->output_string && args->output_string_len)
            {
                size_t len = result->response->string_value_len;
                *args->output_string = (char *)emalloc(len + 1);
                if (*args->output_string)
                {
                    memcpy(*args->output_string, result->response->string_value, len);
                    (*args->output_string)[len] = '\0';
                    *args->output_string_len = len;
                    return 1;
                }
            }
            if (return_value)
            {
                ZVAL_STRINGL(return_value, result->response->string_value, result->response->string_value_len);
                return 1;
            }
        }
    }
    return 0;
}

/**
 * Process scan response (cursor + array)
 */
int process_s_scan_response(CommandResult *result, s_command_args_t *args, zval *return_value)
{
    if (!result || !result->response || result->command_error || !return_value || !args->cursor)
    {
        return 0;
    }

    if (result->response->response_type == Array && result->response->array_value_len >= 2)
    {
        /* Get the new cursor */
        CommandResponse *cursor_resp = &result->response->array_value[0];
        if (cursor_resp->response_type == String)
        {
            *args->cursor = atol(cursor_resp->string_value);
        }

        /* Get the elements array */
        CommandResponse *elements_resp = &result->response->array_value[1];
        if (elements_resp->response_type == Array)
        {
            array_init(return_value);
            for (int i = 0; i < elements_resp->array_value_len; i++)
            {
                CommandResponse *element = &elements_resp->array_value[i];
                if (element->response_type == String)
                {
                    add_next_index_stringl(return_value, element->string_value, element->string_value_len);
                }
            }
            return 1;
        }
    }

    return 0;
}

/* ====================================================================
 * CORE EXECUTION FRAMEWORK
 * ==================================================================== */

/**
 * Generic command execution for S commands
 */
int execute_s_generic_command(const void *glide_client,
                              enum RequestType cmd_type,
                              s_command_category_t category,
                              s_response_type_t response_type,
                              s_command_args_t *args,
                              zval *return_value)
{
    uintptr_t *cmd_args = NULL;
    unsigned long *args_len = NULL;
    int arg_count = 0;
    int status = 0;
    CommandResult *result = NULL;

    /* Validate basic parameters */
    if (!glide_client || !args)
    {
        return 0;
    }

    /* Prepare arguments based on category */
    switch (category)
    {
    case S_CMD_KEY_MEMBERS:
        arg_count = prepare_s_key_members_args(args, &cmd_args, &args_len);
        break;
    case S_CMD_KEY_ONLY:
        arg_count = prepare_s_key_only_args(args, &cmd_args, &args_len);
        break;
    case S_CMD_KEY_MEMBER:
        arg_count = prepare_s_key_member_args(args, &cmd_args, &args_len);
        break;
    case S_CMD_KEY_COUNT:
        arg_count = prepare_s_key_count_args(args, &cmd_args, &args_len);
        break;
    case S_CMD_MULTI_KEY:
        arg_count = prepare_s_multi_key_args(args, &cmd_args, &args_len);
        break;
    case S_CMD_MULTI_KEY_LIMIT:
        arg_count = prepare_s_multi_key_limit_args(args, &cmd_args, &args_len);
        break;
    case S_CMD_DST_MULTI_KEY:
        arg_count = prepare_s_dst_multi_key_args(args, &cmd_args, &args_len);
        break;
    case S_CMD_TWO_KEY_MEMBER:
        arg_count = prepare_s_two_key_member_args(args, &cmd_args, &args_len);
        break;
    case S_CMD_SCAN:
        arg_count = prepare_s_scan_args(args, &cmd_args, &args_len);
        break;
    case S_CMD_SERVER:
        arg_count = prepare_s_server_args(args, &cmd_args, &args_len);
        break;
    default:
        return 0;
    }

    if (arg_count <= 0 || !cmd_args || !args_len)
    {
        goto cleanup;
    }

    /* Execute the command */
    result = execute_command(glide_client, cmd_type, arg_count, cmd_args, args_len);

    /* Process response based on type */
    if (result)
    {
        switch (response_type)
        {
        case S_RESPONSE_INT:
            status = process_s_int_response(result, args, return_value);
            break;
        case S_RESPONSE_BOOL:
            status = process_s_bool_response(result, args, return_value);
            break;
        case S_RESPONSE_SET:
            status = process_s_set_response(result, args, return_value);
            break;
        case S_RESPONSE_MIXED:
            status = process_s_mixed_response(result, args, return_value);
            break;
        case S_RESPONSE_STRING:
            status = process_s_string_response(result, args, return_value);
            break;
        case S_RESPONSE_SCAN:
            status = process_s_scan_response(result, args, return_value);
            break;
        default:
            status = 0;
            break;
        }

        free_command_result(result);
    }

cleanup:
    /* Clean up allocated strings for specific categories */
    if (cmd_args && args_len)
    {
        if (category == S_CMD_KEY_COUNT && args->has_count && arg_count > 1)
        {
            efree((void *)cmd_args[1]); /* Free count string */
        }
        else if (category == S_CMD_MULTI_KEY_LIMIT)
        {
            efree((void *)cmd_args[0]); /* Free numkeys string */
            if (args->has_limit && arg_count > 2 + args->keys_count)
            {
                efree((void *)cmd_args[2 + args->keys_count]); /* Free limit string */
            }
        }
        else if (category == S_CMD_SCAN)
        {
            int has_key = (args->key && args->key_len > 0);
            efree((void *)cmd_args[has_key ? 1 : 0]); /* Free cursor string */
            if (args->has_count)
            {
                /* Find and free count string */
                int count_idx = (has_key ? 1 : 0) + 1 + (args->pattern ? 2 : 0) + 1;
                if (count_idx < arg_count)
                {
                    efree((void *)cmd_args[count_idx]);
                }
            }
        }
    }

    cleanup_s_command_args(cmd_args, args_len);
    return status;
}

/* ====================================================================
 * WRAPPER FUNCTIONS FOR EXISTING COMMANDS
 * ==================================================================== */

/**
 * Execute SADD command using the generic framework
 */
int execute_sadd_command(const void *glide_client, const char *key, size_t key_len,
                         zval *members, int members_count, long *output_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.members = members;
    args.members_count = members_count;
    args.output_long = output_value;

    return execute_s_generic_command(glide_client, SAdd, S_CMD_KEY_MEMBERS, S_RESPONSE_INT, &args, NULL);
}

/**
 * Execute SCARD command using the generic framework
 */
int execute_scard_command(const void *glide_client, const char *key, size_t key_len,
                          long *output_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.output_long = output_value;

    return execute_s_generic_command(glide_client, SCard, S_CMD_KEY_ONLY, S_RESPONSE_INT, &args, NULL);
}

/**
 * Execute SISMEMBER command using the generic framework
 */
int execute_sismember_command(const void *glide_client, const char *key, size_t key_len,
                              const char *member, size_t member_len, int *output_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.member = member;
    args.member_len = member_len;
    args.output_int = output_value;

    return execute_s_generic_command(glide_client, SIsMember, S_CMD_KEY_MEMBER, S_RESPONSE_BOOL, &args, NULL);
}

/**
 * Execute SREM command using the generic framework
 */
int execute_srem_command(const void *glide_client, const char *key, size_t key_len,
                         zval *members, int members_count, long *output_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.members = members;
    args.members_count = members_count;
    args.output_long = output_value;

    return execute_s_generic_command(glide_client, SRem, S_CMD_KEY_MEMBERS, S_RESPONSE_INT, &args, NULL);
}

/**
 * Execute SMOVE command using the generic framework
 */
int execute_smove_command(const void *glide_client, const char *src, size_t src_len,
                          const char *dst, size_t dst_len, const char *member,
                          size_t member_len, int *output_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.src_key = src;
    args.src_key_len = src_len;
    args.dst_key = dst;
    args.dst_key_len = dst_len;
    args.member = member;
    args.member_len = member_len;
    args.output_int = output_value;

    return execute_s_generic_command(glide_client, SMove, S_CMD_TWO_KEY_MEMBER, S_RESPONSE_BOOL, &args, NULL);
}

/**
 * Execute SPOP command using the generic framework
 */
int execute_spop_command(const void *glide_client, const char *key, size_t key_len,
                         long count, zval *return_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.count = count;
    args.has_count = (count > 1);

    return execute_s_generic_command(glide_client, SPop, S_CMD_KEY_COUNT, S_RESPONSE_MIXED, &args, return_value);
}

/**
 * Execute SRANDMEMBER command using the generic framework
 */
int execute_srandmember_command(const void *glide_client, const char *key, size_t key_len,
                                long count, zval *return_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.count = count;
    args.has_count = (count != 1);

    return execute_s_generic_command(glide_client, SRandMember, S_CMD_KEY_COUNT, S_RESPONSE_MIXED, &args, return_value);
}

/**
 * Execute SMEMBERS command using the generic framework
 */
int execute_smembers_command(const void *glide_client, const char *key, size_t key_len,
                             zval *return_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;

    return execute_s_generic_command(glide_client, SMembers, S_CMD_KEY_ONLY, S_RESPONSE_SET, &args, return_value);
}

/**
 * Execute SMISMEMBER command using the generic framework
 */
int execute_smismember_command(const void *glide_client, const char *key, size_t key_len,
                               zval *members, int members_count, zval *return_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.members = members;
    args.members_count = members_count;

    return execute_s_generic_command(glide_client, SMIsMember, S_CMD_KEY_MEMBERS, S_RESPONSE_SET, &args, return_value);
}

/**
 * Execute SINTER command using the generic framework
 */
int execute_sinter_command(const void *glide_client, zval *keys, int keys_count, zval *return_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.keys = keys;
    args.keys_count = keys_count;

    return execute_s_generic_command(glide_client, SInter, S_CMD_MULTI_KEY, S_RESPONSE_SET, &args, return_value);
}

/**
 * Execute SINTERCARD command using the generic framework
 */
int execute_sintercard_command(const void *glide_client, zval *keys, int keys_count, long limit, zval *return_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.keys = keys;
    args.keys_count = keys_count;
    args.limit = limit;
    args.has_limit = (limit > 0);

    return execute_s_generic_command(glide_client, SInterCard, S_CMD_MULTI_KEY_LIMIT, S_RESPONSE_INT, &args, return_value);
}

/**
 * Execute SINTERSTORE command using the generic framework
 */
int execute_sinterstore_command(const void *glide_client, const char *dst, size_t dst_len,
                                zval *keys, int keys_count, long *output_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.dst_key = dst;
    args.dst_key_len = dst_len;
    args.keys = keys;
    args.keys_count = keys_count;
    args.output_long = output_value;

    return execute_s_generic_command(glide_client, SInterStore, S_CMD_DST_MULTI_KEY, S_RESPONSE_INT, &args, NULL);
}

/**
 * Execute SUNION command using the generic framework
 */
int execute_sunion_command(const void *glide_client, zval *keys, int keys_count, zval *return_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.keys = keys;
    args.keys_count = keys_count;

    return execute_s_generic_command(glide_client, SUnion, S_CMD_MULTI_KEY, S_RESPONSE_SET, &args, return_value);
}

/**
 * Execute SUNIONSTORE command using the generic framework
 */
int execute_sunionstore_command(const void *glide_client, const char *dst, size_t dst_len,
                                zval *keys, int keys_count, long *output_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.dst_key = dst;
    args.dst_key_len = dst_len;
    args.keys = keys;
    args.keys_count = keys_count;
    args.output_long = output_value;

    return execute_s_generic_command(glide_client, SUnionStore, S_CMD_DST_MULTI_KEY, S_RESPONSE_INT, &args, NULL);
}

/**
 * Execute SDIFF command using the generic framework
 */
int execute_sdiff_command(const void *glide_client, zval *keys, int keys_count, zval *return_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.keys = keys;
    args.keys_count = keys_count;

    return execute_s_generic_command(glide_client, SDiff, S_CMD_MULTI_KEY, S_RESPONSE_SET, &args, return_value);
}

/**
 * Execute SDIFFSTORE command using the generic framework
 */
int execute_sdiffstore_command(const void *glide_client, const char *dst, size_t dst_len,
                               zval *keys, int keys_count, long *output_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.dst_key = dst;
    args.dst_key_len = dst_len;
    args.keys = keys;
    args.keys_count = keys_count;
    args.output_long = output_value;

    return execute_s_generic_command(glide_client, SDiffStore, S_CMD_DST_MULTI_KEY, S_RESPONSE_INT, &args, NULL);
}

/**
 * Execute SCAN command using the generic framework
 */
int execute_scan_command(const void *glide_client, long *it, const char *pattern, size_t pattern_len,
                         long count, zval *return_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.cursor = it;
    args.pattern = pattern;
    args.pattern_len = pattern_len;
    args.count = count;
    args.has_count = (count > 0);

    return execute_s_generic_command(glide_client, Scan, S_CMD_SCAN, S_RESPONSE_SCAN, &args, return_value);
}

/**
 * Execute SSCAN command using the generic framework
 */
int execute_sscan_command(const void *glide_client, const char *key, size_t key_len,
                          long *it, const char *pattern, size_t pattern_len,
                          long count, zval *return_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.cursor = it;
    args.pattern = pattern;
    args.pattern_len = pattern_len;
    args.count = count;
    args.has_count = (count > 0);

    return execute_s_generic_command(glide_client, SScan, S_CMD_SCAN, S_RESPONSE_SCAN, &args, return_value);
}

/**
 * Execute server name command using the generic framework
 */
int execute_servername_command(const void *glide_client, char **output, size_t *output_len)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.output_string = output;
    args.output_string_len = output_len;

    /* This is a special case that needs custom implementation due to INFO parsing */
    return 0; /* Fallback to original implementation */
}

/**
 * Execute server version command using the generic framework
 */
int execute_serverversion_command(const void *glide_client, char **output, size_t *output_len)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.output_string = output;
    args.output_string_len = output_len;

    /* This is a special case that needs custom implementation due to INFO parsing */
    return 0; /* Fallback to original implementation */
}

/**
 * Execute SADD array command using the generic framework
 */
int execute_sadd_array_command(const void *glide_client, const char *key, size_t key_len,
                               HashTable *members_ht, long *output_value)
{
    s_command_args_t args;
    INIT_S_COMMAND_ARGS(args);

    args.glide_client = glide_client;
    args.key = key;
    args.key_len = key_len;
    args.members_ht = members_ht;
    args.members_count = zend_hash_num_elements(members_ht);
    args.output_long = output_value;

    /* This needs special handling for HashTable conversion */
    return 0; /* Fallback to original implementation */
}
