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

/* {{{ proto string Redis::serverName() */
SERVERNAME_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string Redis::serverVersion() */
SERVERVERSION_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::scan(long &iterator [, string pattern, long count]) */
SCAN_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::sscan(string key, long &iterator [, string pattern, long count]) */
SSCAN_METHOD_IMPL(Redis)
/* }}} */
