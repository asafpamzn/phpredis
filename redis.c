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
#include <ext/hash/php_hash.h>

#if PHP_VERSION_ID < 80400
#include <ext/standard/php_random.h>
#else
#include <ext/random/php_random.h>
#endif

#ifdef PHP_SESSION
#include <ext/session/php_session.h>
#endif

#ifdef HAVE_REDIS_ZSTD
#include <zstd.h>
#endif

#ifdef HAVE_REDIS_LZ4
#include <lz4.h>
#endif

#ifdef PHP_SESSION
extern ps_module ps_mod_redis;
extern ps_module ps_mod_redis_cluster;
#endif

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
    return class_Redis_methods;
}

extern int le_cluster_slot_cache;
int le_redis_pconnect;

PHP_INI_BEGIN()
/* redis arrays */
PHP_INI_ENTRY("redis.arrays.algorithm", "", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.arrays.auth", "", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.arrays.autorehash", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.arrays.connecttimeout", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.arrays.distributor", "", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.arrays.functions", "", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.arrays.hosts", "", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.arrays.index", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.arrays.lazyconnect", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.arrays.names", "", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.arrays.pconnect", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.arrays.previous", "", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.arrays.readtimeout", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.arrays.retryinterval", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.arrays.consistent", "0", PHP_INI_ALL, NULL)

/* redis cluster */
PHP_INI_ENTRY("redis.clusters.cache_slots", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.clusters.auth", "", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.clusters.persistent", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.clusters.read_timeout", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.clusters.seeds", "", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.clusters.timeout", "0", PHP_INI_ALL, NULL)

/* redis pconnect */
PHP_INI_ENTRY("redis.pconnect.pooling_enabled", "1", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.pconnect.connection_limit", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.pconnect.echo_check_liveness", "1", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.pconnect.pool_detect_dirty", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.pconnect.pool_poll_timeout", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.pconnect.pool_pattern", "", PHP_INI_ALL, NULL)

/* redis session */
PHP_INI_ENTRY("redis.session.locking_enabled", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.session.lock_expire", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.session.lock_retries", "100", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.session.lock_wait_time", "20000", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.session.early_refresh", "0", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.session.compression", "none", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("redis.session.compression_level", "3", PHP_INI_ALL, NULL)
PHP_INI_END()

static const zend_module_dep redis_deps[] = {
#ifdef HAVE_REDIS_IGBINARY
    ZEND_MOD_REQUIRED("igbinary")
#endif
#ifdef HAVE_REDIS_MSGPACK
        ZEND_MOD_REQUIRED("msgpack")
#endif
#ifdef HAVE_REDIS_JSON
            ZEND_MOD_REQUIRED("json")
#endif
#ifdef PHP_SESSION
                ZEND_MOD_REQUIRED("session")
#endif
                    ZEND_MOD_END};

ZEND_DECLARE_MODULE_GLOBALS(redis)

zend_module_entry redis_module_entry = {
    STANDARD_MODULE_HEADER_EX,
    NULL,
    redis_deps,
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

zend_object_handlers redis_object_handlers;

/* Send a static DISCARD in case we're in MULTI mode. */

/* {{{ proto boolean Redis::close()
 */
PHP_METHOD(Redis, close)
{

    RETURN_FALSE;
}

/* Passthru for destroying cluster cache */
static void cluster_cache_dtor(zend_resource *rsrc)
{
}

void free_redis_object(zend_object *object)
{
    redis_object *redis = PHPREDIS_GET_OBJECT(redis_object, object);

    /* Free the Valkey Glide client if it exists */
    if (redis->glide_client)
    {
        close_glide_client(redis->glide_client);
        redis->glide_client = NULL;
    }
}

zend_object *
create_redis_object(zend_class_entry *ce)
{

    redis_object *redis = ecalloc(1, sizeof(redis_object) + zend_object_properties_size(ce));

    redis->sock = NULL;
    ClientConfig config;
    config.tls_mode_ = false;
    config.database_ = 0;
    config.request_timeout_ = 250;
    config.client_name_ = "stam";
    config.read_from_ = Primary;
    config.is_cluster = false;
    redis->glide_client = create_glide_client(&config); /* Initialize Valkey Glide client pointer to NULL */

    zend_object_std_init(&redis->std, ce);
    object_properties_init(&redis->std, ce);

    memcpy(&redis_object_handlers, zend_get_std_object_handlers(), sizeof(redis_object_handlers));
    redis_object_handlers.offset = XtOffsetOf(redis_object, std);
    redis_object_handlers.free_obj = free_redis_object;
    redis->std.handlers = &redis_object_handlers;

    return &redis->std;
}

static ZEND_RSRC_DTOR_FUNC(redis_connections_pool_dtor)
{
    if (res->ptr)
    {
        ConnectionPool *p = res->ptr;
        zend_llist_destroy(&p->list);
        pefree(res->ptr, 1);
    }
}

static void redis_random_hex_bytes(char *dst, size_t dstsize)
{
    char chunk[9], *ptr = dst;
    ssize_t rem = dstsize, len, clen;
    size_t bytes;

    /* We need two characters per hex byte */
    bytes = dstsize / 2;
    zend_string *s = zend_string_alloc(bytes, 0);

    /* First try to have PHP generate the bytes */
    if (php_random_bytes_silent(ZSTR_VAL(s), bytes) == SUCCESS)
    {
        php_hash_bin2hex(dst, (unsigned char *)ZSTR_VAL(s), bytes);
        zend_string_release(s);
        return;
    }

    /* PHP shouldn't have failed, but generate manually if it did. */
    while (rem > 0)
    {
        clen = snprintf(chunk, sizeof(chunk), "%08x", rand());
        len = rem >= clen ? clen : rem;
        memcpy(ptr, chunk, len);
        ptr += len;
        rem -= len;
    }

    zend_string_release(s);
}

/**
 * PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(redis)
{
    struct timeval tv;
    printf("file = %s, line = %d\n", __FILE__, __LINE__);

    /* Seed random generator (for RedisCluster failover) */
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec * tv.tv_sec);

    /* Generate our random salt */
    redis_random_hex_bytes(REDIS_G(salt), sizeof(REDIS_G(salt)) - 1);
    REDIS_G(salt)
    [sizeof(REDIS_G(salt)) - 1] = '\0';

    REGISTER_INI_ENTRIES();

    /* Redis class */
    redis_ce = register_class_Redis();
    redis_ce->create_object = create_redis_object;

    /* RedisCluster class */
    // ZEND_MINIT(redis_cluster)(INIT_FUNC_ARGS_PASSTHRU);

    /* Register our cluster cache list item */
    /* le_cluster_slot_cache = zend_register_list_destructors_ex(NULL, cluster_cache_dtor,
                                                               "Redis cluster slot cache",
                                                               module_number);*/

    /* RedisException class */
    redis_exception_ce = register_class_RedisException(spl_ce_RuntimeException);

    /* Register resource destructors */
    le_redis_pconnect = zend_register_list_destructors_ex(NULL, redis_connections_pool_dtor,
                                                          "phpredis persistent connections pool", module_number);

    return SUCCESS;
}

static const char *
get_available_serializers(void)
{
#ifdef HAVE_REDIS_JSON
#ifdef HAVE_REDIS_IGBINARY
#ifdef HAVE_REDIS_MSGPACK
    return "php, json, igbinary, msgpack";
#else
    return "php, json, igbinary";
#endif
#else
#ifdef HAVE_REDIS_MSGPACK
    return "php, json, msgpack";
#else
    return "php, json";
#endif
#endif
#else
#ifdef HAVE_REDIS_IGBINARY
#ifdef HAVE_REDIS_MSGPACK
    return "php, igbinary, msgpack";
#else
    return "php, igbinary";
#endif
#else
#ifdef HAVE_REDIS_MSGPACK
    return "php, msgpack";
#else
    return "php";
#endif
#endif
#endif
}

/**
 * PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(redis)
{
    smart_str names = {
        0,
    };
    printf("file = %s, line = %d\n", __FILE__, __LINE__);

    php_info_print_table_start();
    php_info_print_table_header(2, "Redis Support", "enabled");
    php_info_print_table_row(2, "Redis Version", PHP_REDIS_VERSION);

    php_info_print_table_row(2, "Available serializers", get_available_serializers());

    if (names.s)
    {
        smart_str_0(&names);
        php_info_print_table_row(2, "Available compression", ZSTR_VAL(names.s));
    }
    smart_str_free(&names);
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}

/* {{{ proto Redis Redis::__construct(array $options = null)
    Public constructor */
PHP_METHOD(Redis, __construct)
{
    HashTable *opts = NULL;
    redis_object *redis;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_ARRAY_HT_OR_NULL(opts)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_THROWS());

    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, getThis());
}
/* }}} */

