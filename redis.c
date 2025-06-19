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
#include "redis_glide.h"
#include <ext/spl/spl_exceptions.h>
#include <zend_exceptions.h>
#include <ext/standard/info.h>

zend_class_entry *redis_ce;
zend_class_entry *redis_exception_ce;

#if PHP_VERSION_ID < 80000
#include "redis_legacy_arginfo.h"
#else
#include "zend_attributes.h"
#include "redis_arginfo.h"
#endif

extern const zend_function_entry *redis_get_methods(void)
{
    return class_ValkeyGlide_methods;
}

ZEND_DECLARE_MODULE_GLOBALS(redis)

zend_module_entry redis_module_entry = {
    STANDARD_MODULE_HEADER,
    "redis",
    NULL,
    PHP_MINIT(redis),
    NULL,
    NULL,
    NULL,
    PHP_MINFO(redis),
    PHP_REDIS_VERSION,
    PHP_MODULE_GLOBALS(redis),
    NULL,
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX};

#ifdef COMPILE_DL_REDIS
ZEND_GET_MODULE(redis)
#endif

zend_object_handlers valkey_glide_object_handlers;

void free_valkey_glide_object(zend_object *object)
{
    valkey_glide_object *valkey_glide = PHPREDIS_GET_OBJECT(valkey_glide_object, object);

    /* Free the Valkey Glide client if it exists */
    if (valkey_glide->glide_client)
    {
        close_glide_client(valkey_glide->glide_client);
        valkey_glide->glide_client = NULL;
    }
}

zend_object *create_valkey_glide_object(zend_class_entry *ce)
{
    valkey_glide_object *valkey_glide = ecalloc(1, sizeof(valkey_glide_object) + zend_object_properties_size(ce));

    /* Initialize Valkey Glide client */
    ClientConfig config;
    config.tls_mode_ = false;
    config.database_ = 0;
    config.request_timeout_ = 250;
    config.client_name_ = "valkey-glide-php";
    config.read_from_ = Primary;
    config.is_cluster = false;
    valkey_glide->glide_client = create_glide_client(&config);

    zend_object_std_init(&valkey_glide->std, ce);
    object_properties_init(&valkey_glide->std, ce);

    memcpy(&valkey_glide_object_handlers, zend_get_std_object_handlers(), sizeof(valkey_glide_object_handlers));
    valkey_glide_object_handlers.offset = XtOffsetOf(valkey_glide_object, std);
    valkey_glide_object_handlers.free_obj = free_valkey_glide_object;
    valkey_glide->std.handlers = &valkey_glide_object_handlers;

    return &valkey_glide->std;
}

/**
 * PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(redis)
{
    /* ValkeyGlide class */
    redis_ce = register_class_ValkeyGlide();
    redis_ce->create_object = create_valkey_glide_object;

    /* ValkeyGlideException class */
    redis_exception_ce = register_class_ValkeyGlideException(spl_ce_RuntimeException);

    return SUCCESS;
}

/**
 * PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(redis)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "Valkey Glide Support", "enabled");
    php_info_print_table_row(2, "Valkey Glide Version", PHP_REDIS_VERSION);
    php_info_print_table_end();
}

/* {{{ proto ValkeyGlide ValkeyGlide::__construct(array $options = null)
    Public constructor */
PHP_METHOD(ValkeyGlide, __construct)
{
    HashTable *opts = NULL;
    valkey_glide_object *valkey_glide;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_ARRAY_HT_OR_NULL(opts)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_THROWS());

    valkey_glide = PHPREDIS_ZVAL_GET_OBJECT(valkey_glide_object, getThis());
    /* Options handling can be added here as needed */
}
/* }}} */

/* {{{ proto ValkeyGlide ValkeyGlide::__destruct()
    Public Destructor
 */
PHP_METHOD(ValkeyGlide, __destruct)
{
    if (zend_parse_parameters_none() == FAILURE)
    {
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto boolean ValkeyGlide::close()
 */
PHP_METHOD(ValkeyGlide, close)
{
    /* TODO: Implement ValkeyGlide close */
    RETURN_TRUE;
}
/* }}} */

/* Basic method stubs - these need to be implemented with ValkeyGlide */
PHP_METHOD(ValkeyGlide, pipeline) { /* TODO: Implement */ }
PHP_METHOD(ValkeyGlide, publish) { /* TODO: Implement */ }
PHP_METHOD(ValkeyGlide, psubscribe) { /* TODO: Implement */ }
PHP_METHOD(ValkeyGlide, ssubscribe) { /* TODO: Implement */ }
PHP_METHOD(ValkeyGlide, subscribe) { /* TODO: Implement */ }
PHP_METHOD(ValkeyGlide, unsubscribe) { /* TODO: Implement */ }
PHP_METHOD(ValkeyGlide, punsubscribe) { /* TODO: Implement */ }
PHP_METHOD(ValkeyGlide, sunsubscribe) { /* TODO: Implement */ }

PHP_METHOD(ValkeyGlide, pubsub) { /* TODO: Implement */ }
PHP_METHOD(ValkeyGlide, eval) { /* TODO: Implement */ }
PHP_METHOD(ValkeyGlide, eval_ro) { /* TODO: Implement */ }
PHP_METHOD(ValkeyGlide, evalsha) { /* TODO: Implement */ }
PHP_METHOD(ValkeyGlide, evalsha_ro) { /* TODO: Implement */ }
PHP_METHOD(ValkeyGlide, script) { /* TODO: Implement */ }
