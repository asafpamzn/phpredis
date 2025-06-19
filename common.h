#include "php.h"
#include "php_ini.h"

#ifndef REDIS_COMMON_H
#define REDIS_COMMON_H

#include <stdio.h>
#include <zend_smart_str.h>
#include <ext/standard/php_smart_string.h>

#define PHPREDIS_GET_OBJECT(class_entry, o) (class_entry *)((char *)o - XtOffsetOf(class_entry, std))
#define PHPREDIS_ZVAL_GET_OBJECT(class_entry, z) PHPREDIS_GET_OBJECT(class_entry, Z_OBJ_P(z))

/* NULL check so Eclipse doesn't go crazy */
#ifndef NULL
#define NULL ((void *)0)
#endif

/* We'll fallthrough if we want to */
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif
#if __has_attribute(__fallthrough__)
#define REDIS_FALLTHROUGH __attribute__((__fallthrough__))
#else
#define REDIS_FALLTHROUGH \
    do                    \
    {                     \
    } while (0)
#endif

/* ValkeyGlide data types for compatibility */
#define REDIS_NOT_FOUND 0
#define REDIS_STRING 1
#define REDIS_SET 2
#define REDIS_LIST 3
#define REDIS_ZSET 4
#define REDIS_HASH 5
#define REDIS_STREAM 6

#ifdef PHP_WIN32
#define PHP_REDIS_API __declspec(dllexport)
#else
#define PHP_REDIS_API
#endif

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

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
