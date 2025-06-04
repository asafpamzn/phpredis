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

/* Check if a string value exists in an array */
static int array_has_string_value(HashTable *ht, const char *value, size_t value_len)
{
    zval *entry;
    ZEND_HASH_FOREACH_VAL(ht, entry)
    {
        if (Z_TYPE_P(entry) == IS_STRING &&
            Z_STRLEN_P(entry) == value_len &&
            memcmp(Z_STRVAL_P(entry), value, value_len) == 0)
        {
            return 1;
        }
    }
    ZEND_HASH_FOREACH_END();
    return 0;
}
/* Execute an LCS command using the Valkey Glide client */
int execute_lcs_command(const void *glide_client, const char *key1, size_t key1_len, const char *key2, size_t key2_len, zval *options, zval *result)
{
    /* Check if client and keys are valid */
    if (!glide_client || !key1 || !key2)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2; /* key1 + key2 */
    uintptr_t args[7];           /* Maximum 7 arguments: key1, key2, LEN, IDX, MINMATCHLEN, value, WITHMATCHLEN */
    unsigned long args_len[7];

    /* First argument: key1 */
    args[0] = (uintptr_t)key1;
    args_len[0] = key1_len;

    /* Second argument: key2 */
    args[1] = (uintptr_t)key2;
    args_len[1] = key2_len;

    /* Parse options and flags */
    int has_len = 0;
    int has_idx = 0;
    int has_minmatchlen = 0;
    long minmatchlen_value = 0;
    int has_withmatchlen = 0;

    /* Add options if provided */
    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        /* Get option values from associative array */
        zval *z_len = zend_hash_str_find(Z_ARRVAL_P(options), "len", sizeof("len") - 1);
        zval *z_idx = zend_hash_str_find(Z_ARRVAL_P(options), "idx", sizeof("idx") - 1);
        zval *z_minmatchlen = zend_hash_str_find(Z_ARRVAL_P(options), "minmatchlen", sizeof("minmatchlen") - 1);
        zval *z_withmatchlen = zend_hash_str_find(Z_ARRVAL_P(options), "withmatchlen", sizeof("withmatchlen") - 1);
        /* Check associative array values */
        if (z_len && Z_TYPE_P(z_len) == IS_TRUE)
        {

            has_len = 1;
        }

        if (z_idx && Z_TYPE_P(z_idx) == IS_TRUE)
        {

            has_idx = 1;
        }

        if (z_minmatchlen && Z_TYPE_P(z_minmatchlen) == IS_LONG)
        {

            has_minmatchlen = 1;
            minmatchlen_value = Z_LVAL_P(z_minmatchlen);
        }

        if (z_withmatchlen && Z_TYPE_P(z_withmatchlen) == IS_TRUE)
        {

            has_withmatchlen = 1;
        }

        /* ALSO check for string values in indexed array */
        if (!has_len && array_has_string_value(Z_ARRVAL_P(options), "len", 3))
        {
            has_len = 1;
        }

        if (!has_idx && array_has_string_value(Z_ARRVAL_P(options), "idx", 3))
        {
            has_idx = 1;
        }

        if (!has_withmatchlen && array_has_string_value(Z_ARRVAL_P(options), "withmatchlen", 12))
        {
            has_withmatchlen = 1;
        }
    }

    /* Add LEN option if specified */
    if (has_len)
    {
        args[arg_count] = (uintptr_t)"LEN";
        args_len[arg_count] = 3;
        arg_count++;
    }

    /* Add IDX option if specified */
    if (has_idx)
    {
        args[arg_count] = (uintptr_t)"IDX";
        args_len[arg_count] = 3;
        arg_count++;
    }

    /* Add MINMATCHLEN option if specified */
    if (has_minmatchlen)
    {
        args[arg_count] = (uintptr_t)"MINMATCHLEN";
        args_len[arg_count] = 11;
        arg_count++;

        /* Add the minmatchlen value */
        size_t minmatchlen_len;
        char *minmatchlen_str = long_to_string(minmatchlen_value, &minmatchlen_len);
        if (!minmatchlen_str)
        {
            return 0;
        }
        args[arg_count] = (uintptr_t)minmatchlen_str;
        args_len[arg_count] = minmatchlen_len;
        arg_count++;
    }

    /* Add WITHMATCHLEN option if specified */
    if (has_withmatchlen)
    {
        args[arg_count] = (uintptr_t)"WITHMATCHLEN";
        args_len[arg_count] = 12;
        arg_count++;
    }

    /* Execute the command */
    CommandResult *cmd_result = execute_command(
        glide_client,
        LCS,       /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Check if the command was successful */
    if (!cmd_result)
    {
        return 0;
    }

    /* Process the result based on the response type */
    int ret_val = 0;
    if (cmd_result->response)
    {

        /* Force Map handling if IDX option was requested, regardless of the response type */

        {
            switch (cmd_result->response->response_type)
            {
            case String:
                /* If no options were specified, LCS returns the longest common substring as a string */
                command_response_to_zval(cmd_result->response, result, COMMAND_RESPONSE_NOT_ASSOSIATIVE);
                ret_val = 1;
                break;

            case Int:

                /* If LEN option was specified, LCS returns the length as an integer */
                ZVAL_LONG(result, cmd_result->response->int_value);
                ret_val = 1;
                break;

            case Map:
                /* If IDX option was specified, LCS returns a map structure */
                ret_val = handle_map_response(cmd_result, result);
                return ret_val; /* handle_map_response already frees cmd_result */

            default:
                printf("default response\n");
                /* Unsupported response type */
                ret_val = 0;
                break;
            }
        }
    }

    /* Free the result */
    free_command_result(cmd_result);

    return ret_val;
}
