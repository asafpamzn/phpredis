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

#include "valkey_glide_commands_common.h"

/* Enum support includes - must be BEFORE arginfo includes */
#if PHP_VERSION_ID >= 80100
#include "zend_API.h"
#include "zend_compile.h"
#include "zend_object_handlers.h"
#include "zend_objects.h"
#include "zend_enum.h"
#endif
#include <ext/spl/spl_exceptions.h>
#include <zend_exceptions.h>
#include <ext/standard/info.h>

/* Include configuration parsing */
extern int parse_valkey_glide_client_configuration(zval *config_obj, valkey_glide_client_configuration_t *config);
extern void free_valkey_glide_client_configuration(valkey_glide_client_configuration_t *config);

zend_class_entry *valkey_glide_ce;
zend_class_entry *valkey_glide_exception_ce;

zend_class_entry *valkey_glide_cluster_ce;

/* Exception handler */
zend_class_entry *valkey_glide_cluster_exception_ce;

/* Handlers for ValkeyGlideCluster */
zend_object_handlers valkey_glide_cluster_object_handlers;

zend_class_entry *get_valkey_glide_ce(void)
{
    return valkey_glide_ce;
}

zend_class_entry *get_valkey_glide_exception_ce(void)
{
    return valkey_glide_exception_ce;
}

zend_class_entry *get_valkey_glide_cluster_ce(void)
{
    return valkey_glide_cluster_ce;
}

zend_class_entry *get_valkey_glide_cluster_exception_ce(void)
{
    return valkey_glide_cluster_exception_ce;
}

#if PHP_VERSION_ID < 80000
#include "redis_legacy_arginfo.h"
#include "redis_cluster_legacy_arginfo.h"
#else
#include "zend_attributes.h"
#include "redis_arginfo.h"
#include "redis_cluster_arginfo.h"
#endif

PHP_MINIT_FUNCTION(redis_cluster);
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
    VALKEY_GLIDE_PHP_VERSION,
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
    valkey_glide_object *valkey_glide = VALKEY_GLIDE_PHP_GET_OBJECT(valkey_glide_object, object);

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
    config.read_from_ = CONNECTION_REQUEST__READ_FROM__Primary;
    config.is_cluster = false;
    config.port_ = 6379; // Default port for Valkey Glide
    valkey_glide->glide_client = create_glide_client(&config);

    zend_object_std_init(&valkey_glide->std, ce);
    object_properties_init(&valkey_glide->std, ce);

    memcpy(&valkey_glide_object_handlers, zend_get_std_object_handlers(), sizeof(valkey_glide_object_handlers));
    valkey_glide_object_handlers.offset = XtOffsetOf(valkey_glide_object, std);
    valkey_glide_object_handlers.free_obj = free_valkey_glide_object;
    valkey_glide->std.handlers = &valkey_glide_object_handlers;

    return &valkey_glide->std;
}

zend_object *create_valkey_glide_cluster_object(zend_class_entry *ce)
{
    valkey_glide_object *valkey_glide = ecalloc(1, sizeof(valkey_glide_object) + zend_object_properties_size(ce));

    /* Initialize Valkey Glide client */
    ClientConfig config;
    config.tls_mode_ = false;
    config.database_ = 0;
    config.request_timeout_ = 250;
    config.client_name_ = "valkey-glide-php";
    config.read_from_ = CONNECTION_REQUEST__READ_FROM__Primary;
    config.is_cluster = true;
    config.port_ = 7001;

    valkey_glide->glide_client = create_glide_client(&config);

    zend_object_std_init(&valkey_glide->std, ce);
    object_properties_init(&valkey_glide->std, ce);

    memcpy(&valkey_glide_cluster_object_handlers, zend_get_std_object_handlers(), sizeof(valkey_glide_cluster_object_handlers));
    valkey_glide_cluster_object_handlers.offset = XtOffsetOf(valkey_glide_object, std);
    valkey_glide_cluster_object_handlers.free_obj = free_valkey_glide_object;
    valkey_glide->std.handlers = &valkey_glide_cluster_object_handlers;

    return &valkey_glide->std;
}

/**
 * PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(redis)
{
    /* ValkeyGlide class */
    valkey_glide_ce = register_class_ValkeyGlide();
    valkey_glide_ce->create_object = create_valkey_glide_object;

    /* ValkeyGlideException class */
    valkey_glide_exception_ce = register_class_ValkeyGlideException(spl_ce_RuntimeException);

    valkey_glide_cluster_ce = register_class_ValkeyGlideCluster();

    valkey_glide_cluster_exception_ce = register_class_ValkeyGlideClusterException(spl_ce_RuntimeException);

    valkey_glide_cluster_ce->create_object = create_valkey_glide_cluster_object;

    return SUCCESS;
}

