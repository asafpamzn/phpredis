/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 5eb1b0f0aa13c1e3747ff98bd010d1f4b4ded502 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, seeds)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, read_timeout)
	ZEND_ARG_INFO(0, persistent)
	ZEND_ARG_INFO(0, auth)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_append, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_bitcount, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, end)
	ZEND_ARG_INFO(0, bybit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_bitop, 0, 0, 3)
	ZEND_ARG_INFO(0, operation)
	ZEND_ARG_INFO(0, deskey)
	ZEND_ARG_INFO(0, srckey)
	ZEND_ARG_VARIADIC_INFO(0, otherkeys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_bitpos, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, bit)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, end)
	ZEND_ARG_INFO(0, bybit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_blpop, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, timeout_or_key)
	ZEND_ARG_VARIADIC_INFO(0, extra_args)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_brpop arginfo_class_ValkeyGlideCluster_blpop

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_brpoplpush, 0, 0, 3)
	ZEND_ARG_INFO(0, srckey)
	ZEND_ARG_INFO(0, deskey)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lMove, 0, 0, 4)
	ZEND_ARG_INFO(0, src)
	ZEND_ARG_INFO(0, dst)
	ZEND_ARG_INFO(0, wherefrom)
	ZEND_ARG_INFO(0, whereto)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_blmove, 0, 0, 5)
	ZEND_ARG_INFO(0, src)
	ZEND_ARG_INFO(0, dst)
	ZEND_ARG_INFO(0, wherefrom)
	ZEND_ARG_INFO(0, whereto)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_bzpopmax arginfo_class_ValkeyGlideCluster_blpop

#define arginfo_class_ValkeyGlideCluster_bzpopmin arginfo_class_ValkeyGlideCluster_blpop

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_bzmpop, 0, 0, 3)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, keys)
	ZEND_ARG_INFO(0, from)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zmpop, 0, 0, 2)
	ZEND_ARG_INFO(0, keys)
	ZEND_ARG_INFO(0, from)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_blmpop arginfo_class_ValkeyGlideCluster_bzmpop

#define arginfo_class_ValkeyGlideCluster_lmpop arginfo_class_ValkeyGlideCluster_zmpop

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_clearlasterror, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_close arginfo_class_ValkeyGlideCluster_clearlasterror

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_copy, 0, 0, 2)
	ZEND_ARG_INFO(0, src)
	ZEND_ARG_INFO(0, dst)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_decr, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, by)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_decrby arginfo_class_ValkeyGlideCluster_append

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_del, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_VARIADIC_INFO(0, other_keys)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_discard arginfo_class_ValkeyGlideCluster_clearlasterror

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_dump, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_eval, 0, 0, 1)
	ZEND_ARG_INFO(0, script)
	ZEND_ARG_INFO(0, args)
	ZEND_ARG_INFO(0, num_keys)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_eval_ro arginfo_class_ValkeyGlideCluster_eval

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_evalsha, 0, 0, 1)
	ZEND_ARG_INFO(0, script_sha)
	ZEND_ARG_INFO(0, args)
	ZEND_ARG_INFO(0, num_keys)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_evalsha_ro arginfo_class_ValkeyGlideCluster_evalsha

#define arginfo_class_ValkeyGlideCluster_exec arginfo_class_ValkeyGlideCluster_clearlasterror

#define arginfo_class_ValkeyGlideCluster_exists arginfo_class_ValkeyGlideCluster_del

#define arginfo_class_ValkeyGlideCluster_touch arginfo_class_ValkeyGlideCluster_del

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_expire, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_expireAt, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, timestamp)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_expiretime arginfo_class_ValkeyGlideCluster_dump

#define arginfo_class_ValkeyGlideCluster_pexpiretime arginfo_class_ValkeyGlideCluster_dump

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_geoadd, 0, 0, 4)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, lng)
	ZEND_ARG_INFO(0, lat)
	ZEND_ARG_INFO(0, member)
	ZEND_ARG_VARIADIC_INFO(0, other_triples_and_options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_geodist, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, src)
	ZEND_ARG_INFO(0, dest)
	ZEND_ARG_INFO(0, unit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_geohash, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, member)
	ZEND_ARG_VARIADIC_INFO(0, other_members)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_geopos arginfo_class_ValkeyGlideCluster_geohash

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_georadius, 0, 0, 5)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, lng)
	ZEND_ARG_INFO(0, lat)
	ZEND_ARG_INFO(0, radius)
	ZEND_ARG_INFO(0, unit)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_georadius_ro arginfo_class_ValkeyGlideCluster_georadius

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_georadiusbymember, 0, 0, 4)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, member)
	ZEND_ARG_INFO(0, radius)
	ZEND_ARG_INFO(0, unit)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_georadiusbymember_ro arginfo_class_ValkeyGlideCluster_georadiusbymember

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_geosearch, 0, 0, 4)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, position)
	ZEND_ARG_INFO(0, shape)
	ZEND_ARG_INFO(0, unit)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_geosearchstore, 0, 0, 5)
	ZEND_ARG_INFO(0, dst)
	ZEND_ARG_INFO(0, src)
	ZEND_ARG_INFO(0, position)
	ZEND_ARG_INFO(0, shape)
	ZEND_ARG_INFO(0, unit)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_get arginfo_class_ValkeyGlideCluster_dump

#define arginfo_class_ValkeyGlideCluster_getDel arginfo_class_ValkeyGlideCluster_dump

#define arginfo_class_ValkeyGlideCluster_getWithMeta arginfo_class_ValkeyGlideCluster_dump

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_getEx, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_getBit, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, idx)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_getlasterror arginfo_class_ValkeyGlideCluster_clearlasterror

#define arginfo_class_ValkeyGlideCluster_getmode arginfo_class_ValkeyGlideCluster_clearlasterror

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_getRange, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, end)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lcs, 0, 0, 2)
	ZEND_ARG_INFO(0, key1)
	ZEND_ARG_INFO(0, key2)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_getset arginfo_class_ValkeyGlideCluster_append

