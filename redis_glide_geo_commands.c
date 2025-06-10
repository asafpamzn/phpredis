/* -*- Mode: C; tab-width: 4 -*- */
/*
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2009 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Your Name                                                     |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_redis.h"
#include "redis_glide.h"
#include "command_response.h"
#include <php.h>
#include "zend_API.h"
#include "valkey_glide_geo_common.h"

/* GEOSEARCH implementation */
int execute_geosearch_command(const void *glide_client, const char *key, size_t key_len,
                              zval *from, double *by_radius, const char *by_unit, size_t by_unit_len,
                              zval *options, zval *return_value)
{
    /* Check if client is valid */
    if (!glide_client || !key || !from || !by_radius)
    {
        return 0;
    }

    /* Initialize geo command arguments structure */
    geo_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.from = from;
    args.by_radius = by_radius;
    args.unit = by_unit;
    args.unit_len = by_unit_len;
    args.options = options;

    /* Parse the WITH* options if provided */
    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(options);
        zval *opt;

        ZEND_HASH_FOREACH_VAL(ht, opt)
        {
            if (Z_TYPE_P(opt) == IS_STRING)
            {
                if (strcasecmp(Z_STRVAL_P(opt), "withcoord") == 0)
                {
                    args.radius_opts.with_opts.withcoord = 1;
                }
                else if (strcasecmp(Z_STRVAL_P(opt), "withdist") == 0)
                {
                    args.radius_opts.with_opts.withdist = 1;
                }
                else if (strcasecmp(Z_STRVAL_P(opt), "withhash") == 0)
                {
                    args.radius_opts.with_opts.withhash = 1;
                }
            }
        }
        ZEND_HASH_FOREACH_END();
    }

    /* Create a data structure to pass the WITH* options to the result processor */
    struct
    {
        zval *return_value;
        int withcoord;
        int withdist;
        int withhash;
    } search_data = {
        return_value,
        args.radius_opts.with_opts.withcoord,
        args.radius_opts.with_opts.withdist,
        args.radius_opts.with_opts.withhash};

    /* Execute the generic command with appropriate result processor */
    return execute_geo_generic_command(
        glide_client,
        GeoSearch,
        &args,
        &search_data,
        process_geo_search_result);
}

/* GEOSEARCHSTORE implementation */
int execute_geosearchstore_command(const void *glide_client, const char *dest, size_t dest_len,
                                   const char *src, size_t src_len, zval *from, double *by_radius,
                                   const char *by_unit, size_t by_unit_len, zval *options, long *output_value)
{
    /* Check if client is valid */
    if (!glide_client || !dest || !src || !from || !by_radius)
    {
        return 0;
    }

    /* Initialize geo command arguments structure */
    geo_command_args_t args = {0};
    args.dest = dest;
    args.dest_len = dest_len;
    args.src = src;
    args.src_len = src_len;
    args.from = from;
    args.by_radius = by_radius;
    args.unit = by_unit;
    args.unit_len = by_unit_len;
    args.options = options;

    /* Parse options if provided */
    if (options && Z_TYPE_P(options) == IS_ARRAY)
    {
        HashTable *ht_options = Z_ARRVAL_P(options);
        zval *opt_val;

        /* COUNT option */
        if ((opt_val = zend_hash_str_find(ht_options, "count", sizeof("count") - 1)) != NULL)
        {
            args.radius_opts.count = zval_get_long(opt_val);
        }

        /* SORT option (ASC/DESC) */
        if ((opt_val = zend_hash_str_find(ht_options, "sort", sizeof("sort") - 1)) != NULL)
        {
            if (Z_TYPE_P(opt_val) == IS_STRING)
            {
                args.radius_opts.sort = Z_STRVAL_P(opt_val);
                args.radius_opts.sort_len = Z_STRLEN_P(opt_val);
            }
        }

        /* STOREDIST option */
        if ((opt_val = zend_hash_str_find(ht_options, "storedist", sizeof("storedist") - 1)) != NULL)
        {
            args.radius_opts.store_dist = zval_is_true(opt_val);
        }
    }

    /* Execute the generic command with appropriate result processor */
    return execute_geo_generic_command(
        glide_client,
        GeoSearchStore,
        &args,
        output_value,
        process_geo_int_result);
}
