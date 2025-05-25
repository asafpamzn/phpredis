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

/* Execute a ZADD command using the Valkey Glide client */
int execute_zadd_command(const void *glide_client, const char *key, size_t key_len, zval *z_args, int argc, int flags, long *output_value)
{
    /* Check if client, key, and args are valid */
    if (!glide_client || !key || !z_args || argc < 2)
    {
        return 0;
    }

    /* Check for options array as first argument */
    int options_start = 0;
    int has_xx = 0, has_nx = 0, has_lt = 0, has_gt = 0, has_ch = 0, has_incr = 0;
    int first_score_idx = 0;

    if (Z_TYPE(z_args[0]) == IS_ARRAY)
    {
        /* First argument is an options array */
        HashTable *options_ht = Z_ARRVAL(z_args[0]);
        zval *z_option;
        zend_string *option_key;
        zend_ulong num_key;

        /* Process each option */
        ZEND_HASH_FOREACH_KEY_VAL(options_ht, num_key, option_key, z_option)
        {
            if (Z_TYPE_P(z_option) == IS_STRING)
            {
                char *opt_str = Z_STRVAL_P(z_option);

                if (strcasecmp(opt_str, "XX") == 0)
                {
                    has_xx = 1;
                }
                else if (strcasecmp(opt_str, "NX") == 0)
                {
                    has_nx = 1;
                }
                else if (strcasecmp(opt_str, "LT") == 0)
                {
                    has_lt = 1;
                }
                else if (strcasecmp(opt_str, "GT") == 0)
                {
                    has_gt = 1;
                }
                else if (strcasecmp(opt_str, "CH") == 0)
                {
                    has_ch = 1;
                }
                else if (strcasecmp(opt_str, "INCR") == 0)
                {
                    has_incr = 1;
                }
            }
            else if (option_key != NULL)
            {
                /* Handle associative array options */
                char *opt_key = ZSTR_VAL(option_key);

                if (strcasecmp(opt_key, "XX") == 0 && Z_TYPE_P(z_option) == IS_TRUE)
                {
                    has_xx = 1;
                }
                else if (strcasecmp(opt_key, "NX") == 0 && Z_TYPE_P(z_option) == IS_TRUE)
                {
                    has_nx = 1;
                }
                else if (strcasecmp(opt_key, "LT") == 0 && Z_TYPE_P(z_option) == IS_TRUE)
                {
                    has_lt = 1;
                }
                else if (strcasecmp(opt_key, "GT") == 0 && Z_TYPE_P(z_option) == IS_TRUE)
                {
                    has_gt = 1;
                }
                else if (strcasecmp(opt_key, "CH") == 0 && Z_TYPE_P(z_option) == IS_TRUE)
                {
                    has_ch = 1;
                }
                else if (strcasecmp(opt_key, "INCR") == 0 && Z_TYPE_P(z_option) == IS_TRUE)
                {
                    has_incr = 1;
                }
            }
        }
        ZEND_HASH_FOREACH_END();

        /* Skip the options array */
        first_score_idx = 1;
    }

    /* Verify we have at least one score-member pair */
    if ((argc - first_score_idx) < 2 || (argc - first_score_idx) % 2 != 0)
    {
        return 0;
    }

    /* When INCR option is used, we can only have one score-member pair */
    if (has_incr && (argc - first_score_idx) > 2)
    {
        return 0;
    }

    /* Calculate the number of options to add */
    int num_options = has_xx + has_nx + has_lt + has_gt + has_ch + has_incr;

    /* Calculate actual pairs after options */
    int score_member_pairs = (argc - first_score_idx) / 2;

    /* Calculate total number of arguments */
    unsigned long arg_count = 1 + num_options + (score_member_pairs * 2); /* key + options + (score, member) pairs */

    /* Allocate memory for arguments */
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

    /* Current argument index */
    int arg_idx = 1;

    /* Add options as arguments */
    if (has_xx)
    {
        args[arg_idx] = (uintptr_t)"XX";
        args_len[arg_idx] = 2;
        arg_idx++;
    }

    if (has_nx)
    {
        args[arg_idx] = (uintptr_t)"NX";
        args_len[arg_idx] = 2;
        arg_idx++;
    }

    if (has_lt)
    {
        args[arg_idx] = (uintptr_t)"LT";
        args_len[arg_idx] = 2;
        arg_idx++;
    }

    if (has_gt)
    {
        args[arg_idx] = (uintptr_t)"GT";
        args_len[arg_idx] = 2;
        arg_idx++;
    }

    if (has_ch)
    {
        args[arg_idx] = (uintptr_t)"CH";
        args_len[arg_idx] = 2;
        arg_idx++;
    }

    if (has_incr)
    {
        args[arg_idx] = (uintptr_t)"INCR";
        args_len[arg_idx] = 4;
        arg_idx++;
    }

    /* Add score/member pairs */
    int i;
    size_t score_len;
    char *score_str = NULL;
    char **allocated_scores = (char **)emalloc(score_member_pairs * sizeof(char *));
    int allocated_count = 0;

    if (!allocated_scores)
    {
        efree(args);
        efree(args_len);
        return 0;
    }

    for (i = first_score_idx; i < argc; i += 2)
    {
        /* Score */
        zval *score = &z_args[i];
        if (Z_TYPE_P(score) == IS_DOUBLE)
        {
            score_str = double_to_string(Z_DVAL_P(score), &score_len);
            allocated_scores[allocated_count++] = score_str;
        }
        else if (Z_TYPE_P(score) == IS_LONG)
        {
            score_str = long_to_string(Z_LVAL_P(score), &score_len);
            allocated_scores[allocated_count++] = score_str;
        }
        else if (Z_TYPE_P(score) == IS_STRING)
        {
            score_str = Z_STRVAL_P(score);
            score_len = Z_STRLEN_P(score);
        }
        else
        {
            /* Unsupported type for score */
            for (int j = 0; j < allocated_count; j++)
            {
                efree(allocated_scores[j]);
            }
            efree(allocated_scores);
            efree(args);
            efree(args_len);
            return 0;
        }

        args[arg_idx] = (uintptr_t)score_str;
        args_len[arg_idx] = score_len;
        arg_idx++;

        /* Member */
        zval *member = &z_args[i + 1];
        if (Z_TYPE_P(member) != IS_STRING)
        {
            /* Free allocated score strings */
            for (int j = 0; j < allocated_count; j++)
            {
                efree(allocated_scores[j]);
            }
            efree(allocated_scores);
            efree(args);
            efree(args_len);
            return 0;
        }
        args[arg_idx] = (uintptr_t)Z_STRVAL_P(member);
        args_len[arg_idx] = Z_STRLEN_P(member);
        arg_idx++;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZAdd,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free allocated score strings */
    for (int j = 0; j < allocated_count; j++)
    {
        efree(allocated_scores[j]);
    }
    efree(allocated_scores);
    efree(args);
    efree(args_len);

    /* Process the result */
    int success = 0;

    /* Check if the command was successful */
    if (!result)
    {
        return 0; /* False - failure */
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        free_command_result(result);
        return 0; /* False - failure */
    }

    /* Process the result based on type */
    if (result->response)
    {
        if (has_incr)
        {
            /* For INCR option, we get back a float score or NULL */
            if (result->response->response_type == Float)
            {
                *output_value = (long)(result->response->float_value * 10); /* Store as long with one decimal place */
                success = 1;
            }
            else if (result->response->response_type == String)
            {
                /* Convert string to double */
                double score_val = 0.0;
                char *str_end;
                if (result->response->string_value && result->response->string_value_len > 0)
                {
                    score_val = strtod(result->response->string_value, &str_end);
                    *output_value = (long)(score_val * 10);
                    success = 1;
                }
            }
            else if (result->response->response_type == Null)
            {
                /* Member or key doesn't exist with XX/NX options */
                success = 0;
            }
        }
        else
        {
            /* For regular ZADD, we get an integer count */
            if (result->response->response_type == Int)
            {
                *output_value = result->response->int_value;
                success = 1;
            }
        }
    }

    /* Free the result */
    free_command_result(result);

    /* For ZADD, we need to return success if the command executed correctly,
     * regardless of whether elements were added or not. The actual count or score
     * is stored in output_value.
     */
    return success;
}