#define arginfo_class_ValkeyGlideCluster_hDel arginfo_class_ValkeyGlideCluster_geohash

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_hExists, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, member)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_hGet arginfo_class_ValkeyGlideCluster_hExists

#define arginfo_class_ValkeyGlideCluster_hGetAll arginfo_class_ValkeyGlideCluster_dump

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_hIncrBy, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, member)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_hIncrByFloat arginfo_class_ValkeyGlideCluster_hIncrBy

#define arginfo_class_ValkeyGlideCluster_hKeys arginfo_class_ValkeyGlideCluster_dump

#define arginfo_class_ValkeyGlideCluster_hLen arginfo_class_ValkeyGlideCluster_dump

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_hMget, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_hMset, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, key_values)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_hscan, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(1, iterator)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_expiremember, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, field)
	ZEND_ARG_INFO(0, ttl)
	ZEND_ARG_INFO(0, unit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_expirememberat, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, field)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_hRandField arginfo_class_ValkeyGlideCluster_getEx

#define arginfo_class_ValkeyGlideCluster_hSet arginfo_class_ValkeyGlideCluster_hIncrBy

#define arginfo_class_ValkeyGlideCluster_hSetNx arginfo_class_ValkeyGlideCluster_hIncrBy

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_hStrLen, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_hVals arginfo_class_ValkeyGlideCluster_dump

#define arginfo_class_ValkeyGlideCluster_incr arginfo_class_ValkeyGlideCluster_decr

#define arginfo_class_ValkeyGlideCluster_incrBy arginfo_class_ValkeyGlideCluster_append

#define arginfo_class_ValkeyGlideCluster_incrByFloat arginfo_class_ValkeyGlideCluster_append

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_info, 0, 0, 1)
	ZEND_ARG_INFO(0, route)
	ZEND_ARG_VARIADIC_INFO(0, sections)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_keys, 0, 0, 1)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lget, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_lindex arginfo_class_ValkeyGlideCluster_lget

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lInsert, 0, 0, 4)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, pos)
	ZEND_ARG_INFO(0, pivot)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_lLen arginfo_class_ValkeyGlideCluster_dump

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lPop, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lPos, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lPush, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_VARIADIC_INFO(0, other_values)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_lPushx arginfo_class_ValkeyGlideCluster_append

#define arginfo_class_ValkeyGlideCluster_lrange arginfo_class_ValkeyGlideCluster_getRange

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lrem, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lSet, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_ltrim arginfo_class_ValkeyGlideCluster_getRange

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_mget, 0, 0, 1)
	ZEND_ARG_INFO(0, keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_mset, 0, 0, 1)
	ZEND_ARG_INFO(0, key_values)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_msetnx arginfo_class_ValkeyGlideCluster_mset

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_multi, 0, 0, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_object, 0, 0, 2)
	ZEND_ARG_INFO(0, subcommand)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_persist arginfo_class_ValkeyGlideCluster_dump

#define arginfo_class_ValkeyGlideCluster_pexpire arginfo_class_ValkeyGlideCluster_expire

#define arginfo_class_ValkeyGlideCluster_pexpireAt arginfo_class_ValkeyGlideCluster_expireAt

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_pfadd, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, elements)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_pfcount arginfo_class_ValkeyGlideCluster_dump

#define arginfo_class_ValkeyGlideCluster_pfmerge arginfo_class_ValkeyGlideCluster_hMget

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_ping, 0, 0, 1)
	ZEND_ARG_INFO(0, route)
	ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_psetex, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_psubscribe, 0, 0, 2)
	ZEND_ARG_INFO(0, patterns)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_pttl arginfo_class_ValkeyGlideCluster_dump

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_publish, 0, 0, 2)
	ZEND_ARG_INFO(0, channel)
	ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_pubsub, 0, 0, 1)
	ZEND_ARG_INFO(0, route)
	ZEND_ARG_VARIADIC_INFO(0, values)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_punsubscribe, 0, 0, 1)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_VARIADIC_INFO(0, other_patterns)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_randomKey, 0, 0, 1)
	ZEND_ARG_INFO(0, route)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_rename, 0, 0, 2)
	ZEND_ARG_INFO(0, key_src)
	ZEND_ARG_INFO(0, key_dst)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_renameNx, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, newkey)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_restore, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_rPop arginfo_class_ValkeyGlideCluster_lPop

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_rpoplpush, 0, 0, 2)
	ZEND_ARG_INFO(0, src)
	ZEND_ARG_INFO(0, dst)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_rPush, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_VARIADIC_INFO(0, elements)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_rPushx arginfo_class_ValkeyGlideCluster_append

#define arginfo_class_ValkeyGlideCluster_sadd arginfo_class_ValkeyGlideCluster_lPush

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_saddarray, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, values)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_scan, 0, 0, 2)
	ZEND_ARG_INFO(1, iterator)
	ZEND_ARG_INFO(0, route)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_scard arginfo_class_ValkeyGlideCluster_dump

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_script, 0, 0, 1)
	ZEND_ARG_INFO(0, route)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_sdiff arginfo_class_ValkeyGlideCluster_del

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_sdiffstore, 0, 0, 2)
	ZEND_ARG_INFO(0, dst)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_VARIADIC_INFO(0, other_keys)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_set arginfo_class_ValkeyGlideCluster_lPos

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_setBit, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, onoff)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_setex, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, expire)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_setnx arginfo_class_ValkeyGlideCluster_append

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_setrange, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_sinter arginfo_class_ValkeyGlideCluster_del

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_sintercard, 0, 0, 1)
	ZEND_ARG_INFO(0, keys)
	ZEND_ARG_INFO(0, limit)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_sinterstore arginfo_class_ValkeyGlideCluster_del

#define arginfo_class_ValkeyGlideCluster_sismember arginfo_class_ValkeyGlideCluster_append

#define arginfo_class_ValkeyGlideCluster_smismember arginfo_class_ValkeyGlideCluster_geohash

