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
  | Author: Michael Grunder <michael.grunder@gmail.com>                  |
  | Maintainer: Nicolas Favre-Felix <n.favre-felix@owlient.eu>           |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#if 0
#include "common.h"
#include "php_redis.h"
#include "ext/standard/info.h"
#include "crc16.h"
#include "redis_cluster.h"

#include <ext/spl/spl_exceptions.h>
#include <zend_exceptions.h>

#include <php_variables.h>
#include <SAPI.h>
#include "redis_glide.h"

zend_class_entry *redis_cluster_ce;

/* Exception handler */
zend_class_entry *redis_cluster_exception_ce;

#if PHP_VERSION_ID < 80000
#include "redis_cluster_legacy_arginfo.h"
#else
#include "zend_attributes.h"
#include "redis_cluster_arginfo.h"
#endif

PHP_MINIT_FUNCTION(redis_cluster)
{
    redis_cluster_ce = register_class_ValkeyGlideCluster();

    redis_cluster_exception_ce = register_class_ValkeyGlideClusterException(spl_ce_RuntimeException);

    return SUCCESS;
}

/* Handlers for ValkeyGlideCluster */
zend_object_handlers ValkeyGlideCluster_handlers;

/*
 * PHP Methods
 */

/* Create a ValkeyGlideCluster Object */
PHP_METHOD(ValkeyGlideCluster, __construct)
{
    zval *object, *z_seeds = NULL, *z_auth = NULL, *context = NULL;
    zend_string *user = NULL, *pass = NULL;
    double timeout = 0.0, read_timeout = 0.0;
    size_t name_len;
    zend_bool persistent = 0;
    redisCluster *c = GET_CONTEXT();
    char *name;

    // Parse arguments
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
                                     "Os!|addbza!", &object, redis_cluster_ce, &name,
                                     &name_len, &z_seeds, &timeout, &read_timeout,
                                     &persistent, &z_auth, &context) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* The normal case, loading from arguments */

    valkey_glide_object *valkey_glide = PHPREDIS_GET_OBJECT(valkey_glide_object, object);
    ClientConfig config;

    config.tls_mode_ = false;
    config.database_ = 0;
    config.request_timeout_ = 250;
    config.client_name_ = "stam";
    config.read_from_ = Primary;
    config.is_cluster = false;

    valkey_glide->glide_client = create_glide_client(&config);
}

/*
 * ValkeyGlideCluster method implementation
 */

/* {{{ proto bool ValkeyGlideCluster::close() */
PHP_METHOD(ValkeyGlideCluster, close)
{
    cluster_disconnect(GET_CONTEXT(), 1);
    RETURN_TRUE;
}

/* {{{ proto string ValkeyGlideCluster::get(string key) */
PHP_METHOD(ValkeyGlideCluster, get)
{
    redis_get_implementation(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::getdel(string key) */
PHP_METHOD(ValkeyGlideCluster, getdel)
{
    redis_get_implementation(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto array|false ValkeyGlideCluster::getWithMeta(string key) */
PHP_METHOD(ValkeyGlideCluster, getWithMeta)
{
    redis_get_implementation(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::set(string key, string value) */
PHP_METHOD(ValkeyGlideCluster, set)
{
    redis_get_implementation(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* Generic handler for MGET/MSET/MSETNX */
static int
distcmd_resp_handler(INTERNAL_FUNCTION_PARAMETERS, redisCluster *c, short slot,
                     clusterMultiCmd *mc, zval *z_ret, int last, cluster_cb cb)
{
    clusterMultiCtx *ctx;

    // Finalize multi command
    cluster_multi_fini(mc);

    // Spin up multi context
    ctx = emalloc(sizeof(clusterMultiCtx));
    ctx->z_multi = z_ret;
    ctx->count = mc->argc;
    ctx->last = last;

    // Attempt to send the command
    if (cluster_send_command(c, slot, mc->cmd.c, mc->cmd.len) < 0 || c->err != NULL)
    {
        efree(ctx);
        return -1;
    }

    if (CLUSTER_IS_ATOMIC(c))
    {
        // Process response now
        cb(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, (void *)ctx);
    }
    else
    {
        CLUSTER_ENQUEUE_RESPONSE(c, slot, cb, ctx);
    }

    // Clear out our command but retain allocated memory
    CLUSTER_MULTI_CLEAR(mc);

    return 0;
}

/* Container struct for a key/value pair pulled from an array */
typedef struct clusterKeyValHT
{
    char kbuf[22];

    char *key;
    size_t key_len;
    int key_free;
    short slot;

    char *val;
    size_t val_len;
    int val_free;
} clusterKeyValHT;

/* Helper to pull a key/value pair from a HashTable */
static int get_key_val_ht(redisCluster *c, HashTable *ht, HashPosition *ptr,
                          clusterKeyValHT *kv)
{
    zval *z_val;
    zend_ulong idx;

    // Grab the key, convert it to a string using provided kbuf buffer if it's
    // a LONG style key
    zend_string *zkey;
    switch (zend_hash_get_current_key_ex(ht, &zkey, &idx, ptr))
    {
    case HASH_KEY_IS_STRING:
        kv->key_len = ZSTR_LEN(zkey);
        kv->key = ZSTR_VAL(zkey);
        break;
    case HASH_KEY_IS_LONG:
        kv->key_len = snprintf(kv->kbuf, sizeof(kv->kbuf), "%ld", (long)idx);
        kv->key = kv->kbuf;
        break;
    default:
        CLUSTER_THROW_EXCEPTION("Internal Zend HashTable error", 0);
        return -1;
    }

    // Prefix our key if we need to, set the slot
    kv->key_free = redis_key_prefix(c->flags, &(kv->key), &(kv->key_len));
    kv->slot = cluster_hash_key(kv->key, kv->key_len);

    // Now grab our value
    if ((z_val = zend_hash_get_current_data_ex(ht, ptr)) == NULL)
    {
        CLUSTER_THROW_EXCEPTION("Internal Zend HashTable error", 0);
        return -1;
    }

    // Serialize our value if required
    kv->val_free = redis_pack(c->flags, z_val, &(kv->val), &(kv->val_len));

    // Success
    return 0;
}

/* Helper to pull, prefix, and hash a key from a HashTable value */
static int get_key_ht(redisCluster *c, HashTable *ht, HashPosition *ptr,
                      clusterKeyValHT *kv)
{
    zval *z_key;

    if ((z_key = zend_hash_get_current_data_ex(ht, ptr)) == NULL)
    {
        // Shouldn't happen, but check anyway
        CLUSTER_THROW_EXCEPTION("Internal Zend HashTable error", 0);
        return -1;
    }

    // Always want to work with strings
    convert_to_string(z_key);

    kv->key = Z_STRVAL_P(z_key);
    kv->key_len = Z_STRLEN_P(z_key);
    kv->key_free = redis_key_prefix(c->flags, &(kv->key), &(kv->key_len));

    // Hash our key
    kv->slot = cluster_hash_key(kv->key, kv->key_len);

    // Success
    return 0;
}

/* Turn variable arguments into a HashTable for processing */
static HashTable *method_args_to_ht(zval *z_args, int argc)
{
    HashTable *ht_ret;
    int i;

    /* Allocate our hash table */
    ALLOC_HASHTABLE(ht_ret);
    zend_hash_init(ht_ret, argc, NULL, NULL, 0);

    /* Populate our return hash table with our arguments */
    for (i = 0; i < argc; i++)
    {
        zend_hash_next_index_insert(ht_ret, &z_args[i]);
    }

    /* Return our hash table */
    return ht_ret;
}

/* Convenience handler for commands that take multiple keys such as
 * MGET, DEL, and UNLINK */
static int cluster_mkey_cmd(INTERNAL_FUNCTION_PARAMETERS, char *kw, int kw_len,
                            zval *z_ret, cluster_cb cb)
{
    redisCluster *c = GET_CONTEXT();
    clusterMultiCmd mc = {0};
    clusterKeyValHT kv;
    zval *z_args;
    HashTable *ht_arr;
    HashPosition ptr;
    int i = 1, argc = ZEND_NUM_ARGS(), ht_free = 0;
    short slot;

    /* If we don't have any arguments we're invalid */
    if (!argc)
        return -1;

    /* Extract our arguments into an array */
    z_args = ecalloc(argc, sizeof(zval));
    if (zend_get_parameters_array(ht, argc, z_args) == FAILURE)
    {
        efree(z_args);
        return -1;
    }

    /* Determine if we're working with a single array or variadic args */
    if (argc == 1 && Z_TYPE(z_args[0]) == IS_ARRAY)
    {
        ht_arr = Z_ARRVAL(z_args[0]);
        argc = zend_hash_num_elements(ht_arr);
        if (!argc)
        {
            efree(z_args);
            return -1;
        }
    }
    else
    {
        ht_arr = method_args_to_ht(z_args, argc);
        ht_free = 1;
    }

    /* MGET is readonly, DEL is not */
    c->readonly = kw_len == 4 && CLUSTER_IS_ATOMIC(c);

    // Initialize our "multi" command handler with command/len
    CLUSTER_MULTI_INIT(mc, kw, kw_len);

    // Process the first key outside of our loop, so we don't have to check if
    // it's the first iteration every time, needlessly
    zend_hash_internal_pointer_reset_ex(ht_arr, &ptr);
    if (get_key_ht(c, ht_arr, &ptr, &kv) < 0)
    {
        efree(z_args);
        return -1;
    }

    // Process our key and add it to the command
    cluster_multi_add(&mc, kv.key, kv.key_len);

    // Free key if we prefixed
    if (kv.key_free)
        efree(kv.key);

    // Move to the next key
    zend_hash_move_forward_ex(ht_arr, &ptr);

    // Iterate over keys 2...N
    slot = kv.slot;
    while (zend_hash_has_more_elements_ex(ht_arr, &ptr) == SUCCESS)
    {
        if (get_key_ht(c, ht_arr, &ptr, &kv) < 0)
        {
            cluster_multi_free(&mc);
            if (ht_free)
            {
                zend_hash_destroy(ht_arr);
                efree(ht_arr);
            }
            efree(z_args);
            return -1;
        }

        // If the slots have changed, kick off the keys we've aggregated
        if (slot != kv.slot)
        {
            // Process this batch of MGET keys
            if (distcmd_resp_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, slot,
                                     &mc, z_ret, i == argc, cb) < 0)
            {
                cluster_multi_free(&mc);
                if (ht_free)
                {
                    zend_hash_destroy(ht_arr);
                    efree(ht_arr);
                }
                efree(z_args);
                return -1;
            }
        }

        // Add this key to the command
        cluster_multi_add(&mc, kv.key, kv.key_len);

        // Free key if we prefixed
        if (kv.key_free)
            efree(kv.key);

        // Update the last slot we encountered, and the key we're on
        slot = kv.slot;
        i++;

        zend_hash_move_forward_ex(ht_arr, &ptr);
    }
    efree(z_args);

    // If we've got straggler(s) process them
    if (mc.argc > 0)
    {
        if (distcmd_resp_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, slot,
                                 &mc, z_ret, 1, cb) < 0)
        {
            cluster_multi_free(&mc);
            if (ht_free)
            {
                zend_hash_destroy(ht_arr);
                efree(ht_arr);
            }
            return -1;
        }
    }

    // Free our command
    cluster_multi_free(&mc);

    /* Clean up our hash table if we constructed it from variadic args */
    if (ht_free)
    {
        zend_hash_destroy(ht_arr);
        efree(ht_arr);
    }

    /* Return our object if we're in MULTI mode */
    if (!CLUSTER_IS_ATOMIC(c))
        RETVAL_ZVAL(getThis(), 1, 0);

    // Success
    return 0;
}

/* Handler for both MSET and MSETNX */
static int cluster_mset_cmd(INTERNAL_FUNCTION_PARAMETERS, char *kw, int kw_len,
                            zval *z_ret, cluster_cb cb)
{
    redisCluster *c = GET_CONTEXT();
    clusterKeyValHT kv;
    clusterMultiCmd mc = {0};
    zval *z_arr;
    HashTable *ht_arr;
    HashPosition ptr;
    int i = 1, argc;
    short slot;

    // Parse our arguments
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &z_arr) == FAILURE)
    {
        return -1;
    }

    // No reason to send zero args
    ht_arr = Z_ARRVAL_P(z_arr);
    if ((argc = zend_hash_num_elements(ht_arr)) == 0)
    {
        return -1;
    }

    /* This is a write command */
    c->readonly = 0;

    // Set up our multi command handler
    CLUSTER_MULTI_INIT(mc, kw, kw_len);

    // Process the first key/value pair outside of our loop
    zend_hash_internal_pointer_reset_ex(ht_arr, &ptr);
    if (get_key_val_ht(c, ht_arr, &ptr, &kv) == -1)
        return -1;
    zend_hash_move_forward_ex(ht_arr, &ptr);

    // Add this to our multi cmd, set slot, free key if we prefixed
    cluster_multi_add(&mc, kv.key, kv.key_len);
    cluster_multi_add(&mc, kv.val, kv.val_len);
    if (kv.key_free)
        efree(kv.key);
    if (kv.val_free)
        efree(kv.val);

    // While we've got more keys to set
    slot = kv.slot;
    while (zend_hash_has_more_elements_ex(ht_arr, &ptr) == SUCCESS)
    {
        // Pull the next key/value pair
        if (get_key_val_ht(c, ht_arr, &ptr, &kv) == -1)
        {
            return -1;
        }

        // If the slots have changed, process responses
        if (slot != kv.slot)
        {
            if (distcmd_resp_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, c,
                                     slot, &mc, z_ret, i == argc, cb) < 0)
            {
                cluster_multi_free(&mc);
                return -1;
            }
        }

        // Add this key and value to our command
        cluster_multi_add(&mc, kv.key, kv.key_len);
        cluster_multi_add(&mc, kv.val, kv.val_len);

        // Free our key and value if we need to
        if (kv.key_free)
            efree(kv.key);
        if (kv.val_free)
            efree(kv.val);

        // Update our slot, increment position
        slot = kv.slot;
        i++;

        // Move on
        zend_hash_move_forward_ex(ht_arr, &ptr);
    }

    // If we've got stragglers, process them too
    if (mc.argc > 0)
    {
        if (distcmd_resp_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, slot, &mc,
                                 z_ret, 1, cb) < 0)
        {
            cluster_multi_free(&mc);
            return -1;
        }
    }

    // Free our command
    cluster_multi_free(&mc);

    /* Return our object if we're in MULTI mode */
    if (!CLUSTER_IS_ATOMIC(c))
        RETVAL_ZVAL(getThis(), 1, 0);

    // Success
    return 0;
}

/* Generic passthru for DEL and UNLINK which act identically */
static void cluster_generic_delete(INTERNAL_FUNCTION_PARAMETERS,
                                   char *kw, int kw_len)
{
    zval *z_ret = emalloc(sizeof(*z_ret));

    // Initialize a LONG value to zero for our return
    ZVAL_LONG(z_ret, 0);

    // Parse args, process
    if (cluster_mkey_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, kw, kw_len, z_ret,
                         cluster_del_resp) < 0)
    {
        efree(z_ret);
        RETURN_FALSE;
    }
}

/* {{{ proto array ValkeyGlideCluster::del(string key1, string key2, ... keyN) */
PHP_METHOD(ValkeyGlideCluster, del)
{
    cluster_generic_delete(INTERNAL_FUNCTION_PARAM_PASSTHRU, "DEL", sizeof("DEL") - 1);
}

/* {{{ proto array ValkeyGlideCluster::unlink(string key1, string key2, ... keyN) */
PHP_METHOD(ValkeyGlideCluster, unlink)
{
    cluster_generic_delete(INTERNAL_FUNCTION_PARAM_PASSTHRU, "UNLINK", sizeof("UNLINK") - 1);
}

/* {{{ proto array ValkeyGlideCluster::mget(array keys) */
PHP_METHOD(ValkeyGlideCluster, mget)
{
    zval *z_ret = emalloc(sizeof(*z_ret));

    array_init(z_ret);

    // Parse args, process
    if (cluster_mkey_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "MGET",
                         sizeof("MGET") - 1, z_ret, cluster_mbulk_mget_resp) < 0)
    {
        zval_dtor(z_ret);
        efree(z_ret);
        RETURN_FALSE;
    }
}

