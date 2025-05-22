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

/*
 * Function to execute the OBJECT command using Glide client
 * Returns:
 *  1 on success
 *  0 when key doesn't exist
 * -1 on error
 */
int execute_object_command(const void *glide_client,
                           const char *subcommand, size_t subcommand_len,
                           const char *key, size_t key_len,
                           zval *return_value)
{
    CommandResult *result = NULL;
    int ret_val = -1; /* Default to error */

    /* Create command array: ["OBJECT", subcommand, key] */
    uintptr_t args[3];
    unsigned long args_len[3];

    args[0] = (uintptr_t)"OBJECT";
    args_len[0] = 6; /* strlen("OBJECT") */

    args[1] = (uintptr_t)subcommand;
    args_len[1] = subcommand_len;

    args[2] = (uintptr_t)key;
    args_len[2] = key_len;

    /* Select appropriate request type based on subcommand */
    enum RequestType req_type = CustomCommand; /* Default to CustomCommand */

    if (strncasecmp(subcommand, "REFCOUNT", subcommand_len) == 0)
    {
        req_type = ObjectRefCount;
    }
    else if (strncasecmp(subcommand, "IDLETIME", subcommand_len) == 0)
    {
        req_type = ObjectIdleTime;
    }
    else if (strncasecmp(subcommand, "FREQ", subcommand_len) == 0)
    {
        req_type = ObjectFreq;
    }
    else if (strncasecmp(subcommand, "ENCODING", subcommand_len) == 0)
    {
        req_type = ObjectEncoding;
    }
    /* For HELP and other subcommands, use CustomCommand (default) */

    /* Execute the command */
    result = execute_command(glide_client, req_type, 3, args, args_len);
    if (result == NULL)
    {
        return -1;
    }

    /* Handle different result types based on the subcommand */
    if (strncasecmp(subcommand, "REFCOUNT", subcommand_len) == 0 ||
        strncasecmp(subcommand, "IDLETIME", subcommand_len) == 0 ||
        strncasecmp(subcommand, "FREQ", subcommand_len) == 0)
    {
        /* These subcommands return integers */
        if (result->response && result->response->response_type == Int)
        {
            /* Success, set return value */
            ZVAL_LONG(return_value, (long)result->response->int_value);
            ret_val = 1;
        }
        else if (result->response && result->response->response_type == Null)
        {
            /* Key doesn't exist */
            ZVAL_NULL(return_value);
            ret_val = 0;
        }
    }
    else if (strncasecmp(subcommand, "ENCODING", subcommand_len) == 0)
    {
        /* ENCODING returns a string */
        if (result->response && result->response->response_type == String)
        {
            /* Success, set return value */
            ZVAL_STRINGL(return_value, result->response->string_value, result->response->string_value_len);
            ret_val = 1;
        }
        else if (result->response && result->response->response_type == Null)
        {
            /* Key doesn't exist */
            ZVAL_NULL(return_value);
            ret_val = 0;
        }
    }
    else if (strncasecmp(subcommand, "HELP", subcommand_len) == 0)
    {
        /* HELP returns an array of strings */
        if (result->response && result->response->response_type == Array)
        {
            if (command_response_to_zval(result->response, return_value) == 1)
            {
                ret_val = 1;
            }
            else
            {
                ret_val = -1;
            }
        }
        else
        {
            ret_val = -1;
        }
    }
    else
    {
        /* Unsupported subcommand */
        ret_val = -1;
    }

    /* Clean up */
    free_command_result(result);

    return ret_val;
}