#define arginfo_class_ValkeyGlideCluster_smembers arginfo_class_ValkeyGlideCluster_dump

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_smove, 0, 0, 3)
	ZEND_ARG_INFO(0, src)
	ZEND_ARG_INFO(0, dst)
	ZEND_ARG_INFO(0, member)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_sort arginfo_class_ValkeyGlideCluster_getEx

#define arginfo_class_ValkeyGlideCluster_sort_ro arginfo_class_ValkeyGlideCluster_getEx

#define arginfo_class_ValkeyGlideCluster_spop arginfo_class_ValkeyGlideCluster_lPop

#define arginfo_class_ValkeyGlideCluster_srandmember arginfo_class_ValkeyGlideCluster_lPop

#define arginfo_class_ValkeyGlideCluster_srem arginfo_class_ValkeyGlideCluster_lPush

#define arginfo_class_ValkeyGlideCluster_sscan arginfo_class_ValkeyGlideCluster_hscan

#define arginfo_class_ValkeyGlideCluster_strlen arginfo_class_ValkeyGlideCluster_dump

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_subscribe, 0, 0, 2)
	ZEND_ARG_INFO(0, channels)
	ZEND_ARG_INFO(0, cb)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_sunion arginfo_class_ValkeyGlideCluster_del

#define arginfo_class_ValkeyGlideCluster_sunionstore arginfo_class_ValkeyGlideCluster_sdiffstore

#define arginfo_class_ValkeyGlideCluster_ttl arginfo_class_ValkeyGlideCluster_dump

#define arginfo_class_ValkeyGlideCluster_type arginfo_class_ValkeyGlideCluster_dump

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_unsubscribe, 0, 0, 1)
	ZEND_ARG_INFO(0, channels)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_unlink arginfo_class_ValkeyGlideCluster_del

#define arginfo_class_ValkeyGlideCluster_unwatch arginfo_class_ValkeyGlideCluster_clearlasterror

#define arginfo_class_ValkeyGlideCluster_watch arginfo_class_ValkeyGlideCluster_del

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_xack, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, group)
	ZEND_ARG_INFO(0, ids)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_xadd, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, id)
	ZEND_ARG_INFO(0, values)
	ZEND_ARG_INFO(0, maxlen)
	ZEND_ARG_INFO(0, approx)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_xclaim, 0, 0, 6)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, group)
	ZEND_ARG_INFO(0, consumer)
	ZEND_ARG_INFO(0, min_iddle)
	ZEND_ARG_INFO(0, ids)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_xdel, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, ids)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_xgroup, 0, 0, 1)
	ZEND_ARG_INFO(0, operation)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, group)
	ZEND_ARG_INFO(0, id_or_consumer)
	ZEND_ARG_INFO(0, mkstream)
	ZEND_ARG_INFO(0, entries_read)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_xautoclaim, 0, 0, 5)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, group)
	ZEND_ARG_INFO(0, consumer)
	ZEND_ARG_INFO(0, min_idle)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, count)
	ZEND_ARG_INFO(0, justid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_xinfo, 0, 0, 1)
	ZEND_ARG_INFO(0, operation)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_xlen arginfo_class_ValkeyGlideCluster_dump

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_xpending, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, group)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, end)
	ZEND_ARG_INFO(0, count)
	ZEND_ARG_INFO(0, consumer)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_xrange, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, end)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_xread, 0, 0, 1)
	ZEND_ARG_INFO(0, streams)
	ZEND_ARG_INFO(0, count)
	ZEND_ARG_INFO(0, block)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_xreadgroup, 0, 0, 3)
	ZEND_ARG_INFO(0, group)
	ZEND_ARG_INFO(0, consumer)
	ZEND_ARG_INFO(0, streams)
	ZEND_ARG_INFO(0, count)
	ZEND_ARG_INFO(0, block)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_xrevrange arginfo_class_ValkeyGlideCluster_xrange

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_xtrim, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, maxlen)
	ZEND_ARG_INFO(0, approx)
	ZEND_ARG_INFO(0, minid)
	ZEND_ARG_INFO(0, limit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zAdd, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, score_or_options)
	ZEND_ARG_VARIADIC_INFO(0, more_scores_and_mems)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zCard arginfo_class_ValkeyGlideCluster_dump

#define arginfo_class_ValkeyGlideCluster_zCount arginfo_class_ValkeyGlideCluster_getRange

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zIncrBy, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, member)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zinterstore, 0, 0, 2)
	ZEND_ARG_INFO(0, dst)
	ZEND_ARG_INFO(0, keys)
	ZEND_ARG_INFO(0, weights)
	ZEND_ARG_INFO(0, aggregate)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zintercard arginfo_class_ValkeyGlideCluster_sintercard

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zlexcount, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zpopmax, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zpopmin arginfo_class_ValkeyGlideCluster_zpopmax

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zrange, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, end)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zrangestore, 0, 0, 4)
	ZEND_ARG_INFO(0, dstkey)
	ZEND_ARG_INFO(0, srckey)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, end)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zrandmember arginfo_class_ValkeyGlideCluster_getEx

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zrangebylex, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, max)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zrangebyscore arginfo_class_ValkeyGlideCluster_zrange

#define arginfo_class_ValkeyGlideCluster_zrank arginfo_class_ValkeyGlideCluster_hExists

#define arginfo_class_ValkeyGlideCluster_zrem arginfo_class_ValkeyGlideCluster_lPush

#define arginfo_class_ValkeyGlideCluster_zremrangebylex arginfo_class_ValkeyGlideCluster_zlexcount

#define arginfo_class_ValkeyGlideCluster_zremrangebyrank arginfo_class_ValkeyGlideCluster_zlexcount

#define arginfo_class_ValkeyGlideCluster_zRemRangeByScore arginfo_class_ValkeyGlideCluster_zlexcount

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zrevrange, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, max)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zrevrangebylex arginfo_class_ValkeyGlideCluster_zrevrange

