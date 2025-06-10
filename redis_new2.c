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
  | Original author: Alfonso Jimenez <yo@alfonsojimenez.com>             |
  | Maintainer: Nicolas Favre-Felix <n.favre-felix@owlient.eu>           |
  | Maintainer: Nasreddine Bouafif <n.bouafif@owlient.eu>                |
  | Maintainer: Michael Grunder <michael.grunder@gmail.com>              |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_redis.h"

#include "redis_cluster.h"

#include "redis_glide.h"
#include "command_response.h"        /* Include command_response.h for string conversion functions */
#include "valkey_glide_geo_common.h" /* Include geo common header for macros */
#include <ext/spl/spl_exceptions.h>
#include <zend_exceptions.h>
#include <ext/standard/info.h>
#include <ext/hash/php_hash.h>

#if PHP_VERSION_ID < 80400
#include <ext/standard/php_random.h>
#else
#include <ext/random/php_random.h>
#endif

#ifdef PHP_SESSION
#include <ext/session/php_session.h>
#endif

/* Import string conversion functions */
extern char *long_to_string(long value, size_t *len);
extern char *double_to_string(double value, size_t *len);

#ifdef HAVE_REDIS_ZSTD
#include <zstd.h>
#endif

#ifdef HAVE_REDIS_LZ4
#include <lz4.h>
#endif

#ifdef PHP_SESSION
extern ps_module ps_mod_redis;
extern ps_module ps_mod_redis_cluster;
#endif

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

#if PHP_VERSION_ID < 80000
#include "redis_legacy_arginfo.h"
#else
#include "zend_attributes.h"
#include "redis_arginfo.h"
#endif

/* {{{ proto long Redis::geoadd(string key, float longitude, float latitude, string member, ...) */
GEOADD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto double Redis::geodist(string key, string src, string dst [, string unit]) */
GEODIST_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::geohash(string key, string member [, string ...]) */
GEOHASH_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::geopos(string key, string member [, string ...]) */
GEOPOS_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::georadius(string key, float lng, float lat, float radius, string unit [, array options]) */
GEORADIUS_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::georadius_ro(string key, float lng, float lat, float radius, string unit [, array options]) */
GEORADIUS_RO_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::georadiusbymember(string key, string member, float radius, string unit [, array options]) */
PHP_METHOD(Redis, georadiusbymember)
{
    /* For now, we'll leave this to the standard implementation since it's not
       directly implemented in the redis_geo_glide.c file */
    zval *object;
    redis_object *redis;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
                                     &object, redis_ce) == FAILURE)
    {
        RETURN_FALSE;
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Redis::georadiusbymember_ro(string key, string member, float radius, string unit [, array options]) */
PHP_METHOD(Redis, georadiusbymember_ro)
{
    /* For now, we'll leave this to the standard implementation since it's not
       directly implemented in the redis_geo_glide.c file */
    zval *object;
    redis_object *redis;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
                                     &object, redis_ce) == FAILURE)
    {
        RETURN_FALSE;
    }

    RETURN_FALSE;
}
/* }}} */

/* Import GEOSEARCH functions */
extern int execute_geosearch_command(const void *glide_client, const char *key, size_t key_len,
                                     zval *from, double *by_radius, const char *by_unit, size_t by_unit_len,
                                     zval *options, zval *return_value);

extern int execute_geosearchstore_command(const void *glide_client, const char *dest, size_t dest_len,
                                          const char *src, size_t src_len, zval *from, double *by_radius,
                                          const char *by_unit, size_t by_unit_len, zval *options, long *output_value);

/* {{{ proto array Redis::geosearch(string key, array|string from, array|string by, string|null radius_unit, string|null count, string|null sorting, string|null pattern) */
PHP_METHOD(Redis, geosearch)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *unit = NULL;
    size_t key_len, unit_len;
    zval *from, *options = NULL;
    double radius;

    /* Parse parameters for simple case: geosearch(key, member, radius, unit [, options]) */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oszds|a",
                                     &object, redis_ce, &key, &key_len,
                                     &from, &radius, &unit, &unit_len, &options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* Initialize return value as array */
    array_init(return_value);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the GEOSEARCH command using the Glide client */
        if (execute_geosearch_command(redis->glide_client, key, key_len, from, &radius, unit, unit_len, options, return_value))
        {
            /* Command already populated return_value */
            return;
        }
        else
        {
            /* Command failed */
            zval_dtor(return_value);
            RETURN_FALSE;
        }
    }
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto long Redis::geosearchstore(string dst, string src, array|string from, array|string by, string|null radius_unit, string|null count, string|null sorting, string|null storedist) */
PHP_METHOD(Redis, geosearchstore)
{
    zval *object;
    redis_object *redis;
    char *dest = NULL, *src = NULL, *unit = NULL;
    size_t dest_len, src_len, unit_len;
    zval *from, *options = NULL;
    double radius;
    long result_value;

    /* Parse parameters for simple case: geosearchstore(dest, src, member, radius, unit [, options]) */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osszds|a",
                                     &object, redis_ce, &dest, &dest_len,
                                     &src, &src_len, &from, &radius,
                                     &unit, &unit_len, &options) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the GEOSEARCHSTORE command using the Glide client */
        if (execute_geosearchstore_command(redis->glide_client, dest, dest_len, src, src_len,
                                           from, &radius, unit, unit_len, options, &result_value))
        {
            /* Command succeeded, return the value */
            RETURN_LONG(result_value);
        }
        else
        {
            /* Command failed */
            RETURN_FALSE;
        }
    }
    RETURN_FALSE;
}
/* }}} */