/* Execute a RENAME command using the Valkey Glide client */
int execute_rename_command(const void *glide_client, const char *src, size_t src_len, const char *dst, size_t dst_len)
{
    /* Check if client and keys are valid */
    if (!glide_client || !src || !dst)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2;
    uintptr_t args[2];
    unsigned long args_len[2];

    /* First argument: source key */
    args[0] = (uintptr_t)src;
    args_len[0] = src_len;

    /* Second argument: destination key */
    args[1] = (uintptr_t)dst;
    args_len[1] = dst_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Rename,    /* command type */
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
        printf("Error executing RENAME command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Success is indicated by an OK response */
    int success = 0;
    if (result->response && result->response->response_type == Ok)
    {
        success = 1;
    }

    /* Free the result */
    free_command_result(result);

    return success;
}

/* Execute a RENAMENX command using the Valkey Glide client */
int execute_renamenx_command(const void *glide_client, const char *src, size_t src_len, const char *dst, size_t dst_len)
{
    /* Check if client and keys are valid */
    if (!glide_client || !src || !dst)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2;
    uintptr_t args[2];
    unsigned long args_len[2];

    /* First argument: source key */
    args[0] = (uintptr_t)src;
    args_len[0] = src_len;

    /* Second argument: destination key */
    args[1] = (uintptr_t)dst;
    args_len[1] = dst_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        RenameNX,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    long output_value = 0;
    int ret = handle_int_response(result, &output_value);
    if (ret == 1)
    {
        return (output_value == 1) ? 1 : 0;
    }
    return 0;
}

/* Execute a GETWITHMETA command using the Valkey Glide client */
int execute_getwithmeta_command(const void *glide_client, const char *key, size_t key_len, char **result, size_t *result_len)
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
        Get,       /* command type - using GET for now, replace with GETWITHMETA when available */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_string_response(cmd_result, result, result_len);
}

/* Execute a GETDEL command using the Valkey Glide client */
int execute_getdel_command(const void *glide_client, const char *key, size_t key_len, char **result, size_t *result_len)
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
        GetDel,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_string_response(cmd_result, result, result_len);
}