#define arginfo_class_ValkeyGlideCluster_zrevrangebyscore arginfo_class_ValkeyGlideCluster_zrevrange

#define arginfo_class_ValkeyGlideCluster_zrevrank arginfo_class_ValkeyGlideCluster_hExists

#define arginfo_class_ValkeyGlideCluster_zscan arginfo_class_ValkeyGlideCluster_hscan

#define arginfo_class_ValkeyGlideCluster_zScore arginfo_class_ValkeyGlideCluster_hExists

#define arginfo_class_ValkeyGlideCluster_zMscore arginfo_class_ValkeyGlideCluster_geohash

#define arginfo_class_ValkeyGlideCluster_zunionstore arginfo_class_ValkeyGlideCluster_zinterstore

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zinter, 0, 0, 1)
	ZEND_ARG_INFO(0, keys)
	ZEND_ARG_INFO(0, weights)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zdiffstore, 0, 0, 2)
	ZEND_ARG_INFO(0, dst)
	ZEND_ARG_INFO(0, keys)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zunion arginfo_class_ValkeyGlideCluster_zinter

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zdiff, 0, 0, 1)
	ZEND_ARG_INFO(0, keys)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()


ZEND_METHOD(ValkeyGlideCluster, __construct);
ZEND_METHOD(ValkeyGlideCluster, append);
ZEND_METHOD(ValkeyGlideCluster, bitcount);
ZEND_METHOD(ValkeyGlideCluster, bitop);
ZEND_METHOD(ValkeyGlideCluster, bitpos);
ZEND_METHOD(ValkeyGlideCluster, blpop);
ZEND_METHOD(ValkeyGlideCluster, brpop);
ZEND_METHOD(ValkeyGlideCluster, brpoplpush);
ZEND_METHOD(ValkeyGlideCluster, lMove);
ZEND_METHOD(ValkeyGlideCluster, blmove);
ZEND_METHOD(ValkeyGlideCluster, bzpopmax);
ZEND_METHOD(ValkeyGlideCluster, bzpopmin);
ZEND_METHOD(ValkeyGlideCluster, bzmpop);
ZEND_METHOD(ValkeyGlideCluster, zmpop);
ZEND_METHOD(ValkeyGlideCluster, blmpop);
ZEND_METHOD(ValkeyGlideCluster, lmpop);
ZEND_METHOD(ValkeyGlideCluster, clearlasterror);
ZEND_METHOD(ValkeyGlideCluster, close);
ZEND_METHOD(ValkeyGlideCluster, copy);
ZEND_METHOD(ValkeyGlideCluster, decr);
ZEND_METHOD(ValkeyGlideCluster, decrby);
ZEND_METHOD(ValkeyGlideCluster, del);
ZEND_METHOD(ValkeyGlideCluster, discard);
ZEND_METHOD(ValkeyGlideCluster, dump);
ZEND_METHOD(ValkeyGlideCluster, eval);
ZEND_METHOD(ValkeyGlideCluster, eval_ro);
ZEND_METHOD(ValkeyGlideCluster, evalsha);
ZEND_METHOD(ValkeyGlideCluster, evalsha_ro);
ZEND_METHOD(ValkeyGlideCluster, exec);
ZEND_METHOD(ValkeyGlideCluster, exists);
ZEND_METHOD(ValkeyGlideCluster, touch);
ZEND_METHOD(ValkeyGlideCluster, expire);
ZEND_METHOD(ValkeyGlideCluster, expireAt);
ZEND_METHOD(ValkeyGlideCluster, expiretime);
ZEND_METHOD(ValkeyGlideCluster, pexpiretime);
ZEND_METHOD(ValkeyGlideCluster, geoadd);
ZEND_METHOD(ValkeyGlideCluster, geodist);
ZEND_METHOD(ValkeyGlideCluster, geohash);
ZEND_METHOD(ValkeyGlideCluster, geopos);
ZEND_METHOD(ValkeyGlideCluster, georadius);
ZEND_METHOD(ValkeyGlideCluster, georadius_ro);
ZEND_METHOD(ValkeyGlideCluster, georadiusbymember);
ZEND_METHOD(ValkeyGlideCluster, georadiusbymember_ro);
ZEND_METHOD(ValkeyGlideCluster, geosearch);
ZEND_METHOD(ValkeyGlideCluster, geosearchstore);
ZEND_METHOD(ValkeyGlideCluster, get);
ZEND_METHOD(ValkeyGlideCluster, getDel);
ZEND_METHOD(ValkeyGlideCluster, getWithMeta);
ZEND_METHOD(ValkeyGlideCluster, getEx);
ZEND_METHOD(ValkeyGlideCluster, getBit);
ZEND_METHOD(ValkeyGlideCluster, getlasterror);
ZEND_METHOD(ValkeyGlideCluster, getmode);
ZEND_METHOD(ValkeyGlideCluster, getRange);
ZEND_METHOD(ValkeyGlideCluster, lcs);
ZEND_METHOD(ValkeyGlideCluster, getset);
ZEND_METHOD(ValkeyGlideCluster, hDel);
ZEND_METHOD(ValkeyGlideCluster, hExists);
ZEND_METHOD(ValkeyGlideCluster, hGet);
ZEND_METHOD(ValkeyGlideCluster, hGetAll);
ZEND_METHOD(ValkeyGlideCluster, hIncrBy);
ZEND_METHOD(ValkeyGlideCluster, hIncrByFloat);
ZEND_METHOD(ValkeyGlideCluster, hKeys);
ZEND_METHOD(ValkeyGlideCluster, hLen);
ZEND_METHOD(ValkeyGlideCluster, hMget);
ZEND_METHOD(ValkeyGlideCluster, hMset);
ZEND_METHOD(ValkeyGlideCluster, hscan);
ZEND_METHOD(ValkeyGlideCluster, expiremember);
ZEND_METHOD(ValkeyGlideCluster, expirememberat);
ZEND_METHOD(ValkeyGlideCluster, hRandField);
ZEND_METHOD(ValkeyGlideCluster, hSet);
ZEND_METHOD(ValkeyGlideCluster, hSetNx);
ZEND_METHOD(ValkeyGlideCluster, hStrLen);
ZEND_METHOD(ValkeyGlideCluster, hVals);
ZEND_METHOD(ValkeyGlideCluster, incr);
ZEND_METHOD(ValkeyGlideCluster, incrBy);
ZEND_METHOD(ValkeyGlideCluster, incrByFloat);
ZEND_METHOD(ValkeyGlideCluster, info);
ZEND_METHOD(ValkeyGlideCluster, keys);
ZEND_METHOD(ValkeyGlideCluster, lget);
ZEND_METHOD(ValkeyGlideCluster, lindex);
ZEND_METHOD(ValkeyGlideCluster, lInsert);
ZEND_METHOD(ValkeyGlideCluster, lLen);
ZEND_METHOD(ValkeyGlideCluster, lPop);
ZEND_METHOD(ValkeyGlideCluster, lPos);
ZEND_METHOD(ValkeyGlideCluster, lPush);
ZEND_METHOD(ValkeyGlideCluster, lPushx);
ZEND_METHOD(ValkeyGlideCluster, lrange);
ZEND_METHOD(ValkeyGlideCluster, lrem);
ZEND_METHOD(ValkeyGlideCluster, lSet);
ZEND_METHOD(ValkeyGlideCluster, ltrim);
ZEND_METHOD(ValkeyGlideCluster, mget);
ZEND_METHOD(ValkeyGlideCluster, mset);
ZEND_METHOD(ValkeyGlideCluster, msetnx);
ZEND_METHOD(ValkeyGlideCluster, multi);
ZEND_METHOD(ValkeyGlideCluster, object);
ZEND_METHOD(ValkeyGlideCluster, persist);
ZEND_METHOD(ValkeyGlideCluster, pexpire);
ZEND_METHOD(ValkeyGlideCluster, pexpireAt);
ZEND_METHOD(ValkeyGlideCluster, pfadd);
ZEND_METHOD(ValkeyGlideCluster, pfcount);
ZEND_METHOD(ValkeyGlideCluster, pfmerge);
ZEND_METHOD(ValkeyGlideCluster, ping);
ZEND_METHOD(ValkeyGlideCluster, psetex);
ZEND_METHOD(ValkeyGlideCluster, psubscribe);
ZEND_METHOD(ValkeyGlideCluster, pttl);
ZEND_METHOD(ValkeyGlideCluster, publish);
ZEND_METHOD(ValkeyGlideCluster, pubsub);
ZEND_METHOD(ValkeyGlideCluster, punsubscribe);
ZEND_METHOD(ValkeyGlideCluster, randomKey);
ZEND_METHOD(ValkeyGlideCluster, rename);
ZEND_METHOD(ValkeyGlideCluster, renameNx);
ZEND_METHOD(ValkeyGlideCluster, restore);
ZEND_METHOD(ValkeyGlideCluster, rPop);
ZEND_METHOD(ValkeyGlideCluster, rpoplpush);
ZEND_METHOD(ValkeyGlideCluster, rPush);
ZEND_METHOD(ValkeyGlideCluster, rPushx);
ZEND_METHOD(ValkeyGlideCluster, sadd);
ZEND_METHOD(ValkeyGlideCluster, saddarray);
ZEND_METHOD(ValkeyGlideCluster, scan);
ZEND_METHOD(ValkeyGlideCluster, scard);
ZEND_METHOD(ValkeyGlideCluster, script);
ZEND_METHOD(ValkeyGlideCluster, sdiff);
ZEND_METHOD(ValkeyGlideCluster, sdiffstore);
ZEND_METHOD(ValkeyGlideCluster, set);
ZEND_METHOD(ValkeyGlideCluster, setBit);
ZEND_METHOD(ValkeyGlideCluster, setex);
ZEND_METHOD(ValkeyGlideCluster, setnx);
ZEND_METHOD(ValkeyGlideCluster, setrange);
ZEND_METHOD(ValkeyGlideCluster, sinter);
ZEND_METHOD(ValkeyGlideCluster, sintercard);
ZEND_METHOD(ValkeyGlideCluster, sinterstore);
ZEND_METHOD(ValkeyGlideCluster, sismember);
ZEND_METHOD(ValkeyGlideCluster, smismember);
ZEND_METHOD(ValkeyGlideCluster, smembers);
ZEND_METHOD(ValkeyGlideCluster, smove);
ZEND_METHOD(ValkeyGlideCluster, sort);
ZEND_METHOD(ValkeyGlideCluster, sort_ro);
ZEND_METHOD(ValkeyGlideCluster, spop);
ZEND_METHOD(ValkeyGlideCluster, srandmember);
ZEND_METHOD(ValkeyGlideCluster, srem);
ZEND_METHOD(ValkeyGlideCluster, sscan);
ZEND_METHOD(ValkeyGlideCluster, strlen);
ZEND_METHOD(ValkeyGlideCluster, subscribe);
ZEND_METHOD(ValkeyGlideCluster, sunion);
ZEND_METHOD(ValkeyGlideCluster, sunionstore);
ZEND_METHOD(ValkeyGlideCluster, ttl);
ZEND_METHOD(ValkeyGlideCluster, type);
ZEND_METHOD(ValkeyGlideCluster, unsubscribe);
ZEND_METHOD(ValkeyGlideCluster, unlink);
ZEND_METHOD(ValkeyGlideCluster, unwatch);
ZEND_METHOD(ValkeyGlideCluster, watch);
ZEND_METHOD(ValkeyGlideCluster, xack);
ZEND_METHOD(ValkeyGlideCluster, xadd);
ZEND_METHOD(ValkeyGlideCluster, xclaim);
ZEND_METHOD(ValkeyGlideCluster, xdel);
ZEND_METHOD(ValkeyGlideCluster, xgroup);
ZEND_METHOD(ValkeyGlideCluster, xautoclaim);
ZEND_METHOD(ValkeyGlideCluster, xinfo);
ZEND_METHOD(ValkeyGlideCluster, xlen);
ZEND_METHOD(ValkeyGlideCluster, xpending);
ZEND_METHOD(ValkeyGlideCluster, xrange);
ZEND_METHOD(ValkeyGlideCluster, xread);
ZEND_METHOD(ValkeyGlideCluster, xreadgroup);
ZEND_METHOD(ValkeyGlideCluster, xrevrange);
ZEND_METHOD(ValkeyGlideCluster, xtrim);
ZEND_METHOD(ValkeyGlideCluster, zAdd);
ZEND_METHOD(ValkeyGlideCluster, zCard);
ZEND_METHOD(ValkeyGlideCluster, zCount);
ZEND_METHOD(ValkeyGlideCluster, zIncrBy);
ZEND_METHOD(ValkeyGlideCluster, zinterstore);
ZEND_METHOD(ValkeyGlideCluster, zintercard);
ZEND_METHOD(ValkeyGlideCluster, zlexcount);
ZEND_METHOD(ValkeyGlideCluster, zpopmax);
ZEND_METHOD(ValkeyGlideCluster, zpopmin);
ZEND_METHOD(ValkeyGlideCluster, zrange);
ZEND_METHOD(ValkeyGlideCluster, zrangestore);
ZEND_METHOD(ValkeyGlideCluster, zrandmember);
ZEND_METHOD(ValkeyGlideCluster, zrangebylex);
ZEND_METHOD(ValkeyGlideCluster, zrangebyscore);
ZEND_METHOD(ValkeyGlideCluster, zrank);
ZEND_METHOD(ValkeyGlideCluster, zrem);
ZEND_METHOD(ValkeyGlideCluster, zremrangebylex);
ZEND_METHOD(ValkeyGlideCluster, zremrangebyrank);
ZEND_METHOD(ValkeyGlideCluster, zRemRangeByScore);
ZEND_METHOD(ValkeyGlideCluster, zrevrange);
ZEND_METHOD(ValkeyGlideCluster, zrevrangebylex);
ZEND_METHOD(ValkeyGlideCluster, zrevrangebyscore);
ZEND_METHOD(ValkeyGlideCluster, zrevrank);
ZEND_METHOD(ValkeyGlideCluster, zscan);
ZEND_METHOD(ValkeyGlideCluster, zScore);
ZEND_METHOD(ValkeyGlideCluster, zMscore);
ZEND_METHOD(ValkeyGlideCluster, zunionstore);
ZEND_METHOD(ValkeyGlideCluster, zinter);
ZEND_METHOD(ValkeyGlideCluster, zdiffstore);
ZEND_METHOD(ValkeyGlideCluster, zunion);
ZEND_METHOD(ValkeyGlideCluster, zdiff);


