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

/* Execute a ZADD command using the Valkey Glide client */
int execute_zadd_command(const void *glide_client, const char *key, size_t key_len, zval *z_args, int argc, int flags, long *output_value, double *output_value_double)
{
    z_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.members = z_args;
    args.member_count = argc;

    /* Determine if INCR option is present by parsing first element if it's an array */
    int has_incr = 0;
    if (argc > 0 && Z_TYPE(z_args[0]) == IS_ARRAY)
    {
        zadd_options_t zadd_opts = {0};
        parse_zadd_options(&z_args[0], &zadd_opts);
        has_incr = zadd_opts.incr;
    }

    struct
    {
        long *output_value;
        double *output_value_double;
        int is_incr;
    } zadd_data = {output_value, output_value_double, has_incr};

    return execute_z_generic_command(
        glide_client,
        ZAdd,
        &args,
        &zadd_data,
        process_z_zadd_result);
}

/* Execute a ZRANGESTORE command using the Valkey Glide client */
int execute_zrangestore_command(const void *glide_client, const char *dst, size_t dst_len,
                                const char *src, size_t src_len, zval *z_start, zval *z_end,
                                zval *options, long *output_value)
{
    z_command_args_t args = {0};
    args.key = dst; /* dst is the destination key */
    args.key_len = dst_len;
    args.member = src; /* src is the source key (reuse member field) */
    args.member_len = src_len;
    args.z_start = z_start;
    args.z_end = z_end;
    args.options = options;

    return execute_z_generic_command(
        glide_client,
        ZRangeStore,
        &args,
        output_value,
        process_z_int_result);
}
