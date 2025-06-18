/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 43a43fa735ced4b48a361078ac8a10fb62cb1244 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, seeds)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, read_timeout)
	ZEND_ARG_INFO(0, persistent)
	ZEND_ARG_INFO(0, auth)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster__compress, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster__uncompress arginfo_class_ValkeyGlideCluster__compress

#define arginfo_class_ValkeyGlideCluster__serialize arginfo_class_ValkeyGlideCluster__compress

#define arginfo_class_ValkeyGlideCluster__unserialize arginfo_class_ValkeyGlideCluster__compress

#define arginfo_class_ValkeyGlideCluster__pack arginfo_class_ValkeyGlideCluster__compress

#define arginfo_class_ValkeyGlideCluster__unpack arginfo_class_ValkeyGlideCluster__compress

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster__prefix, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster__masters, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster__redir arginfo_class_ValkeyGlideCluster__masters

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_acl, 0, 0, 2)
	ZEND_ARG_INFO(0, key_or_address)
	ZEND_ARG_INFO(0, subcmd)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_append, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_bgrewriteaof, 0, 0, 1)
	ZEND_ARG_INFO(0, key_or_address)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_waitaof, 0, 0, 4)
	ZEND_ARG_INFO(0, key_or_address)
	ZEND_ARG_INFO(0, numlocal)
	ZEND_ARG_INFO(0, numreplicas)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_bgsave arginfo_class_ValkeyGlideCluster_bgrewriteaof

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

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lmove, 0, 0, 4)
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

#define arginfo_class_ValkeyGlideCluster_clearlasterror arginfo_class_ValkeyGlideCluster__masters

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_client, 0, 0, 2)
	ZEND_ARG_INFO(0, key_or_address)
	ZEND_ARG_INFO(0, subcommand)
	ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_close arginfo_class_ValkeyGlideCluster__masters

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_cluster, 0, 0, 2)
	ZEND_ARG_INFO(0, key_or_address)
	ZEND_ARG_INFO(0, command)
	ZEND_ARG_VARIADIC_INFO(0, extra_args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_command, 0, 0, 0)
	ZEND_ARG_VARIADIC_INFO(0, extra_args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_config, 0, 0, 2)
	ZEND_ARG_INFO(0, key_or_address)
	ZEND_ARG_INFO(0, subcommand)
	ZEND_ARG_VARIADIC_INFO(0, extra_args)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_dbsize arginfo_class_ValkeyGlideCluster_bgrewriteaof

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

#define arginfo_class_ValkeyGlideCluster_decrbyfloat arginfo_class_ValkeyGlideCluster_append

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_del, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_VARIADIC_INFO(0, other_keys)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_discard arginfo_class_ValkeyGlideCluster__masters

#define arginfo_class_ValkeyGlideCluster_dump arginfo_class_ValkeyGlideCluster__prefix

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_echo, 0, 0, 2)
	ZEND_ARG_INFO(0, key_or_address)
	ZEND_ARG_INFO(0, msg)
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

#define arginfo_class_ValkeyGlideCluster_exec arginfo_class_ValkeyGlideCluster__masters

#define arginfo_class_ValkeyGlideCluster_exists arginfo_class_ValkeyGlideCluster_del

#define arginfo_class_ValkeyGlideCluster_touch arginfo_class_ValkeyGlideCluster_del

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_expire, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_expireat, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, timestamp)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_expiretime arginfo_class_ValkeyGlideCluster__prefix

#define arginfo_class_ValkeyGlideCluster_pexpiretime arginfo_class_ValkeyGlideCluster__prefix

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_flushall, 0, 0, 1)
	ZEND_ARG_INFO(0, key_or_address)
	ZEND_ARG_INFO(0, async)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_flushdb arginfo_class_ValkeyGlideCluster_flushall

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

#define arginfo_class_ValkeyGlideCluster_get arginfo_class_ValkeyGlideCluster__prefix

#define arginfo_class_ValkeyGlideCluster_getdel arginfo_class_ValkeyGlideCluster__prefix

#define arginfo_class_ValkeyGlideCluster_getWithMeta arginfo_class_ValkeyGlideCluster__prefix

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_getex, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_getbit arginfo_class_ValkeyGlideCluster_append

#define arginfo_class_ValkeyGlideCluster_getlasterror arginfo_class_ValkeyGlideCluster__masters

#define arginfo_class_ValkeyGlideCluster_getmode arginfo_class_ValkeyGlideCluster__masters

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_getoption, 0, 0, 1)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_getrange, 0, 0, 3)
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

#define arginfo_class_ValkeyGlideCluster_gettransferredbytes arginfo_class_ValkeyGlideCluster__masters

#define arginfo_class_ValkeyGlideCluster_cleartransferredbytes arginfo_class_ValkeyGlideCluster__masters

#define arginfo_class_ValkeyGlideCluster_hdel arginfo_class_ValkeyGlideCluster_geohash

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_hexists, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, member)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_hget arginfo_class_ValkeyGlideCluster_hexists