static const zend_function_entry class_ValkeyGlideCluster_methods[] = {
	ZEND_ME(ValkeyGlideCluster, __construct, arginfo_class_ValkeyGlideCluster___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, append, arginfo_class_ValkeyGlideCluster_append, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, bitcount, arginfo_class_ValkeyGlideCluster_bitcount, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, bitop, arginfo_class_ValkeyGlideCluster_bitop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, bitpos, arginfo_class_ValkeyGlideCluster_bitpos, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, blpop, arginfo_class_ValkeyGlideCluster_blpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, brpop, arginfo_class_ValkeyGlideCluster_brpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, brpoplpush, arginfo_class_ValkeyGlideCluster_brpoplpush, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lMove, arginfo_class_ValkeyGlideCluster_lMove, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, blmove, arginfo_class_ValkeyGlideCluster_blmove, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, bzpopmax, arginfo_class_ValkeyGlideCluster_bzpopmax, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, bzpopmin, arginfo_class_ValkeyGlideCluster_bzpopmin, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, bzmpop, arginfo_class_ValkeyGlideCluster_bzmpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zmpop, arginfo_class_ValkeyGlideCluster_zmpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, blmpop, arginfo_class_ValkeyGlideCluster_blmpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lmpop, arginfo_class_ValkeyGlideCluster_lmpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, clearlasterror, arginfo_class_ValkeyGlideCluster_clearlasterror, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, close, arginfo_class_ValkeyGlideCluster_close, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, copy, arginfo_class_ValkeyGlideCluster_copy, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, decr, arginfo_class_ValkeyGlideCluster_decr, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, decrby, arginfo_class_ValkeyGlideCluster_decrby, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, del, arginfo_class_ValkeyGlideCluster_del, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, discard, arginfo_class_ValkeyGlideCluster_discard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, dump, arginfo_class_ValkeyGlideCluster_dump, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, eval, arginfo_class_ValkeyGlideCluster_eval, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, eval_ro, arginfo_class_ValkeyGlideCluster_eval_ro, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, evalsha, arginfo_class_ValkeyGlideCluster_evalsha, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, evalsha_ro, arginfo_class_ValkeyGlideCluster_evalsha_ro, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, exec, arginfo_class_ValkeyGlideCluster_exec, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, exists, arginfo_class_ValkeyGlideCluster_exists, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, touch, arginfo_class_ValkeyGlideCluster_touch, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, expire, arginfo_class_ValkeyGlideCluster_expire, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, expireAt, arginfo_class_ValkeyGlideCluster_expireAt, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, expiretime, arginfo_class_ValkeyGlideCluster_expiretime, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, pexpiretime, arginfo_class_ValkeyGlideCluster_pexpiretime, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, geoadd, arginfo_class_ValkeyGlideCluster_geoadd, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, geodist, arginfo_class_ValkeyGlideCluster_geodist, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, geohash, arginfo_class_ValkeyGlideCluster_geohash, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, geopos, arginfo_class_ValkeyGlideCluster_geopos, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, georadius, arginfo_class_ValkeyGlideCluster_georadius, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, georadius_ro, arginfo_class_ValkeyGlideCluster_georadius_ro, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, georadiusbymember, arginfo_class_ValkeyGlideCluster_georadiusbymember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, georadiusbymember_ro, arginfo_class_ValkeyGlideCluster_georadiusbymember_ro, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, geosearch, arginfo_class_ValkeyGlideCluster_geosearch, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, geosearchstore, arginfo_class_ValkeyGlideCluster_geosearchstore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, get, arginfo_class_ValkeyGlideCluster_get, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getDel, arginfo_class_ValkeyGlideCluster_getDel, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getWithMeta, arginfo_class_ValkeyGlideCluster_getWithMeta, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getEx, arginfo_class_ValkeyGlideCluster_getEx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getBit, arginfo_class_ValkeyGlideCluster_getBit, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getlasterror, arginfo_class_ValkeyGlideCluster_getlasterror, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getmode, arginfo_class_ValkeyGlideCluster_getmode, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getRange, arginfo_class_ValkeyGlideCluster_getRange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lcs, arginfo_class_ValkeyGlideCluster_lcs, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getset, arginfo_class_ValkeyGlideCluster_getset, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hDel, arginfo_class_ValkeyGlideCluster_hDel, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hExists, arginfo_class_ValkeyGlideCluster_hExists, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hGet, arginfo_class_ValkeyGlideCluster_hGet, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hGetAll, arginfo_class_ValkeyGlideCluster_hGetAll, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hIncrBy, arginfo_class_ValkeyGlideCluster_hIncrBy, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hIncrByFloat, arginfo_class_ValkeyGlideCluster_hIncrByFloat, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hKeys, arginfo_class_ValkeyGlideCluster_hKeys, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hLen, arginfo_class_ValkeyGlideCluster_hLen, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hMget, arginfo_class_ValkeyGlideCluster_hMget, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hMset, arginfo_class_ValkeyGlideCluster_hMset, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hscan, arginfo_class_ValkeyGlideCluster_hscan, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, expiremember, arginfo_class_ValkeyGlideCluster_expiremember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, expirememberat, arginfo_class_ValkeyGlideCluster_expirememberat, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hRandField, arginfo_class_ValkeyGlideCluster_hRandField, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hSet, arginfo_class_ValkeyGlideCluster_hSet, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hSetNx, arginfo_class_ValkeyGlideCluster_hSetNx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hStrLen, arginfo_class_ValkeyGlideCluster_hStrLen, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hVals, arginfo_class_ValkeyGlideCluster_hVals, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, incr, arginfo_class_ValkeyGlideCluster_incr, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, incrBy, arginfo_class_ValkeyGlideCluster_incrBy, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, incrByFloat, arginfo_class_ValkeyGlideCluster_incrByFloat, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, info, arginfo_class_ValkeyGlideCluster_info, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, keys, arginfo_class_ValkeyGlideCluster_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lget, arginfo_class_ValkeyGlideCluster_lget, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lindex, arginfo_class_ValkeyGlideCluster_lindex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lInsert, arginfo_class_ValkeyGlideCluster_lInsert, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lLen, arginfo_class_ValkeyGlideCluster_lLen, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lPop, arginfo_class_ValkeyGlideCluster_lPop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lPos, arginfo_class_ValkeyGlideCluster_lPos, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lPush, arginfo_class_ValkeyGlideCluster_lPush, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lPushx, arginfo_class_ValkeyGlideCluster_lPushx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lrange, arginfo_class_ValkeyGlideCluster_lrange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lrem, arginfo_class_ValkeyGlideCluster_lrem, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lSet, arginfo_class_ValkeyGlideCluster_lSet, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, ltrim, arginfo_class_ValkeyGlideCluster_ltrim, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, mget, arginfo_class_ValkeyGlideCluster_mget, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, mset, arginfo_class_ValkeyGlideCluster_mset, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, msetnx, arginfo_class_ValkeyGlideCluster_msetnx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, multi, arginfo_class_ValkeyGlideCluster_multi, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, object, arginfo_class_ValkeyGlideCluster_object, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, persist, arginfo_class_ValkeyGlideCluster_persist, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, pexpire, arginfo_class_ValkeyGlideCluster_pexpire, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, pexpireAt, arginfo_class_ValkeyGlideCluster_pexpireAt, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, pfadd, arginfo_class_ValkeyGlideCluster_pfadd, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, pfcount, arginfo_class_ValkeyGlideCluster_pfcount, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, pfmerge, arginfo_class_ValkeyGlideCluster_pfmerge, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, ping, arginfo_class_ValkeyGlideCluster_ping, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, psetex, arginfo_class_ValkeyGlideCluster_psetex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, psubscribe, arginfo_class_ValkeyGlideCluster_psubscribe, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, pttl, arginfo_class_ValkeyGlideCluster_pttl, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, publish, arginfo_class_ValkeyGlideCluster_publish, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, pubsub, arginfo_class_ValkeyGlideCluster_pubsub, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, punsubscribe, arginfo_class_ValkeyGlideCluster_punsubscribe, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, randomKey, arginfo_class_ValkeyGlideCluster_randomKey, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, rename, arginfo_class_ValkeyGlideCluster_rename, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, renameNx, arginfo_class_ValkeyGlideCluster_renameNx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, restore, arginfo_class_ValkeyGlideCluster_restore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, rPop, arginfo_class_ValkeyGlideCluster_rPop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, rpoplpush, arginfo_class_ValkeyGlideCluster_rpoplpush, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, rPush, arginfo_class_ValkeyGlideCluster_rPush, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, rPushx, arginfo_class_ValkeyGlideCluster_rPushx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sadd, arginfo_class_ValkeyGlideCluster_sadd, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, saddarray, arginfo_class_ValkeyGlideCluster_saddarray, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, scan, arginfo_class_ValkeyGlideCluster_scan, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, scard, arginfo_class_ValkeyGlideCluster_scard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, script, arginfo_class_ValkeyGlideCluster_script, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sdiff, arginfo_class_ValkeyGlideCluster_sdiff, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sdiffstore, arginfo_class_ValkeyGlideCluster_sdiffstore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, set, arginfo_class_ValkeyGlideCluster_set, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, setBit, arginfo_class_ValkeyGlideCluster_setBit, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, setex, arginfo_class_ValkeyGlideCluster_setex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, setnx, arginfo_class_ValkeyGlideCluster_setnx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, setrange, arginfo_class_ValkeyGlideCluster_setrange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sinter, arginfo_class_ValkeyGlideCluster_sinter, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sintercard, arginfo_class_ValkeyGlideCluster_sintercard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sinterstore, arginfo_class_ValkeyGlideCluster_sinterstore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sismember, arginfo_class_ValkeyGlideCluster_sismember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, smismember, arginfo_class_ValkeyGlideCluster_smismember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, smembers, arginfo_class_ValkeyGlideCluster_smembers, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, smove, arginfo_class_ValkeyGlideCluster_smove, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sort, arginfo_class_ValkeyGlideCluster_sort, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sort_ro, arginfo_class_ValkeyGlideCluster_sort_ro, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, spop, arginfo_class_ValkeyGlideCluster_spop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, srandmember, arginfo_class_ValkeyGlideCluster_srandmember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, srem, arginfo_class_ValkeyGlideCluster_srem, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sscan, arginfo_class_ValkeyGlideCluster_sscan, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, strlen, arginfo_class_ValkeyGlideCluster_strlen, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, subscribe, arginfo_class_ValkeyGlideCluster_subscribe, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sunion, arginfo_class_ValkeyGlideCluster_sunion, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sunionstore, arginfo_class_ValkeyGlideCluster_sunionstore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, ttl, arginfo_class_ValkeyGlideCluster_ttl, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, type, arginfo_class_ValkeyGlideCluster_type, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, unsubscribe, arginfo_class_ValkeyGlideCluster_unsubscribe, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, unlink, arginfo_class_ValkeyGlideCluster_unlink, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, unwatch, arginfo_class_ValkeyGlideCluster_unwatch, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, watch, arginfo_class_ValkeyGlideCluster_watch, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, xack, arginfo_class_ValkeyGlideCluster_xack, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, xadd, arginfo_class_ValkeyGlideCluster_xadd, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, xclaim, arginfo_class_ValkeyGlideCluster_xclaim, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, xdel, arginfo_class_ValkeyGlideCluster_xdel, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, xgroup, arginfo_class_ValkeyGlideCluster_xgroup, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, xautoclaim, arginfo_class_ValkeyGlideCluster_xautoclaim, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, xinfo, arginfo_class_ValkeyGlideCluster_xinfo, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, xlen, arginfo_class_ValkeyGlideCluster_xlen, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, xpending, arginfo_class_ValkeyGlideCluster_xpending, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, xrange, arginfo_class_ValkeyGlideCluster_xrange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, xread, arginfo_class_ValkeyGlideCluster_xread, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, xreadgroup, arginfo_class_ValkeyGlideCluster_xreadgroup, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, xrevrange, arginfo_class_ValkeyGlideCluster_xrevrange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, xtrim, arginfo_class_ValkeyGlideCluster_xtrim, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zAdd, arginfo_class_ValkeyGlideCluster_zAdd, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zCard, arginfo_class_ValkeyGlideCluster_zCard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zCount, arginfo_class_ValkeyGlideCluster_zCount, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zIncrBy, arginfo_class_ValkeyGlideCluster_zIncrBy, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zinterstore, arginfo_class_ValkeyGlideCluster_zinterstore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zintercard, arginfo_class_ValkeyGlideCluster_zintercard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zlexcount, arginfo_class_ValkeyGlideCluster_zlexcount, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zpopmax, arginfo_class_ValkeyGlideCluster_zpopmax, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zpopmin, arginfo_class_ValkeyGlideCluster_zpopmin, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrange, arginfo_class_ValkeyGlideCluster_zrange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrangestore, arginfo_class_ValkeyGlideCluster_zrangestore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrandmember, arginfo_class_ValkeyGlideCluster_zrandmember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrangebylex, arginfo_class_ValkeyGlideCluster_zrangebylex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrangebyscore, arginfo_class_ValkeyGlideCluster_zrangebyscore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrank, arginfo_class_ValkeyGlideCluster_zrank, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrem, arginfo_class_ValkeyGlideCluster_zrem, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zremrangebylex, arginfo_class_ValkeyGlideCluster_zremrangebylex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zremrangebyrank, arginfo_class_ValkeyGlideCluster_zremrangebyrank, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zRemRangeByScore, arginfo_class_ValkeyGlideCluster_zRemRangeByScore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrevrange, arginfo_class_ValkeyGlideCluster_zrevrange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrevrangebylex, arginfo_class_ValkeyGlideCluster_zrevrangebylex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrevrangebyscore, arginfo_class_ValkeyGlideCluster_zrevrangebyscore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrevrank, arginfo_class_ValkeyGlideCluster_zrevrank, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zscan, arginfo_class_ValkeyGlideCluster_zscan, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zScore, arginfo_class_ValkeyGlideCluster_zScore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zMscore, arginfo_class_ValkeyGlideCluster_zMscore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zunionstore, arginfo_class_ValkeyGlideCluster_zunionstore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zinter, arginfo_class_ValkeyGlideCluster_zinter, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zdiffstore, arginfo_class_ValkeyGlideCluster_zdiffstore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zunion, arginfo_class_ValkeyGlideCluster_zunion, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zdiff, arginfo_class_ValkeyGlideCluster_zdiff, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ValkeyGlideClusterException_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_ValkeyGlideCluster(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ValkeyGlideCluster", class_ValkeyGlideCluster_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}

static zend_class_entry *register_class_ValkeyGlideClusterException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ValkeyGlideClusterException", class_ValkeyGlideClusterException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RuntimeException);

	return class_entry;
}
