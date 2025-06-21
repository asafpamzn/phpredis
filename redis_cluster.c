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
#if 1
#include "common.h"

#include "ext/standard/info.h"

#include <ext/spl/spl_exceptions.h>
#include <zend_exceptions.h>

#include <php_variables.h>
#include <SAPI.h>
#include "valkey_glide_commands_common.h"
#include "valkey_glide_z_common.h"
#include "valkey_glide_geo_common.h"
#include "valkey_glide_x_common.h"
#include "valkey_glide_s_common.h"
#include "valkey_glide_list_common.h"
#include "valkey_glide_hash_common.h" /* Include hash command framework */

#if PHP_VERSION_ID < 80000
#include "redis_cluster_legacy_arginfo.h"
#else
#include "zend_attributes.h"
#include "redis_cluster_arginfo.h"
#endif

/*f
 * PHP Methods
 */

/* Create a ValkeyGlideCluster Object */
PHP_METHOD(ValkeyGlideCluster, __construct)
{
    printf("ValkeyGlideCluster::__construct() is not implemented yet.\n");
    zval *object, *z_seeds = NULL, *z_auth = NULL, *context = NULL;
    zend_string *user = NULL, *pass = NULL;
    double timeout = 0.0, read_timeout = 0.0;
    zend_bool persistent = 0;

    valkey_glide_object *valkey_glide = VALKEY_GLIDE_PHP_GET_OBJECT(valkey_glide_object, object);
}

/*
 * ValkeyGlideCluster method implementation
 */

/* {{{ proto bool ValkeyGlideCluster::close() */
PHP_METHOD(ValkeyGlideCluster, close)
{
    RETURN_TRUE;
}

/* {{{ proto string ValkeyGlideCluster::get(string key) */
GET_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto string ValkeyGlideCluster::getdel(string key) */
GETDEL_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto array|false ValkeyGlideCluster::getWithMeta(string key) */
GETWITHMETA_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::set(string key, string value) */
SET_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* Generic handler for MGET/MSET/MSETNX */

/* {{{ proto array ValkeyGlideCluster::del(string key1, string key2, ... keyN) */
PHP_METHOD(ValkeyGlideCluster, del)
{
}

/* {{{ proto array ValkeyGlideCluster::unlink(string key1, string key2, ... keyN) */
PHP_METHOD(ValkeyGlideCluster, unlink)
{
}

/* {{{ proto array ValkeyGlideCluster::mget(array keys) */
PHP_METHOD(ValkeyGlideCluster, mget)
{
}

/* {{{ proto bool ValkeyGlideCluster::mset(array keyvalues) */
PHP_METHOD(ValkeyGlideCluster, mset)
{
}

/* {{{ proto array ValkeyGlideCluster::msetnx(array keyvalues) */
PHP_METHOD(ValkeyGlideCluster, msetnx)
{
}
/* }}} */

PHP_METHOD(ValkeyGlideCluster, getex)
{
}

/* {{{ proto bool ValkeyGlideCluster::setex(string key, string value, int expiry) */
SETEX_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::psetex(string key, string value, int expiry) */
PSETEX_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::setnx(string key, string value) */
SETNX_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto string ValkeyGlideCluster::getSet(string key, string value) */
PHP_METHOD(ValkeyGlideCluster, getset)
{
}
/* }}} */

/* {{{ proto int ValkeyGlideCluster::exists(string $key, string ...$more_keys) */
EXISTS_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto int ValkeyGlideCluster::touch(string $key, string ...$more_keys) */
TOUCH_METHOD_IMPL(ValkeyGlideCluster)

/* }}} */
/* {{{ proto array ValkeyGlide::keys(string pattern) */
PHP_METHOD(ValkeyGlideCluster, keys)
{
}
/* }}} */

/* {{{ proto int ValkeyGlideCluster::type(string key) */
TYPE_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto string ValkeyGlideCluster::pop(string key, [int count = 0]) */
PHP_METHOD(ValkeyGlideCluster, lpop)
{
}
/* }}} */

PHP_METHOD(ValkeyGlideCluster, lpos)
{
}

