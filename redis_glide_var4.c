/*
  +----------------------------------------------------------------------+
  | ValkeyGlide Glide FFI integration for phpredis                             |
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

#include "valkey_glide_commands_common.h"
#include "command_response.h"
#include "valkey_glide_core_common.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern zend_class_entry *get_valkey_glide_ce();
extern zend_class_entry *get_valkey_glide_exception_ce();

/* Execute a PFADD command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */

/* Execute getTimeout command using the Valkey Glide client */
int execute_get_timeout_command(const void *glide_client, double *output_value)
{
    /* Check if client is valid */
    if (!glide_client || !output_value)
    {
        return 0;
    }

    /* Since this is a client configuration getter rather than a ValkeyGlide command,
       we'll use a default value as this isn't directly supported by Glide */
    *output_value = 0.0; /* Default timeout */

    /* Here we'd ideally access the Glide client's configuration, but since
       we don't have direct access to it through the FFI interface, we just
       return success and the default value */

    return 1;
}

/* Unified COPY command implementation */
int execute_copy_command(zval *object, int argc, zval *return_value)
{
    valkey_glide_object *valkey_glide;
    char *src = NULL, *dst = NULL;
    size_t src_len, dst_len;
    zend_bool replace = 0;
    zval *z_opts = NULL;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Oss|a",
                                     &object, get_valkey_glide_ce(), &src, &src_len,
                                     &dst, &dst_len, &z_opts) == FAILURE)
    {
        return 0;
    }

    /* Get ValkeyGlide object */
    valkey_glide = VALKEY_GLIDE_PHP_ZVAL_GET_OBJECT(valkey_glide_object, object);
    if (!valkey_glide || !valkey_glide->glide_client)
    {
        return 0;
    }

    /* Check for the REPLACE option if options array was passed */
    if (z_opts && Z_TYPE_P(z_opts) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(z_opts);
        zval *replace_val;
        replace_val = zend_hash_str_find(ht, "replace", sizeof("replace") - 1);
        if (replace_val && Z_TYPE_P(replace_val) == IS_TRUE)
        {
            replace = 1;
        }
    }

    core_command_args_t args = {0};
    args.glide_client = valkey_glide->glide_client;
    args.cmd_type = Copy;
    args.key = src; /* Source key */
    args.key_len = src_len;

    /* Destination key */
    args.args[0].type = CORE_ARG_TYPE_STRING;
    args.args[0].data.string_arg.value = dst;
    args.args[0].data.string_arg.len = dst_len;

    int arg_count = 1;

    /* Optional REPLACE flag */
    if (replace)
    {
        args.args[1].type = CORE_ARG_TYPE_STRING;
        args.args[1].data.string_arg.value = "REPLACE";
        args.args[1].data.string_arg.len = 7;
        arg_count = 2;
    }

    args.arg_count = arg_count;

    /* Execute the COPY command using the Glide client */
    if (execute_core_command(&args, NULL, process_core_bool_result))
    {
        ZVAL_TRUE(return_value);
        return 1;
    }
    else
    {
        ZVAL_FALSE(return_value);
        return 0;
    }
}

/* Unified PFADD command implementation */
int execute_pfadd_command(zval *object, int argc, zval *return_value)
{
    valkey_glide_object *valkey_glide;
    char *key = NULL;
    size_t key_len;
    zval *z_elements;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Osa",
                                     &object, get_valkey_glide_ce(), &key, &key_len,
                                     &z_elements) == FAILURE)
    {
        return 0;
    }

    /* Get ValkeyGlide object */
    valkey_glide = VALKEY_GLIDE_PHP_ZVAL_GET_OBJECT(valkey_glide_object, object);
    if (!valkey_glide || !valkey_glide->glide_client)
    {
        return 0;
    }

    /* Execute the PFADD command using the Glide client */
    int result_value = 0;
    int elements_count = zend_hash_num_elements(Z_ARRVAL_P(z_elements));

    core_command_args_t args = {0};
    args.glide_client = valkey_glide->glide_client;
    args.cmd_type = PfAdd;
    args.key = key;
    args.key_len = key_len;

    /* Add elements array argument */
    args.args[0].type = CORE_ARG_TYPE_ARRAY;
    args.args[0].data.array_arg.array = z_elements;
    args.args[0].data.array_arg.count = elements_count;
    args.arg_count = 1;

    long result;
    int success = execute_core_command(&args, &result, process_core_int_result);
    if (success)
        result_value = (int)result;

    if (success)
    {
        ZVAL_LONG(return_value, result_value);
        return 1;
    }

    return 0;
}

