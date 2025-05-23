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
#include "redis_array.h"
#include "redis_cluster.h"
#include "redis_commands.h"
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

#include "library.h"

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
static int
redis_send_discard(RedisSock *redis_sock)
{
    char *resp;
    int resp_len, result = FAILURE;

    /* send our DISCARD command */
    if (redis_sock_write(redis_sock, ZEND_STRL(RESP_DISCARD_CMD)) >= 0 &&
        (resp = redis_sock_read(redis_sock, &resp_len)) != NULL)
    {
        /* success if we get OK */
        result = (resp_len == 3 && redis_strncmp(resp, ZEND_STRL("+OK")) == 0) ? SUCCESS : FAILURE;

        /* free our response */
        efree(resp);
    }

    /* return success/failure */
    return result;
}

/* {{{ proto boolean Redis::close()
 */
PHP_METHOD(Redis, close)
{
#if 0
    RedisSock *redis_sock = redis_sock_get_connected(INTERNAL_FUNCTION_PARAM_PASSTHRU);

    if (redis_sock_disconnect(redis_sock, 1, 1) == SUCCESS)
    {
        RETURN_TRUE;
    }
#endif
    RETURN_FALSE;
}

/* Passthru for destroying cluster cache */
static void cluster_cache_dtor(zend_resource *rsrc)
{
    if (rsrc->ptr)
    {
        cluster_cache_free(rsrc->ptr);
    }
}

void free_redis_object(zend_object *object)
{
    redis_object *redis = PHPREDIS_GET_OBJECT(redis_object, object);

    zend_object_std_dtor(&redis->std);
    if (redis->sock)
    {
        redis_sock_disconnect(redis->sock, 0, 1);
        redis_free_socket(redis->sock);
    }

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
    redis->glide_client = create_glide_client(NULL); /* Initialize Valkey Glide client pointer to NULL */

    zend_object_std_init(&redis->std, ce);
    object_properties_init(&redis->std, ce);

    memcpy(&redis_object_handlers, zend_get_std_object_handlers(), sizeof(redis_object_handlers));
    redis_object_handlers.offset = XtOffsetOf(redis_object, std);
    redis_object_handlers.free_obj = free_redis_object;
    redis->std.handlers = &redis_object_handlers;

    return &redis->std;
}

static zend_always_inline RedisSock *
redis_sock_get_instance(zval *id, int no_throw)
{
#if 0
    redis_object *redis;

    if (Z_TYPE_P(id) == IS_OBJECT)
    {
        redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, id);
        if (redis->sock)
        {
            /* Initialize the Valkey Glide client if it doesn't exist */
            if (redis->sock->status >= REDIS_SOCK_STATUS_CONNECTED && !redis->glide_client)
            {
                /* Create a Valkey Glide client using the socket information */
                //  redis->glide_client = create_glide_client(
                //    ZSTR_VAL(redis->sock->host),
                //   redis->sock->port,
                //  redis->sock->user ? ZSTR_VAL(redis->sock->user) : NULL,
                // redis->sock->pass ? ZSTR_VAL(redis->sock->pass) : NULL);
            }
            return redis->sock;
        }
    }
    // Throw an exception unless we've been requested not to
    if (!no_throw)
    {
        REDIS_THROW_EXCEPTION("Redis server went away", 0);
    }
#endif

    return NULL;
}

static zend_never_inline ZEND_COLD void redis_sock_throw_exception(RedisSock *redis_sock)
{
    char *errmsg = NULL;
    if (redis_sock->status == REDIS_SOCK_STATUS_AUTHENTICATED)
    {
        if (redis_sock->err != NULL)
        {
            spprintf(&errmsg, 0, "Could not select database %ld '%s'", redis_sock->dbNumber, ZSTR_VAL(redis_sock->err));
        }
        else
        {
            spprintf(&errmsg, 0, "Could not select database %ld", redis_sock->dbNumber);
        }
    }
    else if (redis_sock->status == REDIS_SOCK_STATUS_CONNECTED)
    {
        if (redis_sock->err != NULL)
        {
            spprintf(&errmsg, 0, "Could not authenticate '%s'", ZSTR_VAL(redis_sock->err));
        }
        else
        {
            spprintf(&errmsg, 0, "Could not authenticate");
        }
    }
    else
    {
        if (redis_sock->port < 0)
        {
            spprintf(&errmsg, 0, "Redis server %s went away", ZSTR_VAL(redis_sock->host));
        }
        else
        {
            spprintf(&errmsg, 0, "Redis server %s:%d went away", ZSTR_VAL(redis_sock->host), redis_sock->port);
        }
    }
    REDIS_THROW_EXCEPTION(errmsg, 0);
    efree(errmsg);
}

/**
 * redis_sock_get
 */
PHP_REDIS_API RedisSock *
redis_sock_get(zval *id, int no_throw)
{
    RedisSock *redis_sock;

    if ((redis_sock = redis_sock_get_instance(id, no_throw)) == NULL)
    {
        return NULL;
    }

    if (UNEXPECTED(redis_sock_server_open(redis_sock) < 0))
    {
        if (!no_throw)
        {
            redis_sock_throw_exception(redis_sock);
        }
        return NULL;
    }

    return redis_sock;
}

/**
 * redis_sock_get_direct
 * Returns our attached RedisSock pointer if we're connected
 */
PHP_REDIS_API RedisSock *redis_sock_get_connected(INTERNAL_FUNCTION_PARAMETERS)
{
    zval *object;
    RedisSock *redis_sock;

    // If we can't grab our object, or get a socket, or we're not connected,
    // return NULL
    if ((zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
                                      &object, redis_ce) == FAILURE) ||
        (redis_sock = redis_sock_get(object, 1)) == NULL ||
        redis_sock->status < REDIS_SOCK_STATUS_CONNECTED)
    {
        return NULL;
    }

    /* Return our socket */
    return redis_sock;
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

    /* RedisArray class */
    ZEND_MINIT(redis_array)(INIT_FUNC_ARGS_PASSTHRU);

    /* RedisCluster class */
    ZEND_MINIT(redis_cluster)(INIT_FUNC_ARGS_PASSTHRU);

    /* Register our cluster cache list item */
    le_cluster_slot_cache = zend_register_list_destructors_ex(NULL, cluster_cache_dtor,
                                                              "Redis cluster slot cache",
                                                              module_number);

    /* RedisException class */
    redis_exception_ce = register_class_RedisException(spl_ce_RuntimeException);

