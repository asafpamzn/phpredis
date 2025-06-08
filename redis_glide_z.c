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
#include "valkey_glide_z_common.h"
#include "command_response.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Helper function to safely convert string to double */
static int safe_strtod(const char *str, size_t len, double *output)
{
    if (!str || !output || len == 0)
    {
        return 0;
    }

    /* Create a null-terminated copy of the string */
    char *temp = emalloc(len + 1);
    if (!temp)
    {
        return 0;
    }
    memcpy(temp, str, len);
    temp[len] = '\0';

    /* Convert to double */
    char *endptr;
    *output = strtod(temp, &endptr);

    /* Check if conversion was successful */
    int success = (*endptr == '\0' || endptr == temp + len);
    efree(temp);

    return success;
}

int execute_zrandmember_command(const void *glide_client, const char *key, size_t key_len, long count, int withscores, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1; /* Start with key */
    if (count != 0)
    {
        arg_count++; /* Add count parameter */
    }
    if (withscores)
    {
        arg_count++; /* Add WITHSCORES parameter */
    }

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

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add count parameter if not default */
    int arg_idx = 1;
    char count_str[32] = {0};
    if (count != 0)
    {
        int count_str_len = snprintf(count_str, sizeof(count_str), "%ld", count);
        args[arg_idx] = (uintptr_t)count_str;
        args_len[arg_idx] = count_str_len;
        arg_idx++;
    }

    /* Add WITHSCORES if required */
    if (withscores)
    {
        const char *withscores_str = "WITHSCORES";
        args[arg_idx] = (uintptr_t)withscores_str;
        args_len[arg_idx] = 10; /* length of "WITHSCORES" */
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZRandMember, /* command type from RequestType enum */
        arg_count,   /* number of arguments */
        args,        /* arguments array */
        args_len     /* argument lengths array */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    int success = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE);
    if (withscores && success && Z_TYPE_P(return_value) == IS_ARRAY)
    {
        /* Use common helper to flatten withscores array */
        flatten_withscores_array(return_value);
    }

    /* Free the result */
    free_command_result(result);

    return success;
}

int execute_zscore_command(const void *glide_client, const char *key, size_t key_len,
                           const char *member, size_t member_len, double *output_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || !member)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2; /* key + member */
    uintptr_t args[2];
    unsigned long args_len[2];

    /* Set arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    args[1] = (uintptr_t)member;
    args_len[1] = member_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZScore,    /* command type from RequestType enum */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        return -1;
    }

    /* Use common helper to handle score response */
    int success = handle_score_response(result, output_value);

    /* Free the result */
    free_command_result(result);

    return success;
}

