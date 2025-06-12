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

#include "redis_cluster.h"

#include "redis_glide.h"
#include "valkey_glide_z_common.h"
#include "valkey_glide_geo_common.h"
#include "valkey_glide_x_common.h"
#include "valkey_glide_s_common.h"

#include "command_response.h" /* Include command_response.h for string conversion functions */
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

/* Import the string conversion functions from command_response.c */
extern char *long_to_string(long value, size_t *len);
extern char *double_to_string(double value, size_t *len);

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

#if PHP_VERSION_ID < 80000
#include "redis_legacy_arginfo.h"
#else
#include "zend_attributes.h"
#include "redis_arginfo.h"
#endif

/* {{{ proto mixed Redis::object(string subcommand, string key) */
PHP_METHOD(Redis, object)
{
    zval *object;
    redis_object *redis;
    char *key = NULL, *subcommand = NULL;
    size_t key_len, subcommand_len;

    /* Parse parameters */
    if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss",
                                     &object, redis_ce, &subcommand, &subcommand_len,
                                     &key, &key_len) == FAILURE)
    {
        RETURN_FALSE;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);

    /* If we have a Glide client, use it */
    if (redis->glide_client)
    {
        /* Execute the OBJECT command using the Glide client */
        if (execute_object_command(redis->glide_client, subcommand, subcommand_len, key, key_len, return_value) >= 0)
        {
            return;
        }
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto array Redis::zRange(string key, mixed start, mixed end [, bool|array options]) */
ZRANGE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto int Redis::zRangeStore(string dest, string src, mixed start, mixed end [, array options]) */
ZRANGESTORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zRevRange(string key, mixed start, mixed end [, array options]) */
ZREVRANGE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zRangeByScore(string key, mixed min, mixed max [, array options]) */
ZRANGEBYSCORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zRevRangeByScore(string key, mixed max, mixed min [, array options]) */
ZREVRANGEBYSCORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zRangeByLex(string key, mixed min, mixed max [, array options | long offset, long count]) */
ZRANGEBYLEX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zRevRangeByLex(string key, mixed max, mixed min [, array options]) */
ZREVRANGEBYLEX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zLexCount(string key, mixed min, mixed max) */
ZLEXCOUNT_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zRemRangeByLex(string key, mixed min, mixed max) */
ZREMRANGEBYLEX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zRem(string key, string member, ...) */
ZREM_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zRemRangeByScore(string key, mixed min, mixed max) */
ZREMRANGEBYSCORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zRemRangeByRank(string key, long start, long end) */
ZREMRANGEBYRANK_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zCount(string key, mixed min, mixed max) */
ZCOUNT_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zCard(string key) */
ZCARD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto double Redis::zScore(string key, string member) */
ZSCORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zMscore(string key, string member, string member2...)
   proto array Redis::zMscore(string key, array members) */
ZMSCORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zRank(string key, string member) */
ZRANK_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zRevRank(string key, string member) */
ZREVRANK_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto double Redis::zIncrBy(string key, double value, string member) */
ZINCRBY_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zdiff(array keys [, array options]) */
ZDIFF_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zinter(array keys [, array weights] [, array options]) */
ZINTER_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::bzPopMax(string|array key [, string otherkeys, ...,], float timeout) */
BZPOPMAX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::bzPopMin(string|array key [, string otherkeys, ...,], float timeout) */
BZPOPMIN_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zintercard(array $keys, int|array $limit_or_options = null) */
ZINTERCARD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zunion(array $keys, array $weights = null, array $options = null) */
ZUNION_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zdiffstore(string dst, array keys) */
ZDIFFSTORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zinterstore(string dst, array keys) */
ZINTERSTORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zunionstore(string dst, array keys) */
ZUNIONSTORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zPopMax(string key, [int count]) */
ZPOPMAX_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zPopMin(string key, [int count]) */
ZPOPMIN_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zscan(string key, long &iterator, [string pattern, long count]) */
ZSCAN_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto Redis|array|false Redis::zmpop(array $keys, string $from, int $count = 1) */
ZMPOP_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::zadd(string key, double score, string member, ...) */
ZADD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::zRandMember(string key [, int|array options [, bool withscores]]) */
ZRANDMEMBER_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto Redis|array|false Redis::bzmpop(double $timeout, array $keys, string $from, int $count = 1) */
BZMPOP_METHOD_IMPL(Redis)
/* }}} */

// GEO commands

/* {{{ proto long Redis::geoadd(string key, float longitude, float latitude, string member, ...) */
GEOADD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto double Redis::geodist(string key, string src, string dst [, string unit]) */
GEODIST_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::geohash(string key, string member [, string ...]) */
GEOHASH_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::geopos(string key, string member [, string ...]) */
GEOPOS_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::georadius(string key, float lng, float lat, float radius, string unit [, array options]) */
GEORADIUS_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::georadius_ro(string key, float lng, float lat, float radius, string unit [, array options]) */
GEORADIUS_RO_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::georadiusbymember(string key, string member, float radius, string unit [, array options]) */
PHP_METHOD(Redis, georadiusbymember)
{

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Redis::georadiusbymember_ro(string key, string member, float radius, string unit [, array options]) */
PHP_METHOD(Redis, georadiusbymember_ro)
{
    /* For now, we'll leave this to the standard implementation since it's not
       directly implemented in the redis_geo_glide.c file */

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Redis::geosearch(string key, array|string from, array|string by, string|null radius_unit, string|null count, string|null sorting, string|null pattern) */
GEOSEARCH_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::geosearchstore(string dst, string src, array|string from, array|string by, string|null radius_unit, string|null count, string|null sorting, string|null storedist) */
GEOSEARCHSTORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::xack(string key, string group, array ids) */
XACK_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string Redis::xadd(string key, string id, array field_values [, int maxlen [, bool approximate]]) */
XADD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::xautoclaim(string key, string group, string consumer, int min_idle_time, string start [, array options]) */
XAUTOCLAIM_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::xclaim(string key, string group, string consumer, int min_idle_time, array ids [, array options]) */
XCLAIM_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::xdel(string key, array ids) */
XDEL_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto mixed Redis::xgroup(string op, [string key, string group, ...]) */
XGROUP_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto mixed Redis::xinfo(string op, [string key, string group, ...]) */
XINFO_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::xlen(string key) */
XLEN_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::xpending(string key, string group [, array options OR string start, string end, int count [, string consumer]]) */
XPENDING_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::xrange(string key, string start, string end [, int count [, array options]]) */
XRANGE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::xread(array streams_and_ids [, int count [, int block]]) */
XREAD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::xreadgroup(string group, string consumer, array streams [, int count [, array options]]) */
XREADGROUP_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::xrevrange(string key, string end, string start [, int count [, array options]]) */
XREVRANGE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::xtrim(string key, string threshold, bool approx = false, bool minid = false, int limit = -1) */
XTRIM_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::sAdd(string key, string member, ...) */
SADD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::sAddArray(string key, array values) */
SADD_ARRAY_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::scard(string key) */
SCARD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::srem(string key, string member, ...) */
SREM_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto bool Redis::sMove(string src, string dst, string member) */
SMOVE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string|array Redis::sPop(string key, [long count]) */
SPOP_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto string|array Redis::sRandMember(string key, [long count]) */
SRANDMEMBER_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto bool Redis::sismember(string key, string member) */
SISMEMBER_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::sMembers(string key) */
SMEMBERS_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::sMisMember(string key, array members) */
SMISMEMBER_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::sInter(string key, ...) */
SINTER_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::sintercard(array keys, [long limit]) */
SINTERCARD_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::sInterStore(string dst, string key1, ...) */
SINTERSTORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::sUnion(string key, ...) */
SUNION_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::sUnionStore(string dst, string key1, ...) */
SUNIONSTORE_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto array Redis::sDiff(string key, ...) */
SDIFF_METHOD_IMPL(Redis)
/* }}} */

/* {{{ proto long Redis::sDiffStore(string dst, string key1, ...) */
SDIFFSTORE_METHOD_IMPL(Redis)
/* }}} */