#ifdef PHP_SESSION
    php_session_register_module(&ps_mod_redis);
    php_session_register_module(&ps_mod_redis_cluster);
#endif

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
#ifdef GIT_REVISION
    php_info_print_table_row(2, "Git revision", "$Id: " GIT_REVISION " $");
#endif
    php_info_print_table_row(2, "Available serializers", get_available_serializers());
#ifdef HAVE_REDIS_LZF
    smart_str_appends(&names, "lzf");
#endif
#ifdef HAVE_REDIS_ZSTD
    if (names.s)
    {
        smart_str_appends(&names, ", ");
    }
    smart_str_appends(&names, "zstd");
#endif
#ifdef HAVE_REDIS_LZ4
    if (names.s)
    {
        smart_str_appends(&names, ", ");
    }
    smart_str_appends(&names, "lz4");
#endif
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
#if 0
   redis->sock = redis_sock_create(ZEND_STRL("127.0.0.1"), 6379, 0, 0, 0, NULL, 0);
    if (opts != NULL && redis_sock_configure(redis->sock, opts) != SUCCESS)
    {
        RETURN_THROWS();
    }
#endif
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

    // Grab our socket
    RedisSock *redis_sock;
    if ((redis_sock = redis_sock_get_instance(getThis(), 1)) == NULL)
    {
        RETURN_FALSE;
    }

    // If we think we're in MULTI mode, send a discard
    if (IS_MULTI(redis_sock))
    {
        if (!IS_PIPELINE(redis_sock) && redis_sock->stream)
        {
            // Discard any multi commands, and free any callbacks that have been
            // queued
            redis_send_discard(redis_sock);
        }
        redis_free_reply_callbacks(redis_sock);
    }
}

/* {{{ proto boolean Redis::connect(string host, int port [, double timeout [, long retry_interval]])
 */
PHP_METHOD(Redis, connect)
{
    printf("file = %s, line = %d\n", __FILE__, __LINE__);
    /* if (redis_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0) == FAILURE)
     {
         RETURN_FALSE;
     }
     else
     {
         RETURN_TRUE;
     }*/
    RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean Redis::pconnect(string host, int port [, double timeout])
 */
PHP_METHOD(Redis, pconnect)
{
    printf("file = %s, line = %d\n", __FILE__, __LINE__);

    /*if (redis_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1) == FAILURE)
    {
        RETURN_FALSE;
    }
    else
    {
        RETURN_TRUE;
    }*/
    RETURN_TRUE;
}
/* }}} */

PHP_REDIS_API int
redis_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
    return 0;
    zval *object, *context = NULL, *ele;
    char *host = NULL, *persistent_id = NULL;
    zend_long port = -1, retry_interval = 0;
    size_t host_len, persistent_id_len;
    double timeout = 0.0, read_timeout = 0.0;
    redis_object *redis;
    int af_unix;

#ifdef ZTS
    /* not sure how in threaded mode this works so disabled persistence at
     * first */
    persistent = 0;
#endif

    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
                                     "Os|lds!lda!", &object, redis_ce, &host,
                                     &host_len, &port, &timeout, &persistent_id,
                                     &persistent_id_len, &retry_interval,
                                     &read_timeout, &context) == FAILURE)
    {
        return FAILURE;
    }

    /* Disregard persistent_id if we're not opening a persistent connection */
    if (!persistent)
    {
        persistent_id = NULL;
    }

    if (timeout > INT_MAX)
    {
        REDIS_VALUE_EXCEPTION("Invalid connect timeout");
        return FAILURE;
    }

    if (read_timeout > INT_MAX)
    {
        REDIS_VALUE_EXCEPTION("Invalid read timeout");
        return FAILURE;
    }

    if (retry_interval < 0L || retry_interval > INT_MAX)
    {
        REDIS_VALUE_EXCEPTION("Invalid retry interval");
        return FAILURE;
    }

    /* Does the host look like a unix socket */
    af_unix = (host_len > 0 && host[0] == '/') ||
              (host_len > 6 && (!strncasecmp(host, "unix://", sizeof("unix://") - 1) ||
                                !strncasecmp(host, "file://", sizeof("file://") - 1)));

    /* If it's not a unix socket, set to default */
    if (port == -1 && !af_unix)
    {
        port = 6379;
    }

    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* if there is a redis sock already we have to remove it */
    if (redis->sock)
    {
        redis_sock_disconnect(redis->sock, 0, 1);
        redis_free_socket(redis->sock);
    }

    /*  redis->sock = redis_sock_create(host, host_len, port, timeout, read_timeout, persistent,
                                      persistent_id, retry_interval);
      printf("file = %s, line = %d\n", __FILE__, __LINE__);
  */
    /* Initialize the Valkey Glide client */
    /* We don't initialize the Glide client here, it will be initialized in redis_sock_get_instance */

    if (context)
    {
#if 0
        /* Stream context (e.g. TLS) */
        if ((ele = REDIS_HASH_STR_FIND_STATIC(Z_ARRVAL_P(context), "stream")))
        {
            redis_sock_set_stream_context(redis->sock, ele);
        }
#endif
        /* AUTH */
        if ((ele = REDIS_HASH_STR_FIND_STATIC(Z_ARRVAL_P(context), "auth")))
        {
            //    redis_sock_set_auth_zval(redis->sock, ele);

            /* If we have auth credentials and a Glide client, update the Glide client */
            if (redis->glide_client && redis->sock->pass)
            {
                /* In a real implementation, we would update the Glide client with the auth credentials */
                /* For now, we'll just leave it as is */
            }
        }
    }
#if 0
    if (redis_sock_connect(redis->sock) != SUCCESS)
    {
        if (redis->sock->err)
        {
            REDIS_THROW_EXCEPTION(ZSTR_VAL(redis->sock->err), 0);
        }
        redis_free_socket(redis->sock);
        redis->sock = NULL;
        return FAILURE;
    }
#endif

    return SUCCESS;
}

/* {{{ proto string Redis::save() */
PHP_METHOD(Redis, save)
{
    REDIS_PROCESS_KW_CMD("SAVE", redis_empty_cmd, redis_boolean_response);
}
/* }}} */

/* {{{ proto string Redis::bgSave() */
PHP_METHOD(Redis, bgSave)
{
    REDIS_PROCESS_KW_CMD("BGSAVE", redis_empty_cmd, redis_boolean_response);
}
/* }}} */