int execute_zmscore_command(const void *glide_client, const char *key, size_t key_len,
                            zval *members, int members_count, zval *return_value)
{
    /* Check if client, key, and members are valid */
    if (!glide_client || !key || !members || members_count <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1 + members_count; /* key + members */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));
    char **allocated_strings = (char **)emalloc(members_count * sizeof(char *));

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

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add members as arguments */
    int i;
    int allocated_count = 0;
    for (i = 0; i < members_count; i++)
    {
        zval *z_member = &members[i];

        if (Z_TYPE_P(z_member) == IS_STRING)
        {
            args[i + 1] = (uintptr_t)Z_STRVAL_P(z_member);
            args_len[i + 1] = Z_STRLEN_P(z_member);
        }
        else
        {
            /* Convert non-string values to string */
            char *str_val = NULL;
            size_t str_len = 0;

            if (Z_TYPE_P(z_member) == IS_LONG)
            {
                str_val = long_to_string(Z_LVAL_P(z_member), &str_len);
            }
            else if (Z_TYPE_P(z_member) == IS_DOUBLE)
            {
                str_val = double_to_string(Z_DVAL_P(z_member), &str_len);
            }
            else if (Z_TYPE_P(z_member) == IS_TRUE)
            {
                str_val = estrdup("1");
                str_len = 1;
            }
            else if (Z_TYPE_P(z_member) == IS_FALSE)
            {
                str_val = estrdup("0");
                str_len = 1;
            }
            else
            {
                /* Handle other types or error */
                int j;
                for (j = 0; j < allocated_count; j++)
                {
                    efree(allocated_strings[j]);
                }
                efree(allocated_strings);
                efree(args);
                efree(args_len);
                return 0;
            }

            if (str_val)
            {
                args[i + 1] = (uintptr_t)str_val;
                args_len[i + 1] = str_len;
                allocated_strings[allocated_count++] = str_val;
            }
            else
            {
                int j;
                for (j = 0; j < allocated_count; j++)
                {
                    efree(allocated_strings[j]);
                }
                efree(allocated_strings);
                efree(args);
                efree(args_len);
                return 0;
            }
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZMScore,   /* command type from RequestType enum */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated strings */
    for (i = 0; i < allocated_count; i++)
    {
        efree(allocated_strings[i]);
    }
    efree(allocated_strings);
    efree(args);
    efree(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    int success = 0;
    if (result->response && result->response->response_type == Array)
    {
        /* Convert array response to PHP array */
        size_t i;
        for (i = 0; i < result->response->array_value_len; i++)
        {
            struct CommandResponse *element = &result->response->array_value[i];
            if (element->response_type == String)
            {
                add_next_index_stringl(return_value, element->string_value, element->string_value_len);
            }
            else if (element->response_type == Null)
            {
                add_next_index_null(return_value);
            }
        }
        success = 1;
    }

    /* Free the result */
    free_command_result(result);

    return success;
}

int execute_zrank_command(const void *glide_client, const char *key, size_t key_len,
                          const char *member, size_t member_len, int withscore,
                          long *rank_value, double *score_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || !member)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = withscore ? 3 : 2; /* key + member + optional WITHSCORE */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return -1;
    }

    /* Set arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    args[1] = (uintptr_t)member;
    args_len[1] = member_len;

    /* Add WITHSCORE if required */
    if (withscore)
    {
        const char *withscore_str = "WITHSCORE";
        args[2] = (uintptr_t)withscore_str;
        args_len[2] = 9; /* length of "WITHSCORE" */
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZRank,     /* command type from RequestType enum */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        return -1;
    }

    /* Use common helper to handle rank response */
    int success = handle_rank_response(result, rank_value, score_value, withscore);

    /* Free the result */
    free_command_result(result);

    return success;
}

int execute_zrevrank_command(const void *glide_client, const char *key, size_t key_len,
                             const char *member, size_t member_len, int withscore,
                             long *rank_value, double *score_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || !member)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = withscore ? 3 : 2; /* key + member + optional WITHSCORE */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return -1;
    }

    /* Set arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    args[1] = (uintptr_t)member;
    args_len[1] = member_len;

    /* Add WITHSCORE if required */
    if (withscore)
    {
        const char *withscore_str = "WITHSCORE";
        args[2] = (uintptr_t)withscore_str;
        args_len[2] = 9; /* length of "WITHSCORE" */
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZRevRank,  /* command type from RequestType enum */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        return -1;
    }

    /* Use common helper to handle rank response */
    int success = handle_rank_response(result, rank_value, score_value, withscore);

    /* Free the result */
    free_command_result(result);

    return success;
}

int execute_zincrby_command(const void *glide_client, const char *key, size_t key_len,
                            double increment, const char *member, size_t member_len,
                            double *output_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || !member)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* key + increment + member */
    uintptr_t args[3];
    unsigned long args_len[3];

    /* Set arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add increment parameter */
    char increment_str[64];
    int increment_str_len = snprintf(increment_str, sizeof(increment_str), "%.17g", increment);
    args[1] = (uintptr_t)increment_str;
    args_len[1] = increment_str_len;

    /* Add member parameter */
    args[2] = (uintptr_t)member;
    args_len[2] = member_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZIncrBy,   /* command type from RequestType enum */
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
        free_command_result(result);
        return 0;
    }

    /* Use common helper to handle score response */
    int success = handle_score_response(result, output_value);

    /* Convert result to expected format for ZINCRBY (0/1 instead of -1/0/1) */
    if (success == -1)
    {
        success = 0;
    }

    /* Free the result */
    free_command_result(result);

    return success;
}

int execute_zcount_command(const void *glide_client, const char *key, size_t key_len,
                           const char *min, size_t min_len, const char *max, size_t max_len,
                           long *output_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || !min || !max)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* key + min + max */
    uintptr_t args[3];
    unsigned long args_len[3];

    /* Set arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    args[1] = (uintptr_t)min;
    args_len[1] = min_len;

    args[2] = (uintptr_t)max;
    args_len[2] = max_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZCount,    /* command type from RequestType enum */
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
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    int success = 0;
    if (result->response && result->response->response_type == Int)
    {
        *output_value = result->response->int_value;
        success = 1;
    }

    /* Free the result */
    free_command_result(result);

    return success;
}

