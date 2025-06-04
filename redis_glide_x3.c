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
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_STREAM_ARRAY_ASSOCIATIVE);
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
                status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE);
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

/* Execute an XGROUP command using the Valkey Glide client */
int execute_xgroup_command(const void *glide_client, const char *subcommand, size_t subcommand_len,
                           zval *args, int args_count, zval *return_value)
{
    /* Check if client and arguments are valid */
    if (!glide_client || !subcommand || subcommand_len <= 0 || !args)
    {
        return 0;
    }

    /* Calculate total args: XGROUP + subcommand + args */
    unsigned long arg_count = 2 + args_count;
    uintptr_t *cmd_args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!cmd_args || !args_len)
    {
        if (cmd_args)
            efree(cmd_args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Set the first argument as "XGROUP" and second as the subcommand */
    const char *xgroup_cmd = "XGROUP";
    cmd_args[0] = (uintptr_t)xgroup_cmd;
    args_len[0] = sizeof("XGROUP") - 1;

    cmd_args[1] = (uintptr_t)subcommand;
    args_len[1] = subcommand_len;

    /* Add all additional arguments */
    int i;
    for (i = 0; i < args_count; i++)
    {
        zval *arg = &args[i];

        /* Convert to string if not already a string */
        if (Z_TYPE_P(arg) != IS_STRING)
        {
            convert_to_string(arg);
        }

        cmd_args[i + 2] = (uintptr_t)Z_STRVAL_P(arg);
        args_len[i + 2] = Z_STRLEN_P(arg);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        CustomCommand, /* XGROUP uses custom command type */
        arg_count,     /* total arguments */
        cmd_args,      /* arguments */
        args_len       /* argument lengths */
    );

    /* Free resources */
    efree(cmd_args);
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
            /* XGROUP response depends on subcommand */
            status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE);
            free_command_result(result);
            return status;
        }

        free_command_result(result);
    }

    return 0;
}