/* {{{ proto integer Redis::lastSave() */
PHP_METHOD(Redis, lastSave)
{
    REDIS_PROCESS_KW_CMD("LASTSAVE", redis_empty_cmd, redis_long_response);
}
/* }}} */

/* {{{ proto bool Redis::failover([array to [,bool abort [,int timeout]]] ) */
PHP_METHOD(Redis, failover)
{
    REDIS_PROCESS_CMD(failover, redis_boolean_response);
}
/* }}} */

/* {{{ proto mixed Redis::function(string op, mixed ...args) */
PHP_METHOD(Redis, function){
    REDIS_PROCESS_CMD(function, redis_function_response)}

/* {{{ proto int Redis::dbSize() */
PHP_METHOD(Redis, dbSize)
{
    REDIS_PROCESS_KW_CMD("DBSIZE", redis_empty_cmd, redis_long_response);
}
/* }}} */

/* {{{ proto bool Redis::auth(string passwd) */
PHP_METHOD(Redis, auth)
{
    REDIS_PROCESS_CMD(auth, redis_boolean_response);
}
/* }}} */

/* {{{ proto bool Redis::select(long dbNumber) */
PHP_METHOD(Redis, select)
{
    REDIS_PROCESS_CMD(select, redis_select_response);
}
/* }}} */

/* {{{ proto bool Redis::swapdb(long srcdb, long dstdb) */
PHP_METHOD(Redis, swapdb)
{
    REDIS_PROCESS_KW_CMD("SWAPDB", redis_long_long_cmd, redis_boolean_response);
}

/* {{{ proto bool Redis::move(string key, long dbindex) */
PHP_METHOD(Redis, move)
{
    REDIS_PROCESS_KW_CMD("MOVE", redis_key_long_cmd, redis_1_response);
}
/* }}} */

/* }}} */

/* {{{ proto Redis::bzPopMax(Array[keys] [, timeout]): Array */
PHP_METHOD(Redis, bzPopMax)
{
    REDIS_PROCESS_KW_CMD("BZPOPMAX", redis_blocking_pop_cmd, redis_sock_read_multibulk_reply);
}
/* }}} */

/* {{{ proto Redis::bzPopMin([keys] [, timeout]): Array */
PHP_METHOD(Redis, bzPopMin)
{
    REDIS_PROCESS_KW_CMD("BZPOPMIN", redis_blocking_pop_cmd, redis_sock_read_multibulk_reply);
}
/* }}} */

/* hashes */

/* flag : get, set {ATOMIC, MULTI, PIPELINE} */

PHP_METHOD(Redis, multi)
{

    RedisSock *redis_sock;
    char *resp;
    int resp_len;
    zval *object;
    zend_long multi_value = MULTI;

    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
                                     "O|l", &object, redis_ce, &multi_value) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* if the flag is activated, send the command, the reply will be "QUEUED"
     * or -ERR */
    if ((redis_sock = redis_sock_get(object, 0)) == NULL)
    {
        RETURN_FALSE;
    }

    if (multi_value == PIPELINE)
    {
        /* Cannot enter pipeline mode in a MULTI block */
        if (IS_MULTI(redis_sock))
        {
            php_error_docref(NULL, E_ERROR, "Can't activate pipeline in multi mode!");
            RETURN_FALSE;
        }

        /* Enable PIPELINE if we're not already in one */
        if (IS_ATOMIC(redis_sock))
        {
            REDIS_ENABLE_MODE(redis_sock, PIPELINE);
        }
    }
    else if (multi_value == MULTI)
    {
        /* Don't want to do anything if we're already in MULTI mode */
        if (!IS_MULTI(redis_sock))
        {
            if (IS_PIPELINE(redis_sock))
            {
                PIPELINE_ENQUEUE_COMMAND(RESP_MULTI_CMD, sizeof(RESP_MULTI_CMD) - 1);
                REDIS_SAVE_CALLBACK(NULL, NULL);
                REDIS_ENABLE_MODE(redis_sock, MULTI);
            }
            else
            {
                if (redis_sock_write(redis_sock, ZEND_STRL(RESP_MULTI_CMD)) < 0)
                {
                    RETURN_FALSE;
                }
                if ((resp = redis_sock_read(redis_sock, &resp_len)) == NULL)
                {
                    RETURN_FALSE;
                }
                else if (redis_strncmp(resp, ZEND_STRL("+OK")) != 0)
                {
                    efree(resp);
                    RETURN_FALSE;
                }
                efree(resp);
                REDIS_ENABLE_MODE(redis_sock, MULTI);
            }
        }
    }
    else
    {
        php_error_docref(NULL, E_WARNING, "Unknown mode sent to Redis::multi");
        RETURN_FALSE;
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

/* discard */
PHP_METHOD(Redis, discard)
{
    int ret = FAILURE;
    RedisSock *redis_sock;
    zval *object;

    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
                                     &object, redis_ce) == FAILURE)
    {
        RETURN_FALSE;
    }

    if ((redis_sock = redis_sock_get(object, 0)) == NULL)
    {
        RETURN_FALSE;
    }

    if (IS_PIPELINE(redis_sock))
    {
        ret = SUCCESS;
        smart_string_free(&redis_sock->pipeline_cmd);
    }
    else if (IS_MULTI(redis_sock))
    {
        ret = redis_send_discard(redis_sock);
    }
    if (ret == SUCCESS)
    {
        redis_free_reply_callbacks(redis_sock);
        redis_sock->mode = ATOMIC;
        RETURN_TRUE;
    }
    RETURN_FALSE;
}

PHP_REDIS_API int
redis_sock_read_multibulk_multi_reply(INTERNAL_FUNCTION_PARAMETERS,
                                      RedisSock *redis_sock, zval *z_tab)
{

    char inbuf[4096];
    size_t len;

    if (redis_sock_gets(redis_sock, inbuf, sizeof(inbuf) - 1, &len) < 0 ||
        *inbuf != TYPE_MULTIBULK || atoi(inbuf + 1) < 0)
    {
        return FAILURE;
    }

    // No command issued, return empty immutable array
    if (redis_sock->reply_callback == NULL)
    {
        ZVAL_EMPTY_ARRAY(z_tab);
        return SUCCESS;
    }

    array_init(z_tab);

    return redis_sock_read_multibulk_multi_reply_loop(INTERNAL_FUNCTION_PARAM_PASSTHRU,
                                                      redis_sock, z_tab);
}

