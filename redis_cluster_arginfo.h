/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2faa07bd8a7767a70a90cc502fdd85076549dbba */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlideCluster___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, seeds, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_MASK(0, timeout, MAY_BE_LONG|MAY_BE_DOUBLE, "0")
	ZEND_ARG_TYPE_MASK(0, read_timeout, MAY_BE_LONG|MAY_BE_DOUBLE, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, persistent, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, auth, IS_MIXED, 0, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, context, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_append, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_bitcount, 0, 1, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, end, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, bybit, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_bitop, 0, 3, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, operation, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, deskey, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, srckey, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, otherkeys, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_bitpos, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, bit, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, end, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, bybit, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_blpop, 0, 2, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_MASK(0, timeout_or_key, MAY_BE_STRING|MAY_BE_DOUBLE|MAY_BE_LONG, NULL)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, extra_args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_brpop arginfo_class_ValkeyGlideCluster_blpop

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_brpoplpush, 0, 3, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, srckey, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, deskey, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_lMove, 0, 4, ValkeyGlide, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, wherefrom, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, whereto, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_blmove, 0, 5, ValkeyGlide, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, wherefrom, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, whereto, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_bzpopmax, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_MASK(0, timeout_or_key, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, extra_args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_bzpopmin arginfo_class_ValkeyGlideCluster_bzpopmax

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_bzmpop, 0, 3, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, from, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zmpop, 0, 2, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, from, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_blmpop arginfo_class_ValkeyGlideCluster_bzmpop

#define arginfo_class_ValkeyGlideCluster_lmpop arginfo_class_ValkeyGlideCluster_zmpop

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_clearlasterror, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_close arginfo_class_ValkeyGlideCluster_clearlasterror

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_copy, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_decr, 0, 1, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, by, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_decrby, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_del, 0, 1, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_discard arginfo_class_ValkeyGlideCluster_clearlasterror

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_dump, 0, 1, ValkeyGlideCluster, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_eval, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, script, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, args, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num_keys, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_eval_ro arginfo_class_ValkeyGlideCluster_eval

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_evalsha, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, script_sha, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, args, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num_keys, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_evalsha_ro arginfo_class_ValkeyGlideCluster_evalsha

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_exec, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_exists, 0, 1, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_touch arginfo_class_ValkeyGlideCluster_exists

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_expire, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_expireAt, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_expiretime, 0, 1, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_pexpiretime arginfo_class_ValkeyGlideCluster_expiretime

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_geoadd, 0, 4, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, lng, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, lat, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_triples_and_options, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_geodist, 0, 3, ValkeyGlideCluster, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dest, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, unit, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_geohash, 0, 2, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_members, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_geopos arginfo_class_ValkeyGlideCluster_geohash

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_georadius, 0, 5, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, lng, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, lat, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, radius, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, unit, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_georadius_ro arginfo_class_ValkeyGlideCluster_georadius

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_georadiusbymember, 0, 4, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, radius, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, unit, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_georadiusbymember_ro arginfo_class_ValkeyGlideCluster_georadiusbymember

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_geosearch, 0, 4, ValkeyGlideCluster, MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, position, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, shape, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
	ZEND_ARG_TYPE_INFO(0, unit, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_geosearchstore, 0, 5, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, position, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, shape, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
	ZEND_ARG_TYPE_INFO(0, unit, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_getDel arginfo_class_ValkeyGlideCluster_get

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_getWithMeta, 0, 1, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_getEx, 0, 1, ValkeyGlideCluster, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_getBit, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, idx, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_getlasterror, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_getmode, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_getRange, 0, 3, ValkeyGlideCluster, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_lcs, 0, 2, ValkeyGlideCluster, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_getset, 0, 2, ValkeyGlideCluster, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_hDel, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_members, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_hExists, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_hGet, 0, 2, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_hGetAll arginfo_class_ValkeyGlideCluster_getWithMeta

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_hIncrBy, 0, 3, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_hIncrByFloat, 0, 3, ValkeyGlideCluster, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_hKeys arginfo_class_ValkeyGlideCluster_getWithMeta

#define arginfo_class_ValkeyGlideCluster_hLen arginfo_class_ValkeyGlideCluster_expiretime

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_hMget, 0, 2, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_hMset, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key_values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_hscan, 0, 2, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(1, iterator, MAY_BE_NULL|MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_expiremember, 0, 3, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ttl, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, unit, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_expirememberat, 0, 3, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_hRandField, 0, 1, ValkeyGlideCluster, MAY_BE_STRING|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_hSet, 0, 3, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_hSetNx, 0, 3, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_hStrLen, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_hVals arginfo_class_ValkeyGlideCluster_getWithMeta

#define arginfo_class_ValkeyGlideCluster_incr arginfo_class_ValkeyGlideCluster_decr

#define arginfo_class_ValkeyGlideCluster_incrBy arginfo_class_ValkeyGlideCluster_decrby

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_incrByFloat, 0, 2, ValkeyGlideCluster, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_info, 0, 1, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, route, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, sections, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_keys, 0, 1, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_lget, 0, 2, ValkeyGlideCluster, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_lindex, 0, 2, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_lInsert, 0, 4, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pos, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pivot, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_lLen, 0, 1, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_lPop, 0, 1, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_STRING|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_lPos, 0, 2, ValkeyGlide, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_lPush, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_lPushx, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_lrange, 0, 3, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_lrem, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_lSet, 0, 3, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_ltrim, 0, 3, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_mget, 0, 1, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_mset, 0, 1, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key_values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_msetnx, 0, 1, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key_values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_multi, 0, 0, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_LONG, 0, "ValkeyGlide::MULTI")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_object, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, subcommand, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_persist, 0, 1, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_pexpire arginfo_class_ValkeyGlideCluster_expire

#define arginfo_class_ValkeyGlideCluster_pexpireAt arginfo_class_ValkeyGlideCluster_expireAt

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_pfadd, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, elements, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_pfcount arginfo_class_ValkeyGlideCluster_expiretime

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_pfmerge, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_ping, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, route, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, message, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_psetex, 0, 3, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_psubscribe, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, patterns, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_pttl arginfo_class_ValkeyGlideCluster_expiretime

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_publish, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, channel, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_pubsub, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, route, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_punsubscribe, 0, 1, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_patterns, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_randomKey, 0, 1, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, route, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_rename, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key_src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key_dst, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_renameNx, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, newkey, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_restore, 0, 3, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_rPop arginfo_class_ValkeyGlideCluster_lPop

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_rpoplpush, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_rPush, 0, 1, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, elements, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_rPushx, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_sAdd, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_sAddArray, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_scan, 0, 2, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_MASK(1, iterator, MAY_BE_NULL|MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, route, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_scard arginfo_class_ValkeyGlideCluster_expiretime

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_script, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, route, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_sDiff, 0, 1, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_sDiffStore, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_set, 0, 2, ValkeyGlideCluster, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_setBit, 0, 3, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, onoff, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_setex, 0, 3, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, expire, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_setnx, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_setRange, 0, 3, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_sInter, 0, 1, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_sintercard, 0, 1, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_sInterStore arginfo_class_ValkeyGlideCluster_del

#define arginfo_class_ValkeyGlideCluster_sismember arginfo_class_ValkeyGlideCluster_setnx

#define arginfo_class_ValkeyGlideCluster_sMisMember arginfo_class_ValkeyGlideCluster_geohash

#define arginfo_class_ValkeyGlideCluster_sMembers arginfo_class_ValkeyGlideCluster_getWithMeta

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_sMove, 0, 3, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_sort, 0, 1, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_sort_ro arginfo_class_ValkeyGlideCluster_sort

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_sPop, 0, 1, ValkeyGlideCluster, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_sRandMember arginfo_class_ValkeyGlideCluster_sPop

#define arginfo_class_ValkeyGlideCluster_srem arginfo_class_ValkeyGlideCluster_sAdd

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_sscan, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(1, iterator, MAY_BE_NULL|MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_strlen arginfo_class_ValkeyGlideCluster_expiretime

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_subscribe, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, channels, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, cb, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_sUnion, 0, 1, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_sUnionStore arginfo_class_ValkeyGlideCluster_sDiffStore

#define arginfo_class_ValkeyGlideCluster_ttl arginfo_class_ValkeyGlideCluster_expiretime

#define arginfo_class_ValkeyGlideCluster_type arginfo_class_ValkeyGlideCluster_expiretime

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_unsubscribe, 0, 1, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, channels, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_unlink arginfo_class_ValkeyGlideCluster_del

#define arginfo_class_ValkeyGlideCluster_unwatch arginfo_class_ValkeyGlideCluster_clearlasterror

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_watch, 0, 1, ValkeyGlideCluster, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_xack, 0, 3, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, group, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ids, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_xadd, 0, 3, ValkeyGlideCluster, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maxlen, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, approx, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_xclaim, 0, 6, ValkeyGlideCluster, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, group, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, consumer, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, min_iddle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, ids, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_xdel, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ids, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_xgroup, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, operation, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, key, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, group, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, id_or_consumer, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mkstream, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, entries_read, IS_LONG, 0, "-2")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_xautoclaim, 0, 5, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, group, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, consumer, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, min_idle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, justid, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlideCluster_xinfo, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, operation, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, arg1, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, arg2, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_xlen arginfo_class_ValkeyGlideCluster_expiretime

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_xpending, 0, 2, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, group, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, end, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, consumer, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_xrange, 0, 3, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_xread, 0, 1, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, streams, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, block, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_xreadgroup, 0, 3, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, group, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, consumer, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, streams, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, block, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_xrevrange arginfo_class_ValkeyGlideCluster_xrange

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_xtrim, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, maxlen, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, approx, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, minid, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zAdd, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, score_or_options, MAY_BE_ARRAY|MAY_BE_DOUBLE, NULL)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, more_scores_and_mems, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zCard arginfo_class_ValkeyGlideCluster_expiretime

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zCount, 0, 3, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zIncrBy, 0, 3, ValkeyGlideCluster, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zinterstore, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, weights, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, aggregate, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zintercard arginfo_class_ValkeyGlideCluster_sintercard

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zLexCount, 0, 3, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zPopMax, 0, 1, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zPopMin arginfo_class_ValkeyGlideCluster_zPopMax

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zRange, 0, 3, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_MIXED, 0)
	ZEND_ARG_TYPE_MASK(0, options, MAY_BE_ARRAY|MAY_BE_BOOL|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zrangestore, 0, 4, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, dstkey, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, srckey, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, options, MAY_BE_ARRAY|MAY_BE_BOOL|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zRandMember arginfo_class_ValkeyGlideCluster_hRandField

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zrangebylex, 0, 3, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zRangeByScore, 0, 3, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zRank, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zRem, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_values, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zRemRangeByLex arginfo_class_ValkeyGlideCluster_zLexCount

#define arginfo_class_ValkeyGlideCluster_zRemRangeByRank arginfo_class_ValkeyGlideCluster_zLexCount

#define arginfo_class_ValkeyGlideCluster_zRemRangeByScore arginfo_class_ValkeyGlideCluster_zLexCount

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zRevRange, 0, 3, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zRevRangeByLex arginfo_class_ValkeyGlideCluster_zRevRange

#define arginfo_class_ValkeyGlideCluster_zRevRangeByScore arginfo_class_ValkeyGlideCluster_zRevRange

#define arginfo_class_ValkeyGlideCluster_zrevrank arginfo_class_ValkeyGlideCluster_zRank

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zscan, 0, 2, ValkeyGlideCluster, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(1, iterator, MAY_BE_NULL|MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zScore, 0, 2, ValkeyGlideCluster, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zMscore, 0, 2, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_members, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zunionstore arginfo_class_ValkeyGlideCluster_zinterstore

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zinter, 0, 1, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, weights, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zdiffstore, 0, 2, ValkeyGlideCluster, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlideCluster_zunion arginfo_class_ValkeyGlideCluster_zinter

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlideCluster_zdiff, 0, 1, ValkeyGlideCluster, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
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
ZEND_METHOD(ValkeyGlideCluster, sAdd);
ZEND_METHOD(ValkeyGlideCluster, sAddArray);
ZEND_METHOD(ValkeyGlideCluster, scan);
ZEND_METHOD(ValkeyGlideCluster, scard);
ZEND_METHOD(ValkeyGlideCluster, script);
ZEND_METHOD(ValkeyGlideCluster, sDiff);
ZEND_METHOD(ValkeyGlideCluster, sDiffStore);
ZEND_METHOD(ValkeyGlideCluster, set);
ZEND_METHOD(ValkeyGlideCluster, setBit);
ZEND_METHOD(ValkeyGlideCluster, setex);
ZEND_METHOD(ValkeyGlideCluster, setnx);
ZEND_METHOD(ValkeyGlideCluster, setRange);
ZEND_METHOD(ValkeyGlideCluster, sInter);
ZEND_METHOD(ValkeyGlideCluster, sintercard);
ZEND_METHOD(ValkeyGlideCluster, sInterStore);
ZEND_METHOD(ValkeyGlideCluster, sismember);
ZEND_METHOD(ValkeyGlideCluster, sMisMember);
ZEND_METHOD(ValkeyGlideCluster, sMembers);
ZEND_METHOD(ValkeyGlideCluster, sMove);
ZEND_METHOD(ValkeyGlideCluster, sort);
ZEND_METHOD(ValkeyGlideCluster, sort_ro);
ZEND_METHOD(ValkeyGlideCluster, sPop);
ZEND_METHOD(ValkeyGlideCluster, sRandMember);
ZEND_METHOD(ValkeyGlideCluster, srem);
ZEND_METHOD(ValkeyGlideCluster, sscan);
ZEND_METHOD(ValkeyGlideCluster, strlen);
ZEND_METHOD(ValkeyGlideCluster, subscribe);
ZEND_METHOD(ValkeyGlideCluster, sUnion);
ZEND_METHOD(ValkeyGlideCluster, sUnionStore);
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
ZEND_METHOD(ValkeyGlideCluster, zLexCount);
ZEND_METHOD(ValkeyGlideCluster, zPopMax);
ZEND_METHOD(ValkeyGlideCluster, zPopMin);
ZEND_METHOD(ValkeyGlideCluster, zRange);
ZEND_METHOD(ValkeyGlideCluster, zrangestore);
ZEND_METHOD(ValkeyGlideCluster, zRandMember);
ZEND_METHOD(ValkeyGlideCluster, zrangebylex);
ZEND_METHOD(ValkeyGlideCluster, zRangeByScore);
ZEND_METHOD(ValkeyGlideCluster, zRank);
ZEND_METHOD(ValkeyGlideCluster, zRem);
ZEND_METHOD(ValkeyGlideCluster, zRemRangeByLex);
ZEND_METHOD(ValkeyGlideCluster, zRemRangeByRank);
ZEND_METHOD(ValkeyGlideCluster, zRemRangeByScore);
ZEND_METHOD(ValkeyGlideCluster, zRevRange);
ZEND_METHOD(ValkeyGlideCluster, zRevRangeByLex);
ZEND_METHOD(ValkeyGlideCluster, zRevRangeByScore);
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
	ZEND_ME(ValkeyGlideCluster, sAdd, arginfo_class_ValkeyGlideCluster_sAdd, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sAddArray, arginfo_class_ValkeyGlideCluster_sAddArray, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, scan, arginfo_class_ValkeyGlideCluster_scan, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, scard, arginfo_class_ValkeyGlideCluster_scard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, script, arginfo_class_ValkeyGlideCluster_script, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sDiff, arginfo_class_ValkeyGlideCluster_sDiff, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sDiffStore, arginfo_class_ValkeyGlideCluster_sDiffStore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, set, arginfo_class_ValkeyGlideCluster_set, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, setBit, arginfo_class_ValkeyGlideCluster_setBit, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, setex, arginfo_class_ValkeyGlideCluster_setex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, setnx, arginfo_class_ValkeyGlideCluster_setnx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, setRange, arginfo_class_ValkeyGlideCluster_setRange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sInter, arginfo_class_ValkeyGlideCluster_sInter, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sintercard, arginfo_class_ValkeyGlideCluster_sintercard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sInterStore, arginfo_class_ValkeyGlideCluster_sInterStore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sismember, arginfo_class_ValkeyGlideCluster_sismember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sMisMember, arginfo_class_ValkeyGlideCluster_sMisMember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sMembers, arginfo_class_ValkeyGlideCluster_sMembers, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sMove, arginfo_class_ValkeyGlideCluster_sMove, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sort, arginfo_class_ValkeyGlideCluster_sort, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sort_ro, arginfo_class_ValkeyGlideCluster_sort_ro, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sPop, arginfo_class_ValkeyGlideCluster_sPop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sRandMember, arginfo_class_ValkeyGlideCluster_sRandMember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, srem, arginfo_class_ValkeyGlideCluster_srem, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sscan, arginfo_class_ValkeyGlideCluster_sscan, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, strlen, arginfo_class_ValkeyGlideCluster_strlen, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, subscribe, arginfo_class_ValkeyGlideCluster_subscribe, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sUnion, arginfo_class_ValkeyGlideCluster_sUnion, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, sUnionStore, arginfo_class_ValkeyGlideCluster_sUnionStore, ZEND_ACC_PUBLIC)
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
	ZEND_ME(ValkeyGlideCluster, zLexCount, arginfo_class_ValkeyGlideCluster_zLexCount, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zPopMax, arginfo_class_ValkeyGlideCluster_zPopMax, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zPopMin, arginfo_class_ValkeyGlideCluster_zPopMin, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zRange, arginfo_class_ValkeyGlideCluster_zRange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrangestore, arginfo_class_ValkeyGlideCluster_zrangestore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zRandMember, arginfo_class_ValkeyGlideCluster_zRandMember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zrangebylex, arginfo_class_ValkeyGlideCluster_zrangebylex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zRangeByScore, arginfo_class_ValkeyGlideCluster_zRangeByScore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zRank, arginfo_class_ValkeyGlideCluster_zRank, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zRem, arginfo_class_ValkeyGlideCluster_zRem, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zRemRangeByLex, arginfo_class_ValkeyGlideCluster_zRemRangeByLex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zRemRangeByRank, arginfo_class_ValkeyGlideCluster_zRemRangeByRank, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zRemRangeByScore, arginfo_class_ValkeyGlideCluster_zRemRangeByScore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zRevRange, arginfo_class_ValkeyGlideCluster_zRevRange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zRevRangeByLex, arginfo_class_ValkeyGlideCluster_zRevRangeByLex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlideCluster, zRevRangeByScore, arginfo_class_ValkeyGlideCluster_zRevRangeByScore, ZEND_ACC_PUBLIC)
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
#if (PHP_VERSION_ID >= 80200)


	zend_add_parameter_attribute(zend_hash_str_find_ptr(&class_entry->function_table, "__construct", sizeof("__construct") - 1), 5, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif

	return class_entry;
}

static zend_class_entry *register_class_ValkeyGlideClusterException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ValkeyGlideClusterException", class_ValkeyGlideClusterException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RuntimeException);

	return class_entry;
}