/* {{{ proto Redis Redis::__destruct()
    Public Destructor
 */
PHP_METHOD(Redis, __destruct)
{
    if (zend_parse_parameters_none() == FAILURE)
    {
        RETURN_FALSE;
    }
}

/* {{{ proto boolean Redis::connect(string host, int port [, double timeout [, long retry_interval]])
 */
PHP_METHOD(Redis, connect)
{
    printf("file = %s, line = %d\n", __FILE__, __LINE__);

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean Redis::pconnect(string host, int port [, double timeout])
 */
PHP_METHOD(Redis, pconnect)
{

    RETURN_TRUE;
}
/* }}} */

PHP_REDIS_API int
redis_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
    return 0;

    return SUCCESS;
}

/* {{{ proto string Redis::save() */
PHP_METHOD(Redis, save)
{
}
/* }}} */

/* {{{ proto string Redis::bgSave() */
PHP_METHOD(Redis, bgSave)
{
}
/* }}} */

/* {{{ proto integer Redis::lastSave() */
PHP_METHOD(Redis, lastSave)
{
}
/* }}} */

/* {{{ proto bool Redis::failover([array to [,bool abort [,int timeout]]] ) */
PHP_METHOD(Redis, failover)
{
}
/* }}} */

PHP_METHOD(Redis, pipeline)
{
}