/* exec */
PHP_METHOD(Redis, exec)
{
    RedisSock *redis_sock;
    int ret;
    zval *object, z_ret;

    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
                                     "O", &object, redis_ce) == FAILURE ||
        (redis_sock = redis_sock_get(object, 0)) == NULL)
    {
        RETURN_FALSE;
    }

    ZVAL_FALSE(&z_ret);

    if (IS_MULTI(redis_sock))
    {
        if (IS_PIPELINE(redis_sock))
        {
            PIPELINE_ENQUEUE_COMMAND(RESP_EXEC_CMD, sizeof(RESP_EXEC_CMD) - 1);
            REDIS_SAVE_CALLBACK(NULL, NULL);
            REDIS_DISABLE_MODE(redis_sock, MULTI);
            RETURN_ZVAL(getThis(), 1, 0);
        }
        if (redis_sock_write(redis_sock, ZEND_STRL(RESP_EXEC_CMD)) < 0)
        {
            RETURN_FALSE;
        }
        ret = redis_sock_read_multibulk_multi_reply(
            INTERNAL_FUNCTION_PARAM_PASSTHRU, redis_sock, &z_ret);
        redis_free_reply_callbacks(redis_sock);
        REDIS_DISABLE_MODE(redis_sock, MULTI);
        redis_sock->watching = 0;
        if (ret < 0)
        {
            zval_dtor(&z_ret);
            ZVAL_FALSE(&z_ret);
        }
    }

    if (IS_PIPELINE(redis_sock))
    {
        if (redis_sock->pipeline_cmd.len == 0)
        {
            /* Empty array when no command was run. */
            ZVAL_EMPTY_ARRAY(&z_ret);
        }
        else
        {
            if (redis_sock_write(redis_sock, redis_sock->pipeline_cmd.c,
                                 redis_sock->pipeline_cmd.len) < 0)
            {
                ZVAL_FALSE(&z_ret);
            }
            else
            {
                array_init(&z_ret);
                if (redis_sock_read_multibulk_multi_reply_loop(
                        INTERNAL_FUNCTION_PARAM_PASSTHRU, redis_sock, &z_ret) != SUCCESS)
                {
                    zval_dtor(&z_ret);
                    ZVAL_FALSE(&z_ret);
                }
            }
            smart_string_free(&redis_sock->pipeline_cmd);
        }
        redis_free_reply_callbacks(redis_sock);
        REDIS_DISABLE_MODE(redis_sock, PIPELINE);
    }
    RETURN_ZVAL(&z_ret, 0, 1);
}

PHP_REDIS_API int
redis_response_enqueued(RedisSock *redis_sock)
{
    char *resp;
    int resp_len, ret = FAILURE;

    if ((resp = redis_sock_read(redis_sock, &resp_len)) != NULL)
    {
        if (redis_strncmp(resp, ZEND_STRL("+QUEUED")) == 0)
        {
            ret = SUCCESS;
        }
        efree(resp);
    }
    return ret;
}

PHP_REDIS_API int
redis_sock_read_multibulk_multi_reply_loop(INTERNAL_FUNCTION_PARAMETERS,
                                           RedisSock *redis_sock, zval *z_tab)
{
    fold_item *fi;
    uint8_t flags;
    size_t i;

    flags = redis_sock->flags;
    for (i = 0; i < redis_sock->reply_callback_count; i++)
    {
        fi = &redis_sock->reply_callback[i];
        if (fi->fun)
        {
            redis_sock->flags = fi->flags;
            fi->fun(INTERNAL_FUNCTION_PARAM_PASSTHRU, redis_sock, z_tab, fi->ctx);
            redis_sock->flags = flags;
            continue;
        }
        size_t len;
        char inbuf[255];

        if (redis_sock_gets(redis_sock, inbuf, sizeof(inbuf) - 1, &len) < 0 ||
            redis_strncmp(inbuf, ZEND_STRL("+OK")) != 0)
        {
            return FAILURE;
        }

        while (redis_sock->reply_callback[++i].fun)
        {
            if (redis_response_enqueued(redis_sock) != SUCCESS)
            {
                return FAILURE;
            }
        }

        if (redis_sock_gets(redis_sock, inbuf, sizeof(inbuf) - 1, &len) < 0)
        {
            return FAILURE;
        }

        zval z_ret;
        array_init(&z_ret);
        add_next_index_zval(z_tab, &z_ret);

        int num = atol(inbuf + 1);

        if (num > 0 && redis_read_multibulk_recursive(redis_sock, num, 0, &z_ret) < 0)
        {
            return FAILURE;
        }
    }
    return SUCCESS;
}

