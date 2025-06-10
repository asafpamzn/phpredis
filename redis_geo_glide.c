/*
  +----------------------------------------------------------------------+
  | Redis Glide FFI integration for phpredis - Geo Commands              |
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

#include "valkey_glide_geo_common.h"
extern zend_class_entry *redis_ce;

/* Execute a GEOADD command using the Valkey Glide client */
int execute_geoadd_command(zval *object, int argc, zval *return_value)
{
    char *key = NULL;
    size_t key_len;
    zval *z_args;
    int variadic_argc = 0;
    const void *glide_client = NULL;
    long result_value = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Os*",
                                     &object, redis_ce, &key, &key_len,
                                     &z_args, &variadic_argc) == FAILURE)
    {
        return 0;
    }

    /* Check that we have the right number of arguments */
    if (variadic_argc < 3 || variadic_argc % 3 != 0)
    {
        php_error_docref(NULL, E_WARNING,
                         "geoadd requires at least one longitude/latitude/member triplet");
        return 0;
    }

    /* Get Redis object */
    redis_object *redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    glide_client = redis->glide_client;

    /* Check if we have a valid glide client */
    if (!glide_client)
    {
        return 0;
    }

    /* Initialize geo command arguments structure */
    geo_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.geo_args = z_args;
    args.geo_args_count = variadic_argc;

    /* Execute the generic command with appropriate result processor */
    int result = execute_geo_generic_command(
        glide_client,
        GeoAdd,
        &args,
        &result_value,
        process_geo_int_result);

    if (result)
    {
        ZVAL_LONG(return_value, result_value);
    }

    return result;
}

/* Execute a GEODIST command using the Valkey Glide client */
int execute_geodist_command(const void *glide_client, const char *key, size_t key_len,
                            char *src, size_t src_len,
                            char *dst, size_t dst_len,
                            char *unit, size_t unit_len,
                            double *output_value)
{
    /* Check if client, key, src, dst are valid */
    if (!glide_client || !key || !src || !dst)
    {
        return 0;
    }

    /* Initialize geo command arguments structure */
    geo_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.src_member = src;
    args.src_member_len = src_len;
    args.dst_member = dst;
    args.dst_member_len = dst_len;
    args.unit = unit;
    args.unit_len = unit_len;

    /* Execute the generic command with appropriate result processor */
    return execute_geo_generic_command(
        glide_client,
        GeoDist,
        &args,
        output_value,
        process_geo_double_result);
}

/* Execute a GEOHASH command using the Valkey Glide client */
int execute_geohash_command(const void *glide_client, const char *key, size_t key_len,
                            zval *members, int member_count, zval *return_value)
{
    /* Check if client, key, and members are valid */
    if (!glide_client || !key || !members || member_count <= 0)
    {
        return 0;
    }

    /* Initialize geo command arguments structure */
    geo_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.members = members;
    args.member_count = member_count;

    /* Execute the generic command with appropriate result processor */
    return execute_geo_generic_command(
        glide_client,
        GeoHash,
        &args,
        return_value,
        process_geo_hash_result);
}

/* Execute a GEOPOS command using the Valkey Glide client */
int execute_geopos_command(const void *glide_client, const char *key, size_t key_len,
                           zval *members, int member_count, zval *return_value)
{
    /* Check if client, key, and members are valid */
    if (!glide_client || !key || !members || member_count <= 0)
    {
        return 0;
    }

    /* Initialize geo command arguments structure */
    geo_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.members = members;
    args.member_count = member_count;

    /* Execute the generic command with appropriate result processor */
    return execute_geo_generic_command(
        glide_client,
        GeoPos,
        &args,
        return_value,
        process_geo_pos_result);
}

