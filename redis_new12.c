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
#include "redis_glide_s_common.h"

/* Forward declarations for the Glide execute functions */

extern int execute_servername_command(const void *glide_client, char **output, size_t *output_len);
extern int execute_serverversion_command(const void *glide_client, char **output, size_t *output_len);
extern int execute_scan_command(const void *glide_client, long *it, const char *pattern, size_t pattern_len,
                                long count, zval *return_value);
extern int execute_sscan_command(const void *glide_client, const char *key, size_t key_len,
                                 long *it, const char *pattern, size_t pattern_len,
                                 long count, zval *return_value);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto long Redis::sAdd(string key, string member, ...) */
SADD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::sAddArray(string key, array values) */
SADD_ARRAY_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::scard(string key) */
SCARD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::srem(string key, string member, ...) */
SREM_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto bool Redis::sMove(string src, string dst, string member) */
SMOVE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string|array Redis::sPop(string key, [long count]) */
SPOP_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string|array Redis::sRandMember(string key, [long count]) */
SRANDMEMBER_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto bool Redis::sismember(string key, string member) */
SISMEMBER_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::sMembers(string key) */
SMEMBERS_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::sMisMember(string key, array members) */
SMISMEMBER_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::sInter(string key, ...) */
SINTER_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::sintercard(array keys, [long limit]) */
SINTERCARD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::sInterStore(string dst, string key1, ...) */
SINTERSTORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::sUnion(string key, ...) */
SUNION_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::sUnionStore(string dst, string key1, ...) */
SUNIONSTORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::sDiff(string key, ...) */
SDIFF_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::sDiffStore(string dst, string key1, ...) */
SDIFFSTORE_METHOD_IMPL(Redis)
/* }}} */