#define arginfo_class_ValkeyGlideCluster_hgetall arginfo_class_ValkeyGlideCluster__prefix

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_hincrby, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, member)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_hincrbyfloat arginfo_class_ValkeyGlideCluster_hincrby

#define arginfo_class_ValkeyGlideCluster_hkeys arginfo_class_ValkeyGlideCluster__prefix

#define arginfo_class_ValkeyGlideCluster_hlen arginfo_class_ValkeyGlideCluster__prefix

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_hmget, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_hmset, 0, 0, 2)
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

#define arginfo_class_ValkeyGlideCluster_hrandfield arginfo_class_ValkeyGlideCluster_getex

#define arginfo_class_ValkeyGlideCluster_hset arginfo_class_ValkeyGlideCluster_hincrby

#define arginfo_class_ValkeyGlideCluster_hsetnx arginfo_class_ValkeyGlideCluster_hincrby

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_hstrlen, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_hvals arginfo_class_ValkeyGlideCluster__prefix

#define arginfo_class_ValkeyGlideCluster_incr arginfo_class_ValkeyGlideCluster_decr

#define arginfo_class_ValkeyGlideCluster_incrby arginfo_class_ValkeyGlideCluster_append

#define arginfo_class_ValkeyGlideCluster_incrbyfloat arginfo_class_ValkeyGlideCluster_append

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_info, 0, 0, 1)
	ZEND_ARG_INFO(0, key_or_address)
	ZEND_ARG_VARIADIC_INFO(0, sections)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_keys, 0, 0, 1)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_lastsave arginfo_class_ValkeyGlideCluster_bgrewriteaof

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lget, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_lindex arginfo_class_ValkeyGlideCluster_lget

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_linsert, 0, 0, 4)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, pos)
	ZEND_ARG_INFO(0, pivot)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_llen arginfo_class_ValkeyGlideCluster__prefix

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lpop, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lpos, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lpush, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_VARIADIC_INFO(0, other_values)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_lpushx arginfo_class_ValkeyGlideCluster_append

#define arginfo_class_ValkeyGlideCluster_lrange arginfo_class_ValkeyGlideCluster_getrange

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lrem, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_lset, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_ltrim arginfo_class_ValkeyGlideCluster_getrange

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

#define arginfo_class_ValkeyGlideCluster_persist arginfo_class_ValkeyGlideCluster__prefix

#define arginfo_class_ValkeyGlideCluster_pexpire arginfo_class_ValkeyGlideCluster_expire

#define arginfo_class_ValkeyGlideCluster_pexpireat arginfo_class_ValkeyGlideCluster_expireat

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_pfadd, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, elements)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_pfcount arginfo_class_ValkeyGlideCluster__prefix

#define arginfo_class_ValkeyGlideCluster_pfmerge arginfo_class_ValkeyGlideCluster_hmget

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_ping, 0, 0, 1)
	ZEND_ARG_INFO(0, key_or_address)
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

#define arginfo_class_ValkeyGlideCluster_pttl arginfo_class_ValkeyGlideCluster__prefix

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_publish, 0, 0, 2)
	ZEND_ARG_INFO(0, channel)
	ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_pubsub, 0, 0, 1)
	ZEND_ARG_INFO(0, key_or_address)
	ZEND_ARG_VARIADIC_INFO(0, values)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_punsubscribe, 0, 0, 1)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_VARIADIC_INFO(0, other_patterns)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_randomkey arginfo_class_ValkeyGlideCluster_bgrewriteaof

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_rawcommand, 0, 0, 2)
	ZEND_ARG_INFO(0, key_or_address)
	ZEND_ARG_INFO(0, command)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_rename, 0, 0, 2)
	ZEND_ARG_INFO(0, key_src)
	ZEND_ARG_INFO(0, key_dst)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_renamenx, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, newkey)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_restore, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_role arginfo_class_ValkeyGlideCluster_bgrewriteaof

#define arginfo_class_ValkeyGlideCluster_rpop arginfo_class_ValkeyGlideCluster_lpop

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_rpoplpush, 0, 0, 2)
	ZEND_ARG_INFO(0, src)
	ZEND_ARG_INFO(0, dst)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_rpush, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_VARIADIC_INFO(0, elements)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_rpushx arginfo_class_ValkeyGlideCluster_append

#define arginfo_class_ValkeyGlideCluster_sadd arginfo_class_ValkeyGlideCluster_lpush

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_saddarray, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, values)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_save arginfo_class_ValkeyGlideCluster_bgrewriteaof

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_scan, 0, 0, 2)
	ZEND_ARG_INFO(1, iterator)
	ZEND_ARG_INFO(0, key_or_address)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_scard arginfo_class_ValkeyGlideCluster__prefix

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_script, 0, 0, 1)
	ZEND_ARG_INFO(0, key_or_address)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_sdiff arginfo_class_ValkeyGlideCluster_del

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_sdiffstore, 0, 0, 2)
	ZEND_ARG_INFO(0, dst)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_VARIADIC_INFO(0, other_keys)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_set arginfo_class_ValkeyGlideCluster_lpos

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_setbit, 0, 0, 3)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_setoption, 0, 0, 2)
	ZEND_ARG_INFO(0, option)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

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