/* Execute a GEORADIUS command using the Valkey Glide client */
int execute_georadius_command(const void *glide_client, const char *key, size_t key_len,
                              double longitude, double latitude, double radius,
                              const char *unit, size_t unit_len,
                              zval *opts, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !unit)
    {
        return 0;
    }

    /* Initialize geo command arguments structure */
    geo_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.longitude = longitude;
    args.latitude = latitude;
    args.radius = radius;
    args.unit = unit;
    args.unit_len = unit_len;
    args.options = opts;

    /* Parse the WITH* options if provided */
    if (opts != NULL && Z_TYPE_P(opts) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(opts);
        zval *z_withcoord, *z_withdist, *z_withhash, *z_count, *z_sort;

        /* Check for WITHCOORD option */
        if ((z_withcoord = zend_hash_str_find(ht, "WITHCOORD", sizeof("WITHCOORD") - 1)) != NULL)
        {
            args.radius_opts.with_opts.withcoord = zval_is_true(z_withcoord);
        }

        /* Check for WITHDIST option */
        if ((z_withdist = zend_hash_str_find(ht, "WITHDIST", sizeof("WITHDIST") - 1)) != NULL)
        {
            args.radius_opts.with_opts.withdist = zval_is_true(z_withdist);
        }

        /* Check for WITHHASH option */
        if ((z_withhash = zend_hash_str_find(ht, "WITHHASH", sizeof("WITHHASH") - 1)) != NULL)
        {
            args.radius_opts.with_opts.withhash = zval_is_true(z_withhash);
        }

        /* Check for COUNT option */
        if ((z_count = zend_hash_str_find(ht, "COUNT", sizeof("COUNT") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_count) == IS_LONG)
            {
                args.radius_opts.count = Z_LVAL_P(z_count);
            }
        }

        /* Check for sorting option (ASC/DESC) */
        if ((z_sort = zend_hash_str_find(ht, "SORT", sizeof("SORT") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_sort) == IS_STRING)
            {
                args.radius_opts.sort = Z_STRVAL_P(z_sort);
                args.radius_opts.sort_len = Z_STRLEN_P(z_sort);
            }
        }
    }

    /* Create a data structure to pass the WITH* options to the result processor */
    struct
    {
        zval *return_value;
        int withcoord;
        int withdist;
        int withhash;
    } radius_data = {
        return_value,
        args.radius_opts.with_opts.withcoord,
        args.radius_opts.with_opts.withdist,
        args.radius_opts.with_opts.withhash};

    /* Execute the generic command with appropriate result processor */
    return execute_geo_generic_command(
        glide_client,
        GeoRadius,
        &args,
        &radius_data,
        process_geo_radius_result);
}

/* Execute a GEORADIUS_RO command using the Valkey Glide client */
int execute_georadius_ro_command(const void *glide_client, const char *key, size_t key_len,
                                 double longitude, double latitude, double radius,
                                 const char *unit, size_t unit_len,
                                 zval *opts, zval *return_value)
{
    /* This is essentially the same as GEORADIUS but with a different command type */
    /* However, we're using the same GeoRadius command type since there's no specific RO enum value */

    /* Check if client and key are valid */
    if (!glide_client || !key || !unit)
    {
        return 0;
    }

    /* Initialize geo command arguments structure */
    geo_command_args_t args = {0};
    args.key = key;
    args.key_len = key_len;
    args.longitude = longitude;
    args.latitude = latitude;
    args.radius = radius;
    args.unit = unit;
    args.unit_len = unit_len;
    args.options = opts;

    /* Parse the WITH* options if provided */
    if (opts != NULL && Z_TYPE_P(opts) == IS_ARRAY)
    {
        HashTable *ht = Z_ARRVAL_P(opts);
        zval *z_withcoord, *z_withdist, *z_withhash, *z_count, *z_sort;

        /* Check for WITHCOORD option */
        if ((z_withcoord = zend_hash_str_find(ht, "WITHCOORD", sizeof("WITHCOORD") - 1)) != NULL)
        {
            args.radius_opts.with_opts.withcoord = zval_is_true(z_withcoord);
        }

        /* Check for WITHDIST option */
        if ((z_withdist = zend_hash_str_find(ht, "WITHDIST", sizeof("WITHDIST") - 1)) != NULL)
        {
            args.radius_opts.with_opts.withdist = zval_is_true(z_withdist);
        }

        /* Check for WITHHASH option */
        if ((z_withhash = zend_hash_str_find(ht, "WITHHASH", sizeof("WITHHASH") - 1)) != NULL)
        {
            args.radius_opts.with_opts.withhash = zval_is_true(z_withhash);
        }

        /* Check for COUNT option */
        if ((z_count = zend_hash_str_find(ht, "COUNT", sizeof("COUNT") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_count) == IS_LONG)
            {
                args.radius_opts.count = Z_LVAL_P(z_count);
            }
        }

        /* Check for sorting option (ASC/DESC) */
        if ((z_sort = zend_hash_str_find(ht, "SORT", sizeof("SORT") - 1)) != NULL)
        {
            if (Z_TYPE_P(z_sort) == IS_STRING)
            {
                args.radius_opts.sort = Z_STRVAL_P(z_sort);
                args.radius_opts.sort_len = Z_STRLEN_P(z_sort);
            }
        }
    }

    /* Create a data structure to pass the WITH* options to the result processor */
    struct
    {
        zval *return_value;
        int withcoord;
        int withdist;
        int withhash;
    } radius_data = {
        return_value,
        args.radius_opts.with_opts.withcoord,
        args.radius_opts.with_opts.withdist,
        args.radius_opts.with_opts.withhash};

    /* Execute the generic command with appropriate result processor */
    return execute_geo_generic_command(
        glide_client,
        GeoRadius, /* Same command type - GeoRadius since no RO variant in enum */
        &args,
        &radius_data,
        process_geo_radius_result);
}
