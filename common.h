#include "php.h"
#include "php_ini.h"

#ifndef VALKEY_GLIDE_COMMON_H
#define VALKEY_GLIDE_COMMON_H

#include <stdio.h>
#include <zend_smart_str.h>
#include <ext/standard/php_smart_string.h>

/* ValkeyGlidePHP version */
#define VALKEY_GLIDE_PHP_VERSION "0.1"

#define VALKEY_GLIDE_PHP_GET_OBJECT(class_entry, o) (class_entry *)((char *)o - XtOffsetOf(class_entry, std))
#define VALKEY_GLIDE_PHP_ZVAL_GET_OBJECT(class_entry, z) VALKEY_GLIDE_PHP_GET_OBJECT(class_entry, Z_OBJ_P(z))

/* NULL check so Eclipse doesn't go crazy */
#ifndef NULL
#define NULL ((void *)0)
#endif

/* We'll fallthrough if we want to */
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

/* ValkeyGlide data types for compatibility */
#define VALKEY_GLIDE_NOT_FOUND 0
#define VALKEY_GLIDE_STRING 1
#define VALKEY_GLIDE_SET 2
#define VALKEY_GLIDE_LIST 3
#define VALKEY_GLIDE_ZSET 4
#define VALKEY_GLIDE_HASH 5
#define VALKEY_GLIDE_STREAM 6

#if PHP_VERSION_ID < 80000
#define Z_PARAM_ARRAY_HT_OR_NULL(dest) \
    Z_PARAM_ARRAY_HT_EX(dest, 1, 0)
#define Z_PARAM_STR_OR_NULL(dest) \
    Z_PARAM_STR_EX(dest, 1, 0)
#define Z_PARAM_ZVAL_OR_NULL(dest) \
    Z_PARAM_ZVAL_EX(dest, 1, 0)
#define Z_PARAM_BOOL_OR_NULL(dest, is_null) \
    Z_PARAM_BOOL_EX(dest, is_null, 1, 0)
#endif

typedef struct
{
    const void *glide_client; /* Valkey Glide client pointer */
    zend_object std;
} valkey_glide_object;

/* For convenience we store the salt as a printable hex string which requires 2
 * characters per byte + 1 for the NULL terminator */
#define REDIS_SALT_BYTES 32
#define REDIS_SALT_SIZE ((2 * REDIS_SALT_BYTES) + 1)

ZEND_BEGIN_MODULE_GLOBALS(redis)
char salt[REDIS_SALT_SIZE];
ZEND_END_MODULE_GLOBALS(redis)

ZEND_EXTERN_MODULE_GLOBALS(redis)
#define REDIS_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(redis, v)

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(redis);
PHP_MSHUTDOWN_FUNCTION(redis);
PHP_MINFO_FUNCTION(redis);

zend_class_entry *get_valkey_glide_ce(void);
zend_class_entry *get_valkey_glide_exception_ce(void);

#endif // VALKEY_GLIDE