#define arginfo_class_ValkeyGlideCluster_slowlog arginfo_class_ValkeyGlideCluster_script

#define arginfo_class_ValkeyGlideCluster_smembers arginfo_class_ValkeyGlideCluster__prefix

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_smove, 0, 0, 3)
	ZEND_ARG_INFO(0, src)
	ZEND_ARG_INFO(0, dst)
	ZEND_ARG_INFO(0, member)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_sort arginfo_class_ValkeyGlideCluster_getex

#define arginfo_class_ValkeyGlideCluster_sort_ro arginfo_class_ValkeyGlideCluster_getex

#define arginfo_class_ValkeyGlideCluster_spop arginfo_class_ValkeyGlideCluster_lpop

#define arginfo_class_ValkeyGlideCluster_srandmember arginfo_class_ValkeyGlideCluster_lpop

#define arginfo_class_ValkeyGlideCluster_srem arginfo_class_ValkeyGlideCluster_lpush

#define arginfo_class_ValkeyGlideCluster_sscan arginfo_class_ValkeyGlideCluster_hscan

#define arginfo_class_ValkeyGlideCluster_strlen arginfo_class_ValkeyGlideCluster__prefix

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_subscribe, 0, 0, 2)
	ZEND_ARG_INFO(0, channels)
	ZEND_ARG_INFO(0, cb)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_sunion arginfo_class_ValkeyGlideCluster_del

#define arginfo_class_ValkeyGlideCluster_sunionstore arginfo_class_ValkeyGlideCluster_sdiffstore

#define arginfo_class_ValkeyGlideCluster_time arginfo_class_ValkeyGlideCluster_bgrewriteaof

#define arginfo_class_ValkeyGlideCluster_ttl arginfo_class_ValkeyGlideCluster__prefix

#define arginfo_class_ValkeyGlideCluster_type arginfo_class_ValkeyGlideCluster__prefix

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_unsubscribe, 0, 0, 1)
	ZEND_ARG_INFO(0, channels)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_unlink arginfo_class_ValkeyGlideCluster_del

#define arginfo_class_ValkeyGlideCluster_unwatch arginfo_class_ValkeyGlideCluster__masters

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

#define arginfo_class_ValkeyGlideCluster_xlen arginfo_class_ValkeyGlideCluster__prefix

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

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zadd, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, score_or_options)
	ZEND_ARG_VARIADIC_INFO(0, more_scores_and_mems)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zcard arginfo_class_ValkeyGlideCluster__prefix

#define arginfo_class_ValkeyGlideCluster_zcount arginfo_class_ValkeyGlideCluster_getrange

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zincrby, 0, 0, 3)
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

#define arginfo_class_ValkeyGlideCluster_zrandmember arginfo_class_ValkeyGlideCluster_getex

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zrangebylex, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, max)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zrangebyscore arginfo_class_ValkeyGlideCluster_zrange

#define arginfo_class_ValkeyGlideCluster_zrank arginfo_class_ValkeyGlideCluster_hexists

#define arginfo_class_ValkeyGlideCluster_zrem arginfo_class_ValkeyGlideCluster_lpush

#define arginfo_class_ValkeyGlideCluster_zremrangebylex arginfo_class_ValkeyGlideCluster_zlexcount

#define arginfo_class_ValkeyGlideCluster_zremrangebyrank arginfo_class_ValkeyGlideCluster_zlexcount

#define arginfo_class_ValkeyGlideCluster_zremrangebyscore arginfo_class_ValkeyGlideCluster_zlexcount

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster_zrevrange, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, max)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zrevrangebylex arginfo_class_ValkeyGlideCluster_zrevrange

#define arginfo_class_ValkeyGlideCluster_zrevrangebyscore arginfo_class_ValkeyGlideCluster_zrevrange

#define arginfo_class_ValkeyGlideCluster_zrevrank arginfo_class_ValkeyGlideCluster_hexists

#define arginfo_class_ValkeyGlideCluster_zscan arginfo_class_ValkeyGlideCluster_hscan

#define arginfo_class_ValkeyGlideCluster_zscore arginfo_class_ValkeyGlideCluster_hexists