int execute_zlexcount_command(const void *glide_client, const char *key, size_t key_len,
                              const char *min, size_t min_len, const char *max, size_t max_len,
                              long *output_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || !min || !max)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* key + min + max */
    uintptr_t args[3];
    unsigned long args_len[3];

    /* Set arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    args[1] = (uintptr_t)min;
    args_len[1] = min_len;

    args[2] = (uintptr_t)max;
    args_len[2] = max_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZLexCount, /* command type from RequestType enum */
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
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    int success = 0;
    if (result->response && result->response->response_type == Int)
    {
        *output_value = result->response->int_value;
        success = 1;
    }

    /* Free the result */
    free_command_result(result);

    return success;
}

int execute_zrem_command(const void *glide_client, const char *key, size_t key_len,
                         zval *members, int members_count, long *output_value)
{
    /* Check if client, key, and members are valid */
    if (!glide_client || !key || !members || members_count <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1 + members_count; /* key + members */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));
    char **allocated_strings = (char **)emalloc(members_count * sizeof(char *));

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

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add members as arguments */
    int i;
    int allocated_count = 0;
    for (i = 0; i < members_count; i++)
    {
        zval *z_member = &members[i];

        if (Z_TYPE_P(z_member) == IS_STRING)
        {
            args[i + 1] = (uintptr_t)Z_STRVAL_P(z_member);
            args_len[i + 1] = Z_STRLEN_P(z_member);
        }
        else
        {
            /* Convert non-string values to string */
            char *str_val = NULL;
            size_t str_len = 0;

            if (Z_TYPE_P(z_member) == IS_LONG)
            {
                str_val = long_to_string(Z_LVAL_P(z_member), &str_len);
            }
            else if (Z_TYPE_P(z_member) == IS_DOUBLE)
            {
                str_val = double_to_string(Z_DVAL_P(z_member), &str_len);
            }
            else if (Z_TYPE_P(z_member) == IS_TRUE)
            {
                str_val = estrdup("1");
                str_len = 1;
            }
            else if (Z_TYPE_P(z_member) == IS_FALSE)
            {
                str_val = estrdup("0");
                str_len = 1;
            }
            else
            {
                /* Handle other types or error */
                int j;
                for (j = 0; j < allocated_count; j++)
                {
                    efree(allocated_strings[j]);
                }
                efree(allocated_strings);
                efree(args);
                efree(args_len);
                return 0;
            }

            if (str_val)
            {
                args[i + 1] = (uintptr_t)str_val;
                args_len[i + 1] = str_len;
                allocated_strings[allocated_count++] = str_val;
            }
            else
            {
                int j;
                for (j = 0; j < allocated_count; j++)
                {
                    efree(allocated_strings[j]);
                }
                efree(allocated_strings);
                efree(args);
                efree(args_len);
                return 0;
            }
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZRem,      /* command type from RequestType enum */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated strings */
    for (i = 0; i < allocated_count; i++)
    {
        efree(allocated_strings[i]);
    }
    efree(allocated_strings);
    efree(args);
    efree(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    int success = 0;
    if (result->response && result->response->response_type == Int)
    {
        *output_value = result->response->int_value;
        success = 1;
    }

    /* Free the result */
    free_command_result(result);

    return success;
}

int execute_zremrangebylex_command(const void *glide_client, const char *key, size_t key_len,
                                   const char *min, size_t min_len, const char *max, size_t max_len,
                                   long *output_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || !min || !max)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* key + min + max */
    uintptr_t args[3];
    unsigned long args_len[3];

    /* Set arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    args[1] = (uintptr_t)min;
    args_len[1] = min_len;

    args[2] = (uintptr_t)max;
    args_len[2] = max_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZRemRangeByLex, /* command type from RequestType enum */
        arg_count,      /* number of arguments */
        args,           /* arguments */
        args_len        /* argument lengths */
    );

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    int success = 0;
    if (result->response && result->response->response_type == Int)
    {
        *output_value = result->response->int_value;
        success = 1;
    }

    /* Free the result */
    free_command_result(result);

    return success;
}

