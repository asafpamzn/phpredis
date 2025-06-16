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
#include "valkey_glide_z_common.h"
#include "valkey_glide_list_common.h"
#include "valkey_glide_commands_common.h"
#include "command_response.h" /* Include command_response.h for string conversion functions */
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

/* Import the string conversion functions from command_response.c */
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

/* {{{ proto string Redis::echo(string msg) */
ECHO_METHOD_IMPL(Redis)
/* }}} */

BITOP_METHOD_IMPL(Redis)

/* }}} */

/* {{{ proto long Redis::getBit(string key, long offset) */
GETBIT_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::setBit(string key, long offset, int value) */
SETBIT_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::del(string key, ...) or Redis::del(array keys) */
DEL_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::bitcount(string key, [int start], [int end])
 */
BITCOUNT_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto integer Redis::bitpos(string key, int bit, [int start, int end]) */
BITPOS_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto boolean Redis::set(string key, mixed val, double|int|array timeout,
 *                              [array opt) */
SET_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto boolean Redis::setex(string key, long expire, string value)
 */
SETEX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto boolean Redis::psetex(string key, long expire, string value)
 */
PSETEX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto boolean Redis::setnx(string key, string value)
 */
SETNX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string Redis::getSet(string key, string value)
 */
GETSET_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string Redis::get(string key) */
GET_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string Redis::randomKey()
 */
RANDOMKEY_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto mixed Redis::lcs(string $key1, string $key2, ?array $options = NULL); */
LCS_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string Redis::setRange(string key, long start, string value) */
SETRANGE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::strlen(string key) */
STRLEN_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::info([string section [, string section...]]) */
INFO_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::ttl(string key) */
TTL_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::pttl(string key) */
PTTL_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string Redis::ping([string message])
 */
PING_METHOD_IMPL(Redis)
/* }}} */

/** {{{ proto bool Redis::reset()
 */
PHP_METHOD(Redis, reset)
{
    RETURN_FALSE;
    // TODO
}
/* }}} */
