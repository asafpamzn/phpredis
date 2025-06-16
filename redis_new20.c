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
#include "valkey_glide_commands_common.h"

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto long Redis::copy(string $source, string $destination, array $options = null) */
COPY_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::hscan(string key, long &iterator, [string pattern, [long count]]) */
HSCAN_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::pfadd(string key, array elements) */
PFADD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::pfcount(string key[, string key2, string key3...]) */
PFCOUNT_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto bool Redis::pfmerge(string dst, array keys) */
PFMERGE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto double Redis::getTimeout() */
GETTIMEOUT_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto double Redis::getReadTimeout() */
GETREADTIMEOUT_METHOD_IMPL(Redis)
/* }}} */