/**
 * PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(redis)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "Valkey Glide Support", "enabled");
    php_info_print_table_row(2, "Valkey Glide Version", VALKEY_GLIDE_PHP_VERSION);
    php_info_print_table_end();
}

/* {{{ proto ValkeyGlide ValkeyGlide::__construct(array $addresses, bool $use_tls, ?array $credentials, ValkeyGlideReadFrom $read_from, ?int $request_timeout, ?array $reconnect_strategy, ?int $database_id, ?string $client_name, ?int $inflight_requests_limit, ?string $client_az, ?array $advanced_config, ?bool $lazy_connect)
    Public constructor */
PHP_METHOD(ValkeyGlide, __construct)
{
    zval *addresses = NULL;
    zend_bool use_tls = 0;
    zval *credentials = NULL;
    zend_long read_from = 0; /* PRIMARY by default */
    zval *request_timeout = NULL;
    zval *reconnect_strategy = NULL;
    zval *database_id = NULL;
    char *client_name = NULL;
    size_t client_name_len = 0;
    zval *inflight_requests_limit = NULL;
    char *client_az = NULL;
    size_t client_az_len = 0;
    zval *advanced_config = NULL;
    zval *lazy_connect = NULL;
    valkey_glide_object *valkey_glide;

    ZEND_PARSE_PARAMETERS_START(1, 12)
    Z_PARAM_ARRAY(addresses)
    Z_PARAM_OPTIONAL
    Z_PARAM_BOOL(use_tls)
    Z_PARAM_ARRAY_OR_NULL(credentials)
    Z_PARAM_LONG(read_from)
    Z_PARAM_ARRAY_OR_NULL(request_timeout)
    Z_PARAM_ARRAY_OR_NULL(reconnect_strategy)
    Z_PARAM_ARRAY_OR_NULL(database_id)
    Z_PARAM_STRING_OR_NULL(client_name, client_name_len)
    Z_PARAM_ARRAY_OR_NULL(inflight_requests_limit)
    Z_PARAM_STRING_OR_NULL(client_az, client_az_len)
    Z_PARAM_ARRAY_OR_NULL(advanced_config)
    Z_PARAM_ARRAY_OR_NULL(lazy_connect)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_THROWS());

    valkey_glide = VALKEY_GLIDE_PHP_ZVAL_GET_OBJECT(valkey_glide_object, getThis());

    /* Build client configuration from individual parameters */
    ClientConfig client_config;
    client_config.tls_mode_ = use_tls;
    client_config.database_ = database_id ? Z_LVAL_P(database_id) : 0;
    client_config.request_timeout_ = request_timeout ? Z_LVAL_P(request_timeout) : 250;
    client_config.client_name_ = client_name ? client_name : "valkey-glide-php";

    /* Map read_from enum value to client's ReadFrom enum */
    switch (read_from)
    {
    case 1: /* PREFER_REPLICA */
        client_config.read_from_ = CONNECTION_REQUEST__READ_FROM__PreferReplica;
        break;
    case 2: /* AZ_AFFINITY */
        client_config.read_from_ = CONNECTION_REQUEST__READ_FROM__AZAffinity;
        break;
    case 3: /* AZ_AFFINITY_REPLICAS_AND_PRIMARY */
        client_config.read_from_ = CONNECTION_REQUEST__READ_FROM__AZAffinityReplicasAndPrimary;
        break;
    case 0: /* PRIMARY */
    default:
        client_config.read_from_ = CONNECTION_REQUEST__READ_FROM__Primary;
        break;
    }
    client_config.is_cluster = false;

    /* Extract port from addresses array - use first address or default */
    if (addresses && zend_hash_num_elements(Z_ARRVAL_P(addresses)) > 0)
    {
        zval *first_addr = zend_hash_index_find(Z_ARRVAL_P(addresses), 0);
        if (first_addr && Z_TYPE_P(first_addr) == IS_ARRAY)
        {
            zval *port_val = zend_hash_str_find(Z_ARRVAL_P(first_addr), "port", 4);
            if (port_val && Z_TYPE_P(port_val) == IS_LONG)
            {
                client_config.port_ = Z_LVAL_P(port_val);
            }
            else
            {
                client_config.port_ = 6379; /* Default port */
            }
        }
        else
        {
            client_config.port_ = 6379; /* Default port */
        }
    }
    else
    {
        client_config.port_ = 6379; /* Default port */
    }

    valkey_glide->glide_client = create_glide_client(&client_config);
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
