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

#ifndef REDIS_GLIDE_ZADD_H
#define REDIS_GLIDE_ZADD_H

#include "php.h"

/* Helper function to convert a zval to a string */
char *zval_to_string(zval *z, size_t *len, int *need_free);

/* Helper function to parse range options */
int parse_range_options(zval *options, int *has_withscores,
                        int *has_byscore, int *has_bylex, int *has_rev,
                        int *has_limit, long *offset, long *count);

/* Helper function to process ZRANGE-like responses */
int process_zrange_response(CommandResult *result, zval *return_value, int has_withscores);

/* Helper function to build arguments for range commands */
int build_range_cmd_args(const void *glide_client, enum RequestType cmd_type, const char *key, size_t key_len,
                         zval *z_start, zval *z_end, int has_withscores, int has_byscore, int has_bylex,
                         int has_rev, int has_limit, long offset, long count, zval *return_value);

/* ZADD command with options */
int execute_zadd_command(const void *glide_client, const char *key, size_t key_len, zval *z_args, int argc, int flags, long *output_value, double *output_value_double);

/* ZRANGE command family */
int execute_zrangestore_command(const void *glide_client, const char *dst, size_t dst_len, const char *src, size_t src_len, zval *z_start, zval *z_end, zval *options, long *output_value);

/* Functions from redis_glide_zadd2.c */
int execute_zrevrange_command(const void *glide_client, const char *key, size_t key_len, zval *z_start, zval *z_end, zval *options, zval *return_value);
int execute_zrangebyscore_command(const void *glide_client, const char *key, size_t key_len, zval *z_min, zval *z_max, zval *options, zval *return_value);
int execute_zrevrangebyscore_command(const void *glide_client, const char *key, size_t key_len, zval *z_max, zval *z_min, zval *options, zval *return_value);
int execute_zrangebylex_command(const void *glide_client, const char *key, size_t key_len, zval *z_min, zval *z_max, zval *options, zval *return_value);

/* Functions from redis_glide_zadd3.c */
int execute_zrevrangebylex_command(const void *glide_client, const char *key, size_t key_len, zval *z_max, zval *z_min, zval *options, zval *return_value);
int execute_zdiff_command(const void *glide_client, zval *keys, zval *options, zval *return_value);
int execute_zinter_command(const void *glide_client, zval *keys, zval *z_weights, zval *options, zval *return_value);

#endif /* REDIS_GLIDE_ZADD_H */