/* Execute a GETEX command using the Valkey Glide client */
int execute_getex_command(const void *glide_client, const char *key, size_t key_len, zval *opts, char **result, size_t *result_len)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return -1;
    }

    /* Count the number of arguments */
    unsigned long arg_count = 1; /* Start with the key */
    int has_ex = 0, has_px = 0, has_exat = 0, has_pxat = 0, has_persist = 0;
    long expire = 0;

    /* Check if we have options */
    if (opts && Z_TYPE_P(opts) == IS_ARRAY)
    {
        HashTable *options_ht = Z_ARRVAL_P(opts);
        zval *z_option;
        zend_string *option_key;
        zend_ulong num_key;

        /* Iterate through all options */
        ZEND_HASH_FOREACH_KEY_VAL(options_ht, num_key, option_key, z_option)
        {
            if (option_key)
            {
                /* Handle string keys - these are options with values */
                char *opt = ZSTR_VAL(option_key);

                /* Check for time-based options */
                if (strcasecmp(opt, "EX") == 0)
                {
                    /* EX option - seconds */
                    if (Z_TYPE_P(z_option) == IS_LONG || Z_TYPE_P(z_option) == IS_DOUBLE)
                    {
                        arg_count += 2;
                        has_ex = 1;
                        expire = zval_get_long(z_option);
                        /* Reset other time options */
                        has_px = has_exat = has_pxat = has_persist = 0;
                    }
                }
                else if (strcasecmp(opt, "PX") == 0)
                {
                    /* PX option - milliseconds */
                    if (Z_TYPE_P(z_option) == IS_LONG || Z_TYPE_P(z_option) == IS_DOUBLE)
                    {
                        arg_count += 2;
                        has_px = 1;
                        expire = zval_get_long(z_option);
                        /* Reset other time options */
                        has_ex = has_exat = has_pxat = has_persist = 0;
                    }
                }
                else if (strcasecmp(opt, "EXAT") == 0)
                {
                    /* EXAT option - unix time in seconds */
                    if (Z_TYPE_P(z_option) == IS_LONG || Z_TYPE_P(z_option) == IS_DOUBLE)
                    {
                        arg_count += 2;
                        has_exat = 1;
                        expire = zval_get_long(z_option);
                        /* Reset other time options */
                        has_ex = has_px = has_pxat = has_persist = 0;
                    }
                }
                else if (strcasecmp(opt, "PXAT") == 0)
                {
                    /* PXAT option - unix time in milliseconds */
                    if (Z_TYPE_P(z_option) == IS_LONG || Z_TYPE_P(z_option) == IS_DOUBLE)
                    {
                        arg_count += 2;
                        has_pxat = 1;
                        expire = zval_get_long(z_option);
                        /* Reset other time options */
                        has_ex = has_px = has_exat = has_persist = 0;
                    }
                }
                else if (strcasecmp(opt, "PERSIST") == 0 && zval_is_true(z_option))
                {
                    /* PERSIST option */
                    arg_count += 1;
                    has_persist = 1;
                    /* Reset other time options */
                    has_ex = has_px = has_exat = has_pxat = 0;
                }
            }
            else if (Z_TYPE_P(z_option) == IS_STRING)
            {
                /* Handle numeric keys with string values - could be ['PERSIST'] format */
                if (strcasecmp(Z_STRVAL_P(z_option), "PERSIST") == 0)
                {
                    /* PERSIST option */
                    arg_count += 1;
                    has_persist = 1;
                    /* Reset other time options */
                    has_ex = has_px = has_exat = has_pxat = 0;
                }
            }
        }
        ZEND_HASH_FOREACH_END();
    }

    /* Allocate memory for arguments */
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return -1;
    }

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Current argument index */
    int arg_idx = 1;

    /* Add EX option */
    if (has_ex)
    {
        args[arg_idx] = (uintptr_t)"EX";
        args_len[arg_idx] = 2;
        arg_idx++;

        /* Add expiry time */
        size_t expire_len;
        char *expire_str = long_to_string(expire, &expire_len);
        if (!expire_str)
        {
            free(args);
            free(args_len);
            return -1;
        }
        args[arg_idx] = (uintptr_t)expire_str;
        args_len[arg_idx] = expire_len;
        arg_idx++;
    }
    /* Add PX option */
    else if (has_px)
    {
        args[arg_idx] = (uintptr_t)"PX";
        args_len[arg_idx] = 2;
        arg_idx++;

        /* Add expiry time */
        size_t expire_len;
        char *expire_str = long_to_string(expire, &expire_len);
        if (!expire_str)
        {
            free(args);
            free(args_len);
            return -1;
        }
        args[arg_idx] = (uintptr_t)expire_str;
        args_len[arg_idx] = expire_len;
        arg_idx++;
    }
    /* Add EXAT option */
    else if (has_exat)
    {
        args[arg_idx] = (uintptr_t)"EXAT";
        args_len[arg_idx] = 4;
        arg_idx++;

        /* Add expiry time */
        size_t expire_len;
        char *expire_str = long_to_string(expire, &expire_len);
        if (!expire_str)
        {
            free(args);
            free(args_len);
            return -1;
        }
        args[arg_idx] = (uintptr_t)expire_str;
        args_len[arg_idx] = expire_len;
        arg_idx++;
    }
    /* Add PXAT option */
    else if (has_pxat)
    {
        args[arg_idx] = (uintptr_t)"PXAT";
        args_len[arg_idx] = 4;
        arg_idx++;

        /* Add expiry time */
        size_t expire_len;
        char *expire_str = long_to_string(expire, &expire_len);
        if (!expire_str)
        {
            free(args);
            free(args_len);
            return -1;
        }
        args[arg_idx] = (uintptr_t)expire_str;
        args_len[arg_idx] = expire_len;
        arg_idx++;
    }
    /* Add PERSIST option */
    else if (has_persist)
    {
        args[arg_idx] = (uintptr_t)"PERSIST";
        args_len[arg_idx] = 7;
        arg_idx++;
    }

    /* Execute the command */
    CommandResult *cmd_result = execute_command(
        glide_client,
        GetEx,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the allocated arguments */
    if (has_ex || has_px || has_exat || has_pxat)
    {
        free((void *)args[2]); /* Free the expire string */
    }
    free(args);
    free(args_len);

    /* Use the generic handler to process the result */
    return handle_string_response(cmd_result, result, result_len);
}

