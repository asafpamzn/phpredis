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
#include "redis_glide_zadd.h"
#include "valkey_glide_z_common.h"
#include "command_response.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Execute a ZREVRANGE command using the Valkey Glide client */
int execute_zrevrange_command(const void *glide_client, const char *key, size_t key_len,
                              zval *z_start, zval *z_end, zval *options, zval *return_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.z_start = z_start;
    args.z_end = z_end;
    args.options = options;

    /* Parse options to determine if withscores is set */
    range_options_t range_opts = {0};
    parse_range_options(options, &range_opts);

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, range_opts.withscores};

    return execute_z_generic_command(
        glide_client,
        ZRevRange,
        &args,
        &array_data,
        process_z_array_result);
}

/* Execute a ZRANGEBYSCORE command using the Valkey Glide client */
int execute_zrangebyscore_command(const void *glide_client, const char *key, size_t key_len,
                                  zval *z_min, zval *z_max, zval *options, zval *return_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.z_start = z_min;
    args.z_end = z_max;
    args.options = options;

    /* Parse options to determine if withscores is set */
    range_options_t range_opts = {0};
    parse_range_options(options, &range_opts);

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, range_opts.withscores};

    return execute_z_generic_command(
        glide_client,
        ZRangeByScore,
        &args,
        &array_data,
        process_z_array_result);
}

/* Execute a ZREVRANGEBYSCORE command using the Valkey Glide client */
int execute_zrevrangebyscore_command(const void *glide_client, const char *key, size_t key_len,
                                     zval *z_max, zval *z_min, zval *options, zval *return_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.z_start = z_max; /* For ZREVRANGEBYSCORE, start is max */
    args.z_end = z_min;   /* For ZREVRANGEBYSCORE, end is min */
    args.options = options;

    /* Parse options to determine if withscores is set */
    range_options_t range_opts = {0};
    parse_range_options(options, &range_opts);

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, range_opts.withscores};

    return execute_z_generic_command(
        glide_client,
        ZRevRangeByScore,
        &args,
        &array_data,
        process_z_array_result);
}

/* Execute a ZRANGEBYLEX command using the Valkey Glide client */
int execute_zrangebylex_command(const void *glide_client, const char *key, size_t key_len,
                                zval *z_min, zval *z_max, zval *options, zval *return_value)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.z_start = z_min;
    args.z_end = z_max;
    args.options = options;

    /* Parse options to determine if withscores is set */
    range_options_t range_opts = {0};
    parse_range_options(options, &range_opts);
    range_opts.bylex = 1; /* ZRANGEBYLEX always has BYLEX */

    struct
    {
        zval *return_value;
        int withscores;
    } array_data = {return_value, range_opts.withscores};

    return execute_z_generic_command(
        glide_client,
        ZRangeByLex,
        &args,
        &array_data,
        process_z_array_result);
}
