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

/* Helper function to parse range options */
static int parse_range_options(zval *options, int *has_withscores,
                               int *has_byscore, int *has_bylex, int *has_rev,
                               int *has_limit, long *offset, long *count)
{
    if (!options || Z_TYPE_P(options) != IS_ARRAY)
    {
        return 0;
    }

    HashTable *options_ht = Z_ARRVAL_P(options);
    zval *z_tmp;

    /* Look for WITHSCORES option */
    if ((z_tmp = zend_hash_str_find(options_ht, "WITHSCORES", sizeof("WITHSCORES") - 1)) != NULL)
    {
        if (Z_TYPE_P(z_tmp) == IS_TRUE ||
            (Z_TYPE_P(z_tmp) == IS_LONG && Z_LVAL_P(z_tmp) == 1))
        {
            *has_withscores = 1;
        }
    }

    /* Look for BY option */
    if ((z_tmp = zend_hash_str_find(options_ht, "BY", sizeof("BY") - 1)) != NULL)
    {
        if (Z_TYPE_P(z_tmp) == IS_STRING)
        {
            if (strcasecmp(Z_STRVAL_P(z_tmp), "SCORE") == 0)
            {
                *has_byscore = 1;
            }
            else if (strcasecmp(Z_STRVAL_P(z_tmp), "LEX") == 0)
            {
                *has_bylex = 1;
            }
        }
    }

    /* Look for REV option */
    if ((z_tmp = zend_hash_str_find(options_ht, "REV", sizeof("REV") - 1)) != NULL)
    {
        if (Z_TYPE_P(z_tmp) == IS_TRUE ||
            (Z_TYPE_P(z_tmp) == IS_LONG && Z_LVAL_P(z_tmp) == 1))
        {
            *has_rev = 1;
        }
    }

    /* Look for LIMIT option */
    if ((z_tmp = zend_hash_str_find(options_ht, "LIMIT", sizeof("LIMIT") - 1)) != NULL)
    {
        if (Z_TYPE_P(z_tmp) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(z_tmp)) == 2)
        {
            zval *z_offset, *z_count;
            HashTable *ht_limit = Z_ARRVAL_P(z_tmp);

            /* Get offset and count from LIMIT array */
            if ((z_offset = zend_hash_index_find(ht_limit, 0)) != NULL &&
                (z_count = zend_hash_index_find(ht_limit, 1)) != NULL)
            {
                *offset = zval_get_long(z_offset);
                *count = zval_get_long(z_count);
                *has_limit = 1;
            }
        }
    }

    return 1;
}

/* Execute a ZREVRANGE command using the Valkey Glide client */
int execute_zrevrange_command(const void *glide_client, const char *key, size_t key_len,
                              zval *z_start, zval *z_end, zval *options, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !z_start || !z_end)
    {
        return 0;
    }

    /* Parse range options */
    int has_withscores = 0, has_byscore = 0, has_bylex = 0, has_rev = 1; /* ZREVRANGE is always REV */
    int has_limit = 0;
    long offset = 0, count = 0;

    if (options)
    {
        parse_range_options(options, &has_withscores, &has_byscore, &has_bylex, NULL,
                            &has_limit, &offset, &count);
    }

    /* Use the helper function to build and execute command */
    return build_range_cmd_args(glide_client, ZRange, key, key_len,
                                z_start, z_end, has_withscores, has_byscore, has_bylex,
                                has_rev, has_limit, offset, count, return_value);
}

/* Execute a ZRANGEBYSCORE command using the Valkey Glide client */
int execute_zrangebyscore_command(const void *glide_client, const char *key, size_t key_len,
                                  zval *z_min, zval *z_max, zval *options, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !z_min || !z_max)
    {
        return 0;
    }

    /* Parse range options */
    int has_withscores = 0, has_byscore = 1, has_bylex = 0, has_rev = 0; /* ZRANGEBYSCORE uses BYSCORE */
    int has_limit = 0;
    long offset = 0, count = 0;

    if (options)
    {
        parse_range_options(options, &has_withscores, NULL, NULL, &has_rev,
                            &has_limit, &offset, &count);
    }

    /* Use the helper function to build and execute command */
    return build_range_cmd_args(glide_client, ZRange, key, key_len,
                                z_min, z_max, has_withscores, has_byscore, has_bylex,
                                has_rev, has_limit, offset, count, return_value);
}

/* Execute a ZREVRANGEBYSCORE command using the Valkey Glide client */
int execute_zrevrangebyscore_command(const void *glide_client, const char *key, size_t key_len,
                                     zval *z_max, zval *z_min, zval *options, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !z_min || !z_max)
    {
        return 0;
    }

    /* Parse range options */
    int has_withscores = 0, has_byscore = 1, has_bylex = 0, has_rev = 1; /* ZREVRANGEBYSCORE uses BYSCORE + REV */
    int has_limit = 0;
    long offset = 0, count = 0;

    if (options)
    {
        parse_range_options(options, &has_withscores, NULL, NULL, NULL,
                            &has_limit, &offset, &count);
    }

    /* Use the helper function to build and execute command */
    return build_range_cmd_args(glide_client, ZRange, key, key_len,
                                z_max, z_min, has_withscores, has_byscore, has_bylex,
                                has_rev, has_limit, offset, count, return_value);
}

/* Execute a ZRANGEBYLEX command using the Valkey Glide client */
int execute_zrangebylex_command(const void *glide_client, const char *key, size_t key_len,
                                zval *z_min, zval *z_max, zval *options, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !z_min || !z_max)
    {
        return 0;
    }

    /* Parse range options */
    int has_withscores = 0, has_byscore = 0, has_bylex = 1, has_rev = 0; /* ZRANGEBYLEX uses BYLEX */
    int has_limit = 0;
    long offset = 0, count = 0;

    if (options)
    {
        parse_range_options(options, NULL, NULL, NULL, &has_rev,
                            &has_limit, &offset, &count);
    }

    /* Use the helper function to build and execute command */
    return build_range_cmd_args(glide_client, ZRange, key, key_len,
                                z_min, z_max, has_withscores, has_byscore, has_bylex,
                                has_rev, has_limit, offset, count, return_value);
}