PHP_METHOD(Redis, pipeline)
{
    RedisSock *redis_sock;
    zval *object;

    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
                                     "O", &object, redis_ce) == FAILURE ||
        (redis_sock = redis_sock_get(object, 0)) == NULL)
    {
        RETURN_FALSE;
    }

    /* User cannot enter MULTI mode if already in a pipeline */
    if (IS_MULTI(redis_sock))
    {
        php_error_docref(NULL, E_ERROR, "Can't activate pipeline in multi mode!");
        RETURN_FALSE;
    }

    /* Enable pipeline mode unless we're already in that mode in which case this
     * is just a NO OP */
    if (IS_ATOMIC(redis_sock))
    {
        /* NB : we keep the function fold, to detect the last function.
         * We need the response format of the n - 1 command. So, we can delete
         * when n > 2, the { 1 .. n - 2} commands */
        REDIS_ENABLE_MODE(redis_sock, PIPELINE);
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

/* {{{ proto long Redis::publish(string channel, string msg) */
PHP_METHOD(Redis, publish)
{
    REDIS_PROCESS_KW_CMD("PUBLISH", redis_key_str_cmd, redis_long_response);
}
/* }}} */

/* {{{ proto void Redis::psubscribe([pattern1, pattern2, ... patternN]) */
PHP_METHOD(Redis, psubscribe)
{
    REDIS_PROCESS_KW_CMD("PSUBSCRIBE", redis_subscribe_cmd,
                         redis_subscribe_response);
}
/* }}} */

/* {{{ proto void Redis::ssubscribe([shardchannel1, shardchannel2, ... shardchannelN]) */
PHP_METHOD(Redis, ssubscribe)
{
    REDIS_PROCESS_KW_CMD("SSUBSCRIBE", redis_subscribe_cmd,
                         redis_subscribe_response);
}
/* }}} */

/* {{{ proto void Redis::subscribe([channel1, channel2, ... channelN]) */
PHP_METHOD(Redis, subscribe)
{
    REDIS_PROCESS_KW_CMD("SUBSCRIBE", redis_subscribe_cmd,
                         redis_subscribe_response);
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
    REDIS_PROCESS_KW_CMD("UNSUBSCRIBE", redis_unsubscribe_cmd,
                         redis_unsubscribe_response);
}

PHP_METHOD(Redis, punsubscribe)
{
    REDIS_PROCESS_KW_CMD("PUNSUBSCRIBE", redis_unsubscribe_cmd,
                         redis_unsubscribe_response);
}

PHP_METHOD(Redis, sunsubscribe)
{
    REDIS_PROCESS_KW_CMD("SUNSUBSCRIBE", redis_unsubscribe_cmd,
                         redis_unsubscribe_response);
}

PHP_METHOD(Redis, waitaof)
{
    REDIS_PROCESS_CMD(waitaof, redis_read_variant_reply);
}

/* {{{ proto string Redis::bgrewriteaof() */
PHP_METHOD(Redis, bgrewriteaof)
{
    REDIS_PROCESS_KW_CMD("BGREWRITEAOF", redis_empty_cmd,
                         redis_boolean_response);
}
/* }}} */

/* {{{ public function slaveof(string $host = NULL, int $port = NULL): Redis|bool }}} */
PHP_METHOD(Redis, slaveof)
{
    REDIS_PROCESS_KW_CMD("SLAVEOF", redis_replicaof_cmd, redis_boolean_response);
}
/* }}} */

/* {{{ public function replicaof(string $host = NULL, int $port = NULL): Redis|bool }}} */
PHP_METHOD(Redis, replicaof)
{
    REDIS_PROCESS_KW_CMD("REPLICAOF", redis_replicaof_cmd, redis_boolean_response);
}

/* }}} */

/* {{{ proto string Redis::getOption($option) */
PHP_METHOD(Redis, getOption)
{
    RedisSock *redis_sock;

    if ((redis_sock = redis_sock_get_instance(getThis(), 0)) == NULL)
    {
        RETURN_FALSE;
    }

    redis_getoption_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, redis_sock, NULL);
}
/* }}} */

/* {{{ proto boolean Redis::config(string op, string key [, mixed value]) */
/* {{{ proto public function config(string $op, string ...$args) }}} */
// CONFIG SET/GET
PHP_METHOD(Redis, config)
{
    REDIS_PROCESS_CMD(config, redis_config_response);
}
/* }}} */

/* {{{ proto boolean Redis::slowlog(string arg, [int option]) */
PHP_METHOD(Redis, slowlog)
{
    REDIS_PROCESS_CMD(slowlog, redis_read_variant_reply);
}

/* {{{ proto Redis::wait(int num_slaves, int ms) }}} */
PHP_METHOD(Redis, wait)
{
    REDIS_PROCESS_KW_CMD("WAIT", redis_long_long_cmd, redis_long_response);
}

/*
 * {{{ proto Redis::pubsub("channels", pattern);
 *     proto Redis::pubsub("numsub", Array channels);
 *     proto Redis::pubsub("numpat"); }}}
 */
PHP_METHOD(Redis, pubsub)
{
    REDIS_PROCESS_CMD(pubsub, redis_pubsub_response);
}

/* {{{ proto variant Redis::eval(string script, [array keys, long num_keys]) */
PHP_METHOD(Redis, eval)
{
    REDIS_PROCESS_KW_CMD("EVAL", redis_eval_cmd, redis_read_raw_variant_reply);
}

/* {{{ proto variant Redis::eval_ro(string script, [array keys, long num_keys]) */
PHP_METHOD(Redis, eval_ro)
{
    REDIS_PROCESS_KW_CMD("EVAL_RO", redis_eval_cmd, redis_read_raw_variant_reply);
}

/* {{{ proto variant Redis::evalsha(string sha1, [array keys, long num_keys]) */
PHP_METHOD(Redis, evalsha)
{
    REDIS_PROCESS_KW_CMD("EVALSHA", redis_eval_cmd, redis_read_raw_variant_reply);
}

/* {{{ proto variant Redis::evalsha_ro(string sha1, [array keys, long num_keys]) */
PHP_METHOD(Redis, evalsha_ro)
{
    REDIS_PROCESS_KW_CMD("EVALSHA_RO", redis_eval_cmd, redis_read_raw_variant_reply);
}

/* {{{ proto variant Redis::fcall(string fn [, array keys [, array args]]) */
PHP_METHOD(Redis, fcall)
{
    REDIS_PROCESS_KW_CMD("FCALL", redis_fcall_cmd, redis_read_raw_variant_reply);
}

/* {{{ proto variant Redis::fcall_ro(string fn [, array keys [, array args]]) */
PHP_METHOD(Redis, fcall_ro)
{
    REDIS_PROCESS_KW_CMD("FCALL_RO", redis_fcall_cmd, redis_read_raw_variant_reply);
}

/* {{{ public function script($args...): mixed }}} */
PHP_METHOD(Redis, script)
{
    REDIS_PROCESS_CMD(script, redis_read_variant_reply);
}

/* {{{ proto DUMP key */
PHP_METHOD(Redis, dump)
{
    REDIS_PROCESS_KW_CMD("DUMP", redis_key_cmd, redis_string_response);
}
/* }}} */

/* {{{ proto Redis::restore(ttl, key, value) */
PHP_METHOD(Redis, restore)
{
    REDIS_PROCESS_CMD(restore, redis_boolean_response);
}
/* }}} */

/* {{{ proto Redis::debug(string key) */
PHP_METHOD(Redis, debug)
{
    REDIS_PROCESS_KW_CMD("DEBUG", redis_key_cmd, redis_string_response);
}
/* }}} */

/* {{{ proto Redis::migrate(host port key dest-db timeout [bool copy,
 *                          bool replace]) */
PHP_METHOD(Redis, migrate)
{
    REDIS_PROCESS_CMD(migrate, redis_boolean_response);
}

/* {{{ proto Redis::_prefix(key) */
PHP_METHOD(Redis, _prefix)
{
    RedisSock *redis_sock;

    if ((redis_sock = redis_sock_get_instance(getThis(), 0)) == NULL)
    {
        RETURN_FALSE;
    }

    redis_prefix_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, redis_sock);
}