/* {{{ proto bool ValkeyGlideCluster::mset(array keyvalues) */
PHP_METHOD(ValkeyGlideCluster, mset)
{
    zval *z_ret = emalloc(sizeof(*z_ret));

    ZVAL_TRUE(z_ret);

    // Parse args and process.  If we get a failure, free zval and return FALSE.
    if (cluster_mset_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "MSET",
                         sizeof("MSET") - 1, z_ret, cluster_mset_resp) == -1)
    {
        efree(z_ret);
        RETURN_FALSE;
    }
}

/* {{{ proto array ValkeyGlideCluster::msetnx(array keyvalues) */
PHP_METHOD(ValkeyGlideCluster, msetnx)
{
    zval *z_ret = emalloc(sizeof(*z_ret));

    array_init(z_ret);

    // Parse args and process.  If we get a failure, free mem and return FALSE
    if (cluster_mset_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "MSETNX",
                         sizeof("MSETNX") - 1, z_ret, cluster_msetnx_resp) == -1)
    {
        zval_dtor(z_ret);
        efree(z_ret);
        RETURN_FALSE;
    }
}
/* }}} */

PHP_METHOD(ValkeyGlideCluster, getex)
{
    CLUSTER_PROCESS_CMD(getex, cluster_bulk_resp, 0);
}

