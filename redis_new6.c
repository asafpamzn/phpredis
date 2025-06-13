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
#include "redis_glide_list_common.h"
#include "command_response.h"
#include <ext/spl/spl_exceptions.h>
#include <zend_exceptions.h>
#include <ext/standard/info.h>

#ifdef PHP_SESSION
#include <ext/session/php_session.h>
#endif

/* Import needed external variables */
extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* {{{ proto long Redis::lPush(string key, mixed value1, mixed value2, mixed valueN) */
LPUSH_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::lPushx(string key, mixed value) */
LPUSHX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::rPushx(string key, mixed value) */
RPUSHX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string|array Redis::lPop(string key [, int count]) */
LPOP_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string|array Redis::rPop(string key [, int count]) */
RPOP_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::blPop(array keys, double timeout) */
BLPOP_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::brPop(array keys, double timeout) */
BRPOP_METHOD_IMPL(Redis)
/* }}} */