/* {{{ proto Redis::_serialize(value) */
PHP_METHOD(Redis, _serialize)
{
    RedisSock *redis_sock;

    // Grab socket
    if ((redis_sock = redis_sock_get_instance(getThis(), 0)) == NULL)
    {
        RETURN_FALSE;
    }

    redis_serialize_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, redis_sock);
}

/* {{{ proto Redis::_unserialize(value) */
PHP_METHOD(Redis, _unserialize)
{
    RedisSock *redis_sock;

    // Grab socket
    if ((redis_sock = redis_sock_get_instance(getThis(), 0)) == NULL)
    {
        RETURN_FALSE;
    }

    redis_unserialize_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, redis_sock,
                              redis_exception_ce);
}

PHP_METHOD(Redis, _compress)
{
    RedisSock *redis_sock;

    // Grab socket
    if ((redis_sock = redis_sock_get_instance(getThis(), 0)) == NULL)
    {
        RETURN_FALSE;
    }

    redis_compress_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, redis_sock);
}

PHP_METHOD(Redis, _uncompress)
{
    RedisSock *redis_sock;

    // Grab socket
    if ((redis_sock = redis_sock_get_instance(getThis(), 0)) == NULL)
    {
        RETURN_FALSE;
    }

    redis_uncompress_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, redis_sock,
                             redis_exception_ce);
}

PHP_METHOD(Redis, _pack)
{
    RedisSock *redis_sock;

    // Grab socket
    if ((redis_sock = redis_sock_get_instance(getThis(), 0)) == NULL)
    {
        RETURN_FALSE;
    }

    redis_pack_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, redis_sock);
}

PHP_METHOD(Redis, _unpack)
{
    RedisSock *redis_sock;

    // Grab socket
    if ((redis_sock = redis_sock_get_instance(getThis(), 0)) == NULL)
    {
        RETURN_FALSE;
    }

    redis_unpack_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, redis_sock);
}

/* {{{ proto Redis::getLastError() */
PHP_METHOD(Redis, getLastError)
{
#if 0
    zval *object;
    RedisSock *redis_sock;

    // Grab our object
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
                                     &object, redis_ce) == FAILURE)
    {
        RETURN_FALSE;
    }

    // Grab socket
    if ((redis_sock = redis_sock_get_instance(object, 0)) == NULL)
    {
        RETURN_FALSE;
    }

    /* Return our last error or NULL if we don't have one */
    if (redis_sock->err)
    {
        RETURN_STRINGL(ZSTR_VAL(redis_sock->err), ZSTR_LEN(redis_sock->err));
    }
#endif
    RETURN_NULL();
}

/* {{{ proto Redis::clearLastError() */
PHP_METHOD(Redis, clearLastError)
{
    zval *object;
    RedisSock *redis_sock;

    // Grab our object
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
                                     &object, redis_ce) == FAILURE)
    {
        RETURN_FALSE;
    }
    // Grab socket
    if ((redis_sock = redis_sock_get_instance(object, 0)) == NULL)
    {
        RETURN_FALSE;
    }

    // Clear error message
    if (redis_sock->err)
    {
        zend_string_release(redis_sock->err);
        redis_sock->err = NULL;
    }

    RETURN_TRUE;
}

/*
 * {{{ proto long Redis::getMode()
 */
PHP_METHOD(Redis, getMode)
{
    zval *object;
    RedisSock *redis_sock;

    /* Grab our object */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &object, redis_ce) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Grab socket */
    if ((redis_sock = redis_sock_get_instance(object, 0)) == NULL)
    {
        RETURN_FALSE;
    }

    if (IS_PIPELINE(redis_sock))
    {
        RETVAL_LONG(PIPELINE);
    }
    else if (IS_MULTI(redis_sock))
    {
        RETVAL_LONG(MULTI);
    }
    else
    {
        RETVAL_LONG(ATOMIC);
    }
}

/*
 * Introspection stuff
 */

/* {{{ proto Redis::IsConnected */
PHP_METHOD(Redis, isConnected)
{
    zval *object;
    RedisSock *redis_sock;

    /* Grab our object */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &object, redis_ce) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Grab socket */
    if ((redis_sock = redis_sock_get_instance(object, 1)) == NULL)
    {
        RETURN_FALSE;
    }

    RETURN_BOOL(redis_sock->status >= REDIS_SOCK_STATUS_CONNECTED);
}

/* {{{ proto Redis::getHost() */
PHP_METHOD(Redis, getHost)
{
    RedisSock *redis_sock;

    if ((redis_sock = redis_sock_get_connected(INTERNAL_FUNCTION_PARAM_PASSTHRU)))
    {
        RETURN_STRINGL(ZSTR_VAL(redis_sock->host), ZSTR_LEN(redis_sock->host));
    }
    else
    {
        RETURN_FALSE;
    }
}

/* {{{ proto Redis::getPort() */
PHP_METHOD(Redis, getPort)
{
    RedisSock *redis_sock;

    if ((redis_sock = redis_sock_get_connected(INTERNAL_FUNCTION_PARAM_PASSTHRU)))
    {
        /* Return our port */
        RETURN_LONG(redis_sock->port);
    }
    else
    {
        RETURN_FALSE;
    }
}

/* {{{ proto Redis::getDBNum */
PHP_METHOD(Redis, getDBNum)
{
    RedisSock *redis_sock;

    if ((redis_sock = redis_sock_get_connected(INTERNAL_FUNCTION_PARAM_PASSTHRU)))
    {
        /* Return our db number */
        RETURN_LONG(redis_sock->dbNumber);
    }
    else
    {
        RETURN_FALSE;
    }
}

PHP_METHOD(Redis, getTransferredBytes)
{
    RedisSock *redis_sock;

    if ((redis_sock = redis_sock_get_instance(getThis(), 0)) == NULL)
    {
        RETURN_THROWS();
    }

    array_init_size(return_value, 2);
    add_next_index_long(return_value, redis_sock->txBytes);
    add_next_index_long(return_value, redis_sock->rxBytes);
}

PHP_METHOD(Redis, clearTransferredBytes)
{
    RedisSock *redis_sock;

    if ((redis_sock = redis_sock_get_instance(getThis(), 0)) == NULL)
    {
        RETURN_THROWS();
    }

    redis_sock->txBytes = 0;
    redis_sock->rxBytes = 0;
}

/* {{{ proto Redis::getTimeout */
PHP_METHOD(Redis, getTimeout)
{
    RedisSock *redis_sock;

    if ((redis_sock = redis_sock_get_connected(INTERNAL_FUNCTION_PARAM_PASSTHRU)))
    {
        RETURN_DOUBLE(redis_sock->timeout);
    }
    else
    {
        RETURN_FALSE;
    }
}