/* Execute an XINFO command using the Valkey Glide client */
int execute_xinfo_command(const void *glide_client, const char *subcommand, size_t subcommand_len,
                          zval *args, int args_count, zval *return_value)
{
    /* Check if client and arguments are valid */
    if (!glide_client || !subcommand || subcommand_len <= 0)
    {
        return 0;
    }

    /* Determine which XINFO command to use based on subcommand */
    enum RequestType command_type;

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

    /* For XINFO STREAM, we need special handling */
    if (strcasecmp(subcommand, "STREAM") == 0)
    {
        /* We need at least the key argument */
        if (!args || args_count < 1 || Z_TYPE(args[0]) == IS_NULL)
        {
            return 0;
        }

        /* Prepare the key argument (always needed) */
        zval key_zval;
        if (Z_TYPE(args[0]) == IS_STRING)
        {
            ZVAL_STRINGL(&key_zval, Z_STRVAL(args[0]), Z_STRLEN(args[0]));
        }
        else
        {
            /* Copy and convert to string */
            ZVAL_COPY(&key_zval, &args[0]);
            convert_to_string(&key_zval);
        }

        /* Check if we have the 'FULL' option (2nd argument) */
        zend_bool has_full = 0;
        if (args_count >= 2 && Z_TYPE(args[1]) != IS_NULL)
        {
            if (Z_TYPE(args[1]) == IS_STRING &&
                strcasecmp(Z_STRVAL(args[1]), "FULL") == 0)
            {
                has_full = 1;
            }
        }

        /* Check if we have a valid count parameter (not -1) */
        zend_bool has_count = 0;
        long count_value = 0;
        if (has_full && args_count >= 3 && Z_TYPE(args[2]) != IS_NULL)
        {
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
        }

        /* Also filter out 4th argument if it's -1 */
        if (!has_count && args_count >= 4 && Z_TYPE(args[3]) != IS_NULL)
        {
            if (Z_TYPE(args[3]) == IS_LONG)
            {
                count_value = Z_LVAL(args[3]);
                if (count_value != -1)
                {
                    has_count = 1;
                }
            }
            else if (Z_TYPE(args[3]) == IS_STRING)
            {
                if (Z_STRLEN(args[3]) != 2 || strcmp(Z_STRVAL(args[3]), "-1") != 0)
                {
                    has_count = 1;
                    count_value = atol(Z_STRVAL(args[3]));
                }
            }
        }

        /* Create arrays for arguments */
        unsigned long arg_count = 1; /* start with the key */
        if (has_full)
            arg_count++;
        if (has_count)
            arg_count += 2; /* Need two slots: one for "COUNT" and one for the count value */

        uintptr_t *cmd_args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
        unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

        if (!cmd_args || !args_len)
        {
            zval_dtor(&key_zval);
            if (cmd_args)
                efree(cmd_args);
            if (args_len)
                efree(args_len);
            return 0;
        }

        /* Add key */
        unsigned long arg_idx = 0;
        cmd_args[arg_idx] = (uintptr_t)Z_STRVAL(key_zval);
        args_len[arg_idx] = Z_STRLEN(key_zval);
        arg_idx++;

        /* Add 'FULL' option if present */
        if (has_full)
        {
            cmd_args[arg_idx] = (uintptr_t)"FULL";
            args_len[arg_idx] = sizeof("FULL") - 1;
            arg_idx++;
        }

        /* Add count if needed */
        if (has_count)
        {
            /* First add the "COUNT" keyword */
            cmd_args[arg_idx] = (uintptr_t)"COUNT";
            args_len[arg_idx] = sizeof("COUNT") - 1;
            arg_idx++;

            /* Then add the count value */
            char count_str[32];
            size_t count_str_len = snprintf(count_str, sizeof(count_str), "%ld", count_value);

            /* Allocate memory for the count string that will survive the function call */
            char *allocated_count_str = emalloc(count_str_len + 1);
            memcpy(allocated_count_str, count_str, count_str_len);
            allocated_count_str[count_str_len] = '\0';

            cmd_args[arg_idx] = (uintptr_t)allocated_count_str;
            args_len[arg_idx] = count_str_len;
            arg_idx++;
        }

        /* Execute the command */
        CommandResult *result = execute_command(
            glide_client,
            command_type,
            arg_count,
            cmd_args,
            args_len);

        /* Free resources */
        zval_dtor(&key_zval);
        if (has_count)
        {
            /* Free the allocated count string */
            efree((void *)cmd_args[arg_idx - 1]);
        }
        efree(cmd_args);
        efree(args_len);

        /* Handle the response */
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
                /* XINFO returns information about the stream */
                status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_ASSOSIATIVE_ARRAY);
                free_command_result(result);
                return status;
            }
            free_command_result(result);
        }

        return 0;
    }
    else
    {
        /* For CONSUMERS and GROUPS, handle non-NULL arguments */
        unsigned long valid_arg_count = 0;

        /* Count valid args (not NULL) */
        if (args && args_count > 0)
        {
            int i;
            for (i = 0; i < args_count; i++)
            {
                if (Z_TYPE(args[i]) != IS_NULL)
                {
                    valid_arg_count++;
                }
            }
        }

        uintptr_t *cmd_args = NULL;
        unsigned long *args_len = NULL;

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
        }

        /* Process and add non-NULL arguments */
        unsigned long arg_idx = 0;

        if (args && args_count > 0)
        {
            int i;
            for (i = 0; i < args_count; i++)
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

                cmd_args[arg_idx] = (uintptr_t)Z_STRVAL(args[i]);
                args_len[arg_idx] = Z_STRLEN(args[i]);
                arg_idx++;
            }
        }

        /* Execute the command */
        CommandResult *result = NULL;
        if (valid_arg_count > 0)
        {
            result = execute_command(
                glide_client,
                command_type,    /* Use specific XINFO command type */
                valid_arg_count, /* total arguments */
                cmd_args,        /* arguments */
                args_len         /* argument lengths */
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

        /* Free resources if allocated */
        if (cmd_args)
            efree(cmd_args);
        if (args_len)
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
                /* XINFO returns information about the stream or consumers */
                status = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_ASSOSIATIVE_ARRAY);
                free_command_result(result);
                return status;
            }
            free_command_result(result);
        }

        return 0;
    }
}
