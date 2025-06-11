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