/* {{{ proto bool ValkeyGlideCluster::setex(string key, string value, int expiry) */
PHP_METHOD(ValkeyGlideCluster, setex)
{
    CLUSTER_PROCESS_KW_CMD("SETEX", redis_key_long_val_cmd, cluster_bool_resp, 0);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::psetex(string key, string value, int expiry) */
PHP_METHOD(ValkeyGlideCluster, psetex)
{
    CLUSTER_PROCESS_KW_CMD("PSETEX", redis_key_long_val_cmd, cluster_bool_resp, 0);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::setnx(string key, string value) */
PHP_METHOD(ValkeyGlideCluster, setnx)
{
    CLUSTER_PROCESS_KW_CMD("SETNX", redis_kv_cmd, cluster_1_resp, 0);
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::getSet(string key, string value) */
PHP_METHOD(ValkeyGlideCluster, getset)
{
    CLUSTER_PROCESS_KW_CMD("GETSET", redis_kv_cmd, cluster_bulk_resp, 0);
}
/* }}} */

/* {{{ proto int ValkeyGlideCluster::exists(string $key, string ...$more_keys) */
PHP_METHOD(ValkeyGlideCluster, exists)
{
    CLUSTER_PROCESS_KW_CMD("EXISTS", redis_varkey_cmd, cluster_long_resp, 1);
}
/* }}} */

/* {{{ proto int ValkeyGlideCluster::exists(string $key, string ...$more_keys) */
PHP_METHOD(ValkeyGlideCluster, touch)
{
    CLUSTER_PROCESS_KW_CMD("TOUCH", redis_varkey_cmd, cluster_long_resp, 0);
}

/* }}} */
/* {{{ proto array ValkeyGlide::keys(string pattern) */
PHP_METHOD(ValkeyGlideCluster, keys)
{
    redisCluster *c = GET_CONTEXT();
    redisClusterNode *node;
    size_t pat_len;
    char *pat, *cmd;
    clusterReply *resp;
    int i, cmd_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &pat, &pat_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Prefix and then build our command */
    cmd_len = redis_spprintf(c->flags, NULL, &cmd, "KEYS", "k", pat, pat_len);

    array_init(return_value);

    /* Treat as readonly */
    c->readonly = CLUSTER_IS_ATOMIC(c);

    /* Iterate over our known nodes */
    ZEND_HASH_FOREACH_PTR(c->nodes, node)
    {
        if (node == NULL)
            continue;
        if (cluster_send_slot(c, node->slot, cmd, cmd_len, TYPE_MULTIBULK) < 0)
        {
            php_error_docref(0, E_ERROR, "Can't send KEYS to %s:%d",
                             ZSTR_VAL(node->sock->host), node->sock->port);
            zval_dtor(return_value);
            efree(cmd);
            RETURN_FALSE;
        }

        /* Ensure we can get a response */
        resp = cluster_read_resp(c, 0);
        if (!resp)
        {
            php_error_docref(0, E_WARNING,
                             "Can't read response from %s:%d", ZSTR_VAL(node->sock->host),
                             node->sock->port);
            continue;
        }

        /* Iterate keys, adding to our big array */
        for (i = 0; i < resp->elements; i++)
        {
            /* Skip non bulk responses, they should all be bulk */
            if (resp->element[i]->type != TYPE_BULK)
            {
                continue;
            }

            add_next_index_stringl(return_value, resp->element[i]->str,
                                   resp->element[i]->len);
        }

        /* Free response, don't free data */
        cluster_free_reply(resp, 1);
    }
    ZEND_HASH_FOREACH_END();

    efree(cmd);
}
/* }}} */

/* {{{ proto int ValkeyGlideCluster::type(string key) */
PHP_METHOD(ValkeyGlideCluster, type)
{
    CLUSTER_PROCESS_KW_CMD("TYPE", redis_key_cmd, cluster_type_resp, 1);
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::pop(string key, [int count = 0]) */
PHP_METHOD(ValkeyGlideCluster, lpop)
{
    CLUSTER_PROCESS_KW_CMD("LPOP", redis_pop_cmd, cluster_pop_resp, 0);
}
/* }}} */

PHP_METHOD(ValkeyGlideCluster, lpos)
{
    CLUSTER_PROCESS_CMD(lpos, cluster_lpos_resp, 1);
}

/* {{{ proto string ValkeyGlideCluster::rpop(string key, [int count = 0]) */
PHP_METHOD(ValkeyGlideCluster, rpop)
{
    CLUSTER_PROCESS_KW_CMD("RPOP", redis_pop_cmd, cluster_pop_resp, 0);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::lset(string key, long index, string val) */
PHP_METHOD(ValkeyGlideCluster, lset)
{
    CLUSTER_PROCESS_KW_CMD("LSET", redis_key_long_val_cmd, cluster_bool_resp, 0);
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::spop(string key) */
PHP_METHOD(ValkeyGlideCluster, spop)
{
    if (ZEND_NUM_ARGS() == 1)
    {
        CLUSTER_PROCESS_KW_CMD("SPOP", redis_key_cmd, cluster_bulk_resp, 0);
    }
    else if (ZEND_NUM_ARGS() == 2)
    {
        CLUSTER_PROCESS_KW_CMD("SPOP", redis_key_long_cmd, cluster_mbulk_resp, 0);
    }
    else
    {
        ZEND_WRONG_PARAM_COUNT();
    }
}
/* }}} */

/* {{{ proto string|array ValkeyGlideCluster::srandmember(string key, [long count]) */
PHP_METHOD(ValkeyGlideCluster, srandmember)
{
    CLUSTER_PROCESS_CMD(srandmember, cluster_srandmember_resp, 1);
}

/* {{{ proto string ValkeyGlideCluster::strlen(string key) */
PHP_METHOD(ValkeyGlideCluster, strlen)
{
    CLUSTER_PROCESS_KW_CMD("STRLEN", redis_key_cmd, cluster_long_resp, 1);
}

/* {{{ proto long ValkeyGlideCluster::lpush(string key, string val1, ... valN) */
PHP_METHOD(ValkeyGlideCluster, lpush)
{
    CLUSTER_PROCESS_KW_CMD("LPUSH", redis_key_varval_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::rpush(string key, string val1, ... valN) */
PHP_METHOD(ValkeyGlideCluster, rpush)
{
    CLUSTER_PROCESS_KW_CMD("RPUSH", redis_key_varval_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::blpop(string key1, ... keyN, long timeout) */
PHP_METHOD(ValkeyGlideCluster, blpop)
{
    CLUSTER_PROCESS_KW_CMD("BLPOP", redis_blocking_pop_cmd, cluster_mbulk_resp, 0);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::brpop(string key1, ... keyN, long timeout */
PHP_METHOD(ValkeyGlideCluster, brpop)
{
    CLUSTER_PROCESS_KW_CMD("BRPOP", redis_blocking_pop_cmd, cluster_mbulk_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::rpushx(string key, mixed value) */
PHP_METHOD(ValkeyGlideCluster, rpushx)
{
    CLUSTER_PROCESS_KW_CMD("RPUSHX", redis_kv_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::lpushx(string key, mixed value) */
PHP_METHOD(ValkeyGlideCluster, lpushx)
{
    CLUSTER_PROCESS_KW_CMD("LPUSHX", redis_kv_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::linsert(string k,string pos,mix pvt,mix val) */
PHP_METHOD(ValkeyGlideCluster, linsert)
{
    CLUSTER_PROCESS_CMD(linsert, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::lindex(string key, long index) */
PHP_METHOD(ValkeyGlideCluster, lindex)
{
    CLUSTER_PROCESS_KW_CMD("LINDEX", redis_key_long_cmd, cluster_bulk_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::lrem(string key, long count, string val) */
PHP_METHOD(ValkeyGlideCluster, lrem)
{
    CLUSTER_PROCESS_CMD(lrem, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::rpoplpush(string key, string key) */
PHP_METHOD(ValkeyGlideCluster, rpoplpush)
{
    CLUSTER_PROCESS_KW_CMD("RPOPLPUSH", redis_key_key_cmd, cluster_bulk_resp, 0);
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::brpoplpush(string key, string key, long tm) */
PHP_METHOD(ValkeyGlideCluster, brpoplpush)
{
    CLUSTER_PROCESS_CMD(brpoplpush, cluster_bulk_resp, 0);
}
/* }}} */

PHP_METHOD(ValkeyGlideCluster, lmove)
{
    CLUSTER_PROCESS_KW_CMD("LMOVE", redis_lmove_cmd, cluster_bulk_resp, 0);
}

PHP_METHOD(ValkeyGlideCluster, blmove)
{
    CLUSTER_PROCESS_KW_CMD("BLMOVE", redis_lmove_cmd, cluster_bulk_resp, 0);
}

/* {{{ proto long ValkeyGlideCluster::llen(string key)  */
PHP_METHOD(ValkeyGlideCluster, llen)
{
    CLUSTER_PROCESS_KW_CMD("LLEN", redis_key_cmd, cluster_long_resp, 1);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::scard(string key) */
PHP_METHOD(ValkeyGlideCluster, scard)
{
    CLUSTER_PROCESS_KW_CMD("SCARD", redis_key_cmd, cluster_long_resp, 1);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::smembers(string key) */
PHP_METHOD(ValkeyGlideCluster, smembers)
{
    CLUSTER_PROCESS_KW_CMD("SMEMBERS", redis_key_cmd, cluster_mbulk_resp, 1);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::sismember(string key) */
PHP_METHOD(ValkeyGlideCluster, sismember)
{
    CLUSTER_PROCESS_KW_CMD("SISMEMBER", redis_kv_cmd, cluster_1_resp, 1);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::smismember(string key, string member0, ...memberN) */
PHP_METHOD(ValkeyGlideCluster, smismember)
{
    CLUSTER_PROCESS_KW_CMD("SMISMEMBER", redis_key_varval_cmd, cluster_variant_resp, 1);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::sadd(string key, string val1 [, ...]) */
PHP_METHOD(ValkeyGlideCluster, sadd)
{
    CLUSTER_PROCESS_KW_CMD("SADD", redis_key_varval_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::saddarray(string key, array values) */
PHP_METHOD(ValkeyGlideCluster, saddarray)
{
    CLUSTER_PROCESS_KW_CMD("SADD", redis_key_val_arr_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::srem(string key, string val1 [, ...]) */
PHP_METHOD(ValkeyGlideCluster, srem)
{
    CLUSTER_PROCESS_KW_CMD("SREM", redis_key_varval_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::sunion(string key1, ... keyN) */
PHP_METHOD(ValkeyGlideCluster, sunion)
{
    CLUSTER_PROCESS_KW_CMD("SUNION", redis_varkey_cmd, cluster_mbulk_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::sunionstore(string dst, string k1, ... kN) */
PHP_METHOD(ValkeyGlideCluster, sunionstore)
{
    CLUSTER_PROCESS_KW_CMD("SUNIONSTORE", redis_varkey_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ ptoto array ValkeyGlideCluster::sinter(string k1, ... kN) */
PHP_METHOD(ValkeyGlideCluster, sinter)
{
    CLUSTER_PROCESS_KW_CMD("SINTER", redis_varkey_cmd, cluster_mbulk_resp, 0);
}

/* {{{ proto ValkeyGlideCluster::sintercard(array $keys, int $count = -1) */
PHP_METHOD(ValkeyGlideCluster, sintercard)
{
    CLUSTER_PROCESS_KW_CMD("SINTERCARD", redis_intercard_cmd, cluster_long_resp, 0);
}
/* }}} */

/* }}} */

/* {{{ ptoto long ValkeyGlideCluster::sinterstore(string dst, string k1, ... kN) */
PHP_METHOD(ValkeyGlideCluster, sinterstore)
{
    CLUSTER_PROCESS_KW_CMD("SINTERSTORE", redis_varkey_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::sdiff(string k1, ... kN) */
PHP_METHOD(ValkeyGlideCluster, sdiff)
{
    CLUSTER_PROCESS_KW_CMD("SDIFF", redis_varkey_cmd, cluster_mbulk_resp, 1);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::sdiffstore(string dst, string k1, ... kN) */
PHP_METHOD(ValkeyGlideCluster, sdiffstore)
{
    CLUSTER_PROCESS_KW_CMD("SDIFFSTORE", redis_varkey_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::smove(string src, string dst, string mem) */
PHP_METHOD(ValkeyGlideCluster, smove)
{
    CLUSTER_PROCESS_CMD(smove, cluster_1_resp, 0);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::persist(string key) */
PHP_METHOD(ValkeyGlideCluster, persist)
{
    CLUSTER_PROCESS_KW_CMD("PERSIST", redis_key_cmd, cluster_1_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::ttl(string key) */
PHP_METHOD(ValkeyGlideCluster, ttl)
{
    CLUSTER_PROCESS_KW_CMD("TTL", redis_key_cmd, cluster_long_resp, 1);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::pttl(string key) */
PHP_METHOD(ValkeyGlideCluster, pttl)
{
    CLUSTER_PROCESS_KW_CMD("PTTL", redis_key_cmd, cluster_long_resp, 1);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::zcard(string key) */
PHP_METHOD(ValkeyGlideCluster, zcard)
{
    CLUSTER_PROCESS_KW_CMD("ZCARD", redis_key_cmd, cluster_long_resp, 1);
}
/* }}} */

/* {{{ proto double ValkeyGlideCluster::zscore(string key) */
PHP_METHOD(ValkeyGlideCluster, zscore)
{
    CLUSTER_PROCESS_KW_CMD("ZSCORE", redis_kv_cmd, cluster_dbl_resp, 1);
}
/* }}} */

PHP_METHOD(ValkeyGlideCluster, zmscore)
{
    CLUSTER_PROCESS_KW_CMD("ZMSCORE", redis_key_varval_cmd, cluster_mbulk_dbl_resp, 1);
}

/* {{{ proto long ValkeyGlideCluster::zadd(string key,double score,string mem, ...) */
PHP_METHOD(ValkeyGlideCluster, zadd)
{
    CLUSTER_PROCESS_CMD(zadd, cluster_zadd_resp, 0);
}
/* }}} */

/* {{{ proto double ValkeyGlideCluster::zincrby(string key, double by, string mem) */
PHP_METHOD(ValkeyGlideCluster, zincrby)
{
    CLUSTER_PROCESS_CMD(zincrby, cluster_dbl_resp, 0);
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::zremrangebyscore(string k, string s, string e) */
PHP_METHOD(ValkeyGlideCluster, zremrangebyscore)
{
    CLUSTER_PROCESS_KW_CMD("ZREMRANGEBYSCORE", redis_key_str_str_cmd,
                           cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::zcount(string key, string s, string e) */
PHP_METHOD(ValkeyGlideCluster, zcount)
{
    CLUSTER_PROCESS_KW_CMD("ZCOUNT", redis_key_str_str_cmd, cluster_long_resp, 1);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::zrank(string key, mixed member) */
PHP_METHOD(ValkeyGlideCluster, zrank)
{
    CLUSTER_PROCESS_KW_CMD("ZRANK", redis_kv_cmd, cluster_long_resp, 1);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::zrevrank(string key, mixed member) */
PHP_METHOD(ValkeyGlideCluster, zrevrank)
{
    CLUSTER_PROCESS_KW_CMD("ZREVRANK", redis_kv_cmd, cluster_long_resp, 1);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::hlen(string key) */
PHP_METHOD(ValkeyGlideCluster, hlen)
{
    CLUSTER_PROCESS_KW_CMD("HLEN", redis_key_cmd, cluster_long_resp, 1);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::hkeys(string key) */
PHP_METHOD(ValkeyGlideCluster, hkeys)
{
    CLUSTER_PROCESS_KW_CMD("HKEYS", redis_key_cmd, cluster_mbulk_raw_resp, 1);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::hvals(string key) */
PHP_METHOD(ValkeyGlideCluster, hvals)
{
    CLUSTER_PROCESS_KW_CMD("HVALS", redis_key_cmd, cluster_mbulk_resp, 1);
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::hget(string key, string mem) */
PHP_METHOD(ValkeyGlideCluster, hget)
{
    CLUSTER_PROCESS_KW_CMD("HGET", redis_key_str_cmd, cluster_bulk_resp, 1);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::hset(string key, string mem, string val) */
PHP_METHOD(ValkeyGlideCluster, hset)
{
    CLUSTER_PROCESS_CMD(hset, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::hsetnx(string key, string mem, string val) */
PHP_METHOD(ValkeyGlideCluster, hsetnx)
{
    CLUSTER_PROCESS_CMD(hsetnx, cluster_1_resp, 0);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::hgetall(string key) */
PHP_METHOD(ValkeyGlideCluster, hgetall)
{
    CLUSTER_PROCESS_KW_CMD("HGETALL", redis_key_cmd,
                           cluster_mbulk_zipstr_resp, 1);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::hexists(string key, string member) */
PHP_METHOD(ValkeyGlideCluster, hexists)
{
    CLUSTER_PROCESS_KW_CMD("HEXISTS", redis_key_str_cmd, cluster_1_resp, 1);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::hincr(string key, string mem, long val) */
PHP_METHOD(ValkeyGlideCluster, hincrby)
{
    CLUSTER_PROCESS_CMD(hincrby, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto double ValkeyGlideCluster::hincrbyfloat(string k, string m, double v) */
PHP_METHOD(ValkeyGlideCluster, hincrbyfloat)
{
    CLUSTER_PROCESS_CMD(hincrbyfloat, cluster_dbl_resp, 0);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::hmset(string key, array key_vals) */
PHP_METHOD(ValkeyGlideCluster, hmset)
{
    CLUSTER_PROCESS_CMD(hmset, cluster_bool_resp, 0);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::hrandfield(string key, [array $options]) */
PHP_METHOD(ValkeyGlideCluster, hrandfield)
{
    CLUSTER_PROCESS_CMD(hrandfield, cluster_hrandfield_resp, 1);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::hdel(string key, string mem1, ... memN) */
PHP_METHOD(ValkeyGlideCluster, hdel)
{
    CLUSTER_PROCESS_CMD(hdel, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::hmget(string key, array members) */
PHP_METHOD(ValkeyGlideCluster, hmget)
{
    CLUSTER_PROCESS_CMD(hmget, cluster_mbulk_assoc_resp, 1);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::hstrlen(string key, string field) */
PHP_METHOD(ValkeyGlideCluster, hstrlen)
{
    CLUSTER_PROCESS_CMD(hstrlen, cluster_long_resp, 1);
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::dump(string key) */
PHP_METHOD(ValkeyGlideCluster, dump)
{
    CLUSTER_PROCESS_KW_CMD("DUMP", redis_key_cmd, cluster_bulk_raw_resp, 1);
}

/* {{{ proto long ValkeyGlideCluster::incr(string key) */
PHP_METHOD(ValkeyGlideCluster, incr)
{
    CLUSTER_PROCESS_CMD(incr, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::incrby(string key, long byval) */
PHP_METHOD(ValkeyGlideCluster, incrby)
{
    CLUSTER_PROCESS_KW_CMD("INCRBY", redis_key_long_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::decr(string key) */
PHP_METHOD(ValkeyGlideCluster, decr)
{
    CLUSTER_PROCESS_CMD(decr, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::decrby(string key, long byval) */
PHP_METHOD(ValkeyGlideCluster, decrby)
{
    CLUSTER_PROCESS_KW_CMD("DECRBY", redis_key_long_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto double ValkeyGlideCluster::incrbyfloat(string key, double val) */
PHP_METHOD(ValkeyGlideCluster, incrbyfloat)
{
    CLUSTER_PROCESS_KW_CMD("INCRBYFLOAT", redis_key_dbl_cmd,
                           cluster_dbl_resp, 0);
}
/* }}} */

/* {{{ proto double ValkeyGlideCluster::decrbyfloat(string key, double val) */
PHP_METHOD(ValkeyGlideCluster, decrbyfloat)
{
    CLUSTER_PROCESS_KW_CMD("DECRBYFLOAT", redis_key_dbl_cmd,
                           cluster_dbl_resp, 0);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::expire(string key, long sec) */
PHP_METHOD(ValkeyGlideCluster, expire)
{
    CLUSTER_PROCESS_KW_CMD("EXPIRE", redis_expire_cmd, cluster_1_resp, 0);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::expireat(string key, long ts) */
PHP_METHOD(ValkeyGlideCluster, expireat)
{
    CLUSTER_PROCESS_KW_CMD("EXPIREAT", redis_expire_cmd, cluster_1_resp, 0);
}

/* {{{ proto bool ValkeyGlideCluster::pexpire(string key, long ms) */
PHP_METHOD(ValkeyGlideCluster, pexpire)
{
    CLUSTER_PROCESS_KW_CMD("PEXPIRE", redis_expire_cmd, cluster_1_resp, 0);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::pexpireat(string key, long ts) */
PHP_METHOD(ValkeyGlideCluster, pexpireat)
{
    CLUSTER_PROCESS_KW_CMD("PEXPIREAT", redis_expire_cmd, cluster_1_resp, 0);
}
/* }}} */

/* {{{ ValkeyGlide::expiretime(string $key): int */
PHP_METHOD(ValkeyGlideCluster, expiretime)
{
    CLUSTER_PROCESS_KW_CMD("EXPIRETIME", redis_key_cmd, cluster_long_resp, 1);
}

/* {{{ ValkeyGlide::pexpiretime(string $key): int */
PHP_METHOD(ValkeyGlideCluster, pexpiretime)
{
    CLUSTER_PROCESS_KW_CMD("PEXPIRETIME", redis_key_cmd, cluster_long_resp, 1);
}

/* {{{ proto long ValkeyGlideCluster::append(string key, string val) */
PHP_METHOD(ValkeyGlideCluster, append)
{
    CLUSTER_PROCESS_KW_CMD("APPEND", redis_kv_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::getbit(string key, long val) */
PHP_METHOD(ValkeyGlideCluster, getbit)
{
    CLUSTER_PROCESS_KW_CMD("GETBIT", redis_key_long_cmd, cluster_long_resp, 1);
}
/* }}} */

PHP_METHOD(ValkeyGlideCluster, expiremember)
{
    CLUSTER_PROCESS_CMD(expiremember, cluster_long_resp, 0);
}

PHP_METHOD(ValkeyGlideCluster, expirememberat)
{
    CLUSTER_PROCESS_CMD(expiremember, cluster_long_resp, 0);
}

/* {{{ proto long ValkeyGlideCluster::setbit(string key, long offset, bool onoff) */
PHP_METHOD(ValkeyGlideCluster, setbit)
{
    CLUSTER_PROCESS_CMD(setbit, cluster_long_resp, 0);
}

/* {{{ proto long ValkeyGlideCluster::bitop(string op,string key,[string key2,...]) */
PHP_METHOD(ValkeyGlideCluster, bitop)
{
    CLUSTER_PROCESS_CMD(bitop, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::bitcount(string key, [int start, int end]) */
PHP_METHOD(ValkeyGlideCluster, bitcount)
{
    CLUSTER_PROCESS_CMD(bitcount, cluster_long_resp, 1);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::bitpos(string key, int bit, [int s, int end]) */
PHP_METHOD(ValkeyGlideCluster, bitpos)
{
    CLUSTER_PROCESS_CMD(bitpos, cluster_long_resp, 1);
}
/* }}} */

/* {{{ proto string ValkeyGlide::lget(string key, long index) */
PHP_METHOD(ValkeyGlideCluster, lget)
{
    CLUSTER_PROCESS_KW_CMD("LINDEX", redis_key_long_cmd, cluster_bulk_resp, 1);
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::getrange(string key, long start, long end) */ PHP_METHOD(ValkeyGlideCluster, getrange)
{
    CLUSTER_PROCESS_KW_CMD("GETRANGE", redis_key_long_long_cmd,
                           cluster_bulk_resp, 1);
}
/* }}} */

/* {{{ prot ValkeyGlideCluster::lcs(string $key1, string $key2, ?array $options = NULL): mixed; */
PHP_METHOD(ValkeyGlideCluster, lcs)
{
    CLUSTER_PROCESS_CMD(lcs, cluster_variant_resp, 1);
}

/* {{{ proto ValkeyGlide|array|false ValkeyGlide::lmpop(array $keys, string $from, int $count = 1) */
PHP_METHOD(ValkeyGlideCluster, lmpop)
{
    CLUSTER_PROCESS_KW_CMD("LMPOP", redis_mpop_cmd, cluster_mpop_resp, 0);
}
/* }}} */

/* {{{ proto ValkeyGlide|array|false ValkeyGlide::blmpop(double $timeout, array $keys, string $from, int $count = 1) */
PHP_METHOD(ValkeyGlideCluster, blmpop)
{
    CLUSTER_PROCESS_KW_CMD("BLMPOP", redis_mpop_cmd, cluster_mpop_resp, 0);
}
/* }}} */

/* {{{ proto ValkeyGlide|array|false ValkeyGlide::zmpop(array $keys, string $from, int $count = 1) */
PHP_METHOD(ValkeyGlideCluster, zmpop)
{
    CLUSTER_PROCESS_KW_CMD("ZMPOP", redis_mpop_cmd, cluster_mpop_resp, 0);
}
/* }}} */

/* {{{ proto ValkeyGlide|array|false ValkeyGlide::bzmpop(double $timeout, array $keys, string $from, int $count = 1) */
PHP_METHOD(ValkeyGlideCluster, bzmpop)
{
    CLUSTER_PROCESS_KW_CMD("BZMPOP", redis_mpop_cmd, cluster_mpop_resp, 0);
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::ltrim(string key, long start, long end) */
PHP_METHOD(ValkeyGlideCluster, ltrim)
{
    CLUSTER_PROCESS_KW_CMD("LTRIM", redis_key_long_long_cmd, cluster_bool_resp, 0);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::lrange(string key, long start, long end) */
PHP_METHOD(ValkeyGlideCluster, lrange)
{
    CLUSTER_PROCESS_KW_CMD("LRANGE", redis_key_long_long_cmd,
                           cluster_mbulk_resp, 1);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::zremrangebyrank(string k, long s, long e) */
PHP_METHOD(ValkeyGlideCluster, zremrangebyrank)
{
    CLUSTER_PROCESS_KW_CMD("ZREMRANGEBYRANK", redis_key_long_long_cmd,
                           cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::publish(string key, string msg) */
PHP_METHOD(ValkeyGlideCluster, publish)
{
    CLUSTER_PROCESS_KW_CMD("PUBLISH", redis_key_str_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::rename(string key1, string key2) */
PHP_METHOD(ValkeyGlideCluster, rename)
{
    CLUSTER_PROCESS_KW_CMD("RENAME", redis_key_key_cmd, cluster_bool_resp, 0);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::renamenx(string key1, string key2) */
PHP_METHOD(ValkeyGlideCluster, renamenx)
{
    CLUSTER_PROCESS_KW_CMD("RENAMENX", redis_key_key_cmd, cluster_1_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::pfcount(string key) */
PHP_METHOD(ValkeyGlideCluster, pfcount)
{
    CLUSTER_PROCESS_CMD(pfcount, cluster_long_resp, 1);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::pfadd(string key, array vals) */
PHP_METHOD(ValkeyGlideCluster, pfadd)
{
    CLUSTER_PROCESS_CMD(pfadd, cluster_1_resp, 0);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::pfmerge(string key, array keys) */
PHP_METHOD(ValkeyGlideCluster, pfmerge)
{
    CLUSTER_PROCESS_CMD(pfmerge, cluster_bool_resp, 0);
}
/* }}} */

/* {{{ proto boolean ValkeyGlideCluster::restore(string key, long ttl, string val) */
PHP_METHOD(ValkeyGlideCluster, restore)
{
    CLUSTER_PROCESS_CMD(restore, cluster_bool_resp, 0);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::setrange(string key, long offset, string val) */
PHP_METHOD(ValkeyGlideCluster, setrange)
{
    CLUSTER_PROCESS_KW_CMD("SETRANGE", redis_key_long_str_cmd,
                           cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto
 *     array ValkeyGlideCluster::zrange(string k, long s, long e, bool score = 0) */
PHP_METHOD(ValkeyGlideCluster, zrange)
{
    CLUSTER_PROCESS_KW_CMD("ZRANGE", redis_zrange_cmd, cluster_zrange_resp, 1);
}
/* }}} */

/* {{{ proto
 *     array ValkeyGlideCluster::zrange(string $dstkey, string $srckey, long s, long e, array|bool $options = false) */
PHP_METHOD(ValkeyGlideCluster, zrangestore)
{
    CLUSTER_PROCESS_KW_CMD("ZRANGESTORE", redis_zrange_cmd, cluster_long_resp, 0);
}

/* }}} */
/* {{{ proto
 *     array ValkeyGlideCluster::zrevrange(string k,long s,long e,bool scores = 0) */
PHP_METHOD(ValkeyGlideCluster, zrevrange)
{
    CLUSTER_PROCESS_KW_CMD("ZREVRANGE", redis_zrange_cmd, cluster_zrange_resp, 1);
}
/* }}} */

/* {{{ proto array
 *     ValkeyGlideCluster::zrangebyscore(string k, long s, long e, array opts) */
PHP_METHOD(ValkeyGlideCluster, zrangebyscore)
{
    CLUSTER_PROCESS_KW_CMD("ZRANGEBYSCORE", redis_zrange_cmd, cluster_zrange_resp, 1);
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::zunionstore(string dst, array keys, [array weights,
 *                                     string agg]) */
PHP_METHOD(ValkeyGlideCluster, zunionstore)
{
    CLUSTER_PROCESS_KW_CMD("ZUNIONSTORE", redis_zinterunionstore_cmd, cluster_long_resp, 0);
}
/* }}} */

PHP_METHOD(ValkeyGlideCluster, zdiff)
{
    CLUSTER_PROCESS_CMD(zdiff, cluster_zdiff_resp, 1);
}

PHP_METHOD(ValkeyGlideCluster, zdiffstore)
{
    CLUSTER_PROCESS_CMD(zdiffstore, cluster_long_resp, 0);
}

PHP_METHOD(ValkeyGlideCluster, zinter)
{
    CLUSTER_PROCESS_KW_CMD("ZUNION", redis_zinterunion_cmd, cluster_zdiff_resp, 1);
}

PHP_METHOD(ValkeyGlideCluster, zunion)
{
    CLUSTER_PROCESS_KW_CMD("ZINTER", redis_zinterunion_cmd, cluster_zdiff_resp, 1);
}

/* {{{ proto array ValkeyGlideCluster::zrandmember(string key, array options) */
PHP_METHOD(ValkeyGlideCluster, zrandmember)
{
    CLUSTER_PROCESS_CMD(zrandmember, cluster_zrandmember_resp, 1);
}

/* }}} */
/* {{{ proto ValkeyGlideCluster::zinterstore(string dst, array keys, [array weights,
 *                                     string agg]) */
PHP_METHOD(ValkeyGlideCluster, zinterstore)
{
    CLUSTER_PROCESS_KW_CMD("ZINTERSTORE", redis_zinterunionstore_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::zintercard(array $keys, int $count = -1) */
PHP_METHOD(ValkeyGlideCluster, zintercard)
{
    CLUSTER_PROCESS_KW_CMD("ZINTERCARD", redis_intercard_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::zrem(string key, string val1, ... valN) */
PHP_METHOD(ValkeyGlideCluster, zrem)
{
    CLUSTER_PROCESS_KW_CMD("ZREM", redis_key_varval_cmd, cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto array
 *     ValkeyGlideCluster::zrevrangebyscore(string k, long s, long e, array opts) */
PHP_METHOD(ValkeyGlideCluster, zrevrangebyscore)
{
    CLUSTER_PROCESS_KW_CMD("ZREVRANGEBYSCORE", redis_zrange_cmd, cluster_zrange_resp, 1);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::zrangebylex(string key, string min, string max,
 *                                           [offset, count]) */
PHP_METHOD(ValkeyGlideCluster, zrangebylex)
{
    CLUSTER_PROCESS_KW_CMD("ZRANGEBYLEX", redis_zrangebylex_cmd,
                           cluster_mbulk_resp, 1);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::zrevrangebylex(string key, string min,
 *                                              string min, [long off, long limit) */
PHP_METHOD(ValkeyGlideCluster, zrevrangebylex)
{
    CLUSTER_PROCESS_KW_CMD("ZREVRANGEBYLEX", redis_zrangebylex_cmd,
                           cluster_mbulk_resp, 1);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::zlexcount(string key, string min, string max) */
PHP_METHOD(ValkeyGlideCluster, zlexcount)
{
    CLUSTER_PROCESS_KW_CMD("ZLEXCOUNT", redis_gen_zlex_cmd, cluster_long_resp, 1);
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::zremrangebylex(string key, string min, string max) */
PHP_METHOD(ValkeyGlideCluster, zremrangebylex)
{
    CLUSTER_PROCESS_KW_CMD("ZREMRANGEBYLEX", redis_gen_zlex_cmd,
                           cluster_long_resp, 0);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::zpopmax(string key) */
PHP_METHOD(ValkeyGlideCluster, zpopmax)
{
    if (ZEND_NUM_ARGS() == 1)
    {
        CLUSTER_PROCESS_KW_CMD("ZPOPMAX", redis_key_cmd, cluster_mbulk_zipdbl_resp, 0);
    }
    else if (ZEND_NUM_ARGS() == 2)
    {
        CLUSTER_PROCESS_KW_CMD("ZPOPMAX", redis_key_long_cmd, cluster_mbulk_zipdbl_resp, 0);
    }
    else
    {
        ZEND_WRONG_PARAM_COUNT();
    }
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::zpopmin(string key) */
PHP_METHOD(ValkeyGlideCluster, zpopmin)
{
    if (ZEND_NUM_ARGS() == 1)
    {
        CLUSTER_PROCESS_KW_CMD("ZPOPMIN", redis_key_cmd, cluster_mbulk_zipdbl_resp, 0);
    }
    else if (ZEND_NUM_ARGS() == 2)
    {
        CLUSTER_PROCESS_KW_CMD("ZPOPMIN", redis_key_long_cmd, cluster_mbulk_zipdbl_resp, 0);
    }
    else
    {
        ZEND_WRONG_PARAM_COUNT();
    }
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::bzPopMin(Array keys [, timeout]) }}} */
PHP_METHOD(ValkeyGlideCluster, bzpopmax)
{
    CLUSTER_PROCESS_KW_CMD("BZPOPMAX", redis_blocking_pop_cmd, cluster_mbulk_resp, 0);
}

/* {{{ proto array ValkeyGlideCluster::bzPopMax(Array keys [, timeout]) }}} */
PHP_METHOD(ValkeyGlideCluster, bzpopmin)
{
    CLUSTER_PROCESS_KW_CMD("BZPOPMIN", redis_blocking_pop_cmd, cluster_mbulk_resp, 0);
}

/* {{{ proto ValkeyGlideCluster::sort(string key, array options) */
PHP_METHOD(ValkeyGlideCluster, sort)
{
    CLUSTER_PROCESS_KW_CMD("SORT", redis_sort_cmd, cluster_variant_resp, 0);
}

/* {{{ proto ValkeyGlideCluster::sort_ro(string key, array options) */
PHP_METHOD(ValkeyGlideCluster, sort_ro)
{
    CLUSTER_PROCESS_KW_CMD("SORT_RO", redis_sort_cmd, cluster_variant_resp, 1);
}

/* {{{ proto ValkeyGlideCluster::object(string subcmd, string key) */
PHP_METHOD(ValkeyGlideCluster, object)
{
    CLUSTER_PROCESS_CMD(object, cluster_object_resp, 1);
}

/* {{{ proto null ValkeyGlideCluster::subscribe(array chans, callable cb) */
PHP_METHOD(ValkeyGlideCluster, subscribe)
{
    CLUSTER_PROCESS_KW_CMD("SUBSCRIBE", redis_subscribe_cmd, cluster_sub_resp, 0);
}
/* }}} */

/* {{{ proto null ValkeyGlideCluster::psubscribe(array pats, callable cb) */
PHP_METHOD(ValkeyGlideCluster, psubscribe)
{
    CLUSTER_PROCESS_KW_CMD("PSUBSCRIBE", redis_subscribe_cmd, cluster_sub_resp, 0);
}
/* }}} */

static void generic_unsub_cmd(INTERNAL_FUNCTION_PARAMETERS, redisCluster *c,
                              char *kw)
{
    char *cmd;
    int cmd_len;
    void *ctx;
    short slot;

    // There is not reason to unsubscribe outside of a subscribe loop
    if (c->subscribed_slot == -1)
    {
        php_error_docref(0, E_WARNING,
                         "You can't unsubscribe outside of a subscribe loop");
        RETURN_FALSE;
    }

    // Call directly because we're going to set the slot manually
    if (redis_unsubscribe_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, c->flags, kw,
                              &cmd, &cmd_len, &slot, &ctx) == FAILURE)
    {
        RETURN_FALSE;
    }

    // This has to operate on our subscribe slot
    if (cluster_send_slot(c, c->subscribed_slot, cmd, cmd_len, TYPE_MULTIBULK) == FAILURE)
    {
        CLUSTER_THROW_EXCEPTION("Failed to UNSUBSCRIBE within our subscribe loop!", 0);
        RETURN_FALSE;
    }

    // Now process response from the slot we're subscribed on
    cluster_unsub_resp(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, ctx);

    // Cleanup our command
    efree(cmd);
}

/* {{{ proto array ValkeyGlideCluster::unsubscribe(array chans) */
PHP_METHOD(ValkeyGlideCluster, unsubscribe)
{
    generic_unsub_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, GET_CONTEXT(),
                      "UNSUBSCRIBE");
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::punsubscribe(array pats) */
PHP_METHOD(ValkeyGlideCluster, punsubscribe)
{
    generic_unsub_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, GET_CONTEXT(),
                      "PUNSUBSCRIBE");
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::eval(string script, [array args, int numkeys) */
PHP_METHOD(ValkeyGlideCluster, eval)
{
    CLUSTER_PROCESS_KW_CMD("EVAL", redis_eval_cmd, cluster_variant_raw_resp, 0);
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::eval_ro(string script, [array args, int numkeys) */
PHP_METHOD(ValkeyGlideCluster, eval_ro)
{
    CLUSTER_PROCESS_KW_CMD("EVAL_RO", redis_eval_cmd, cluster_variant_raw_resp, 1);
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::evalsha(string sha, [array args, int numkeys]) */
PHP_METHOD(ValkeyGlideCluster, evalsha)
{
    CLUSTER_PROCESS_KW_CMD("EVALSHA", redis_eval_cmd, cluster_variant_raw_resp, 0);
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::evalsha_ro(string sha, [array args, int numkeys]) */
PHP_METHOD(ValkeyGlideCluster, evalsha_ro)
{
    CLUSTER_PROCESS_KW_CMD("EVALSHA_RO", redis_eval_cmd, cluster_variant_raw_resp, 1);
}

/* }}} */
/* Commands that do not interact with ValkeyGlide, but just report stuff about
 * various options, etc */

/* {{{ proto string ValkeyGlideCluster::getmode() */
PHP_METHOD(ValkeyGlideCluster, getmode)
{
    redisCluster *c = GET_CONTEXT();
    RETURN_LONG(c->flags->mode);
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::getlasterror() */
PHP_METHOD(ValkeyGlideCluster, getlasterror)
{
    redisCluster *c = GET_CONTEXT();

    if (c->err)
    {
        RETURN_STRINGL(ZSTR_VAL(c->err), ZSTR_LEN(c->err));
    }
    RETURN_NULL();
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::clearlasterror() */
PHP_METHOD(ValkeyGlideCluster, clearlasterror)
{
    redisCluster *c = GET_CONTEXT();

    if (c->err)
    {
        zend_string_release(c->err);
        c->err = NULL;
    }

    RETURN_TRUE;
}

static void redisSumNodeBytes(redisClusterNode *node, zend_long *tx, zend_long *rx)
{
    struct redisClusterNode *slave;

    *tx += node->sock->txBytes;
    *rx += node->sock->rxBytes;

    if (node->slaves)
    {
        ZEND_HASH_FOREACH_PTR(node->slaves, slave)
        {
            *tx += slave->sock->txBytes;
            *rx += slave->sock->rxBytes;
        }
        ZEND_HASH_FOREACH_END();
    }
}

static void redisClearNodeBytes(redisClusterNode *node)
{
    struct redisClusterNode *slave;

    node->sock->txBytes = 0;
    node->sock->rxBytes = 0;

    if (node->slaves)
    {
        ZEND_HASH_FOREACH_PTR(node->slaves, slave)
        {
            slave->sock->txBytes = 0;
            slave->sock->rxBytes = 0;
        }
        ZEND_HASH_FOREACH_END();
    }
}

PHP_METHOD(ValkeyGlideCluster, gettransferredbytes)
{
    redisCluster *c = GET_CONTEXT();
    zend_long rx = 0, tx = 0;
    redisClusterNode *node;

    ZEND_HASH_FOREACH_PTR(c->nodes, node)
    {
        redisSumNodeBytes(node, &tx, &rx);
    }
    ZEND_HASH_FOREACH_END();

    array_init_size(return_value, 2);
    add_next_index_long(return_value, tx);
    add_next_index_long(return_value, rx);
}
/* }}} */

PHP_METHOD(ValkeyGlideCluster, cleartransferredbytes)
{
    redisCluster *c = GET_CONTEXT();
    redisClusterNode *node;

    ZEND_HASH_FOREACH_PTR(c->nodes, node)
    {
        redisClearNodeBytes(node);
    }
    ZEND_HASH_FOREACH_END();
}

/* {{{ proto long ValkeyGlideCluster::getOption(long option */
PHP_METHOD(ValkeyGlideCluster, getoption)
{
    redisCluster *c = GET_CONTEXT();
    redis_getoption_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, c->flags, c);
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::setOption(long option, mixed value) */
PHP_METHOD(ValkeyGlideCluster, setoption)
{
    redisCluster *c = GET_CONTEXT();
    redis_setoption_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, c->flags, c);
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::_prefix(string key) */
PHP_METHOD(ValkeyGlideCluster, _prefix)
{
    redisCluster *c = GET_CONTEXT();
    redis_prefix_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, c->flags);
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::_serialize(mixed val) */
PHP_METHOD(ValkeyGlideCluster, _serialize)
{
    redisCluster *c = GET_CONTEXT();
    redis_serialize_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, c->flags);
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::_unserialize(string val) */
PHP_METHOD(ValkeyGlideCluster, _unserialize)
{
    redisCluster *c = GET_CONTEXT();
    redis_unserialize_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU,
                              c->flags, redis_cluster_exception_ce);
}
/* }}} */

PHP_METHOD(ValkeyGlideCluster, _compress)
{
    redisCluster *c = GET_CONTEXT();
    redis_compress_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, c->flags);
}

PHP_METHOD(ValkeyGlideCluster, _uncompress)
{
    redisCluster *c = GET_CONTEXT();
    redis_uncompress_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, c->flags,
                             redis_cluster_exception_ce);
}

PHP_METHOD(ValkeyGlideCluster, _pack)
{
    redisCluster *c = GET_CONTEXT();
    redis_pack_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, c->flags);
}

PHP_METHOD(ValkeyGlideCluster, _unpack)
{
    redisCluster *c = GET_CONTEXT();
    redis_unpack_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, c->flags);
}

/* {{{ proto array ValkeyGlideCluster::_masters() */
PHP_METHOD(ValkeyGlideCluster, _masters)
{
    redisCluster *c = GET_CONTEXT();
    redisClusterNode *node;

    array_init(return_value);

    ZEND_HASH_FOREACH_PTR(c->nodes, node)
    {
        if (node == NULL)
            break;

        zval z_sub;

        array_init(&z_sub);

        add_next_index_stringl(&z_sub, ZSTR_VAL(node->sock->host), ZSTR_LEN(node->sock->host));
        add_next_index_long(&z_sub, node->sock->port);
        add_next_index_zval(return_value, &z_sub);
    }
    ZEND_HASH_FOREACH_END();
}

PHP_METHOD(ValkeyGlideCluster, _redir)
{
    redisCluster *c = GET_CONTEXT();
    char buf[255];
    size_t len;

    len = snprintf(buf, sizeof(buf), "%s:%d", c->redir_host, c->redir_port);
    if (*c->redir_host && c->redir_host_len)
    {
        RETURN_STRINGL(buf, len);
    }
    else
    {
        RETURN_NULL();
    }
}

/*
 * Transaction handling
 */

/* {{{ proto bool ValkeyGlideCluster::multi() */
PHP_METHOD(ValkeyGlideCluster, multi)
{
    redisCluster *c = GET_CONTEXT();
    zend_long value = MULTI;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(value)
    ZEND_PARSE_PARAMETERS_END();

    if (value != MULTI)
    {
        php_error_docref(NULL, E_WARNING, "ValkeyGlideCluster does not support PIPELINING");
    }

    if (c->flags->mode == MULTI)
    {
        php_error_docref(NULL, E_WARNING,
                         "ValkeyGlideCluster is already in MULTI mode, ignoring");
        RETURN_FALSE;
    }

    /* Flag that we're in MULTI mode */
    c->flags->mode = MULTI;

    c->flags->txBytes = 0;
    c->flags->rxBytes = 0;

    /* Return our object so we can chain MULTI calls */
    RETVAL_ZVAL(getThis(), 1, 0);
}

/* {{{ proto bool ValkeyGlideCluster::watch() */
PHP_METHOD(ValkeyGlideCluster, watch)
{
    redisCluster *c = GET_CONTEXT();
    HashTable *ht_dist;
    clusterDistList *dl;
    smart_string cmd = {0};
    zval *z_args;
    int argc = ZEND_NUM_ARGS(), i;
    zend_ulong slot;
    zend_string *zstr;

    // Disallow in MULTI mode
    if (c->flags->mode == MULTI)
    {
        php_error_docref(NULL, E_WARNING,
                         "WATCH command not allowed in MULTI mode");
        RETURN_FALSE;
    }

    // Don't need to process zero arguments
    if (!argc)
        RETURN_FALSE;

    // Create our distribution HashTable
    ht_dist = cluster_dist_create();

    // Allocate args, and grab them
    z_args = emalloc(sizeof(zval) * argc);
    if (zend_get_parameters_array(ht, argc, z_args) == FAILURE)
    {
        efree(z_args);
        cluster_dist_free(ht_dist);
        RETURN_FALSE;
    }

    // Loop through arguments, prefixing if needed
    for (i = 0; i < argc; i++)
    {
        // We'll need the key as a string
        zstr = zval_get_string(&z_args[i]);

        // Add this key to our distribution handler
        if (cluster_dist_add_key(c, ht_dist, ZSTR_VAL(zstr), ZSTR_LEN(zstr), NULL) == FAILURE)
        {
            CLUSTER_THROW_EXCEPTION("Can't issue WATCH command as the keyspace isn't fully mapped", 0);
            zend_string_release(zstr);
            RETURN_FALSE;
        }
        zend_string_release(zstr);
    }

    // Iterate over each node we'll be sending commands to
    ZEND_HASH_FOREACH_PTR(ht_dist, dl)
    {
        // Grab the clusterDistList pointer itself
        if (dl == NULL)
        {
            CLUSTER_THROW_EXCEPTION("Internal error in a PHP HashTable", 0);
            cluster_dist_free(ht_dist);
            efree(z_args);
            efree(cmd.c);
            RETURN_FALSE;
        }
        else if (zend_hash_get_current_key(ht_dist, NULL, &slot) != HASH_KEY_IS_LONG)
        {
            break;
        }

        // Construct our watch command for this node
        redis_cmd_init_sstr(&cmd, dl->len, "WATCH", sizeof("WATCH") - 1);
        for (i = 0; i < dl->len; i++)
        {
            redis_cmd_append_sstr(&cmd, dl->entry[i].key,
                                  dl->entry[i].key_len);
        }

        // If we get a failure from this, we have to abort
        if (cluster_send_command(c, (short)slot, cmd.c, cmd.len) == -1)
        {
            RETURN_FALSE;
        }

        // This node is watching
        SLOT_SOCK(c, (short)slot)->watching = 1;

        // Zero out our command buffer
        cmd.len = 0;
    }
    ZEND_HASH_FOREACH_END();

    // Cleanup
    cluster_dist_free(ht_dist);
    efree(z_args);
    efree(cmd.c);

    RETURN_TRUE;
}

/* {{{ proto bool ValkeyGlideCluster::unwatch() */
PHP_METHOD(ValkeyGlideCluster, unwatch)
{
    redisCluster *c = GET_CONTEXT();
    short slot;

    // Send UNWATCH to nodes that need it
    for (slot = 0; slot < REDIS_CLUSTER_SLOTS; slot++)
    {
        if (c->master[slot] && SLOT_SOCK(c, slot)->watching)
        {
            if (cluster_send_slot(c, slot, RESP_UNWATCH_CMD,
                                  sizeof(RESP_UNWATCH_CMD) - 1,
                                  TYPE_LINE) == -1)
            {
                CLUSTER_RETURN_BOOL(c, 0);
            }

            // No longer watching
            SLOT_SOCK(c, slot)->watching = 0;
        }
    }

    CLUSTER_RETURN_BOOL(c, 1);
}

/* {{{ proto array ValkeyGlideCluster::exec() */
PHP_METHOD(ValkeyGlideCluster, exec)
{
    redisCluster *c = GET_CONTEXT();
    clusterFoldItem *fi;

    // Verify we are in fact in multi mode
    if (CLUSTER_IS_ATOMIC(c))
    {
        php_error_docref(NULL, E_WARNING, "ValkeyGlideCluster is not in MULTI mode");
        RETURN_FALSE;
    }

    // First pass, send EXEC and abort on failure
    fi = c->multi_head;
    while (fi)
    {
        if (SLOT_SOCK(c, fi->slot)->mode == MULTI)
        {
            if (cluster_send_exec(c, fi->slot) < 0)
            {
                cluster_abort_exec(c);
                CLUSTER_THROW_EXCEPTION("Error processing EXEC across the cluster", 0);

                // Free our queue, reset MULTI state
                CLUSTER_FREE_QUEUE(c);
                CLUSTER_RESET_MULTI(c);

                RETURN_FALSE;
            }
            SLOT_SOCK(c, fi->slot)->mode = ATOMIC;
            SLOT_SOCK(c, fi->slot)->watching = 0;
        }
        fi = fi->next;
    }

    // MULTI multi-bulk response handler
    cluster_multi_mbulk_resp(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, NULL);

    // Free our callback queue, any enqueued distributed command context items
    // and reset our MULTI state.
    CLUSTER_FREE_QUEUE(c);
    CLUSTER_RESET_MULTI(c);
}

/* {{{ proto bool ValkeyGlideCluster::discard() */
PHP_METHOD(ValkeyGlideCluster, discard)
{
    redisCluster *c = GET_CONTEXT();

    if (CLUSTER_IS_ATOMIC(c))
    {
        php_error_docref(NULL, E_WARNING, "Cluster is not in MULTI mode");
        RETURN_FALSE;
    }

    if (cluster_abort_exec(c) < 0)
    {
        CLUSTER_RESET_MULTI(c);
    }

    CLUSTER_FREE_QUEUE(c);

    RETURN_TRUE;
}

/* Get a slot either by key (string) or host/port array */
static short
cluster_cmd_get_slot(redisCluster *c, zval *z_arg)
{
    size_t key_len;
    int key_free;
    zval *z_host, *z_port;
    short slot;
    char *key;
    zend_string *zstr;

    /* If it's a string, treat it as a key.  Otherwise, look for a two
     * element array */
    if (Z_TYPE_P(z_arg) == IS_STRING || Z_TYPE_P(z_arg) == IS_LONG ||
        Z_TYPE_P(z_arg) == IS_DOUBLE)
    {
        /* Allow for any scalar here */
        zstr = zval_get_string(z_arg);
        key = ZSTR_VAL(zstr);
        key_len = ZSTR_LEN(zstr);

        /* Hash it */
        key_free = redis_key_prefix(c->flags, &key, &key_len);
        slot = cluster_hash_key(key, key_len);
        zend_string_release(zstr);
        if (key_free)
            efree(key);
    }
    else if (Z_TYPE_P(z_arg) == IS_ARRAY &&
             (z_host = zend_hash_index_find(Z_ARRVAL_P(z_arg), 0)) != NULL &&
             (z_port = zend_hash_index_find(Z_ARRVAL_P(z_arg), 1)) != NULL &&
             Z_TYPE_P(z_host) == IS_STRING && Z_TYPE_P(z_port) == IS_LONG)
    {
        /* Attempt to find this specific node by host:port */
        slot = cluster_find_slot(c, (const char *)Z_STRVAL_P(z_host),
                                 (unsigned short)Z_LVAL_P(z_port));

        /* Inform the caller if they've passed bad data */
        if (slot < 0)
        {
            php_error_docref(0, E_WARNING, "Unknown node %s:" ZEND_LONG_FMT,
                             Z_STRVAL_P(z_host), Z_LVAL_P(z_port));
        }
    }
    else
    {
        php_error_docref(0, E_WARNING,
                         "Directed commands must be passed a key or [host,port] array");
        return -1;
    }

    return slot;
}

/* Generic handler for things we want directed at a given node, like SAVE,
 * BGSAVE, FLUSHDB, FLUSHALL, etc */
static void
cluster_empty_node_cmd(INTERNAL_FUNCTION_PARAMETERS, char *kw,
                       REDIS_REPLY_TYPE reply_type, cluster_cb cb)
{
    redisCluster *c = GET_CONTEXT();
    char *cmd;
    int cmd_len;
    zval *z_arg;
    short slot;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &z_arg) == FAILURE)
    {
        RETURN_FALSE;
    }

    // One argument means find the node (treated like a key), and two means
    // send the command to a specific host and port
    slot = cluster_cmd_get_slot(c, z_arg);
    if (slot < 0)
    {
        RETURN_FALSE;
    }

    // Construct our command
    cmd_len = redis_spprintf(NULL, NULL, &cmd, kw, "");

    // Kick off our command
    if (cluster_send_slot(c, slot, cmd, cmd_len, reply_type) < 0)
    {
        CLUSTER_THROW_EXCEPTION("Unable to send command at a specific node", 0);
        efree(cmd);
        RETURN_FALSE;
    }

    // Our response callback
    cb(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, NULL);

    // Free our command
    efree(cmd);
}

static void
cluster_flush_cmd(INTERNAL_FUNCTION_PARAMETERS, char *kw, REDIS_REPLY_TYPE reply_type, cluster_cb cb)
{
    redisCluster *c = GET_CONTEXT();
    char *cmd;
    int cmd_len;
    zval *z_arg;
    zend_bool async = 0;
    short slot;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &z_arg, &async) == FAILURE)
    {
        RETURN_FALSE;
    }

    // One argument means find the node (treated like a key), and two means
    // send the command to a specific host and port
    slot = cluster_cmd_get_slot(c, z_arg);
    if (slot < 0)
    {
        RETURN_FALSE;
    }

    // Construct our command
    if (async)
    {
        cmd_len = redis_spprintf(NULL, NULL, &cmd, kw, "s", "ASYNC", sizeof("ASYNC") - 1);
    }
    else
    {
        cmd_len = redis_spprintf(NULL, NULL, &cmd, kw, "");
    }

    // Kick off our command
    if (cluster_send_slot(c, slot, cmd, cmd_len, reply_type) < 0)
    {
        CLUSTER_THROW_EXCEPTION("Unable to send command at a specific node", 0);
        efree(cmd);
        RETURN_FALSE;
    }

    // Our response callback
    cb(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, NULL);

    // Free our command
    efree(cmd);
}

/* Generic routine for handling various commands which need to be directed at
 * a node, but have complex syntax.  We simply parse out the arguments and send
 * the command as constructed by the caller */
static void cluster_raw_cmd(INTERNAL_FUNCTION_PARAMETERS, char *kw, int kw_len)
{
    redisCluster *c = GET_CONTEXT();
    smart_string cmd = {0};
    zval *z_args;
    short slot;
    int i, argc = ZEND_NUM_ARGS();

    /* Commands using this pass-through don't need to be enabled in MULTI mode */
    if (!CLUSTER_IS_ATOMIC(c))
    {
        php_error_docref(0, E_WARNING,
                         "Command can't be issued in MULTI mode");
        RETURN_FALSE;
    }

    /* We at least need the key or [host,port] argument */
    if (argc < 1)
    {
        php_error_docref(0, E_WARNING,
                         "Command requires at least an argument to direct to a node");
        RETURN_FALSE;
    }

    /* Allocate an array to process arguments */
    z_args = emalloc(argc * sizeof(zval));

    /* Grab args */
    if (zend_get_parameters_array(ht, argc, z_args) == FAILURE)
    {
        efree(z_args);
        RETURN_FALSE;
    }

    /* First argument needs to be the "where" */
    if ((slot = cluster_cmd_get_slot(c, &z_args[0])) < 0)
    {
        efree(z_args);
        RETURN_FALSE;
    }

    /* Initialize our command */
    redis_cmd_init_sstr(&cmd, argc - 1, kw, kw_len);

    /* Iterate, appending args */
    for (i = 1; i < argc; i++)
    {
        zend_string *zstr = zval_get_string(&z_args[i]);
        redis_cmd_append_sstr(&cmd, ZSTR_VAL(zstr), ZSTR_LEN(zstr));
        zend_string_release(zstr);
    }

    /* Send it off */
    if (cluster_send_slot(c, slot, cmd.c, cmd.len, TYPE_EOF) < 0)
    {
        CLUSTER_THROW_EXCEPTION("Couldn't send command to node", 0);
        efree(cmd.c);
        efree(z_args);
        RETURN_FALSE;
    }

    /* Read the response variant */
    cluster_variant_resp(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, NULL);

    efree(cmd.c);
    efree(z_args);
}

/* Generic method for HSCAN, SSCAN, and ZSCAN */
static void cluster_kscan_cmd(INTERNAL_FUNCTION_PARAMETERS,
                              REDIS_SCAN_TYPE type)
{
    redisCluster *c = GET_CONTEXT();
    char *cmd, *pat = NULL, *key = NULL;
    size_t key_len = 0, pat_len = 0, pat_free = 0;
    int cmd_len, key_free = 0;
    short slot;
    zval *z_it;
    HashTable *hash;
    long num_ele;
    zend_long count = 0;
    zend_bool completed;
    uint64_t cursor;

    // Can't be in MULTI mode
    if (!CLUSTER_IS_ATOMIC(c))
    {
        CLUSTER_THROW_EXCEPTION("SCAN type commands can't be called in MULTI mode!", 0);
        RETURN_FALSE;
    }

    /* Parse arguments */
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz/|s!l", &key,
                              &key_len, &z_it, &pat, &pat_len, &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Treat as readonly */
    c->readonly = 1;

    /* Get our scan cursor and return early if we're done */
    cursor = redisGetScanCursor(z_it, &completed);
    if (completed)
        RETURN_FALSE;

    // Apply any key prefix we have, get the slot
    key_free = redis_key_prefix(c->flags, &key, &key_len);
    slot = cluster_hash_key(key, key_len);

    if (c->flags->scan & REDIS_SCAN_PREFIX)
    {
        pat_free = redis_key_prefix(c->flags, &pat, &pat_len);
    }

    // If SCAN_RETRY is set, loop until we get a zero iterator or until
    // we get non-zero elements.  Otherwise we just send the command once.
    do
    {
        /* Free our return value if we're back in the loop */
        if (Z_TYPE_P(return_value) == IS_ARRAY)
        {
            zval_dtor(return_value);
            ZVAL_NULL(return_value);
        }

        // Create command
        cmd_len = redis_fmt_scan_cmd(&cmd, type, key, key_len, cursor, pat, pat_len,
                                     count);

        // Send it off
        if (cluster_send_command(c, slot, cmd, cmd_len) == FAILURE)
        {
            CLUSTER_THROW_EXCEPTION("Couldn't send SCAN command", 0);
            if (key_free)
                efree(key);
            efree(cmd);
            RETURN_FALSE;
        }

        // Read response
        if (cluster_scan_resp(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, type,
                              &cursor) == FAILURE)
        {
            CLUSTER_THROW_EXCEPTION("Couldn't read SCAN response", 0);
            if (key_free)
                efree(key);
            efree(cmd);
            RETURN_FALSE;
        }

        // Count the elements we got back
        hash = Z_ARRVAL_P(return_value);
        num_ele = zend_hash_num_elements(hash);

        // Free our command
        efree(cmd);
    } while (c->flags->scan & REDIS_SCAN_RETRY && cursor != 0 && num_ele == 0);

    // Free our pattern
    if (pat_free)
        efree(pat);

    // Free our key
    if (key_free)
        efree(key);

    // Update iterator reference
    redisSetScanCursor(z_it, cursor);
}

static int redis_acl_op_readonly(zend_string *op)
{
    /* Only return read-only for operations we know to be */
    if (ZSTR_STRICMP_STATIC(op, "LIST") ||
        ZSTR_STRICMP_STATIC(op, "USERS") ||
        ZSTR_STRICMP_STATIC(op, "GETUSER") ||
        ZSTR_STRICMP_STATIC(op, "CAT") ||
        ZSTR_STRICMP_STATIC(op, "GENPASS") ||
        ZSTR_STRICMP_STATIC(op, "WHOAMI") ||
        ZSTR_STRICMP_STATIC(op, "LOG"))
        return 1;

    return 0;
}

PHP_METHOD(ValkeyGlideCluster, acl)
{
    redisCluster *c = GET_CONTEXT();
    smart_string cmdstr = {0};
    int argc = ZEND_NUM_ARGS(), i, readonly;
    cluster_cb cb;
    zend_string *zs;
    zval *zargs;
    void *ctx = NULL;
    short slot;

    /* ACL in cluster needs a slot argument, and then at least the op */
    if (argc < 2)
    {
        WRONG_PARAM_COUNT;
        RETURN_FALSE;
    }

    /* Grab all our arguments and determine the command slot */
    zargs = emalloc(argc * sizeof(*zargs));
    if (zend_get_parameters_array(ht, argc, zargs) == FAILURE ||
        (slot = cluster_cmd_get_slot(c, &zargs[0]) < 0))
    {
        efree(zargs);
        RETURN_FALSE;
    }

    REDIS_CMD_INIT_SSTR_STATIC(&cmdstr, argc - 1, "ACL");

    /* Read the op, determine if it's readonly, and add it */
    zs = zval_get_string(&zargs[1]);
    readonly = redis_acl_op_readonly(zs);
    redis_cmd_append_sstr_zstr(&cmdstr, zs);

    /* We have specialized handlers for GETUSER and LOG, whereas every
     * other ACL command can be handled generically */
    if (zend_string_equals_literal_ci(zs, "GETUSER"))
    {
        cb = cluster_acl_getuser_resp;
    }
    else if (zend_string_equals_literal_ci(zs, "LOG"))
    {
        cb = cluster_acl_log_resp;
    }
    else
    {
        cb = cluster_variant_resp;
    }

    zend_string_release(zs);

    /* Process remaining args */
    for (i = 2; i < argc; i++)
    {
        zs = zval_get_string(&zargs[i]);
        redis_cmd_append_sstr_zstr(&cmdstr, zs);
        zend_string_release(zs);
    }

    /* Can we use replicas? */
    c->readonly = readonly && CLUSTER_IS_ATOMIC(c);

    /* Kick off our command */
    if (cluster_send_slot(c, slot, cmdstr.c, cmdstr.len, TYPE_EOF) < 0)
    {
        CLUSTER_THROW_EXCEPTION("Unabler to send ACL command", 0);
        efree(zargs);
        RETURN_FALSE;
    }

    if (CLUSTER_IS_ATOMIC(c))
    {
        cb(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, NULL);
    }
    else
    {
        CLUSTER_ENQUEUE_RESPONSE(c, slot, cb, ctx);
    }

    efree(cmdstr.c);
    efree(zargs);
}

/* {{{ proto ValkeyGlideCluster::scan(string master, long it [, string pat, long cnt]) */
PHP_METHOD(ValkeyGlideCluster, scan)
{
    redisCluster *c = GET_CONTEXT();
    char *cmd, *pat = NULL;
    size_t pat_len = 0;
    int cmd_len;
    short slot;
    zval *zcursor, *z_node;
    long num_ele, pat_free = 0;
    zend_long count = 0;
    zend_bool completed;
    uint64_t cursor;

    /* Treat as read-only */
    c->readonly = CLUSTER_IS_ATOMIC(c);

    /* Can't be in MULTI mode */
    if (!CLUSTER_IS_ATOMIC(c))
    {
        CLUSTER_THROW_EXCEPTION("SCAN type commands can't be called in MULTI mode", 0);
        RETURN_FALSE;
    }

    /* Parse arguments */
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z/z|s!l", &zcursor,
                              &z_node, &pat, &pat_len, &count) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get the scan cursor and return early if we're done */
    cursor = redisGetScanCursor(zcursor, &completed);
    if (completed)
        RETURN_FALSE;

    if (c->flags->scan & REDIS_SCAN_PREFIX)
    {
        pat_free = redis_key_prefix(c->flags, &pat, &pat_len);
    }

    /* With SCAN_RETRY on, loop until we get some keys, otherwise just return
     * what ValkeyGlide does, as it does */
    do
    {
        /* Free our return value if we're back in the loop */
        if (Z_TYPE_P(return_value) == IS_ARRAY)
        {
            zval_dtor(return_value);
            ZVAL_NULL(return_value);
        }

        /* Construct our command */
        cmd_len = redis_fmt_scan_cmd(&cmd, TYPE_SCAN, NULL, 0, cursor, pat, pat_len,
                                     count);

        if ((slot = cluster_cmd_get_slot(c, z_node)) < 0)
        {
            RETURN_FALSE;
        }

        // Send it to the node in question
        if (cluster_send_command(c, slot, cmd, cmd_len) < 0)
        {
            CLUSTER_THROW_EXCEPTION("Couldn't send SCAN to node", 0);
            efree(cmd);
            RETURN_FALSE;
        }

        if (cluster_scan_resp(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, TYPE_SCAN,
                              &cursor) == FAILURE ||
            Z_TYPE_P(return_value) != IS_ARRAY)
        {
            CLUSTER_THROW_EXCEPTION("Couldn't process SCAN response from node", 0);
            efree(cmd);
            RETURN_FALSE;
        }

        efree(cmd);

        num_ele = zend_hash_num_elements(Z_ARRVAL_P(return_value));
    } while (c->flags->scan & REDIS_SCAN_RETRY && cursor != 0 && num_ele == 0);

    if (pat_free)
        efree(pat);

    redisSetScanCursor(zcursor, cursor);
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::sscan(string key, long it [string pat, long cnt]) */
PHP_METHOD(ValkeyGlideCluster, sscan)
{
    cluster_kscan_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, TYPE_SSCAN);
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::zscan(string key, long it [string pat, long cnt]) */
PHP_METHOD(ValkeyGlideCluster, zscan)
{
    cluster_kscan_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, TYPE_ZSCAN);
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::hscan(string key, long it [string pat, long cnt]) */
PHP_METHOD(ValkeyGlideCluster, hscan)
{
    cluster_kscan_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, TYPE_HSCAN);
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::save(string key)
 *     proto ValkeyGlideCluster::save(array host_port) */
PHP_METHOD(ValkeyGlideCluster, save)
{
    cluster_empty_node_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "SAVE", TYPE_LINE,
                           cluster_bool_resp);
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::bgsave(string key)
 *     proto ValkeyGlideCluster::bgsave(array host_port) */
PHP_METHOD(ValkeyGlideCluster, bgsave)
{
    cluster_empty_node_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "BGSAVE",
                           TYPE_LINE, cluster_bool_resp);
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::flushdb(string key, [bool async])
 *     proto ValkeyGlideCluster::flushdb(array host_port, [bool async]) */
PHP_METHOD(ValkeyGlideCluster, flushdb)
{
    cluster_flush_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "FLUSHDB",
                      TYPE_LINE, cluster_bool_resp);
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::flushall(string key, [bool async])
 *     proto ValkeyGlideCluster::flushall(array host_port, [bool async]) */
PHP_METHOD(ValkeyGlideCluster, flushall)
{
    cluster_flush_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "FLUSHALL",
                      TYPE_LINE, cluster_bool_resp);
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::dbsize(string key)
 *     proto ValkeyGlideCluster::dbsize(array host_port) */
PHP_METHOD(ValkeyGlideCluster, dbsize)
{
    cluster_empty_node_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "DBSIZE",
                           TYPE_INT, cluster_long_resp);
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::bgrewriteaof(string key)
 *     proto ValkeyGlideCluster::bgrewriteaof(array host_port) */
PHP_METHOD(ValkeyGlideCluster, bgrewriteaof)
{
    cluster_empty_node_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "BGREWRITEAOF",
                           TYPE_LINE, cluster_bool_resp);
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::lastsave(string key)
 *     proto ValkeyGlideCluster::lastsave(array $host_port) */
PHP_METHOD(ValkeyGlideCluster, lastsave)
{
    cluster_empty_node_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "LASTSAVE",
                           TYPE_INT, cluster_long_resp);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::info(string key, [string $arg])
 *     proto array ValkeyGlideCluster::info(array host_port, [string $arg]) */
PHP_METHOD(ValkeyGlideCluster, info)
{
    redisCluster *c = GET_CONTEXT();
    zval *node = NULL, *args = NULL;
    smart_string cmdstr = {0};
    REDIS_REPLY_TYPE rtype;
    zend_string *section;
    void *ctx = NULL;
    int i, argc;
    short slot;

    ZEND_PARSE_PARAMETERS_START(1, -1)
    Z_PARAM_ZVAL(node)
    Z_PARAM_OPTIONAL
    Z_PARAM_VARIADIC('*', args, argc)
    ZEND_PARSE_PARAMETERS_END();

    if ((slot = cluster_cmd_get_slot(c, node)) < 0)
        RETURN_FALSE;

    REDIS_CMD_INIT_SSTR_STATIC(&cmdstr, argc, "INFO");

    /* Direct this command at the master */
    c->readonly = 0;

    for (i = 0; i < argc; i++)
    {
        section = zval_get_string(&args[i]);
        redis_cmd_append_sstr_zstr(&cmdstr, section);
        zend_string_release(section);
    }

    rtype = CLUSTER_IS_ATOMIC(c) ? TYPE_BULK : TYPE_LINE;
    if (cluster_send_slot(c, slot, cmdstr.c, cmdstr.len, rtype) < 0)
    {
        CLUSTER_THROW_EXCEPTION("Unable to send INFO command to specific node", 0);
        efree(cmdstr.c);
        RETURN_FALSE;
    }

    if (CLUSTER_IS_ATOMIC(c))
    {
        cluster_info_resp(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, NULL);
    }
    else
    {
        CLUSTER_ENQUEUE_RESPONSE(c, slot, cluster_info_resp, ctx);
    }

    efree(cmdstr.c);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::client('list')
 *     proto bool ValkeyGlideCluster::client('kill', $ipport)
 *     proto bool ValkeyGlideCluster::client('setname', $name)
 *     proto string ValkeyGlideCluster::client('getname')
 */
PHP_METHOD(ValkeyGlideCluster, client)
{
    redisCluster *c = GET_CONTEXT();
    char *cmd, *opt = NULL, *arg = NULL;
    int cmd_len;
    size_t opt_len, arg_len = 0;
    REDIS_REPLY_TYPE rtype;
    zval *z_node;
    short slot;
    cluster_cb cb;

    /* Parse args */
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "zs|s", &z_node, &opt,
                              &opt_len, &arg, &arg_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Make sure we can properly resolve the slot */
    slot = cluster_cmd_get_slot(c, z_node);
    if (slot < 0)
        RETURN_FALSE;

    /* Our return type and reply callback is different for all subcommands */
    if (opt_len == 4 && !strncasecmp(opt, "list", 4))
    {
        rtype = CLUSTER_IS_ATOMIC(c) ? TYPE_BULK : TYPE_LINE;
        cb = cluster_client_list_resp;
    }
    else if ((opt_len == 4 && !strncasecmp(opt, "kill", 4)) ||
             (opt_len == 7 && !strncasecmp(opt, "setname", 7)))
    {
        rtype = TYPE_LINE;
        cb = cluster_bool_resp;
    }
    else if (opt_len == 7 && !strncasecmp(opt, "getname", 7))
    {
        rtype = CLUSTER_IS_ATOMIC(c) ? TYPE_BULK : TYPE_LINE;
        cb = cluster_bulk_resp;
    }
    else
    {
        php_error_docref(NULL, E_WARNING,
                         "Invalid CLIENT subcommand (LIST, KILL, GETNAME, and SETNAME are valid");
        RETURN_FALSE;
    }

    /* Construct the command */
    if (ZEND_NUM_ARGS() == 3)
    {
        cmd_len = redis_spprintf(NULL, NULL, &cmd, "CLIENT", "ss",
                                 opt, opt_len, arg, arg_len);
    }
    else if (ZEND_NUM_ARGS() == 2)
    {
        cmd_len = redis_spprintf(NULL, NULL, &cmd, "CLIENT", "s",
                                 opt, opt_len);
    }
    else
    {
        zend_wrong_param_count();
        RETURN_FALSE;
    }

    /* Attempt to write our command */
    if (cluster_send_slot(c, slot, cmd, cmd_len, rtype) < 0)
    {
        CLUSTER_THROW_EXCEPTION("Unable to send CLIENT command to specific node", 0);
        efree(cmd);
        RETURN_FALSE;
    }

    /* Now enqueue or process response */
    if (CLUSTER_IS_ATOMIC(c))
    {
        cb(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, NULL);
    }
    else
    {
        void *ctx = NULL;
        CLUSTER_ENQUEUE_RESPONSE(c, slot, cb, ctx);
    }

    efree(cmd);
}

/* {{{ proto mixed ValkeyGlideCluster::cluster(variant) */
PHP_METHOD(ValkeyGlideCluster, cluster)
{
    cluster_raw_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "CLUSTER",
                    sizeof("CLUSTER") - 1);
}
/* }}} */

/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::config(string key, ...)
 *     proto mixed ValkeyGlideCluster::config(array host_port, ...) */
PHP_METHOD(ValkeyGlideCluster, config)
{
    cluster_raw_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "CONFIG",
                    sizeof("CONFIG") - 1);
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::pubsub(string key, ...)
 *     proto mixed ValkeyGlideCluster::pubsub(array host_port, ...) */
PHP_METHOD(ValkeyGlideCluster, pubsub)
{
    cluster_raw_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "PUBSUB",
                    sizeof("PUBSUB") - 1);
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::script(string key, ...)
 *     proto mixed ValkeyGlideCluster::script(array host_port, ...) */
PHP_METHOD(ValkeyGlideCluster, script)
{
    redisCluster *c = GET_CONTEXT();
    smart_string cmd = {0};
    zval *z_args;
    short slot;
    int argc = ZEND_NUM_ARGS();

    /* Commands using this pass-through don't need to be enabled in MULTI mode */
    if (!CLUSTER_IS_ATOMIC(c))
    {
        php_error_docref(0, E_WARNING,
                         "Command can't be issued in MULTI mode");
        RETURN_FALSE;
    }

    /* We at least need the key or [host,port] argument */
    if (argc < 2)
    {
        php_error_docref(0, E_WARNING,
                         "Command requires at least an argument to direct to a node");
        RETURN_FALSE;
    }

    /* Allocate an array to process arguments */
    z_args = ecalloc(argc, sizeof(zval));

    /* Grab args */
    if (zend_get_parameters_array(ht, argc, z_args) == FAILURE ||
        (slot = cluster_cmd_get_slot(c, &z_args[0])) < 0 ||
        redis_build_script_cmd(&cmd, argc - 1, &z_args[1]) == NULL)
    {
        efree(z_args);
        RETURN_FALSE;
    }

    /* Send it off */
    if (cluster_send_slot(c, slot, cmd.c, cmd.len, TYPE_EOF) < 0)
    {
        CLUSTER_THROW_EXCEPTION("Couldn't send command to node", 0);
        efree(cmd.c);
        efree(z_args);
        RETURN_FALSE;
    }

    /* Read the response variant */
    cluster_variant_resp(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, NULL);

    efree(cmd.c);
    efree(z_args);
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::slowlog(string key, ...)
 *     proto mixed ValkeyGlideCluster::slowlog(array host_port, ...) */
PHP_METHOD(ValkeyGlideCluster, slowlog)
{
    cluster_raw_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "SLOWLOG",
                    sizeof("SLOWLOG") - 1);
}
/* }}} */

/* {{{ proto int ValkeyGlideCluster::geoadd(string key, float long float lat string mem, ...) */
PHP_METHOD(ValkeyGlideCluster, geoadd)
{
    CLUSTER_PROCESS_CMD(geoadd, cluster_long_resp, 0);
}

/* {{{ proto array ValkeyGlideCluster::geohash(string key, string mem1, [string mem2...]) */
PHP_METHOD(ValkeyGlideCluster, geohash)
{
    CLUSTER_PROCESS_KW_CMD("GEOHASH", redis_key_varval_cmd, cluster_mbulk_raw_resp, 1);
}

/* {{{ proto array ValkeyGlideCluster::geopos(string key, string mem1, [string mem2...]) */
PHP_METHOD(ValkeyGlideCluster, geopos)
{
    CLUSTER_PROCESS_KW_CMD("GEOPOS", redis_key_varval_cmd, cluster_variant_resp, 1);
}

/* {{{ proto array ValkeyGlideCluster::geodist(string key, string mem1, string mem2 [string unit]) */
PHP_METHOD(ValkeyGlideCluster, geodist)
{
    CLUSTER_PROCESS_CMD(geodist, cluster_dbl_resp, 1);
}

/* {{{ proto array ValkeyGlideCluster::georadius() }}} */
PHP_METHOD(ValkeyGlideCluster, georadius)
{
    CLUSTER_PROCESS_KW_CMD("GEORADIUS", redis_georadius_cmd, cluster_variant_resp, 1);
}

/* {{{ proto array ValkeyGlideCluster::georadius() }}} */
PHP_METHOD(ValkeyGlideCluster, georadius_ro)
{
    CLUSTER_PROCESS_KW_CMD("GEORADIUS_RO", redis_georadius_cmd, cluster_variant_resp, 1);
}

/* {{{ proto array ValkeyGlideCluster::georadiusbymember() }}} */
PHP_METHOD(ValkeyGlideCluster, georadiusbymember)
{
    CLUSTER_PROCESS_KW_CMD("GEORADIUSBYMEMBER", redis_georadiusbymember_cmd, cluster_variant_resp, 1);
}

/* {{{ proto array ValkeyGlideCluster::georadiusbymember() }}} */
PHP_METHOD(ValkeyGlideCluster, georadiusbymember_ro)
{
    CLUSTER_PROCESS_KW_CMD("GEORADIUSBYMEMBER_RO", redis_georadiusbymember_cmd, cluster_variant_resp, 1);
}

PHP_METHOD(ValkeyGlideCluster, geosearch)
{
    CLUSTER_PROCESS_CMD(geosearch, cluster_geosearch_resp, 1);
}

PHP_METHOD(ValkeyGlideCluster, geosearchstore)
{
    CLUSTER_PROCESS_CMD(geosearchstore, cluster_long_resp, 0);
}

/* {{{ proto array ValkeyGlideCluster::role(string key)
 *     proto array ValkeyGlideCluster::role(array host_port) */
PHP_METHOD(ValkeyGlideCluster, role)
{
    cluster_empty_node_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "ROLE",
                           TYPE_MULTIBULK, cluster_variant_resp);
}

/* {{{ proto array ValkeyGlideCluster::time(string key)
 *     proto array ValkeyGlideCluster::time(array host_port) */
PHP_METHOD(ValkeyGlideCluster, time)
{
    cluster_empty_node_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "TIME",
                           TYPE_MULTIBULK, cluster_variant_resp);
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::randomkey(string key)
 *     proto string ValkeyGlideCluster::randomkey(array host_port) */
PHP_METHOD(ValkeyGlideCluster, randomkey)
{
    cluster_empty_node_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, "RANDOMKEY",
                           TYPE_BULK, cluster_bulk_resp);
}
/* }}} */

PHP_METHOD(ValkeyGlideCluster, waitaof)
{
    zend_long numlocal, numreplicas, timeout;
    redisCluster *c = GET_CONTEXT();
    smart_string cmdstr = {0};
    void *ctx = NULL;
    short slot;
    zval *node;

    ZEND_PARSE_PARAMETERS_START(4, 4)
    Z_PARAM_ZVAL(node)
    Z_PARAM_LONG(numlocal)
    Z_PARAM_LONG(numreplicas)
    Z_PARAM_LONG(timeout)
    ZEND_PARSE_PARAMETERS_END();

    if (numlocal < 0 || numreplicas < 0 || timeout < 0)
    {
        php_error_docref(NULL, E_WARNING, "No arguments can be negative");
        RETURN_FALSE;
    }

    slot = cluster_cmd_get_slot(c, node);
    if (slot < 0)
    {
        RETURN_FALSE;
    }

    REDIS_CMD_INIT_SSTR_STATIC(&cmdstr, 3, "WAITAOF");
    redis_cmd_append_sstr_long(&cmdstr, numlocal);
    redis_cmd_append_sstr_long(&cmdstr, numreplicas);
    redis_cmd_append_sstr_long(&cmdstr, timeout);

    c->readonly = 0;

    if (cluster_send_slot(c, slot, cmdstr.c, cmdstr.len, TYPE_MULTIBULK) < 0)
    {
        CLUSTER_THROW_EXCEPTION("Unable to send command at the specified node", 0);
        smart_string_free(&cmdstr);
        RETURN_FALSE;
    }

    if (CLUSTER_IS_ATOMIC(c))
    {
        cluster_variant_resp(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, NULL);
    }
    else
    {
        CLUSTER_ENQUEUE_RESPONSE(c, slot, cluster_variant_resp, ctx);
    }

    smart_string_free(&cmdstr);
}

/* {{{ proto bool ValkeyGlideCluster::ping(string key| string msg)
 *     proto bool ValkeyGlideCluster::ping(array host_port| string msg) */
PHP_METHOD(ValkeyGlideCluster, ping)
{
    redisCluster *c = GET_CONTEXT();
    REDIS_REPLY_TYPE rtype;
    void *ctx = NULL;
    zval *z_node;
    char *cmd, *arg = NULL;
    int cmdlen;
    size_t arglen;
    short slot;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|s!", &z_node, &arg,
                              &arglen) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Treat this as a readonly command */
    c->readonly = CLUSTER_IS_ATOMIC(c);

    /* Grab slot either by key or host/port */
    slot = cluster_cmd_get_slot(c, z_node);
    if (slot < 0)
    {
        RETURN_FALSE;
    }

    /* Construct our command */
    if (arg != NULL)
    {
        cmdlen = redis_spprintf(NULL, NULL, &cmd, "PING", "s", arg, arglen);
    }
    else
    {
        cmdlen = redis_spprintf(NULL, NULL, &cmd, "PING", "");
    }

    /* Send it off */
    rtype = CLUSTER_IS_ATOMIC(c) && arg != NULL ? TYPE_BULK : TYPE_LINE;
    if (cluster_send_slot(c, slot, cmd, cmdlen, rtype) < 0)
    {
        CLUSTER_THROW_EXCEPTION("Unable to send command at the specified node", 0);
        efree(cmd);
        RETURN_FALSE;
    }

    /* We're done with our command */
    efree(cmd);

    /* Process response */
    if (CLUSTER_IS_ATOMIC(c))
    {
        if (arg != NULL)
        {
            cluster_bulk_resp(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, NULL);
        }
        else
        {
            /* If we're atomic and didn't send an argument then we have already
             * processed the reply (which must have been successful. */
            RETURN_TRUE;
        }
    }
    else
    {
        if (arg != NULL)
        {
            CLUSTER_ENQUEUE_RESPONSE(c, slot, cluster_bulk_resp, ctx);
        }
        else
        {
            CLUSTER_ENQUEUE_RESPONSE(c, slot, cluster_variant_resp, ctx);
        }

        RETURN_ZVAL(getThis(), 1, 0);
    }
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::xack(string key, string group, array ids) }}} */
PHP_METHOD(ValkeyGlideCluster, xack)
{
    CLUSTER_PROCESS_CMD(xack, cluster_long_resp, 0);
}

/* {{{ proto string ValkeyGlideCluster::xadd(string key, string id, array field_values) }}} */
PHP_METHOD(ValkeyGlideCluster, xadd)
{
    CLUSTER_PROCESS_CMD(xadd, cluster_bulk_raw_resp, 0);
}

/* {{{ proto array ValkeyGlideCluster::xclaim(string key, string group, string consumer,
 *                                      long min_idle_time, array ids, array options) */
PHP_METHOD(ValkeyGlideCluster, xclaim)
{
    CLUSTER_PROCESS_CMD(xclaim, cluster_xclaim_resp, 0);
}

PHP_METHOD(ValkeyGlideCluster, xautoclaim)
{
    CLUSTER_PROCESS_CMD(xautoclaim, cluster_xclaim_resp, 0);
}

PHP_METHOD(ValkeyGlideCluster, xdel)
{
    CLUSTER_PROCESS_KW_CMD("XDEL", redis_key_str_arr_cmd, cluster_long_resp, 0);
}

/* {{{ proto variant ValkeyGlideCluster::xgroup(string op, [string key, string arg1, string arg2]) }}} */
PHP_METHOD(ValkeyGlideCluster, xgroup)
{
    CLUSTER_PROCESS_CMD(xgroup, cluster_variant_resp, 0);
}

/* {{{ proto variant ValkeyGlideCluster::xinfo(string op, [string arg1, string arg2]); */
PHP_METHOD(ValkeyGlideCluster, xinfo)
{
    CLUSTER_PROCESS_CMD(xinfo, cluster_xinfo_resp, 0);
}

/* {{{ proto string ValkeyGlideCluster::xlen(string key) }}} */
PHP_METHOD(ValkeyGlideCluster, xlen)
{
    CLUSTER_PROCESS_KW_CMD("XLEN", redis_key_cmd, cluster_long_resp, 1);
}

PHP_METHOD(ValkeyGlideCluster, xpending)
{
    CLUSTER_PROCESS_CMD(xpending, cluster_variant_resp_strings, 1);
}

PHP_METHOD(ValkeyGlideCluster, xrange)
{
    CLUSTER_PROCESS_KW_CMD("XRANGE", redis_xrange_cmd, cluster_xrange_resp, 1);
}

PHP_METHOD(ValkeyGlideCluster, xrevrange)
{
    CLUSTER_PROCESS_KW_CMD("XREVRANGE", redis_xrange_cmd, cluster_xrange_resp, 1);
}

PHP_METHOD(ValkeyGlideCluster, xread)
{
    CLUSTER_PROCESS_CMD(xread, cluster_xread_resp, 1);
}

PHP_METHOD(ValkeyGlideCluster, xreadgroup)
{
    CLUSTER_PROCESS_CMD(xreadgroup, cluster_xread_resp, 0);
}

PHP_METHOD(ValkeyGlideCluster, xtrim)
{
    CLUSTER_PROCESS_CMD(xtrim, cluster_long_resp, 0);
}

/* {{{ proto string ValkeyGlideCluster::echo(string key, string msg)
 *     proto string ValkeyGlideCluster::echo(array host_port, string msg) */
PHP_METHOD(ValkeyGlideCluster, echo)
{
    redisCluster *c = GET_CONTEXT();
    REDIS_REPLY_TYPE rtype;
    zval *z_arg;
    char *cmd, *msg;
    int cmd_len;
    size_t msg_len;
    short slot;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "zs", &z_arg, &msg,
                              &msg_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Treat this as a readonly command */
    c->readonly = CLUSTER_IS_ATOMIC(c);

    /* Grab slot either by key or host/port */
    slot = cluster_cmd_get_slot(c, z_arg);
    if (slot < 0)
    {
        RETURN_FALSE;
    }

    /* Construct our command */
    cmd_len = redis_spprintf(NULL, NULL, &cmd, "ECHO", "s", msg, msg_len);

    /* Send it off */
    rtype = CLUSTER_IS_ATOMIC(c) ? TYPE_BULK : TYPE_LINE;
    if (cluster_send_slot(c, slot, cmd, cmd_len, rtype) < 0)
    {
        CLUSTER_THROW_EXCEPTION("Unable to send command at the specified node", 0);
        efree(cmd);
        RETURN_FALSE;
    }

    /* Process bulk response */
    if (CLUSTER_IS_ATOMIC(c))
    {
        cluster_bulk_resp(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, NULL);
    }
    else
    {
        void *ctx = NULL;
        CLUSTER_ENQUEUE_RESPONSE(c, slot, cluster_bulk_resp, ctx);
    }

    efree(cmd);
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::rawcommand(string $key, string $cmd, [ $argv1 .. $argvN])
 *     proto mixed ValkeyGlideCluster::rawcommand(array $host_port, string $cmd, [ $argv1 .. $argvN]) */
PHP_METHOD(ValkeyGlideCluster, rawcommand)
{
    REDIS_REPLY_TYPE rtype;
    int argc = ZEND_NUM_ARGS(), cmd_len;
    redisCluster *c = GET_CONTEXT();
    char *cmd = NULL;
    zval *z_args;
    short slot;

    /* Sanity check on our arguments */
    if (argc < 2)
    {
        php_error_docref(NULL, E_WARNING,
                         "You must pass at least node information as well as at least a command.");
        RETURN_FALSE;
    }
    z_args = emalloc(argc * sizeof(zval));
    if (zend_get_parameters_array(ht, argc, z_args) == FAILURE)
    {
        php_error_docref(NULL, E_WARNING,
                         "Internal PHP error parsing method parameters.");
        efree(z_args);
        RETURN_FALSE;
    }
    else if (redis_build_raw_cmd(&z_args[1], argc - 1, &cmd, &cmd_len) ||
             (slot = cluster_cmd_get_slot(c, &z_args[0])) < 0)
    {
        if (cmd)
            efree(cmd);
        efree(z_args);
        RETURN_FALSE;
    }

    /* Free argument array */
    efree(z_args);

    /* Direct the command */
    rtype = CLUSTER_IS_ATOMIC(c) ? TYPE_EOF : TYPE_LINE;
    if (cluster_send_slot(c, slot, cmd, cmd_len, rtype) < 0)
    {
        CLUSTER_THROW_EXCEPTION("Unable to send command to the specified node", 0);
        efree(cmd);
        RETURN_FALSE;
    }

    /* Process variant response */
    if (CLUSTER_IS_ATOMIC(c))
    {
        cluster_variant_raw_resp(INTERNAL_FUNCTION_PARAM_PASSTHRU, c, NULL);
    }
    else
    {
        void *ctx = NULL;
        CLUSTER_ENQUEUE_RESPONSE(c, slot, cluster_variant_raw_resp, ctx);
    }

    efree(cmd);
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::command()
 *     proto array ValkeyGlideCluster::command('INFO', string cmd)
 *     proto array ValkeyGlideCluster::command('GETKEYS', array cmd_args) */
PHP_METHOD(ValkeyGlideCluster, command)
{
    CLUSTER_PROCESS_CMD(command, cluster_variant_resp, 0);
}

PHP_METHOD(ValkeyGlideCluster, copy)
{
    CLUSTER_PROCESS_CMD(copy, cluster_1_resp, 0)
}
#endif /* PHP_REDIS_CLUSTER_C */
/* vim: set tabstop=4 softtabstop=4 expandtab shiftwidth=4: */