/* Unified PFCOUNT command implementation */
int execute_pfcount_command(zval *object, int argc, zval *return_value)
{
    valkey_glide_object *valkey_glide;
    zval *z_args = NULL;
    int arg_count = 0;
    long result_value = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "O+",
                                     &object, get_valkey_glide_ce(), &z_args, &arg_count) == FAILURE)
    {
        return 0;
    }

    /* Get ValkeyGlide object */
    valkey_glide = VALKEY_GLIDE_PHP_ZVAL_GET_OBJECT(valkey_glide_object, object);
    if (!valkey_glide || !valkey_glide->glide_client)
    {
        return 0;
    }

    /* Execute the PFCOUNT command using the Glide client */

    if (execute_multi_key_command(valkey_glide->glide_client, PfCount, z_args, arg_count, &result_value))
    {
        ZVAL_LONG(return_value, result_value);
        return 1;
    }

    return 0;
}

/* Unified PFMERGE command implementation */
int execute_pfmerge_command(zval *object, int argc, zval *return_value)
{
    valkey_glide_object *valkey_glide;
    char *dst = NULL;
    size_t dst_len;
    zval *z_keys;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Osa",
                                     &object, get_valkey_glide_ce(), &dst, &dst_len,
                                     &z_keys) == FAILURE)
    {
        return 0;
    }

    /* Get ValkeyGlide object */
    valkey_glide = VALKEY_GLIDE_PHP_ZVAL_GET_OBJECT(valkey_glide_object, object);
    if (!valkey_glide || !valkey_glide->glide_client)
    {
        return 0;
    }

    /* Execute the PFMERGE command using the Glide client */
    int keys_count = zend_hash_num_elements(Z_ARRVAL_P(z_keys));

    core_command_args_t args = {0};
    args.glide_client = valkey_glide->glide_client;
    args.cmd_type = PfMerge;
    args.key = dst; /* Destination key */
    args.key_len = dst_len;

    /* Add source keys array */
    args.args[0].type = CORE_ARG_TYPE_ARRAY;
    args.args[0].data.array_arg.array = z_keys;
    args.args[0].data.array_arg.count = keys_count;
    args.arg_count = 1;

    if (execute_core_command(&args, NULL, process_core_bool_result))
    {
        ZVAL_TRUE(return_value);
        return 1;
    }

    ZVAL_FALSE(return_value);
    return 0;
}

/* Unified getTimeout command implementation */
int execute_gettimeout_command(zval *object, int argc, zval *return_value)
{
    valkey_glide_object *valkey_glide;
    double timeout;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "O",
                                     &object, get_valkey_glide_ce()) == FAILURE)
    {
        return 0;
    }

    /* Get ValkeyGlide object */
    valkey_glide = VALKEY_GLIDE_PHP_ZVAL_GET_OBJECT(valkey_glide_object, object);
    if (!valkey_glide || !valkey_glide->glide_client)
    {
        return 0;
    }

    /* Execute the getTimeout command */
    if (execute_get_timeout_command(valkey_glide->glide_client, &timeout))
    {
        ZVAL_DOUBLE(return_value, timeout);
        return 1;
    }

    return 0;
}

