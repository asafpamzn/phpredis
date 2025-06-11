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
#include "valkey_glide_x_common.h"

/* Forward declarations for the Glide execute functions */

extern int execute_xadd_command(zval *object, int argc, zval *return_value);

extern int execute_xclaim_command(zval *object, int argc, zval *return_value);
extern int execute_xdel_command(zval *object, int argc, zval *return_value);
extern int execute_xgroup_command(zval *object, int argc, zval *return_value);
extern int execute_xinfo_command(zval *object, int argc, zval *return_value);
extern int execute_xlen_command(zval *object, int argc, zval *return_value);
extern int execute_xpending_command(zval *object, int argc, zval *return_value);
extern int execute_xrange_command(zval *object, int argc, zval *return_value);
extern int execute_xread_command(zval *object, int argc, zval *return_value);

extern int execute_xrevrange_command(zval *object, int argc, zval *return_value);
extern int execute_xtrim_command(zval *object, int argc, zval *return_value);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto long Redis::xack(string key, string group, array ids) */
XACK_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string Redis::xadd(string key, string id, array field_values [, int maxlen [, bool approximate]]) */
XADD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::xautoclaim(string key, string group, string consumer, int min_idle_time, string start [, array options]) */
XAUTOCLAIM_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::xclaim(string key, string group, string consumer, int min_idle_time, array ids [, array options]) */
XCLAIM_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::xdel(string key, array ids) */
XDEL_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto mixed Redis::xgroup(string op, [string key, string group, ...]) */
XGROUP_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto mixed Redis::xinfo(string op, [string key, string group, ...]) */
XINFO_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::xlen(string key) */
XLEN_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::xpending(string key, string group [, array options OR string start, string end, int count [, string consumer]]) */
XPENDING_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::xrange(string key, string start, string end [, int count [, array options]]) */
XRANGE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::xread(array streams_and_ids [, int count [, int block]]) */
XREAD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::xreadgroup(string group, string consumer, array streams [, int count [, array options]]) */
XREADGROUP_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::xrevrange(string key, string end, string start [, int count [, array options]]) */
XREVRANGE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::xtrim(string key, string threshold, bool approx = false, bool minid = false, int limit = -1) */
XTRIM_METHOD_IMPL(Redis)
/* }}} */
