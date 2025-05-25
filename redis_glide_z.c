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

    int success = 0;
    if (result->response)
    {
        if (result->response->response_type == Null)
        {
            /* Member doesn't exist in the sorted set */
            success = 0;
        }
        else if (result->response->response_type == String)
        {
            /* Parse string as double */
            if (safe_strtod(result->response->string_value, result->response->string_value_len, output_value))
            {
                success = 1;
            }
        }
    }

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

    int success = 0;
    if (result->response)
    {
        if (result->response->response_type == Null)
        {
            /* Member doesn't exist in the sorted set */
            success = 0;
        }
        else if (result->response->response_type == Int)
        {
            /* Integer rank */
            *rank_value = result->response->int_value;
            success = 1;
        }
        else if (result->response->response_type == Array && withscore)
        {
            /* Array with rank and score [rank, score] */
            if (result->response->array_value_len >= 2)
            {
                struct CommandResponse *rank = &result->response->array_value[0];
                struct CommandResponse *score = &result->response->array_value[1];

                if (rank->response_type == Int &&
                    (score->response_type == String || score->response_type == Float))
                {
                    *rank_value = rank->int_value;

                    if (score->response_type == String)
                    {
                        safe_strtod(score->string_value, score->string_value_len, score_value);
                    }
                    else
                    {
                        *score_value = score->float_value;
                    }

                    success = 1;
                }
            }
        }
    }

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

    int success = 0;
    if (result->response)
    {
        if (result->response->response_type == Null)
        {
            /* Member doesn't exist in the sorted set */
            success = 0;
        }
        else if (result->response->response_type == Int)
        {
            /* Integer rank */
            *rank_value = result->response->int_value;
            success = 1;
        }
        else if (result->response->response_type == Array && withscore)
        {
            /* Array with rank and score [rank, score] */
            if (result->response->array_value_len >= 2)
            {
                struct CommandResponse *rank = &result->response->array_value[0];
                struct CommandResponse *score = &result->response->array_value[1];

                if (rank->response_type == Int &&
                    (score->response_type == String || score->response_type == Float))
                {
                    *rank_value = rank->int_value;

                    if (score->response_type == String)
                    {
                        safe_strtod(score->string_value, score->string_value_len, score_value);
                    }
                    else
                    {
                        *score_value = score->float_value;
                    }

                    success = 1;
                }
            }
        }
    }

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

    /* Process the result */
    int success = 0;
    if (result->response)
    {
        if (result->response->response_type == String)
        {
            /* Parse string as double */
            success = safe_strtod(result->response->string_value, result->response->string_value_len, output_value);
        }
        else if (result->response->response_type == Float)
        {
            *output_value = result->response->float_value;
            success = 1;
        }
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
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* key + start + end */
    uintptr_t *args = NULL;
    unsigned long *args_len = NULL;
    char **allocated_strings = NULL;
    int allocated_count = 0;
    int success = 0;

    /* Check if we have options that need to be added */
    int withscores = 0;
    if (options != NULL && Z_TYPE_P(options) == IS_ARRAY)
    {
        zval *z_withscores;
        if ((z_withscores = zend_hash_str_find(Z_ARRVAL_P(options), "WITHSCORES", sizeof("WITHSCORES") - 1)) != NULL &&
            Z_TYPE_P(z_withscores) == IS_TRUE)
        {
            withscores = 1;
            arg_count++; /* Add WITHSCORES parameter */
        }
    }

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

    /* Add WITHSCORES if required */
    if (withscores)
    {
        const char *withscores_str = "WITHSCORES";
        args[3] = (uintptr_t)withscores_str;
        args_len[3] = 10; /* length of "WITHSCORES" */
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

int execute_zcard_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1; /* just key */
    uintptr_t args[1];
    unsigned long args_len[1];

    /* Set arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZCard,     /* command type from RequestType enum */
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