/* {{{ proto long Redis::publish(string channel, string msg) */
PHP_METHOD(Redis, publish)
{
}
/* }}} */

/* {{{ proto void Redis::psubscribe([pattern1, pattern2, ... patternN]) */
PHP_METHOD(Redis, psubscribe)
{
}
/* }}} */

/* {{{ proto void Redis::ssubscribe([shardchannel1, shardchannel2, ... shardchannelN]) */
PHP_METHOD(Redis, ssubscribe)
{
}
/* }}} */

/* {{{ proto void Redis::subscribe([channel1, channel2, ... channelN]) */
PHP_METHOD(Redis, subscribe)
{
}

/**
 *  [ps]unsubscribe channel_0 channel_1 ... channel_n
 *  [ps]unsubscribe([channel_0, channel_1, ..., channel_n])
 * response format :
 * array(
 *    channel_0 => TRUE|FALSE,
 *    channel_1 => TRUE|FALSE,
 *    ...
 *    channel_n => TRUE|FALSE
 * );
 **/

PHP_METHOD(Redis, unsubscribe)
{
}

PHP_METHOD(Redis, punsubscribe)
{
}

PHP_METHOD(Redis, sunsubscribe)
{
}

PHP_METHOD(Redis, waitaof)
{
}

/* {{{ proto string Redis::bgrewriteaof() */
PHP_METHOD(Redis, bgrewriteaof)
{
}
/* }}} */

/* {{{ public function slaveof(string $host = NULL, int $port = NULL): Redis|bool }}} */
PHP_METHOD(Redis, slaveof)
{
}
/* }}} */

/* {{{ public function replicaof(string $host = NULL, int $port = NULL): Redis|bool }}} */
PHP_METHOD(Redis, replicaof)
{
}

/* }}} */

/* {{{ proto string Redis::getOption($option) */
PHP_METHOD(Redis, getOption)
{
}
/* }}} */

/* {{{ proto boolean Redis::slowlog(string arg, [int option]) */
PHP_METHOD(Redis, slowlog)
{
}

/*
 * {{{ proto Redis::pubsub("channels", pattern);
 *     proto Redis::pubsub("numsub", Array channels);
 *     proto Redis::pubsub("numpat"); }}}
 */
PHP_METHOD(Redis, pubsub)
{
}

/* {{{ proto variant Redis::eval(string script, [array keys, long num_keys]) */
PHP_METHOD(Redis, eval)
{
}

/* {{{ proto variant Redis::eval_ro(string script, [array keys, long num_keys]) */
PHP_METHOD(Redis, eval_ro)
{
}

/* {{{ proto variant Redis::evalsha(string sha1, [array keys, long num_keys]) */
PHP_METHOD(Redis, evalsha)
{
}

/* {{{ proto variant Redis::evalsha_ro(string sha1, [array keys, long num_keys]) */
PHP_METHOD(Redis, evalsha_ro)
{
}

/* {{{ public function script($args...): mixed }}} */
PHP_METHOD(Redis, script)
{
}

/* {{{ proto Redis::debug(string key) */
PHP_METHOD(Redis, debug)
{
}
/* }}} */

/* {{{ proto Redis::migrate(host port key dest-db timeout [bool copy,
 *                          bool replace]) */
PHP_METHOD(Redis, migrate)
{
}

/* {{{ proto Redis::_prefix(key) */
PHP_METHOD(Redis, _prefix)
{
}

/* {{{ proto Redis::_serialize(value) */
PHP_METHOD(Redis, _serialize)
{
}

/* {{{ proto Redis::_unserialize(value) */
PHP_METHOD(Redis, _unserialize)
{
}

PHP_METHOD(Redis, _compress)
{
}

PHP_METHOD(Redis, _uncompress)
{
}

PHP_METHOD(Redis, _pack)
{
}

PHP_METHOD(Redis, _unpack)
{
}

/* {{{ proto Redis::getLastError() */
PHP_METHOD(Redis, getLastError)
{

    RETURN_NULL();
}

/* {{{ proto Redis::clearLastError() */
PHP_METHOD(Redis, clearLastError)
{

    RETURN_TRUE;
}

/*
 * {{{ proto long Redis::getMode()
 */
PHP_METHOD(Redis, getMode)
{
    RETURN_FALSE;
}

/*
 * Introspection stuff
 */

/* {{{ proto Redis::IsConnected */
PHP_METHOD(Redis, isConnected)
{
    RETURN_FALSE;
}

/* {{{ proto Redis::getHost() */
PHP_METHOD(Redis, getHost)
{

    RETURN_FALSE;
}

/* {{{ proto Redis::getPort() */
PHP_METHOD(Redis, getPort)
{

    RETURN_FALSE;
}

/* {{{ proto Redis::getDBNum */
PHP_METHOD(Redis, getDBNum)
{
    RETURN_FALSE;
}

PHP_METHOD(Redis, getTransferredBytes)
{
    RETURN_FALSE;
}

PHP_METHOD(Redis, clearTransferredBytes)
{
    RETURN_FALSE;
}

/* Helper to format any combination of SCAN arguments */