/* {{{ proto Redis::getReadTimeout */
PHP_METHOD(Redis, getReadTimeout)
{
    RedisSock *redis_sock;

    if ((redis_sock = redis_sock_get_connected(INTERNAL_FUNCTION_PARAM_PASSTHRU)))
    {
        RETURN_DOUBLE(redis_sock->read_timeout);
    }
    else
    {
        RETURN_FALSE;
    }
}

/* {{{ proto Redis::getPersistentID */
PHP_METHOD(Redis, getPersistentID)
{
    RedisSock *redis_sock;

    if ((redis_sock = redis_sock_get_connected(INTERNAL_FUNCTION_PARAM_PASSTHRU)) == NULL)
    {
        RETURN_FALSE;
    }
    else if (redis_sock->persistent_id == NULL)
    {
        RETURN_NULL();
    }
    RETURN_STRINGL(ZSTR_VAL(redis_sock->persistent_id), ZSTR_LEN(redis_sock->persistent_id));
}

/* {{{ proto Redis::getAuth */
PHP_METHOD(Redis, getAuth)
{
    RedisSock *redis_sock;
    zval zret;

    if (zend_parse_parameters_none() == FAILURE)
    {
        RETURN_FALSE;
    }

    redis_sock = redis_sock_get_connected(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    if (redis_sock == NULL)
        RETURN_FALSE;

    if (redis_sock->user && redis_sock->pass)
    {
        array_init(&zret);
        add_next_index_str(&zret, zend_string_copy(redis_sock->user));
        add_next_index_str(&zret, zend_string_copy(redis_sock->pass));
        RETURN_ZVAL(&zret, 0, 0);
    }
    else if (redis_sock->pass)
    {
        RETURN_STR_COPY(redis_sock->pass);
    }
    else
    {
        RETURN_NULL();
    }
}

/* {{{ proto mixed Redis::client(string $command, [ $arg1 ... $argN]) */
PHP_METHOD(Redis, client)
{
    REDIS_PROCESS_CMD(client, redis_client_response);
}
/* }}} */

/* {{{ proto mixed Redis::rawcommand(string $command, [ $arg1 ... $argN]) */
PHP_METHOD(Redis, rawcommand)
{
    int argc, cmd_len;
    char *cmd = NULL;
    RedisSock *redis_sock;
    zval *z_args;

    ZEND_PARSE_PARAMETERS_START(1, -1)
    Z_PARAM_VARIADIC('+', z_args, argc)
    ZEND_PARSE_PARAMETERS_END();

    if (redis_build_raw_cmd(z_args, argc, &cmd, &cmd_len) < 0 ||
        (redis_sock = redis_sock_get(getThis(), 0)) == NULL)
    {
        if (cmd)
            efree(cmd);
        RETURN_FALSE;
    }

    /* Execute our command */
    REDIS_PROCESS_REQUEST(redis_sock, cmd, cmd_len);
    if (IS_ATOMIC(redis_sock))
    {
        redis_read_raw_variant_reply(INTERNAL_FUNCTION_PARAM_PASSTHRU, redis_sock, NULL, NULL);
    }
    REDIS_PROCESS_RESPONSE(redis_read_variant_reply);
}
/* }}} */

/* {{{ proto array Redis::command()
 *     proto array Redis::command('info', string cmd)
 *     proto array Redis::command('getkeys', array cmd_args) */
PHP_METHOD(Redis, command)
{
    REDIS_PROCESS_CMD(command, redis_command_response);
}
/* }}} */

/* {{{ proto array Redis::copy(string $source, string $destination, array $options = null) */
PHP_METHOD(Redis, copy)
{
    REDIS_PROCESS_CMD(copy, redis_1_response)
}
/* }}} */

/* Helper to format any combination of SCAN arguments */
static int
redis_build_scan_cmd(char **cmd, REDIS_SCAN_TYPE type, char *key, int key_len,
                     uint64_t cursor, char *pattern, int pattern_len, int count,
                     zend_string *match_type)
{
    smart_string cmdstr = {0};
    char *keyword;
    int argc;

    /* Count our arguments +1 for key if it's got one, and + 2 for pattern */
    /* or count given that they each carry keywords with them. */
    argc = 1 + (key_len > 0) + (pattern_len > 0 ? 2 : 0) + (count > 0 ? 2 : 0) + (match_type ? 2 : 0);

    /* Turn our type into a keyword */
    switch (type)
    {
    case TYPE_SCAN:
        keyword = "SCAN";
        break;
    case TYPE_SSCAN:
        keyword = "SSCAN";
        break;
    case TYPE_HSCAN:
        keyword = "HSCAN";
        break;
    case TYPE_ZSCAN:
    default:
        keyword = "ZSCAN";
        break;
    }

    /* Start the command */
    redis_cmd_init_sstr(&cmdstr, argc, keyword, strlen(keyword));
    if (key_len)
        redis_cmd_append_sstr(&cmdstr, key, key_len);
    redis_cmd_append_sstr_u64(&cmdstr, cursor);

    /* Append COUNT if we've got it */
    if (count)
    {
        REDIS_CMD_APPEND_SSTR_STATIC(&cmdstr, "COUNT");
        redis_cmd_append_sstr_int(&cmdstr, count);
    }

    /* Append MATCH if we've got it */
    if (pattern_len)
    {
        REDIS_CMD_APPEND_SSTR_STATIC(&cmdstr, "MATCH");
        redis_cmd_append_sstr(&cmdstr, pattern, pattern_len);
    }

    if (match_type)
    {
        REDIS_CMD_APPEND_SSTR_STATIC(&cmdstr, "TYPE");
        redis_cmd_append_sstr(&cmdstr, ZSTR_VAL(match_type), ZSTR_LEN(match_type));
    }

    /* Return our command length */
    *cmd = cmdstr.c;
    return cmdstr.len;
}

/* {{{ proto redis::scan(&$cursor, [pattern, [count, [type]]]) */
PHP_REDIS_API void
generic_scan_cmd(INTERNAL_FUNCTION_PARAMETERS, REDIS_SCAN_TYPE type)
{
    zval *object, *z_cursor;
    RedisSock *redis_sock;
    HashTable *hash;
    char *pattern = NULL, *cmd, *key = NULL;
    int cmd_len, num_elements, key_free = 0, pattern_free = 0;
    size_t key_len = 0, pattern_len = 0;
    zend_string *match_type = NULL;
    zend_long count = 0;
    zend_bool completed;
    uint64_t cursor;

    /* Different prototype depending on if this is a key based scan */
    if (type != TYPE_SCAN)
    {
        // Requires a key
        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
                                         "Os!z/|s!l", &object, redis_ce, &key,
                                         &key_len, &z_cursor, &pattern,
                                         &pattern_len, &count) == FAILURE)
        {
            RETURN_FALSE;
        }
    }
    else
    {
        // Doesn't require a key
        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
                                         "Oz/|s!lS!", &object, redis_ce, &z_cursor,
                                         &pattern, &pattern_len, &count, &match_type) == FAILURE)
        {
            RETURN_FALSE;
        }
    }

    /* Grab our socket */
    if ((redis_sock = redis_sock_get(object, 0)) == NULL)
    {
        RETURN_FALSE;
    }

    /* Calling this in a pipeline makes no sense */
    if (!IS_ATOMIC(redis_sock))
    {
        php_error_docref(NULL, E_ERROR,
                         "Can't call SCAN commands in multi or pipeline mode!");
        RETURN_FALSE;
    }

    /* Get our SCAN cursor short circuiting if we're done */
    cursor = redisGetScanCursor(z_cursor, &completed);
    if (completed)
        RETURN_FALSE;

    /* Prefix our key if we've got one and we have a prefix set */
    if (key_len)
    {
        key_free = redis_key_prefix(redis_sock, &key, &key_len);
    }

    if (redis_sock->scan & REDIS_SCAN_PREFIX)
    {
        pattern_free = redis_key_prefix(redis_sock, &pattern, &pattern_len);
    }

    /**
     * Redis can return to us empty keys, especially in the case where there
     * are a large number of keys to scan, and we're matching against a
     * pattern.  phpredis can be set up to abstract this from the user, by
     * setting OPT_SCAN to REDIS_SCAN_RETRY.  Otherwise we will return empty
     * keys and the user will need to make subsequent calls with an updated
     * cursor.
     */
    do
    {
        /* Free our previous reply if we're back in the loop.  We know we are
         * if our return_value is an array */
        if (Z_TYPE_P(return_value) == IS_ARRAY)
        {
            zval_dtor(return_value);
            ZVAL_NULL(return_value);
        }

        // Format our SCAN command
        cmd_len = redis_build_scan_cmd(&cmd, type, key, key_len, cursor,
                                       pattern, pattern_len, count, match_type);

        /* Execute our command getting our new cursor value */
        REDIS_PROCESS_REQUEST(redis_sock, cmd, cmd_len);
        if (redis_sock_read_scan_reply(INTERNAL_FUNCTION_PARAM_PASSTHRU,
                                       redis_sock, type, &cursor) < 0)
        {
            if (key_free)
                efree(key);
            RETURN_FALSE;
        }

        /* Get the number of elements */
        hash = Z_ARRVAL_P(return_value);
        num_elements = zend_hash_num_elements(hash);
    } while (redis_sock->scan & REDIS_SCAN_RETRY && cursor != 0 &&
             num_elements == 0);

    /* Free our pattern if it was prefixed */
    if (pattern_free)
        efree(pattern);

    /* Free our key if it was prefixed */
    if (key_free)
        efree(key);

    /* Update our cursor reference */
    redisSetScanCursor(z_cursor, cursor);
}