int execute_zremrangebyrank_command(const void *glide_client, const char *key, size_t key_len,
                                    long start, long end, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* key + start + end */
    uintptr_t args[3];
    unsigned long args_len[3];

    /* Set arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add start and end parameters */
    char start_str[32], end_str[32];
    int start_str_len = snprintf(start_str, sizeof(start_str), "%ld", start);
    int end_str_len = snprintf(end_str, sizeof(end_str), "%ld", end);

    args[1] = (uintptr_t)start_str;
    args_len[1] = start_str_len;

    args[2] = (uintptr_t)end_str;
    args_len[2] = end_str_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZRemRangeByRank, /* command type from RequestType enum */
        arg_count,       /* number of arguments */
        args,            /* arguments */
        args_len         /* argument lengths */
    );

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    int success = 0;
    if (result->response && result->response->response_type == Int)
    {
        *output_value = result->response->int_value;
        success = 1;
    }

    /* Free the result */
    free_command_result(result);

    return success;
}

int execute_zremrangebyscore_command(const void *glide_client, const char *key, size_t key_len,
                                     const char *min, size_t min_len, const char *max, size_t max_len,
                                     long *output_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || !min || !max)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* key + min + max */
    uintptr_t args[3];
    unsigned long args_len[3];

    /* Set arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    args[1] = (uintptr_t)min;
    args_len[1] = min_len;

    args[2] = (uintptr_t)max;
    args_len[2] = max_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZRemRangeByScore, /* command type from RequestType enum */
        arg_count,        /* number of arguments */
        args,             /* arguments */
        args_len          /* argument lengths */
    );

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    int success = 0;
    if (result->response && result->response->response_type == Int)
    {
        *output_value = result->response->int_value;
        success = 1;
    }

    /* Free the result */
    free_command_result(result);

    return success;
}