/* Execute an INCR command using the Valkey Glide client */
long execute_incr_command(const void *glide_client, const char *key, size_t key_len)
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
        Incr,      /* command type */
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
        printf("Error executing INCR command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Get the result value */
    long ret_val = 0;
    if (result->response && result->response->response_type == Int)
    {
        ret_val = result->response->int_value;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute an INCRBY command using the Valkey Glide client */
long execute_incrby_command(const void *glide_client, const char *key, size_t key_len, long increment)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
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

    /* Second argument: increment */
    size_t incr_len;
    char *incr_str = long_to_string(increment, &incr_len);
    if (!incr_str)
    {
        return 0;
    }
    args[1] = (uintptr_t)incr_str;
    args_len[1] = incr_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        IncrBy,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the increment string */
    free(incr_str);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing INCRBY command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Get the result value */
    long ret_val = 0;
    if (result->response && result->response->response_type == Int)
    {
        ret_val = result->response->int_value;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute an INCRBYFLOAT command using the Valkey Glide client */
int execute_incrbyfloat_command(const void *glide_client, const char *key, size_t key_len, double increment, double *result_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !result_value)
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

    /* Second argument: increment */
    size_t incr_len;
    char *incr_str = double_to_string(increment, &incr_len);
    if (!incr_str)
    {
        return 0;
    }
    args[1] = (uintptr_t)incr_str;
    args_len[1] = incr_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        IncrByFloat, /* command type */
        arg_count,   /* number of arguments */
        args,        /* arguments */
        args_len     /* argument lengths */
    );

    /* Free the increment string */
    free(incr_str);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing INCRBYFLOAT command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Get the result value */
    int ret = 0;
    if (result->response)
    {
        if (result->response->response_type == Float)
        {
            *result_value = result->response->float_value;
            ret = 1;
        }
        else if (result->response->response_type == String)
        {
            /* If the result is a string, convert it to double */
            if (result->response->string_value)
            {
                *result_value = atof(result->response->string_value);
                ret = 1;
            }
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret;
}

/* Execute a DECR command using the Valkey Glide client */
int execute_decr_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !output_value)
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
        Decr,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute a DECRBY command using the Valkey Glide client */
int execute_decrby_command(const void *glide_client, const char *key, size_t key_len, long decrement, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !output_value)
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

    /* Second argument: decrement */
    size_t decr_len;
    char *decr_str = long_to_string(decrement, &decr_len);
    if (!decr_str)
    {
        return 0;
    }
    args[1] = (uintptr_t)decr_str;
    args_len[1] = decr_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        DecrBy,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the decrement string */
    free(decr_str);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute an MGET command using the Valkey Glide client */
int execute_mget_command(const void *glide_client, zval *keys, zval *return_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys || !return_value)
    {
        return 0;
    }

    /* Get the hash table from the keys array */
    HashTable *keys_hash = Z_ARRVAL_P(keys);
    int keys_count = zend_hash_num_elements(keys_hash);

    if (keys_count == 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = keys_count;
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return 0;
    }

    /* Add keys as arguments */
    zval *data;
    zend_string *key;
    zend_ulong idx;
    int i = 0;

    ZEND_HASH_FOREACH_KEY_VAL(keys_hash, idx, key, data)
    {
        if (Z_TYPE_P(data) == IS_STRING)
        {
            /* Use value if it's a string */
            args[i] = (uintptr_t)Z_STRVAL_P(data);
            args_len[i] = Z_STRLEN_P(data);
        }
        else if (Z_TYPE_P(data) == IS_LONG)
        {
            /* Convert integer to string */
            size_t len;
            char *str = long_to_string(Z_LVAL_P(data), &len);
            if (str)
            {
                args[i] = (uintptr_t)str;
                args_len[i] = len;
                /* Flag to free this after command execution */
                i++;
                continue; /* Skip to next iteration */
            }
            else
            {
                /* Failed to convert integer to string */
                int j;
                /* Free any previously allocated strings */
                for (j = 0; j < i; j++)
                {
                    if (args[j])
                        free((void *)args[j]);
                }
                free(args);
                free(args_len);
                return 0;
            }
        }
        else if (Z_TYPE_P(data) == IS_DOUBLE)
        {
            /* Convert double to string */
            size_t len;
            char *str = double_to_string(Z_DVAL_P(data), &len);
            if (str)
            {
                args[i] = (uintptr_t)str;
                args_len[i] = len;
                /* Flag to free this after command execution */
                i++;
                continue; /* Skip to next iteration */
            }
            else
            {
                /* Failed to convert double to string */
                int j;
                /* Free any previously allocated strings */
                for (j = 0; j < i; j++)
                {
                    if (args[j])
                        free((void *)args[j]);
                }
                free(args);
                free(args_len);
                return 0;
            }
        }
        else if (key)
        {
            /* Use hash key if entry is not a string */
            args[i] = (uintptr_t)ZSTR_VAL(key);
            args_len[i] = ZSTR_LEN(key);
        }
        else
        {
            /* Not a valid key */
            free(args);
            free(args_len);
            return 0;
        }
        i++;
    }
    ZEND_HASH_FOREACH_END();

    /* Track which arguments need to be freed after execution */
    char **allocated_strings = (char **)calloc(arg_count, sizeof(char *));
    if (!allocated_strings)
    {
        /* Free previously allocated memory */
        int j;
        for (j = 0; j < i; j++)
        {
            if (Z_TYPE_P(&keys[j]) == IS_LONG || Z_TYPE_P(&keys[j]) == IS_DOUBLE)
            {
                free((void *)args[j]);
            }
        }
        free(args);
        free(args_len);
        return 0;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        MGet,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free any allocated strings */
    i = 0;
    ZEND_HASH_FOREACH_KEY_VAL(keys_hash, idx, key, data)
    {
        if (Z_TYPE_P(data) == IS_LONG || Z_TYPE_P(data) == IS_DOUBLE)
        {
            free((void *)args[i]);
        }
        i++;
    }
    ZEND_HASH_FOREACH_END();
    free(allocated_strings);

    /* Free the argument arrays */
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing MGET command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    int ret_val = 0;
    if (result->response && result->response->response_type == Array)
    {
        /* If existing array has elements, reinitialize it */
        if (Z_TYPE_P(return_value) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(return_value)) > 0)
        {
            zval_dtor(return_value);
            array_init(return_value);
        }

        /* Add array elements to the result */
        i = 0;
        ZEND_HASH_FOREACH_KEY_VAL(keys_hash, idx, key, data)
        {
            if (i < result->response->array_value_len)
            {
                CommandResponse *element = &result->response->array_value[i];
                zval value;

                /* Process each element based on its type */
                if (element->response_type == Null)
                {
                    ZVAL_BOOL(&value, 0); /* Return false for non-existent keys */
                }
                else if (element->response_type == String)
                {
                    ZVAL_STRINGL(&value, element->string_value, element->string_value_len);
                }
                else
                {
                    ZVAL_NULL(&value);
                }

                /* Add the value to the return array */
                if (key)
                {
                    add_assoc_zval_ex(return_value, ZSTR_VAL(key), ZSTR_LEN(key), &value);
                }
                else
                {
                    add_index_zval(return_value, idx, &value);
                }
            }
            i++;
        }
        ZEND_HASH_FOREACH_END();

        ret_val = 1;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute an EXISTS command using the Valkey Glide client */
int execute_exists_command(const void *glide_client, zval *keys, int keys_count, long *output_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys || keys_count <= 0 || !output_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = keys_count;
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return 0;
    }

    /* Add keys as arguments */
    int i;
    for (i = 0; i < keys_count; i++)
    {
        zval *key = &keys[i];
        if (Z_TYPE_P(key) != IS_STRING)
        {
            free(args);
            free(args_len);
            return 0;
        }
        args[i] = (uintptr_t)Z_STRVAL_P(key);
        args_len[i] = Z_STRLEN_P(key);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Exists,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    free(args);
    free(args_len);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute a TOUCH command using the Valkey Glide client */
int execute_touch_command(const void *glide_client, zval *keys, int keys_count, long *output_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys || keys_count <= 0 || !output_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = keys_count;
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return 0;
    }

    /* Add keys as arguments */
    int i;
    for (i = 0; i < keys_count; i++)
    {
        zval *key = &keys[i];
        if (Z_TYPE_P(key) != IS_STRING)
        {
            free(args);
            free(args_len);
            return 0;
        }
        args[i] = (uintptr_t)Z_STRVAL_P(key);
        args_len[i] = Z_STRLEN_P(key);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Touch,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    free(args);
    free(args_len);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute an UNLINK command using the Valkey Glide client */
int execute_unlink_command(const void *glide_client, zval *keys, int keys_count, long *output_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys || keys_count <= 0 || !output_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = keys_count;
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return 0;
    }

    /* Add keys as arguments */
    int i;
    for (i = 0; i < keys_count; i++)
    {
        zval *key = &keys[i];
        if (Z_TYPE_P(key) != IS_STRING)
        {
            free(args);
            free(args_len);
            return 0;
        }
        args[i] = (uintptr_t)Z_STRVAL_P(key);
        args_len[i] = Z_STRLEN_P(key);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Unlink,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    free(args);
    free(args_len);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}