#define arginfo_class_ValkeyGlideCluster_zmscore arginfo_class_ValkeyGlideCluster_geohash

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
ZEND_METHOD(ValkeyGlideCluster, _compress);
ZEND_METHOD(ValkeyGlideCluster, _uncompress);
ZEND_METHOD(ValkeyGlideCluster, _serialize);
ZEND_METHOD(ValkeyGlideCluster, _unserialize);
ZEND_METHOD(ValkeyGlideCluster, _pack);
ZEND_METHOD(ValkeyGlideCluster, _unpack);
ZEND_METHOD(ValkeyGlideCluster, _prefix);
ZEND_METHOD(ValkeyGlideCluster, _masters);
ZEND_METHOD(ValkeyGlideCluster, _redir);
ZEND_METHOD(ValkeyGlideCluster, acl);
ZEND_METHOD(ValkeyGlideCluster, append);
ZEND_METHOD(ValkeyGlideCluster, bgrewriteaof);
ZEND_METHOD(ValkeyGlideCluster, waitaof);
ZEND_METHOD(ValkeyGlideCluster, bgsave);
ZEND_METHOD(ValkeyGlideCluster, bitcount);
ZEND_METHOD(ValkeyGlideCluster, bitop);
ZEND_METHOD(ValkeyGlideCluster, bitpos);
ZEND_METHOD(ValkeyGlideCluster, blpop);
ZEND_METHOD(ValkeyGlideCluster, brpop);
ZEND_METHOD(ValkeyGlideCluster, brpoplpush);
ZEND_METHOD(ValkeyGlideCluster, lmove);
ZEND_METHOD(ValkeyGlideCluster, blmove);
ZEND_METHOD(ValkeyGlideCluster, bzpopmax);
ZEND_METHOD(ValkeyGlideCluster, bzpopmin);
ZEND_METHOD(ValkeyGlideCluster, bzmpop);
ZEND_METHOD(ValkeyGlideCluster, zmpop);
ZEND_METHOD(ValkeyGlideCluster, blmpop);
ZEND_METHOD(ValkeyGlideCluster, lmpop);
ZEND_METHOD(ValkeyGlideCluster, clearlasterror);
ZEND_METHOD(ValkeyGlideCluster, client);
ZEND_METHOD(ValkeyGlideCluster, close);
ZEND_METHOD(ValkeyGlideCluster, cluster);
ZEND_METHOD(ValkeyGlideCluster, command);
ZEND_METHOD(ValkeyGlideCluster, config);
ZEND_METHOD(ValkeyGlideCluster, dbsize);
ZEND_METHOD(ValkeyGlideCluster, copy);
ZEND_METHOD(ValkeyGlideCluster, decr);
ZEND_METHOD(ValkeyGlideCluster, decrby);
ZEND_METHOD(ValkeyGlideCluster, decrbyfloat);
ZEND_METHOD(ValkeyGlideCluster, del);
ZEND_METHOD(ValkeyGlideCluster, discard);
ZEND_METHOD(ValkeyGlideCluster, dump);
ZEND_METHOD(ValkeyGlideCluster, echo);
ZEND_METHOD(ValkeyGlideCluster, eval);
ZEND_METHOD(ValkeyGlideCluster, eval_ro);
ZEND_METHOD(ValkeyGlideCluster, evalsha);
ZEND_METHOD(ValkeyGlideCluster, evalsha_ro);
ZEND_METHOD(ValkeyGlideCluster, exec);
ZEND_METHOD(ValkeyGlideCluster, exists);
ZEND_METHOD(ValkeyGlideCluster, touch);
ZEND_METHOD(ValkeyGlideCluster, expire);
ZEND_METHOD(ValkeyGlideCluster, expireat);
ZEND_METHOD(ValkeyGlideCluster, expiretime);
ZEND_METHOD(ValkeyGlideCluster, pexpiretime);
ZEND_METHOD(ValkeyGlideCluster, flushall);
ZEND_METHOD(ValkeyGlideCluster, flushdb);
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
ZEND_METHOD(ValkeyGlideCluster, getdel);
ZEND_METHOD(ValkeyGlideCluster, getWithMeta);
ZEND_METHOD(ValkeyGlideCluster, getex);
ZEND_METHOD(ValkeyGlideCluster, getbit);
ZEND_METHOD(ValkeyGlideCluster, getlasterror);
ZEND_METHOD(ValkeyGlideCluster, getmode);
ZEND_METHOD(ValkeyGlideCluster, getoption);
ZEND_METHOD(ValkeyGlideCluster, getrange);
ZEND_METHOD(ValkeyGlideCluster, lcs);
ZEND_METHOD(ValkeyGlideCluster, getset);
ZEND_METHOD(ValkeyGlideCluster, gettransferredbytes);
ZEND_METHOD(ValkeyGlideCluster, cleartransferredbytes);
ZEND_METHOD(ValkeyGlideCluster, hdel);
ZEND_METHOD(ValkeyGlideCluster, hexists);
ZEND_METHOD(ValkeyGlideCluster, hget);
ZEND_METHOD(ValkeyGlideCluster, hgetall);
ZEND_METHOD(ValkeyGlideCluster, hincrby);
ZEND_METHOD(ValkeyGlideCluster, hincrbyfloat);
ZEND_METHOD(ValkeyGlideCluster, hkeys);
ZEND_METHOD(ValkeyGlideCluster, hlen);
ZEND_METHOD(ValkeyGlideCluster, hmget);
ZEND_METHOD(ValkeyGlideCluster, hmset);
ZEND_METHOD(ValkeyGlideCluster, hscan);
ZEND_METHOD(ValkeyGlideCluster, expiremember);
ZEND_METHOD(ValkeyGlideCluster, expirememberat);
ZEND_METHOD(ValkeyGlideCluster, hrandfield);
ZEND_METHOD(ValkeyGlideCluster, hset);
ZEND_METHOD(ValkeyGlideCluster, hsetnx);
ZEND_METHOD(ValkeyGlideCluster, hstrlen);
ZEND_METHOD(ValkeyGlideCluster, hvals);
ZEND_METHOD(ValkeyGlideCluster, incr);
ZEND_METHOD(ValkeyGlideCluster, incrby);
ZEND_METHOD(ValkeyGlideCluster, incrbyfloat);
ZEND_METHOD(ValkeyGlideCluster, info);
ZEND_METHOD(ValkeyGlideCluster, keys);
ZEND_METHOD(ValkeyGlideCluster, lastsave);
ZEND_METHOD(ValkeyGlideCluster, lget);
ZEND_METHOD(ValkeyGlideCluster, lindex);
ZEND_METHOD(ValkeyGlideCluster, linsert);
ZEND_METHOD(ValkeyGlideCluster, llen);
ZEND_METHOD(ValkeyGlideCluster, lpop);
ZEND_METHOD(ValkeyGlideCluster, lpos);
ZEND_METHOD(ValkeyGlideCluster, lpush);
ZEND_METHOD(ValkeyGlideCluster, lpushx);
ZEND_METHOD(ValkeyGlideCluster, lrange);
ZEND_METHOD(ValkeyGlideCluster, lrem);
ZEND_METHOD(ValkeyGlideCluster, lset);
ZEND_METHOD(ValkeyGlideCluster, ltrim);
ZEND_METHOD(ValkeyGlideCluster, mget);
ZEND_METHOD(ValkeyGlideCluster, mset);
ZEND_METHOD(ValkeyGlideCluster, msetnx);
ZEND_METHOD(ValkeyGlideCluster, multi);
ZEND_METHOD(ValkeyGlideCluster, object);
ZEND_METHOD(ValkeyGlideCluster, persist);
ZEND_METHOD(ValkeyGlideCluster, pexpire);
ZEND_METHOD(ValkeyGlideCluster, pexpireat);
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
ZEND_METHOD(ValkeyGlideCluster, randomkey);
ZEND_METHOD(ValkeyGlideCluster, rawcommand);
ZEND_METHOD(ValkeyGlideCluster, rename);
ZEND_METHOD(ValkeyGlideCluster, renamenx);
ZEND_METHOD(ValkeyGlideCluster, restore);
ZEND_METHOD(ValkeyGlideCluster, role);
ZEND_METHOD(ValkeyGlideCluster, rpop);
ZEND_METHOD(ValkeyGlideCluster, rpoplpush);
ZEND_METHOD(ValkeyGlideCluster, rpush);
ZEND_METHOD(ValkeyGlideCluster, rpushx);
ZEND_METHOD(ValkeyGlideCluster, sadd);
ZEND_METHOD(ValkeyGlideCluster, saddarray);
ZEND_METHOD(ValkeyGlideCluster, save);
ZEND_METHOD(ValkeyGlideCluster, scan);
ZEND_METHOD(ValkeyGlideCluster, scard);
ZEND_METHOD(ValkeyGlideCluster, script);
ZEND_METHOD(ValkeyGlideCluster, sdiff);
ZEND_METHOD(ValkeyGlideCluster, sdiffstore);
ZEND_METHOD(ValkeyGlideCluster, set);
ZEND_METHOD(ValkeyGlideCluster, setbit);
ZEND_METHOD(ValkeyGlideCluster, setex);
ZEND_METHOD(ValkeyGlideCluster, setnx);
ZEND_METHOD(ValkeyGlideCluster, setoption);
ZEND_METHOD(ValkeyGlideCluster, setrange);
ZEND_METHOD(ValkeyGlideCluster, sinter);
ZEND_METHOD(ValkeyGlideCluster, sintercard);
ZEND_METHOD(ValkeyGlideCluster, sinterstore);
ZEND_METHOD(ValkeyGlideCluster, sismember);
ZEND_METHOD(ValkeyGlideCluster, smismember);
ZEND_METHOD(ValkeyGlideCluster, slowlog);
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
ZEND_METHOD(ValkeyGlideCluster, time);
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
ZEND_METHOD(ValkeyGlideCluster, zadd);
ZEND_METHOD(ValkeyGlideCluster, zcard);
ZEND_METHOD(ValkeyGlideCluster, zcount);
ZEND_METHOD(ValkeyGlideCluster, zincrby);
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
ZEND_METHOD(ValkeyGlideCluster, zremrangebyscore);
ZEND_METHOD(ValkeyGlideCluster, zrevrange);
ZEND_METHOD(ValkeyGlideCluster, zrevrangebylex);
ZEND_METHOD(ValkeyGlideCluster, zrevrangebyscore);
ZEND_METHOD(ValkeyGlideCluster, zrevrank);
ZEND_METHOD(ValkeyGlideCluster, zscan);
ZEND_METHOD(ValkeyGlideCluster, zscore);
ZEND_METHOD(ValkeyGlideCluster, zmscore);
ZEND_METHOD(ValkeyGlideCluster, zunionstore);
ZEND_METHOD(ValkeyGlideCluster, zinter);
ZEND_METHOD(ValkeyGlideCluster, zdiffstore);
ZEND_METHOD(ValkeyGlideCluster, zunion);
ZEND_METHOD(ValkeyGlideCluster, zdiff);