int execute_zrange_command(const void *glide_client, const char *key, size_t key_len,
                           zval *z_start, zval *z_end, zval *options, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        printf("Invalid glide client or key for ZRANGE command.\n");
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* key + start + end */
    uintptr_t *args = NULL;
    unsigned long *args_len = NULL;
    char **allocated_strings = NULL;
    int allocated_count = 0;
    int success = 0;

    /* Parse range options using common helper */
    range_options_t range_opts = {0};
    if (!parse_range_options(options, &range_opts))
    {
        return 0;
    }

    /* Calculate argument count based on options */
    if (range_opts.withscores)
        arg_count++; /* Add WITHSCORES parameter */
    if (range_opts.byscore)
        arg_count++; /* Add BYSCORE parameter */
    if (range_opts.bylex)
        arg_count++; /* Add BYLEX parameter */
    if (range_opts.rev)
        arg_count++; /* Add REV parameter */
    if (range_opts.has_limit)
        arg_count += 3; /* Add LIMIT + offset + count parameters */
    /* Allocate memory for arguments */
    args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));
    allocated_strings = (char **)emalloc((arg_count - 1) * sizeof(char *)); /* For potential string conversions */

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

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Convert start to string if needed */
    if (Z_TYPE_P(z_start) == IS_STRING)
    {
        args[1] = (uintptr_t)Z_STRVAL_P(z_start);
        args_len[1] = Z_STRLEN_P(z_start);
    }
    else
    {
        char *str_val = NULL;
        size_t str_len = 0;

        if (Z_TYPE_P(z_start) == IS_LONG)
        {
            str_val = long_to_string(Z_LVAL_P(z_start), &str_len);
        }
        else if (Z_TYPE_P(z_start) == IS_DOUBLE)
        {
            str_val = double_to_string(Z_DVAL_P(z_start), &str_len);
        }

        if (str_val)
        {
            args[1] = (uintptr_t)str_val;
            args_len[1] = str_len;
            allocated_strings[allocated_count++] = str_val;
        }
        else
        {
            /* Error handling */
            int i;
            for (i = 0; i < allocated_count; i++)
            {
                efree(allocated_strings[i]);
            }
            efree(allocated_strings);
            efree(args);
            efree(args_len);

            return 0;
        }
    }
    /* Convert end to string if needed */
    if (Z_TYPE_P(z_end) == IS_STRING)
    {
        args[2] = (uintptr_t)Z_STRVAL_P(z_end);
        args_len[2] = Z_STRLEN_P(z_end);
    }
    else
    {
        char *str_val = NULL;
        size_t str_len = 0;

        if (Z_TYPE_P(z_end) == IS_LONG)
        {
            str_val = long_to_string(Z_LVAL_P(z_end), &str_len);
        }
        else if (Z_TYPE_P(z_end) == IS_DOUBLE)
        {
            str_val = double_to_string(Z_DVAL_P(z_end), &str_len);
        }

        if (str_val)
        {
            args[2] = (uintptr_t)str_val;
            args_len[2] = str_len;
            allocated_strings[allocated_count++] = str_val;
        }
        else
        {
            /* Error handling */
            int i;
            for (i = 0; i < allocated_count; i++)
            {
                efree(allocated_strings[i]);
            }
            efree(allocated_strings);
            efree(args);
            efree(args_len);

            return 0;
        }
    }
    /* Add optional parameters in the correct order */
    int arg_idx = 3; /* Start after key, start, end */

    /* Add BYSCORE parameter if required */
    if (range_opts.byscore)
    {
        const char *byscore_str = "BYSCORE";
        args[arg_idx] = (uintptr_t)byscore_str;
        args_len[arg_idx] = 7; /* length of "BYSCORE" */
        arg_idx++;
    }

    /* Add BYLEX parameter if required */
    if (range_opts.bylex)
    {
        const char *bylex_str = "BYLEX";
        args[arg_idx] = (uintptr_t)bylex_str;
        args_len[arg_idx] = 5; /* length of "BYLEX" */
        arg_idx++;
    }

    /* Add REV parameter if required */
    if (range_opts.rev)
    {
        const char *rev_str = "REV";
        args[arg_idx] = (uintptr_t)rev_str;
        args_len[arg_idx] = 3; /* length of "REV" */
        arg_idx++;
    }

    /* Add LIMIT parameter if required */
    if (range_opts.has_limit)
    {
        /* Add LIMIT keyword */
        const char *limit_str = "LIMIT";
        args[arg_idx] = (uintptr_t)limit_str;
        args_len[arg_idx] = 5; /* length of "LIMIT" */
        arg_idx++;

        /* Add offset parameter */
        char offset_str[32];
        int offset_str_len = snprintf(offset_str, sizeof(offset_str), "%ld", range_opts.limit_offset);
        char *offset_str_copy = estrndup(offset_str, offset_str_len);
        if (!offset_str_copy)
        {
            /* Error handling */
            int i;
            for (i = 0; i < allocated_count; i++)
            {
                efree(allocated_strings[i]);
            }
            efree(allocated_strings);
            efree(args);
            efree(args_len);

            return 0;
        }
        args[arg_idx] = (uintptr_t)offset_str_copy;
        args_len[arg_idx] = offset_str_len;
        allocated_strings[allocated_count++] = offset_str_copy;
        arg_idx++;

        /* Add count parameter */
        char count_str[32];
        int count_str_len = snprintf(count_str, sizeof(count_str), "%ld", range_opts.limit_count);
        char *count_str_copy = estrndup(count_str, count_str_len);
        if (!count_str_copy)
        {
            /* Error handling */
            int i;
            for (i = 0; i < allocated_count; i++)
            {
                efree(allocated_strings[i]);
            }
            efree(allocated_strings);
            efree(args);
            efree(args_len);

            return 0;
        }
        args[arg_idx] = (uintptr_t)count_str_copy;
        args_len[arg_idx] = count_str_len;
        allocated_strings[allocated_count++] = count_str_copy;
        arg_idx++;
    }

    /* Add WITHSCORES if required - add it last as per Redis command syntax */
    if (range_opts.withscores)
    {
        const char *withscores_str = "WITHSCORES";
        args[arg_idx] = (uintptr_t)withscores_str;
        args_len[arg_idx] = 10; /* length of "WITHSCORES" */
        arg_idx++;
    }

    /* Execute the command */

    CommandResult *result = execute_command(
        glide_client,
        ZRange,    /* command type from RequestType enum */
        arg_count, /* number of arguments */
        args,      /* arguments array */
        args_len   /* argument lengths array */
    );
    /* Free allocated strings */
    int i;
    for (i = 0; i < allocated_count; i++)
    {
        efree(allocated_strings[i]);
    }
    efree(allocated_strings);
    efree(args);
    efree(args_len);

    /* Check if the command was successful */
    if (!result)
    {

        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);

        return 0;
    }
    /* Process the result */
    success = command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_ASSOSIATIVE_ARRAY);

    free_command_result(result);

    return success;
}

int execute_zcard_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;

    return execute_z_generic_command(
        glide_client,
        ZCard,
        &args,
        output_value,
        process_z_int_result);
}
