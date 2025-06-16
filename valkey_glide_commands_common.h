/*
  +----------------------------------------------------------------------+
  | Valkey Glide Commands Common Framework                               |
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

#ifndef VALKEY_GLIDE_COMMANDS_COMMON_H
#define VALKEY_GLIDE_COMMANDS_COMMON_H

#include "php_redis.h"

/* ====================================================================
 * UNIFIED COMMAND FUNCTION DECLARATIONS (Now in redis_glide.c)
 * ==================================================================== */

/* These functions are now implemented directly in redis_glide.c with unified signatures */
/* No separate unified wrapper functions needed - cleaner architecture! */

/* Function declarations from redis_glide.h - included here for reference */
extern int execute_echo_command(zval *object, int argc, zval *return_value);
extern int execute_bitop_command(zval *object, int argc, zval *return_value);
extern int execute_getbit_command(zval *object, int argc, zval *return_value);
extern int execute_setbit_command(zval *object, int argc, zval *return_value);
extern int execute_bitcount_command(zval *object, int argc, zval *return_value);
extern int execute_bitpos_command(zval *object, int argc, zval *return_value);

/* DEL command uses different signature - handled separately */

extern int execute_del_array(const void *glide_client, HashTable *keys_hash, long *output_value);

/* ====================================================================
 * METHOD IMPLEMENTATION MACROS
 * ==================================================================== */

#define ECHO_METHOD_IMPL(class_name)                                        \
    PHP_METHOD(class_name, echo)                                            \
    {                                                                       \
        if (execute_echo_command(getThis(), ZEND_NUM_ARGS(), return_value)) \
        {                                                                   \
            return;                                                         \
        }                                                                   \
        zval_dtor(return_value);                                            \
        RETURN_FALSE;                                                       \
    }

#define BITOP_METHOD_IMPL(class_name)                                        \
    PHP_METHOD(class_name, bitop)                                            \
    {                                                                        \
        if (execute_bitop_command(getThis(), ZEND_NUM_ARGS(), return_value)) \
        {                                                                    \
            return;                                                          \
        }                                                                    \
        zval_dtor(return_value);                                             \
        RETURN_FALSE;                                                        \
    }

#define GETBIT_METHOD_IMPL(class_name)                                        \
    PHP_METHOD(class_name, getBit)                                            \
    {                                                                         \
        if (execute_getbit_command(getThis(), ZEND_NUM_ARGS(), return_value)) \
        {                                                                     \
            return;                                                           \
        }                                                                     \
        zval_dtor(return_value);                                              \
        RETURN_FALSE;                                                         \
    }

#define SETBIT_METHOD_IMPL(class_name)                                        \
    PHP_METHOD(class_name, setBit)                                            \
    {                                                                         \
        if (execute_setbit_command(getThis(), ZEND_NUM_ARGS(), return_value)) \
        {                                                                     \
            return;                                                           \
        }                                                                     \
        zval_dtor(return_value);                                              \
        RETURN_FALSE;                                                         \
    }

#define BITCOUNT_METHOD_IMPL(class_name)                                        \
    PHP_METHOD(class_name, bitcount)                                            \
    {                                                                           \
        if (execute_bitcount_command(getThis(), ZEND_NUM_ARGS(), return_value)) \
        {                                                                       \
            return;                                                             \
        }                                                                       \
        zval_dtor(return_value);                                                \
        RETURN_FALSE;                                                           \
    }

#define BITPOS_METHOD_IMPL(class_name)                                        \
    PHP_METHOD(class_name, bitpos)                                            \
    {                                                                         \
        if (execute_bitpos_command(getThis(), ZEND_NUM_ARGS(), return_value)) \
        {                                                                     \
            return;                                                           \
        }                                                                     \
        zval_dtor(return_value);                                              \
        RETURN_FALSE;                                                         \
    }

/* DEL command needs special handling since it has different signature */
#define DEL_METHOD_IMPL(class_name)                                        \
    PHP_METHOD(class_name, del)                                            \
    {                                                                      \
        if (execute_del_command(getThis(), ZEND_NUM_ARGS(), return_value)) \
        {                                                                  \
            return;                                                        \
        }                                                                  \
        zval_dtor(return_value);                                           \
        RETURN_FALSE;                                                      \
    }

/* Additional unified macros for new converted commands */
#define GET_METHOD_IMPL(class_name)                                        \
    PHP_METHOD(class_name, get)                                            \
    {                                                                      \
        if (execute_get_command(getThis(), ZEND_NUM_ARGS(), return_value)) \
        {                                                                  \
            return;                                                        \
        }                                                                  \
        zval_dtor(return_value);                                           \
        RETURN_FALSE;                                                      \
    }

#define RANDOMKEY_METHOD_IMPL(class_name)                                        \
    PHP_METHOD(class_name, randomKey)                                            \
    {                                                                            \
        if (execute_randomkey_command(getThis(), ZEND_NUM_ARGS(), return_value)) \
        {                                                                        \
            return;                                                              \
        }                                                                        \
        zval_dtor(return_value);                                                 \
        RETURN_FALSE;                                                            \
    }

#define STRLEN_METHOD_IMPL(class_name)                                        \
    PHP_METHOD(class_name, strlen)                                            \
    {                                                                         \
        if (execute_strlen_command(getThis(), ZEND_NUM_ARGS(), return_value)) \
        {                                                                     \
            return;                                                           \
        }                                                                     \
        zval_dtor(return_value);                                              \
        RETURN_FALSE;                                                         \
    }

#define TTL_METHOD_IMPL(class_name)                                        \
    PHP_METHOD(class_name, ttl)                                            \
    {                                                                      \
        if (execute_ttl_command(getThis(), ZEND_NUM_ARGS(), return_value)) \
        {                                                                  \
            return;                                                        \
        }                                                                  \
        zval_dtor(return_value);                                           \
        RETURN_FALSE;                                                      \
    }

#define PTTL_METHOD_IMPL(class_name)                                        \
    PHP_METHOD(class_name, pttl)                                            \
    {                                                                       \
        if (execute_pttl_command(getThis(), ZEND_NUM_ARGS(), return_value)) \
        {                                                                   \
            return;                                                         \
        }                                                                   \
        zval_dtor(return_value);                                            \
        RETURN_FALSE;                                                       \
    }

#define PING_METHOD_IMPL(class_name)                                        \
    PHP_METHOD(class_name, ping)                                            \
    {                                                                       \
        if (execute_ping_command(getThis(), ZEND_NUM_ARGS(), return_value)) \
        {                                                                   \
            return;                                                         \
        }                                                                   \
        zval_dtor(return_value);                                            \
        RETURN_FALSE;                                                       \
    }

#define INFO_METHOD_IMPL(class_name)                                        \
    PHP_METHOD(class_name, info)                                            \
    {                                                                       \
        if (execute_info_command(getThis(), ZEND_NUM_ARGS(), return_value)) \
        {                                                                   \
            return;                                                         \
        }                                                                   \
        zval_dtor(return_value);                                            \
        RETURN_FALSE;                                                       \
    }

#endif /* VALKEY_GLIDE_COMMANDS_COMMON_H */