static const zend_function_entry class_ValkeyGlideCluster_methods[] = {
	ZEND_ME(ValkeyGlideCluster, __construct, arginfo_class_ValkeyGlideCluster___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, _compress, arginfo_class_ValkeyGlideCluster__compress, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, _uncompress, arginfo_class_ValkeyGlideCluster__uncompress, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, _serialize, arginfo_class_ValkeyGlideCluster__serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, _unserialize, arginfo_class_ValkeyGlideCluster__unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, _pack, arginfo_class_ValkeyGlideCluster__pack, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, _unpack, arginfo_class_ValkeyGlideCluster__unpack, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, _prefix, arginfo_class_ValkeyGlideCluster__prefix, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, _masters, arginfo_class_ValkeyGlideCluster__masters, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, _redir, arginfo_class_ValkeyGlideCluster__redir, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, acl, arginfo_class_ValkeyGlideCluster_acl, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, append, arginfo_class_ValkeyGlideCluster_append, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, bgrewriteaof, arginfo_class_ValkeyGlideCluster_bgrewriteaof, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, waitaof, arginfo_class_ValkeyGlideCluster_waitaof, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, bgsave, arginfo_class_ValkeyGlideCluster_bgsave, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, bitcount, arginfo_class_ValkeyGlideCluster_bitcount, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, bitop, arginfo_class_ValkeyGlideCluster_bitop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, bitpos, arginfo_class_ValkeyGlideCluster_bitpos, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, blpop, arginfo_class_ValkeyGlideCluster_blpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, brpop, arginfo_class_ValkeyGlideCluster_brpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, brpoplpush, arginfo_class_ValkeyGlideCluster_brpoplpush, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lmove, arginfo_class_ValkeyGlideCluster_lmove, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, blmove, arginfo_class_ValkeyGlideCluster_blmove, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, bzpopmax, arginfo_class_ValkeyGlideCluster_bzpopmax, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, bzpopmin, arginfo_class_ValkeyGlideCluster_bzpopmin, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, bzmpop, arginfo_class_ValkeyGlideCluster_bzmpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zmpop, arginfo_class_ValkeyGlideCluster_zmpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, blmpop, arginfo_class_ValkeyGlideCluster_blmpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lmpop, arginfo_class_ValkeyGlideCluster_lmpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, clearlasterror, arginfo_class_ValkeyGlideCluster_clearlasterror, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, client, arginfo_class_ValkeyGlideCluster_client, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, close, arginfo_class_ValkeyGlideCluster_close, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, cluster, arginfo_class_ValkeyGlideCluster_cluster, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, command, arginfo_class_ValkeyGlideCluster_command, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, config, arginfo_class_ValkeyGlideCluster_config, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, dbsize, arginfo_class_ValkeyGlideCluster_dbsize, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, copy, arginfo_class_ValkeyGlideCluster_copy, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, decr, arginfo_class_ValkeyGlideCluster_decr, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, decrby, arginfo_class_ValkeyGlideCluster_decrby, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, decrbyfloat, arginfo_class_ValkeyGlideCluster_decrbyfloat, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, del, arginfo_class_ValkeyGlideCluster_del, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, discard, arginfo_class_ValkeyGlideCluster_discard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, dump, arginfo_class_ValkeyGlideCluster_dump, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, echo, arginfo_class_ValkeyGlideCluster_echo, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, eval, arginfo_class_ValkeyGlideCluster_eval, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, eval_ro, arginfo_class_ValkeyGlideCluster_eval_ro, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, evalsha, arginfo_class_ValkeyGlideCluster_evalsha, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, evalsha_ro, arginfo_class_ValkeyGlideCluster_evalsha_ro, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, exec, arginfo_class_ValkeyGlideCluster_exec, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, exists, arginfo_class_ValkeyGlideCluster_exists, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, touch, arginfo_class_ValkeyGlideCluster_touch, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, expire, arginfo_class_ValkeyGlideCluster_expire, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, expireat, arginfo_class_ValkeyGlideCluster_expireat, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, expiretime, arginfo_class_ValkeyGlideCluster_expiretime, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, pexpiretime, arginfo_class_ValkeyGlideCluster_pexpiretime, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, flushall, arginfo_class_ValkeyGlideCluster_flushall, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, flushdb, arginfo_class_ValkeyGlideCluster_flushdb, ZEND_ACC_PUBLIC)
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
	ZEND_ME(ValkeyGlideCluster, getdel, arginfo_class_ValkeyGlideCluster_getdel, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getWithMeta, arginfo_class_ValkeyGlideCluster_getWithMeta, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getex, arginfo_class_ValkeyGlideCluster_getex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getbit, arginfo_class_ValkeyGlideCluster_getbit, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getlasterror, arginfo_class_ValkeyGlideCluster_getlasterror, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getmode, arginfo_class_ValkeyGlideCluster_getmode, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getoption, arginfo_class_ValkeyGlideCluster_getoption, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getrange, arginfo_class_ValkeyGlideCluster_getrange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lcs, arginfo_class_ValkeyGlideCluster_lcs, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, getset, arginfo_class_ValkeyGlideCluster_getset, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, gettransferredbytes, arginfo_class_ValkeyGlideCluster_gettransferredbytes, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, cleartransferredbytes, arginfo_class_ValkeyGlideCluster_cleartransferredbytes, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hdel, arginfo_class_ValkeyGlideCluster_hdel, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hexists, arginfo_class_ValkeyGlideCluster_hexists, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hget, arginfo_class_ValkeyGlideCluster_hget, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hgetall, arginfo_class_ValkeyGlideCluster_hgetall, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hincrby, arginfo_class_ValkeyGlideCluster_hincrby, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hincrbyfloat, arginfo_class_ValkeyGlideCluster_hincrbyfloat, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hkeys, arginfo_class_ValkeyGlideCluster_hkeys, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hlen, arginfo_class_ValkeyGlideCluster_hlen, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hmget, arginfo_class_ValkeyGlideCluster_hmget, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hmset, arginfo_class_ValkeyGlideCluster_hmset, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hscan, arginfo_class_ValkeyGlideCluster_hscan, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, expiremember, arginfo_class_ValkeyGlideCluster_expiremember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, expirememberat, arginfo_class_ValkeyGlideCluster_expirememberat, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hrandfield, arginfo_class_ValkeyGlideCluster_hrandfield, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hset, arginfo_class_ValkeyGlideCluster_hset, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hsetnx, arginfo_class_ValkeyGlideCluster_hsetnx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hstrlen, arginfo_class_ValkeyGlideCluster_hstrlen, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, hvals, arginfo_class_ValkeyGlideCluster_hvals, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, incr, arginfo_class_ValkeyGlideCluster_incr, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, incrby, arginfo_class_ValkeyGlideCluster_incrby, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, incrbyfloat, arginfo_class_ValkeyGlideCluster_incrbyfloat, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, info, arginfo_class_ValkeyGlideCluster_info, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, keys, arginfo_class_ValkeyGlideCluster_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lastsave, arginfo_class_ValkeyGlideCluster_lastsave, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lget, arginfo_class_ValkeyGlideCluster_lget, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lindex, arginfo_class_ValkeyGlideCluster_lindex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, linsert, arginfo_class_ValkeyGlideCluster_linsert, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, llen, arginfo_class_ValkeyGlideCluster_llen, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lpop, arginfo_class_ValkeyGlideCluster_lpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lpos, arginfo_class_ValkeyGlideCluster_lpos, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lpush, arginfo_class_ValkeyGlideCluster_lpush, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lpushx, arginfo_class_ValkeyGlideCluster_lpushx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lrange, arginfo_class_ValkeyGlideCluster_lrange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lrem, arginfo_class_ValkeyGlideCluster_lrem, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, lset, arginfo_class_ValkeyGlideCluster_lset, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, ltrim, arginfo_class_ValkeyGlideCluster_ltrim, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, mget, arginfo_class_ValkeyGlideCluster_mget, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, mset, arginfo_class_ValkeyGlideCluster_mset, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, msetnx, arginfo_class_ValkeyGlideCluster_msetnx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, multi, arginfo_class_ValkeyGlideCluster_multi, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, object, arginfo_class_ValkeyGlideCluster_object, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, persist, arginfo_class_ValkeyGlideCluster_persist, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, pexpire, arginfo_class_ValkeyGlideCluster_pexpire, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, pexpireat, arginfo_class_ValkeyGlideCluster_pexpireat, ZEND_ACC_PUBLIC)
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
	ZEND_ME(ValkeyGlideCluster, randomkey, arginfo_class_ValkeyGlideCluster_randomkey, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, rawcommand, arginfo_class_ValkeyGlideCluster_rawcommand, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, rename, arginfo_class_ValkeyGlideCluster_rename, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, renamenx, arginfo_class_ValkeyGlideCluster_renamenx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, restore, arginfo_class_ValkeyGlideCluster_restore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, role, arginfo_class_ValkeyGlideCluster_role, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, rpop, arginfo_class_ValkeyGlideCluster_rpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, rpoplpush, arginfo_class_ValkeyGlideCluster_rpoplpush, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, rpush, arginfo_class_ValkeyGlideCluster_rpush, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, rpushx, arginfo_class_ValkeyGlideCluster_rpushx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sadd, arginfo_class_ValkeyGlideCluster_sadd, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, saddarray, arginfo_class_ValkeyGlideCluster_saddarray, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, save, arginfo_class_ValkeyGlideCluster_save, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, scan, arginfo_class_ValkeyGlideCluster_scan, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, scard, arginfo_class_ValkeyGlideCluster_scard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, script, arginfo_class_ValkeyGlideCluster_script, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sdiff, arginfo_class_ValkeyGlideCluster_sdiff, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sdiffstore, arginfo_class_ValkeyGlideCluster_sdiffstore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, set, arginfo_class_ValkeyGlideCluster_set, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, setbit, arginfo_class_ValkeyGlideCluster_setbit, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, setex, arginfo_class_ValkeyGlideCluster_setex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, setnx, arginfo_class_ValkeyGlideCluster_setnx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, setoption, arginfo_class_ValkeyGlideCluster_setoption, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, setrange, arginfo_class_ValkeyGlideCluster_setrange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sinter, arginfo_class_ValkeyGlideCluster_sinter, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sintercard, arginfo_class_ValkeyGlideCluster_sintercard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sinterstore, arginfo_class_ValkeyGlideCluster_sinterstore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sismember, arginfo_class_ValkeyGlideCluster_sismember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, smismember, arginfo_class_ValkeyGlideCluster_smismember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, slowlog, arginfo_class_ValkeyGlideCluster_slowlog, ZEND_ACC_PUBLIC)
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
	ZEND_ME(ValkeyGlideCluster, time, arginfo_class_ValkeyGlideCluster_time, ZEND_ACC_PUBLIC)
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
	ZEND_ME(ValkeyGlideCluster, zadd, arginfo_class_ValkeyGlideCluster_zadd, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zcard, arginfo_class_ValkeyGlideCluster_zcard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zcount, arginfo_class_ValkeyGlideCluster_zcount, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zincrby, arginfo_class_ValkeyGlideCluster_zincrby, ZEND_ACC_PUBLIC)
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
	ZEND_ME(ValkeyGlideCluster, zremrangebyscore, arginfo_class_ValkeyGlideCluster_zremrangebyscore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrevrange, arginfo_class_ValkeyGlideCluster_zrevrange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrevrangebylex, arginfo_class_ValkeyGlideCluster_zrevrangebylex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrevrangebyscore, arginfo_class_ValkeyGlideCluster_zrevrangebyscore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrevrank, arginfo_class_ValkeyGlideCluster_zrevrank, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zscan, arginfo_class_ValkeyGlideCluster_zscan, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zscore, arginfo_class_ValkeyGlideCluster_zscore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zmscore, arginfo_class_ValkeyGlideCluster_zmscore, ZEND_ACC_PUBLIC)
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

	zval const_OPT_SLAVE_FAILOVER_value;
	ZVAL_LONG(&const_OPT_SLAVE_FAILOVER_value, REDIS_OPT_FAILOVER);
	zend_string *const_OPT_SLAVE_FAILOVER_name = zend_string_init_interned("OPT_SLAVE_FAILOVER", sizeof("OPT_SLAVE_FAILOVER") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_OPT_SLAVE_FAILOVER_name, &const_OPT_SLAVE_FAILOVER_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_OPT_SLAVE_FAILOVER_name);

	zval const_FAILOVER_NONE_value;
	ZVAL_LONG(&const_FAILOVER_NONE_value, REDIS_FAILOVER_NONE);
	zend_string *const_FAILOVER_NONE_name = zend_string_init_interned("FAILOVER_NONE", sizeof("FAILOVER_NONE") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_FAILOVER_NONE_name, &const_FAILOVER_NONE_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_FAILOVER_NONE_name);

	zval const_FAILOVER_ERROR_value;
	ZVAL_LONG(&const_FAILOVER_ERROR_value, REDIS_FAILOVER_ERROR);
	zend_string *const_FAILOVER_ERROR_name = zend_string_init_interned("FAILOVER_ERROR", sizeof("FAILOVER_ERROR") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_FAILOVER_ERROR_name, &const_FAILOVER_ERROR_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_FAILOVER_ERROR_name);

	zval const_FAILOVER_DISTRIBUTE_value;
	ZVAL_LONG(&const_FAILOVER_DISTRIBUTE_value, REDIS_FAILOVER_DISTRIBUTE);
	zend_string *const_FAILOVER_DISTRIBUTE_name = zend_string_init_interned("FAILOVER_DISTRIBUTE", sizeof("FAILOVER_DISTRIBUTE") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_FAILOVER_DISTRIBUTE_name, &const_FAILOVER_DISTRIBUTE_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_FAILOVER_DISTRIBUTE_name);

	zval const_FAILOVER_DISTRIBUTE_SLAVES_value;
	ZVAL_LONG(&const_FAILOVER_DISTRIBUTE_SLAVES_value, REDIS_FAILOVER_DISTRIBUTE_SLAVES);
	zend_string *const_FAILOVER_DISTRIBUTE_SLAVES_name = zend_string_init_interned("FAILOVER_DISTRIBUTE_SLAVES", sizeof("FAILOVER_DISTRIBUTE_SLAVES") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_FAILOVER_DISTRIBUTE_SLAVES_name, &const_FAILOVER_DISTRIBUTE_SLAVES_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_FAILOVER_DISTRIBUTE_SLAVES_name);

	return class_entry;
}

static zend_class_entry *register_class_ValkeyGlideClusterException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ValkeyGlideClusterException", class_ValkeyGlideClusterException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RuntimeException);

	return class_entry;
}