/* {{{ proto string ValkeyGlideCluster::rpop(string key, [int count = 0]) */
PHP_METHOD(ValkeyGlideCluster, rpop)
{
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::lset(string key, long index, string val) */
PHP_METHOD(ValkeyGlideCluster, lset)
{
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::spop(string key) */
PHP_METHOD(ValkeyGlideCluster, spop)
{
}
/* }}} */

/* {{{ proto string|array ValkeyGlideCluster::srandmember(string key, [long count]) */
PHP_METHOD(ValkeyGlideCluster, srandmember)
{
}

/* {{{ proto string ValkeyGlideCluster::strlen(string key) */
STRLEN_METHOD_IMPL(ValkeyGlideCluster)

/* {{{ proto long ValkeyGlideCluster::lpush(string key, string val1, ... valN) */
PHP_METHOD(ValkeyGlideCluster, lpush)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::rpush(string key, string val1, ... valN) */
PHP_METHOD(ValkeyGlideCluster, rpush)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::blpop(string key1, ... keyN, long timeout) */
PHP_METHOD(ValkeyGlideCluster, blpop)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::brpop(string key1, ... keyN, long timeout */
PHP_METHOD(ValkeyGlideCluster, brpop)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::rpushx(string key, mixed value) */
PHP_METHOD(ValkeyGlideCluster, rpushx)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::lpushx(string key, mixed value) */
PHP_METHOD(ValkeyGlideCluster, lpushx)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::linsert(string k,string pos,mix pvt,mix val) */
PHP_METHOD(ValkeyGlideCluster, linsert)
{
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::lindex(string key, long index) */
PHP_METHOD(ValkeyGlideCluster, lindex)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::lrem(string key, long count, string val) */
PHP_METHOD(ValkeyGlideCluster, lrem)
{
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::rpoplpush(string key, string key) */
PHP_METHOD(ValkeyGlideCluster, rpoplpush)
{
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::brpoplpush(string key, string key, long tm) */
PHP_METHOD(ValkeyGlideCluster, brpoplpush)
{
}
/* }}} */

PHP_METHOD(ValkeyGlideCluster, lmove)
{
}

PHP_METHOD(ValkeyGlideCluster, blmove)
{
}

/* {{{ proto long ValkeyGlideCluster::llen(string key)  */
PHP_METHOD(ValkeyGlideCluster, llen)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::scard(string key) */
PHP_METHOD(ValkeyGlideCluster, scard)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::smembers(string key) */
PHP_METHOD(ValkeyGlideCluster, smembers)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::sismember(string key) */
PHP_METHOD(ValkeyGlideCluster, sismember)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::smismember(string key, string member0, ...memberN) */
PHP_METHOD(ValkeyGlideCluster, smismember)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::sadd(string key, string val1 [, ...]) */
PHP_METHOD(ValkeyGlideCluster, sadd)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::saddarray(string key, array values) */
PHP_METHOD(ValkeyGlideCluster, saddarray)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::srem(string key, string val1 [, ...]) */
PHP_METHOD(ValkeyGlideCluster, srem)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::sunion(string key1, ... keyN) */
PHP_METHOD(ValkeyGlideCluster, sunion)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::sunionstore(string dst, string k1, ... kN) */
PHP_METHOD(ValkeyGlideCluster, sunionstore)
{
}
/* }}} */

/* {{{ ptoto array ValkeyGlideCluster::sinter(string k1, ... kN) */
PHP_METHOD(ValkeyGlideCluster, sinter)
{
}

/* {{{ proto ValkeyGlideCluster::sintercard(array $keys, int $count = -1) */
PHP_METHOD(ValkeyGlideCluster, sintercard)
{
}
/* }}} */

/* }}} */

/* {{{ ptoto long ValkeyGlideCluster::sinterstore(string dst, string k1, ... kN) */
PHP_METHOD(ValkeyGlideCluster, sinterstore)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::sdiff(string k1, ... kN) */
PHP_METHOD(ValkeyGlideCluster, sdiff)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::sdiffstore(string dst, string k1, ... kN) */
PHP_METHOD(ValkeyGlideCluster, sdiffstore)
{
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::smove(string src, string dst, string mem) */
PHP_METHOD(ValkeyGlideCluster, smove)
{
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::persist(string key) */
PHP_METHOD(ValkeyGlideCluster, persist)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::ttl(string key) */
TTL_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto long ValkeyGlideCluster::pttl(string key) */
PTTL_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto long ValkeyGlideCluster::zcard(string key) */
ZCARD_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto double ValkeyGlideCluster::zscore(string key) */
ZSCORE_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

ZMSCORE_METHOD_IMPL(ValkeyGlideCluster)

/* {{{ proto long ValkeyGlideCluster::zadd(string key,double score,string mem, ...) */
ZADD_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto double ValkeyGlideCluster::zincrby(string key, double by, string mem) */
ZINCRBY_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto ValkeyGlideCluster::zremrangebyscore(string k, string s, string e) */
ZREMRANGEBYSCORE_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto ValkeyGlideCluster::zcount(string key, string s, string e) */
ZCOUNT_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto long ValkeyGlideCluster::zrank(string key, mixed member) */
PHP_METHOD(ValkeyGlideCluster, zrank)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::zrevrank(string key, mixed member) */
PHP_METHOD(ValkeyGlideCluster, zrevrank)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::hlen(string key) */
HLEN_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto array ValkeyGlideCluster::hkeys(string key) */
HKEYS_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto array ValkeyGlideCluster::hvals(string key) */
HVALS_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto string ValkeyGlideCluster::hget(string key, string mem) */
HGET_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::hset(string key, string mem, string val) */
HSET_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::hsetnx(string key, string mem, string val) */
HSETNX_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto array ValkeyGlideCluster::hgetall(string key) */
HGETALL_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::hexists(string key, string member) */
HEXISTS_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto long ValkeyGlideCluster::hincr(string key, string mem, long val) */
PHP_METHOD(ValkeyGlideCluster, hincrby)
{
}
/* }}} */

/* {{{ proto double ValkeyGlideCluster::hincrbyfloat(string k, string m, double v) */
PHP_METHOD(ValkeyGlideCluster, hincrbyfloat)
{
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::hmset(string key, array key_vals) */
PHP_METHOD(ValkeyGlideCluster, hmset)
{
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::hrandfield(string key, [array $options]) */
PHP_METHOD(ValkeyGlideCluster, hrandfield)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::hdel(string key, string mem1, ... memN) */
PHP_METHOD(ValkeyGlideCluster, hdel)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::hmget(string key, array members) */
PHP_METHOD(ValkeyGlideCluster, hmget)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::hstrlen(string key, string field) */
PHP_METHOD(ValkeyGlideCluster, hstrlen)
{
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::dump(string key) */
PHP_METHOD(ValkeyGlideCluster, dump)
{
}

/* {{{ proto long ValkeyGlideCluster::incr(string key) */
PHP_METHOD(ValkeyGlideCluster, incr)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::incrby(string key, long byval) */
PHP_METHOD(ValkeyGlideCluster, incrby)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::decr(string key) */
PHP_METHOD(ValkeyGlideCluster, decr)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::decrby(string key, long byval) */
PHP_METHOD(ValkeyGlideCluster, decrby)
{
}
/* }}} */

/* {{{ proto double ValkeyGlideCluster::incrbyfloat(string key, double val) */
INCRBYFLOAT_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::expire(string key, long sec) */
PHP_METHOD(ValkeyGlideCluster, expire)
{
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::expireat(string key, long ts) */
PHP_METHOD(ValkeyGlideCluster, expireat)
{
}

/* {{{ proto bool ValkeyGlideCluster::pexpire(string key, long ms) */
PHP_METHOD(ValkeyGlideCluster, pexpire)
{
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::pexpireat(string key, long ts) */
PHP_METHOD(ValkeyGlideCluster, pexpireat)
{
}
/* }}} */

/* {{{ ValkeyGlide::expiretime(string $key): int */
PHP_METHOD(ValkeyGlideCluster, expiretime)
{
}

/* {{{ ValkeyGlide::pexpiretime(string $key): int */
PHP_METHOD(ValkeyGlideCluster, pexpiretime)
{
}

/* {{{ proto long ValkeyGlideCluster::append(string key, string val) */
APPEND_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto long ValkeyGlideCluster::getbit(string key, long val) */
GETBIT_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

PHP_METHOD(ValkeyGlideCluster, expiremember)
{
}

PHP_METHOD(ValkeyGlideCluster, expirememberat)
{
}

/* {{{ proto long ValkeyGlideCluster::setbit(string key, long offset, bool onoff) */
SETBIT_METHOD_IMPL(ValkeyGlideCluster)

/* {{{ proto long ValkeyGlideCluster::bitop(string op,string key,[string key2,...]) */
PHP_METHOD(ValkeyGlideCluster, bitop)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::bitcount(string key, [int start, int end]) */
PHP_METHOD(ValkeyGlideCluster, bitcount)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::bitpos(string key, int bit, [int s, int end]) */
PHP_METHOD(ValkeyGlideCluster, bitpos)
{
}
/* }}} */

/* {{{ proto string ValkeyGlide::lget(string key, long index) */
PHP_METHOD(ValkeyGlideCluster, lget)
{
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::getrange(string key, long start, long end) */
PHP_METHOD(ValkeyGlideCluster, getrange)
{
}
/* }}} */

/* {{{ prot ValkeyGlideCluster::lcs(string $key1, string $key2, ?array $options = NULL): mixed; */
PHP_METHOD(ValkeyGlideCluster, lcs)
{
}

/* {{{ proto ValkeyGlide|array|false ValkeyGlide::lmpop(array $keys, string $from, int $count = 1) */
PHP_METHOD(ValkeyGlideCluster, lmpop)
{
}
/* }}} */

/* {{{ proto ValkeyGlide|array|false ValkeyGlide::blmpop(double $timeout, array $keys, string $from, int $count = 1) */
PHP_METHOD(ValkeyGlideCluster, blmpop)
{
}
/* }}} */

/* {{{ proto ValkeyGlide|array|false ValkeyGlide::zmpop(array $keys, string $from, int $count = 1) */
PHP_METHOD(ValkeyGlideCluster, zmpop)
{
}
/* }}} */

/* {{{ proto ValkeyGlide|array|false ValkeyGlide::bzmpop(double $timeout, array $keys, string $from, int $count = 1) */
PHP_METHOD(ValkeyGlideCluster, bzmpop)
{
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::ltrim(string key, long start, long end) */
PHP_METHOD(ValkeyGlideCluster, ltrim)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::lrange(string key, long start, long end) */
PHP_METHOD(ValkeyGlideCluster, lrange)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::zremrangebyrank(string k, long s, long e) */
PHP_METHOD(ValkeyGlideCluster, zremrangebyrank)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::publish(string key, string msg) */
PHP_METHOD(ValkeyGlideCluster, publish)
{
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::rename(string key1, string key2) */
PHP_METHOD(ValkeyGlideCluster, rename)
{
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::renamenx(string key1, string key2) */
PHP_METHOD(ValkeyGlideCluster, renamenx)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::pfcount(string key) */
PFCOUNT_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::pfadd(string key, array vals) */
PFADD_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::pfmerge(string key, array keys) */
PFMERGE_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto boolean ValkeyGlideCluster::restore(string key, long ttl, string val) */
PHP_METHOD(ValkeyGlideCluster, restore)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::setrange(string key, long offset, string val) */
PHP_METHOD(ValkeyGlideCluster, setrange)
{
}
/* }}} */

/* {{{ proto
 *     array ValkeyGlideCluster::zrange(string k, long s, long e, bool score = 0) */
PHP_METHOD(ValkeyGlideCluster, zrange)
{
}
/* }}} */

/* {{{ proto
 *     array ValkeyGlideCluster::zrange(string $dstkey, string $srckey, long s, long e, array|bool $options = false) */
PHP_METHOD(ValkeyGlideCluster, zrangestore)
{
}

/* }}} */
/* {{{ proto
 *     array ValkeyGlideCluster::zrevrange(string k,long s,long e,bool scores = 0) */
PHP_METHOD(ValkeyGlideCluster, zrevrange)
{
}
/* }}} */

/* {{{ proto array
 *     ValkeyGlideCluster::zrangebyscore(string k, long s, long e, array opts) */
PHP_METHOD(ValkeyGlideCluster, zrangebyscore)
{
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::zunionstore(string dst, array keys, [array weights,
 *                                     string agg]) */
PHP_METHOD(ValkeyGlideCluster, zunionstore)
{
}
/* }}} */

PHP_METHOD(ValkeyGlideCluster, zdiff)
{
}

PHP_METHOD(ValkeyGlideCluster, zdiffstore)
{
}

PHP_METHOD(ValkeyGlideCluster, zinter)
{
}

PHP_METHOD(ValkeyGlideCluster, zunion)
{
}

/* {{{ proto array ValkeyGlideCluster::zrandmember(string key, array options) */
PHP_METHOD(ValkeyGlideCluster, zrandmember)
{
}

/* }}} */
/* {{{ proto ValkeyGlideCluster::zinterstore(string dst, array keys, [array weights,
 *                                     string agg]) */
PHP_METHOD(ValkeyGlideCluster, zinterstore)
{
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::zintercard(array $keys, int $count = -1) */
PHP_METHOD(ValkeyGlideCluster, zintercard)
{
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::zrem(string key, string val1, ... valN) */
PHP_METHOD(ValkeyGlideCluster, zrem)
{
}
/* }}} */

/* {{{ proto array
 *     ValkeyGlideCluster::zrevrangebyscore(string k, long s, long e, array opts) */
PHP_METHOD(ValkeyGlideCluster, zrevrangebyscore)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::zrangebylex(string key, string min, string max,
 *                                           [offset, count]) */
PHP_METHOD(ValkeyGlideCluster, zrangebylex)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::zrevrangebylex(string key, string min,
 *                                              string min, [long off, long limit) */
PHP_METHOD(ValkeyGlideCluster, zrevrangebylex)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::zlexcount(string key, string min, string max) */
PHP_METHOD(ValkeyGlideCluster, zlexcount)
{
}
/* }}} */

/* {{{ proto long ValkeyGlideCluster::zremrangebylex(string key, string min, string max) */
PHP_METHOD(ValkeyGlideCluster, zremrangebylex)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::zpopmax(string key) */
PHP_METHOD(ValkeyGlideCluster, zpopmax)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::zpopmin(string key) */
PHP_METHOD(ValkeyGlideCluster, zpopmin)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::bzPopMin(Array keys [, timeout]) }}} */
PHP_METHOD(ValkeyGlideCluster, bzpopmax)
{
}

/* {{{ proto array ValkeyGlideCluster::bzPopMax(Array keys [, timeout]) }}} */
PHP_METHOD(ValkeyGlideCluster, bzpopmin)
{
}

/* {{{ proto ValkeyGlideCluster::sort(string key, array options) */
PHP_METHOD(ValkeyGlideCluster, sort)
{
}

/* {{{ proto ValkeyGlideCluster::sort_ro(string key, array options) */
PHP_METHOD(ValkeyGlideCluster, sort_ro)
{
}

/* {{{ proto ValkeyGlideCluster::object(string subcmd, string key) */
PHP_METHOD(ValkeyGlideCluster, object)
{
}

/* {{{ proto null ValkeyGlideCluster::subscribe(array chans, callable cb) */
PHP_METHOD(ValkeyGlideCluster, subscribe)
{
}
/* }}} */

/* {{{ proto null ValkeyGlideCluster::psubscribe(array pats, callable cb) */
PHP_METHOD(ValkeyGlideCluster, psubscribe)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::unsubscribe(array chans) */
PHP_METHOD(ValkeyGlideCluster, unsubscribe)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::punsubscribe(array pats) */
PHP_METHOD(ValkeyGlideCluster, punsubscribe)
{
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::eval(string script, [array args, int numkeys) */
PHP_METHOD(ValkeyGlideCluster, eval)
{
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::eval_ro(string script, [array args, int numkeys) */
PHP_METHOD(ValkeyGlideCluster, eval_ro)
{
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::evalsha(string sha, [array args, int numkeys]) */
PHP_METHOD(ValkeyGlideCluster, evalsha)
{
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::evalsha_ro(string sha, [array args, int numkeys]) */
PHP_METHOD(ValkeyGlideCluster, evalsha_ro)
{
}

/* }}} */
/* Commands that do not interact with ValkeyGlide, but just report stuff about
 * various options, etc */

/* {{{ proto string ValkeyGlideCluster::getmode() */
PHP_METHOD(ValkeyGlideCluster, getmode)
{
}
/* }}} */

/* {{{ proto string ValkeyGlideCluster::getlasterror() */
PHP_METHOD(ValkeyGlideCluster, getlasterror)
{
}
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::clearlasterror() */
PHP_METHOD(ValkeyGlideCluster, clearlasterror)
{
}

/*
 * Transaction handling
 */

/* {{{ proto bool ValkeyGlideCluster::multi() */
PHP_METHOD(ValkeyGlideCluster, multi)
{
}

/* {{{ proto bool ValkeyGlideCluster::watch() */
PHP_METHOD(ValkeyGlideCluster, watch)
{
}

/* {{{ proto bool ValkeyGlideCluster::unwatch() */
PHP_METHOD(ValkeyGlideCluster, unwatch)
{
}

/* {{{ proto array ValkeyGlideCluster::exec() */
PHP_METHOD(ValkeyGlideCluster, exec)
{
}

/* {{{ proto bool ValkeyGlideCluster::discard() */
PHP_METHOD(ValkeyGlideCluster, discard)
{
}

/* {{{ proto ValkeyGlideCluster::scan(string master, long it [, string pat, long cnt]) */
PHP_METHOD(ValkeyGlideCluster, scan)
{
}
/* }}} */

/* {{{ proto ValkeyGlideCluster::sscan(string key, long it [string pat, long cnt]) */
SSCAN_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto ValkeyGlideCluster::zscan(string key, long it [string pat, long cnt]) */
ZSCAN_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto ValkeyGlideCluster::hscan(string key, long it [string pat, long cnt]) */
HSCAN_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto ValkeyGlideCluster::flushdb(string key, [bool async])
 *     proto ValkeyGlideCluster::flushdb(array host_port, [bool async]) */
FLUSHDB_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto ValkeyGlideCluster::flushall(string key, [bool async])
 *     proto ValkeyGlideCluster::flushall(array host_port, [bool async]) */
FLUSHALL_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto ValkeyGlideCluster::dbsize(string key)
 *     proto ValkeyGlideCluster::dbsize(array host_port) */
DBSIZE_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto array ValkeyGlideCluster::info(string key, [string $arg])
 *     proto array ValkeyGlideCluster::info(array host_port, [string $arg]) */
INFO_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto array ValkeyGlideCluster::client('list')
 *     proto bool ValkeyGlideCluster::client('kill', $ipport)
 *     proto bool ValkeyGlideCluster::client('setname', $name)
 *     proto string ValkeyGlideCluster::client('getname')
 */
PHP_METHOD(ValkeyGlideCluster, client)
{
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::config(string key, ...)
 *     proto mixed ValkeyGlideCluster::config(array host_port, ...) */
PHP_METHOD(ValkeyGlideCluster, config)
{
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::pubsub(string key, ...)
 *     proto mixed ValkeyGlideCluster::pubsub(array host_port, ...) */
PHP_METHOD(ValkeyGlideCluster, pubsub)
{
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::script(string key, ...)
 *     proto mixed ValkeyGlideCluster::script(array host_port, ...) */
PHP_METHOD(ValkeyGlideCluster, script)
{
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::slowlog(string key, ...)
 *     proto mixed ValkeyGlideCluster::slowlog(array host_port, ...) */
PHP_METHOD(ValkeyGlideCluster, slowlog)
{
}
/* }}} */

/* {{{ proto int ValkeyGlideCluster::geoadd(string key, float long float lat string mem, ...) */
PHP_METHOD(ValkeyGlideCluster, geoadd)
{
}

/* {{{ proto array ValkeyGlideCluster::geohash(string key, string mem1, [string mem2...]) */
PHP_METHOD(ValkeyGlideCluster, geohash)
{
}

/* {{{ proto array ValkeyGlideCluster::geopos(string key, string mem1, [string mem2...]) */
PHP_METHOD(ValkeyGlideCluster, geopos)
{
}

/* {{{ proto array ValkeyGlideCluster::geodist(string key, string mem1, string mem2 [string unit]) */
PHP_METHOD(ValkeyGlideCluster, geodist)
{
}

/* {{{ proto array ValkeyGlideCluster::georadius() }}} */
PHP_METHOD(ValkeyGlideCluster, georadius)
{
}

/* {{{ proto array ValkeyGlideCluster::georadius() }}} */
PHP_METHOD(ValkeyGlideCluster, georadius_ro)
{
}

/* {{{ proto array ValkeyGlideCluster::georadiusbymember() }}} */
PHP_METHOD(ValkeyGlideCluster, georadiusbymember)
{
}

/* {{{ proto array ValkeyGlideCluster::georadiusbymember() }}} */
PHP_METHOD(ValkeyGlideCluster, georadiusbymember_ro)
{
}

PHP_METHOD(ValkeyGlideCluster, geosearch)
{
}

PHP_METHOD(ValkeyGlideCluster, geosearchstore)
{
}

/* {{{ proto array ValkeyGlideCluster::role(string key)
 *     proto array ValkeyGlideCluster::role(array host_port) */
ROLE_METHOD_IMPL(ValkeyGlideCluster)

/* {{{ proto array ValkeyGlideCluster::time(string key)
 *     proto array ValkeyGlideCluster::time(array host_port) */
TIME_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto string ValkeyGlideCluster::randomkey(string key)
 *     proto string ValkeyGlideCluster::randomkey(array host_port) */
RANDOMKEY_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto bool ValkeyGlideCluster::ping(string key| string msg)
 *     proto bool ValkeyGlideCluster::ping(array host_port| string msg) */
PING_METHOD_IMPL(ValkeyGlideCluster)
/* }}} */

/* {{{ proto long ValkeyGlideCluster::xack(string key, string group, array ids) }}} */
PHP_METHOD(ValkeyGlideCluster, xack)
{
}

/* {{{ proto string ValkeyGlideCluster::xadd(string key, string id, array field_values) }}} */
PHP_METHOD(ValkeyGlideCluster, xadd)
{
}

/* {{{ proto array ValkeyGlideCluster::xclaim(string key, string group, string consumer,
 *                                      long min_idle_time, array ids, array options) */
PHP_METHOD(ValkeyGlideCluster, xclaim)
{
}

PHP_METHOD(ValkeyGlideCluster, xautoclaim)
{
}

PHP_METHOD(ValkeyGlideCluster, xdel)
{
}

/* {{{ proto variant ValkeyGlideCluster::xgroup(string op, [string key, string arg1, string arg2]) }}} */
PHP_METHOD(ValkeyGlideCluster, xgroup)
{
}

/* {{{ proto variant ValkeyGlideCluster::xinfo(string op, [string arg1, string arg2]); */
PHP_METHOD(ValkeyGlideCluster, xinfo)
{
}

/* {{{ proto string ValkeyGlideCluster::xlen(string key) }}} */
PHP_METHOD(ValkeyGlideCluster, xlen)
{
}

PHP_METHOD(ValkeyGlideCluster, xpending)
{
}

PHP_METHOD(ValkeyGlideCluster, xrange)
{
}

PHP_METHOD(ValkeyGlideCluster, xrevrange)
{
}

PHP_METHOD(ValkeyGlideCluster, xread)
{
}

PHP_METHOD(ValkeyGlideCluster, xreadgroup)
{
}

PHP_METHOD(ValkeyGlideCluster, xtrim)
{
}

/* {{{ proto string ValkeyGlideCluster::echo(string key, string msg)
 *     proto string ValkeyGlideCluster::echo(array host_port, string msg) */
PHP_METHOD(ValkeyGlideCluster, echo)
{
}
/* }}} */

/* {{{ proto mixed ValkeyGlideCluster::rawcommand(string $key, string $cmd, [ $argv1 .. $argvN])
 *     proto mixed ValkeyGlideCluster::rawcommand(array $host_port, string $cmd, [ $argv1 .. $argvN]) */
PHP_METHOD(ValkeyGlideCluster, rawcommand)
{
}
/* }}} */

/* {{{ proto array ValkeyGlideCluster::command()
 *     proto array ValkeyGlideCluster::command('INFO', string cmd)
 *     proto array ValkeyGlideCluster::command('GETKEYS', array cmd_args) */
PHP_METHOD(ValkeyGlideCluster, command)
{
}

PHP_METHOD(ValkeyGlideCluster, copy)
{
}
#endif /* PHP_REDIS_CLUSTER_C */
/* vim: set tabstop=4 softtabstop=4 expandtab shiftwidth=4: */