/* Execute a SELECT command using the Valkey Glide client */
int execute_select_command_internal(const void *glide_client, long dbindex)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Select;

    /* Add database index argument */
    args.args[0].type = CORE_ARG_TYPE_LONG;
    args.args[0].data.long_arg.value = dbindex;
    args.arg_count = 1;

    return execute_core_command(&args, NULL, process_core_bool_result);
}

/* Execute a SELECT command - UNIFIED IMPLEMENTATION */
int execute_select_command(zval *object, int argc, zval *return_value)
{
    valkey_glide_object *valkey_glide;
    long dbindex;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Ol",
                                     &object, get_valkey_glide_ce(), &dbindex) == FAILURE)
    {
        return 0;
    }

    /* Get ValkeyGlide object */
    valkey_glide = VALKEY_GLIDE_PHP_ZVAL_GET_OBJECT(valkey_glide_object, object);
    if (!valkey_glide || !valkey_glide->glide_client)
    {
        return 0;
    }

    /* Execute the SELECT command using the Glide client */
    if (execute_select_command_internal(valkey_glide->glide_client, dbindex))
    {
        ZVAL_TRUE(return_value);
        return 1;
    }

    return 0;
}

/* Execute a SWAPDB command using the Valkey Glide client */
int execute_swapdb_command_internal(const void *glide_client, long db1, long db2)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = SwapDb;

    /* Add db1 argument */
    args.args[0].type = CORE_ARG_TYPE_LONG;
    args.args[0].data.long_arg.value = db1;

    /* Add db2 argument */
    args.args[1].type = CORE_ARG_TYPE_LONG;
    args.args[1].data.long_arg.value = db2;
    args.arg_count = 2;

    return execute_core_command(&args, NULL, process_core_bool_result);
}

/* Execute a SWAPDB command - UNIFIED IMPLEMENTATION */
int execute_swapdb_command(zval *object, int argc, zval *return_value)
{
    valkey_glide_object *valkey_glide;
    long db1, db2;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Oll",
                                     &object, get_valkey_glide_ce(), &db1, &db2) == FAILURE)
    {
        return 0;
    }

    /* Get ValkeyGlide object */
    valkey_glide = VALKEY_GLIDE_PHP_ZVAL_GET_OBJECT(valkey_glide_object, object);
    if (!valkey_glide || !valkey_glide->glide_client)
    {
        return 0;
    }

    /* Execute the SWAPDB command using the Glide client */
    if (execute_swapdb_command_internal(valkey_glide->glide_client, db1, db2))
    {
        ZVAL_TRUE(return_value);
        return 1;
    }

    return 0;
}

/* Execute a MOVE command using the Valkey Glide client */
int execute_move_command_internal(const void *glide_client, const char *key, size_t key_len, long db, int *output_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Move;
    args.key = key;
    args.key_len = key_len;

    /* Add db argument */
    args.args[0].type = CORE_ARG_TYPE_LONG;
    args.args[0].data.long_arg.value = db;
    args.arg_count = 1;

    return execute_core_command(&args, output_value, process_core_bool_result);
}

/* Execute a MOVE command - UNIFIED IMPLEMENTATION */
int execute_move_command(zval *object, int argc, zval *return_value)
{
    valkey_glide_object *valkey_glide;
    char *key = NULL;
    size_t key_len;
    long dbindex;
    int result_value = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Osl",
                                     &object, get_valkey_glide_ce(), &key, &key_len,
                                     &dbindex) == FAILURE)
    {
        return 0;
    }

    /* Get ValkeyGlide object */
    valkey_glide = VALKEY_GLIDE_PHP_ZVAL_GET_OBJECT(valkey_glide_object, object);
    if (!valkey_glide || !valkey_glide->glide_client)
    {
        return 0;
    }

    /* Execute the MOVE command using the Glide client */
    if (execute_move_command_internal(valkey_glide->glide_client, key, key_len, dbindex, &result_value))
    {
        if (result_value == 1)
        {
            ZVAL_TRUE(return_value);
        }
        else
        {
            ZVAL_FALSE(return_value);
        }
        return 1;
    }

    return 0;
}