PHP_METHOD(Redis, hscan)
{
    generic_scan_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, TYPE_HSCAN);
}

/*
 * HyperLogLog based commands
 */

/* {{{ proto Redis::pfAdd(string key, array elements) }}} */
PHP_METHOD(Redis, pfadd)
{
    REDIS_PROCESS_CMD(pfadd, redis_long_response);
}

/* {{{ proto Redis::pfCount(string key) }}}*/
PHP_METHOD(Redis, pfcount)
{
    REDIS_PROCESS_CMD(pfcount, redis_long_response);
}

/* {{{ proto Redis::pfMerge(string dstkey, array keys) }}}*/
PHP_METHOD(Redis, pfmerge)
{
    REDIS_PROCESS_CMD(pfmerge, redis_boolean_response);
}

/*
 * Geo commands
 */

/*
 * Streams
 */

PHP_METHOD(Redis, xack)
{
    REDIS_PROCESS_CMD(xack, redis_long_response);
}

PHP_METHOD(Redis, xadd)
{
    REDIS_PROCESS_CMD(xadd, redis_read_variant_reply);
}

PHP_METHOD(Redis, xautoclaim)
{
    REDIS_PROCESS_CMD(xautoclaim, redis_xclaim_reply);
}

PHP_METHOD(Redis, xclaim)
{
    REDIS_PROCESS_CMD(xclaim, redis_xclaim_reply);
}

PHP_METHOD(Redis, xdel)
{
    REDIS_PROCESS_KW_CMD("XDEL", redis_key_str_arr_cmd, redis_long_response);
}

PHP_METHOD(Redis, xgroup)
{
    REDIS_PROCESS_CMD(xgroup, redis_read_variant_reply);
}

PHP_METHOD(Redis, xinfo)
{
    REDIS_PROCESS_CMD(xinfo, redis_xinfo_reply);
}

PHP_METHOD(Redis, xlen)
{
    REDIS_PROCESS_KW_CMD("XLEN", redis_key_cmd, redis_long_response);
}

PHP_METHOD(Redis, xpending)
{
    REDIS_PROCESS_CMD(xpending, redis_read_variant_reply_strings);
}

PHP_METHOD(Redis, xrange)
{
    REDIS_PROCESS_KW_CMD("XRANGE", redis_xrange_cmd, redis_xrange_reply);
}

PHP_METHOD(Redis, xread)
{
    REDIS_PROCESS_CMD(xread, redis_xread_reply);
}

PHP_METHOD(Redis, xreadgroup)
{
    REDIS_PROCESS_CMD(xreadgroup, redis_xread_reply);
}

PHP_METHOD(Redis, xrevrange)
{
    REDIS_PROCESS_KW_CMD("XREVRANGE", redis_xrange_cmd, redis_xrange_reply);
}

PHP_METHOD(Redis, xtrim)
{
    REDIS_PROCESS_CMD(xtrim, redis_long_response);
}

/* vim: set tabstop=4 softtabstop=4 expandtab shiftwidth=4: */
