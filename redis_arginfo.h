/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 56189710d408b2286365ee806caaa43551619dfa */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlide___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlide___destruct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_append, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_auth, 0, 1, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, credentials, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_bitcount, 0, 1, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, end, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, bybit, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_bitop, 0, 3, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, operation, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, deskey, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, srckey, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_bitpos, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, bit, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, end, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, bybit, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_blPop, 0, 2, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, key_or_keys, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_MASK(0, timeout_or_key, MAY_BE_STRING|MAY_BE_DOUBLE|MAY_BE_LONG, NULL)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, extra_args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_brPop arginfo_class_ValkeyGlide_blPop

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_brpoplpush, 0, 3, ValkeyGlide, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, timeout, MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_bzPopMax, 0, 2, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_MASK(0, timeout_or_key, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, extra_args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_bzPopMin arginfo_class_ValkeyGlide_bzPopMax

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_bzmpop, 0, 3, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, from, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zmpop, 0, 2, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, from, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_blmpop arginfo_class_ValkeyGlide_bzmpop

#define arginfo_class_ValkeyGlide_lmpop arginfo_class_ValkeyGlide_zmpop

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_client, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, opt, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_close, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_config, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, operation, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, key_or_settings, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_copy, 0, 2, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_dbSize, 0, 0, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_decr, 0, 1, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, by, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_decrBy, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_del, 0, 1, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_delete arginfo_class_ValkeyGlide_del

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_discard, 0, 0, ValkeyGlide, MAY_BE_BOOL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_dump, 0, 1, ValkeyGlide, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_echo, 0, 1, ValkeyGlide, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_eval, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, script, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, args, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num_keys, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_eval_ro, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, script_sha, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, args, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num_keys, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_evalsha, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, sha1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, args, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num_keys, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_evalsha_ro arginfo_class_ValkeyGlide_evalsha

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_exec, 0, 0, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_exists, 0, 1, ValkeyGlide, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_expire, 0, 2, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_expireAt, 0, 2, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_expiretime, 0, 1, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_pexpiretime arginfo_class_ValkeyGlide_expiretime

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_fcall, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, fn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, keys, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, args, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_fcall_ro arginfo_class_ValkeyGlide_fcall

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_flushAll, 0, 0, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, sync, _IS_BOOL, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_flushDB arginfo_class_ValkeyGlide_flushAll

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_function, 0, 1, ValkeyGlide, MAY_BE_BOOL|MAY_BE_STRING|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, operation, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_geoadd, 0, 4, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, lng, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, lat, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_triples_and_options, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_geodist, 0, 3, ValkeyGlide, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, unit, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_geohash, 0, 2, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_members, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_geopos arginfo_class_ValkeyGlide_geohash

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_georadius, 0, 5, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, lng, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, lat, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, radius, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, unit, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_georadius_ro arginfo_class_ValkeyGlide_georadius

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_georadiusbymember, 0, 4, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, radius, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, unit, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_georadiusbymember_ro arginfo_class_ValkeyGlide_georadiusbymember

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_geosearch, 0, 4, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, position, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, shape, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
	ZEND_ARG_TYPE_INFO(0, unit, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_geosearchstore, 0, 5, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, position, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, shape, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
	ZEND_ARG_TYPE_INFO(0, unit, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_getWithMeta, 0, 1, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_getAuth, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_getBit, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, idx, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_getEx, 0, 1, ValkeyGlide, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_getDel, 0, 1, ValkeyGlide, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_getPersistentID, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_ValkeyGlide_serverName, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_serverVersion arginfo_class_ValkeyGlide_serverName

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_getRange, 0, 3, ValkeyGlide, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_lcs, 0, 2, ValkeyGlide, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_getset, 0, 2, ValkeyGlide, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_hDel, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_fields, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_hExists, 0, 2, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_hGet, 0, 2, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_hGetAll arginfo_class_ValkeyGlide_getWithMeta

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_hIncrBy, 0, 3, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_hIncrByFloat, 0, 3, ValkeyGlide, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_hKeys arginfo_class_ValkeyGlide_getWithMeta

#define arginfo_class_ValkeyGlide_hLen arginfo_class_ValkeyGlide_expiretime

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_hMget, 0, 2, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, fields, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_hMset, 0, 2, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, fieldvals, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_hRandField, 0, 1, ValkeyGlide, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_hSet, 0, 1, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, fields_and_vals, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_hSetNx, 0, 3, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_hStrLen, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_hVals arginfo_class_ValkeyGlide_getWithMeta

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_hscan, 0, 2, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(1, iterator, MAY_BE_NULL|MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_expiremember, 0, 3, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ttl, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, unit, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_expirememberat, 0, 3, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_incr arginfo_class_ValkeyGlide_decr

#define arginfo_class_ValkeyGlide_incrBy arginfo_class_ValkeyGlide_decrBy

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_incrByFloat, 0, 2, ValkeyGlide, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_info, 0, 0, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, sections, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlide_keys, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlide_lInsert, 0, 0, 4)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pos, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pivot, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_lLen arginfo_class_ValkeyGlide_expiretime

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_lMove, 0, 4, ValkeyGlide, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, wherefrom, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, whereto, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_blmove, 0, 5, ValkeyGlide, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, wherefrom, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, whereto, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_lPop, 0, 1, ValkeyGlide, MAY_BE_BOOL|MAY_BE_STRING|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_lPos, 0, 2, ValkeyGlide, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_lPush, 0, 1, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, elements, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_rPush arginfo_class_ValkeyGlide_lPush

#define arginfo_class_ValkeyGlide_lPushx arginfo_class_ValkeyGlide_append

#define arginfo_class_ValkeyGlide_rPushx arginfo_class_ValkeyGlide_append

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_lSet, 0, 3, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_lindex, 0, 2, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_lrange, 0, 3, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_lrem, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_ltrim, 0, 3, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_mget, 0, 1, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_move, 0, 2, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_mset, 0, 1, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key_values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_msetnx arginfo_class_ValkeyGlide_mset

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_multi, 0, 0, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_LONG, 0, "ValkeyGlide::MULTI")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_object, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, subcommand, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_persist, 0, 1, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_pexpire, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_pexpireAt arginfo_class_ValkeyGlide_expireAt

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_pfadd, 0, 2, ValkeyGlide, MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, elements, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_pfcount, 0, 1, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, key_or_keys, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_pfmerge, 0, 2, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, srckeys, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_ping, 0, 0, ValkeyGlide, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, message, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_pipeline, 0, 0, ValkeyGlide, MAY_BE_BOOL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_psetex, 0, 3, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, expire, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_psubscribe, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, patterns, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, cb, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_pttl arginfo_class_ValkeyGlide_expiretime

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_publish, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, channel, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_pubsub, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, command, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, arg, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_punsubscribe, 0, 1, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, patterns, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_rPop, 0, 1, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_randomKey, 0, 0, ValkeyGlide, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_rawcommand, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, command, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_rename, 0, 2, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, old_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, new_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_renameNx, 0, 2, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key_src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key_dst, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_reset arginfo_class_ValkeyGlide_discard

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_restore, 0, 3, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ttl, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_role arginfo_class_ValkeyGlide_getAuth

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_rpoplpush, 0, 2, ValkeyGlide, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, srckey, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dstkey, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_sAdd, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_sAddArray, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_sDiff, 0, 1, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_sDiffStore, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_sInter, 0, 1, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_sintercard, 0, 1, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_sInterStore arginfo_class_ValkeyGlide_del

#define arginfo_class_ValkeyGlide_sMembers arginfo_class_ValkeyGlide_getWithMeta

#define arginfo_class_ValkeyGlide_sMisMember arginfo_class_ValkeyGlide_geohash

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_sMove, 0, 3, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_sPop, 0, 1, ValkeyGlide, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_sRandMember, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_sUnion arginfo_class_ValkeyGlide_sDiff

#define arginfo_class_ValkeyGlide_sUnionStore arginfo_class_ValkeyGlide_sDiffStore

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_ValkeyGlide_scan, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(1, iterator, MAY_BE_NULL|MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_scard arginfo_class_ValkeyGlide_expiretime

#define arginfo_class_ValkeyGlide_script arginfo_class_ValkeyGlide_rawcommand

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_select, 0, 1, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, db, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_set, 0, 2, ValkeyGlide, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_setBit, 0, 3, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, idx, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_setRange, 0, 3, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ValkeyGlide_setex, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, expire, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_setnx, 0, 2, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_sismember arginfo_class_ValkeyGlide_setnx

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_touch, 0, 1, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, key_or_array, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, more_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_sort, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_sort_ro arginfo_class_ValkeyGlide_sort

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_sortAsc, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, get, IS_MIXED, 0, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, store, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_sortAscAlpha arginfo_class_ValkeyGlide_sortAsc

#define arginfo_class_ValkeyGlide_sortDesc arginfo_class_ValkeyGlide_sortAsc

#define arginfo_class_ValkeyGlide_sortDescAlpha arginfo_class_ValkeyGlide_sortAsc

#define arginfo_class_ValkeyGlide_srem arginfo_class_ValkeyGlide_sAdd

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_ValkeyGlide_sscan, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(1, iterator, MAY_BE_NULL|MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_ssubscribe, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, channels, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, cb, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_strlen arginfo_class_ValkeyGlide_expiretime

#define arginfo_class_ValkeyGlide_subscribe arginfo_class_ValkeyGlide_ssubscribe

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_sunsubscribe, 0, 1, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, channels, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_swapdb, 0, 2, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, src, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_time, 0, 0, ValkeyGlide, MAY_BE_ARRAY)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_ttl arginfo_class_ValkeyGlide_expiretime

#define arginfo_class_ValkeyGlide_type arginfo_class_ValkeyGlide_expiretime

#define arginfo_class_ValkeyGlide_unlink arginfo_class_ValkeyGlide_del

#define arginfo_class_ValkeyGlide_unsubscribe arginfo_class_ValkeyGlide_sunsubscribe

#define arginfo_class_ValkeyGlide_unwatch arginfo_class_ValkeyGlide_discard

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_watch, 0, 1, ValkeyGlide, MAY_BE_BOOL)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_ValkeyGlide_wait, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, numreplicas, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_ValkeyGlide_xack, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, group, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ids, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_xadd, 0, 3, ValkeyGlide, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maxlen, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, approx, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, nomkstream, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_xautoclaim, 0, 5, ValkeyGlide, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, group, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, consumer, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, min_idle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, justid, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_xclaim, 0, 6, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, group, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, consumer, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, min_idle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, ids, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_xdel, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ids, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_xgroup, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, operation, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, key, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, group, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, id_or_consumer, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mkstream, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, entries_read, IS_LONG, 0, "-2")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ValkeyGlide_xinfo, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, operation, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, arg1, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, arg2, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_xlen arginfo_class_ValkeyGlide_expiretime

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_xpending, 0, 2, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, group, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, end, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, consumer, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_xrange, 0, 3, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_xread, 0, 1, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, streams, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, block, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_xreadgroup, 0, 3, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, group, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, consumer, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, streams, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, block, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_xrevrange, 0, 3, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_xtrim, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, threshold, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, approx, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, minid, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zAdd, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, score_or_options, MAY_BE_ARRAY|MAY_BE_DOUBLE, NULL)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, more_scores_and_mems, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_zCard arginfo_class_ValkeyGlide_expiretime

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zCount, 0, 3, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, start, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, end, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zIncrBy, 0, 3, ValkeyGlide, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zLexCount, 0, 3, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zMscore, 0, 2, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_members, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zPopMax, 0, 1, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_zPopMin arginfo_class_ValkeyGlide_zPopMax

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zRange, 0, 3, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, start, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_MASK(0, end, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_MASK(0, options, MAY_BE_ARRAY|MAY_BE_BOOL|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zRangeByLex, 0, 3, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zRangeByScore, 0, 3, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zrangestore, 0, 4, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, dstkey, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, srckey, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, options, MAY_BE_ARRAY|MAY_BE_BOOL|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zRandMember, 0, 1, ValkeyGlide, MAY_BE_STRING|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zRank, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zRem, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_members, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_zRemRangeByLex arginfo_class_ValkeyGlide_zLexCount

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zRemRangeByRank, 0, 3, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zRemRangeByScore, 0, 3, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zRevRange, 0, 3, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scores, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zRevRangeByLex, 0, 3, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zRevRangeByScore, 0, 3, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, options, MAY_BE_ARRAY|MAY_BE_BOOL, "[]")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_zRevRank arginfo_class_ValkeyGlide_zRank

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zScore, 0, 2, ValkeyGlide, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zdiff, 0, 1, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zdiffstore, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zinter, 0, 1, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, weights, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_zintercard arginfo_class_ValkeyGlide_sintercard

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zinterstore, 0, 2, ValkeyGlide, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, weights, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, aggregate, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ValkeyGlide_zscan, 0, 2, ValkeyGlide, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(1, iterator, MAY_BE_NULL|MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_ValkeyGlide_zunion arginfo_class_ValkeyGlide_zinter

#define arginfo_class_ValkeyGlide_zunionstore arginfo_class_ValkeyGlide_zinterstore


ZEND_METHOD(ValkeyGlide, __construct);
ZEND_METHOD(ValkeyGlide, __destruct);
ZEND_METHOD(ValkeyGlide, append);
ZEND_METHOD(ValkeyGlide, auth);
ZEND_METHOD(ValkeyGlide, bitcount);
ZEND_METHOD(ValkeyGlide, bitop);
ZEND_METHOD(ValkeyGlide, bitpos);
ZEND_METHOD(ValkeyGlide, blPop);
ZEND_METHOD(ValkeyGlide, brPop);
ZEND_METHOD(ValkeyGlide, brpoplpush);
ZEND_METHOD(ValkeyGlide, bzPopMax);
ZEND_METHOD(ValkeyGlide, bzPopMin);
ZEND_METHOD(ValkeyGlide, bzmpop);
ZEND_METHOD(ValkeyGlide, zmpop);
ZEND_METHOD(ValkeyGlide, blmpop);
ZEND_METHOD(ValkeyGlide, lmpop);
ZEND_METHOD(ValkeyGlide, client);
ZEND_METHOD(ValkeyGlide, close);
ZEND_METHOD(ValkeyGlide, config);
ZEND_METHOD(ValkeyGlide, copy);
ZEND_METHOD(ValkeyGlide, dbSize);
ZEND_METHOD(ValkeyGlide, decr);
ZEND_METHOD(ValkeyGlide, decrBy);
ZEND_METHOD(ValkeyGlide, del);
ZEND_METHOD(ValkeyGlide, discard);
ZEND_METHOD(ValkeyGlide, dump);
ZEND_METHOD(ValkeyGlide, echo);
ZEND_METHOD(ValkeyGlide, eval);
ZEND_METHOD(ValkeyGlide, eval_ro);
ZEND_METHOD(ValkeyGlide, evalsha);
ZEND_METHOD(ValkeyGlide, evalsha_ro);
ZEND_METHOD(ValkeyGlide, exec);
ZEND_METHOD(ValkeyGlide, exists);
ZEND_METHOD(ValkeyGlide, expire);
ZEND_METHOD(ValkeyGlide, expireAt);
ZEND_METHOD(ValkeyGlide, expiretime);
ZEND_METHOD(ValkeyGlide, pexpiretime);
ZEND_METHOD(ValkeyGlide, fcall);
ZEND_METHOD(ValkeyGlide, fcall_ro);
ZEND_METHOD(ValkeyGlide, flushAll);
ZEND_METHOD(ValkeyGlide, flushDB);
ZEND_METHOD(ValkeyGlide, function);
ZEND_METHOD(ValkeyGlide, geoadd);
ZEND_METHOD(ValkeyGlide, geodist);
ZEND_METHOD(ValkeyGlide, geohash);
ZEND_METHOD(ValkeyGlide, geopos);
ZEND_METHOD(ValkeyGlide, georadius);
ZEND_METHOD(ValkeyGlide, georadius_ro);
ZEND_METHOD(ValkeyGlide, georadiusbymember);
ZEND_METHOD(ValkeyGlide, georadiusbymember_ro);
ZEND_METHOD(ValkeyGlide, geosearch);
ZEND_METHOD(ValkeyGlide, geosearchstore);
ZEND_METHOD(ValkeyGlide, get);
ZEND_METHOD(ValkeyGlide, getWithMeta);
ZEND_METHOD(ValkeyGlide, getAuth);
ZEND_METHOD(ValkeyGlide, getBit);
ZEND_METHOD(ValkeyGlide, getEx);
ZEND_METHOD(ValkeyGlide, getDel);
ZEND_METHOD(ValkeyGlide, getPersistentID);
ZEND_METHOD(ValkeyGlide, serverName);
ZEND_METHOD(ValkeyGlide, serverVersion);
ZEND_METHOD(ValkeyGlide, getRange);
ZEND_METHOD(ValkeyGlide, lcs);
ZEND_METHOD(ValkeyGlide, getset);
ZEND_METHOD(ValkeyGlide, hDel);
ZEND_METHOD(ValkeyGlide, hExists);
ZEND_METHOD(ValkeyGlide, hGet);
ZEND_METHOD(ValkeyGlide, hGetAll);
ZEND_METHOD(ValkeyGlide, hIncrBy);
ZEND_METHOD(ValkeyGlide, hIncrByFloat);
ZEND_METHOD(ValkeyGlide, hKeys);
ZEND_METHOD(ValkeyGlide, hLen);
ZEND_METHOD(ValkeyGlide, hMget);
ZEND_METHOD(ValkeyGlide, hMset);
ZEND_METHOD(ValkeyGlide, hRandField);
ZEND_METHOD(ValkeyGlide, hSet);
ZEND_METHOD(ValkeyGlide, hSetNx);
ZEND_METHOD(ValkeyGlide, hStrLen);
ZEND_METHOD(ValkeyGlide, hVals);
ZEND_METHOD(ValkeyGlide, hscan);
ZEND_METHOD(ValkeyGlide, expiremember);
ZEND_METHOD(ValkeyGlide, expirememberat);
ZEND_METHOD(ValkeyGlide, incr);
ZEND_METHOD(ValkeyGlide, incrBy);
ZEND_METHOD(ValkeyGlide, incrByFloat);
ZEND_METHOD(ValkeyGlide, info);
ZEND_METHOD(ValkeyGlide, keys);
ZEND_METHOD(ValkeyGlide, lInsert);
ZEND_METHOD(ValkeyGlide, lLen);
ZEND_METHOD(ValkeyGlide, lMove);
ZEND_METHOD(ValkeyGlide, blmove);
ZEND_METHOD(ValkeyGlide, lPop);
ZEND_METHOD(ValkeyGlide, lPos);
ZEND_METHOD(ValkeyGlide, lPush);
ZEND_METHOD(ValkeyGlide, rPush);
ZEND_METHOD(ValkeyGlide, lPushx);
ZEND_METHOD(ValkeyGlide, rPushx);
ZEND_METHOD(ValkeyGlide, lSet);
ZEND_METHOD(ValkeyGlide, lindex);
ZEND_METHOD(ValkeyGlide, lrange);
ZEND_METHOD(ValkeyGlide, lrem);
ZEND_METHOD(ValkeyGlide, ltrim);
ZEND_METHOD(ValkeyGlide, mget);
ZEND_METHOD(ValkeyGlide, move);
ZEND_METHOD(ValkeyGlide, mset);
ZEND_METHOD(ValkeyGlide, msetnx);
ZEND_METHOD(ValkeyGlide, multi);
ZEND_METHOD(ValkeyGlide, object);
ZEND_METHOD(ValkeyGlide, persist);
ZEND_METHOD(ValkeyGlide, pexpire);
ZEND_METHOD(ValkeyGlide, pexpireAt);
ZEND_METHOD(ValkeyGlide, pfadd);
ZEND_METHOD(ValkeyGlide, pfcount);
ZEND_METHOD(ValkeyGlide, pfmerge);
ZEND_METHOD(ValkeyGlide, ping);
ZEND_METHOD(ValkeyGlide, pipeline);
ZEND_METHOD(ValkeyGlide, psetex);
ZEND_METHOD(ValkeyGlide, psubscribe);
ZEND_METHOD(ValkeyGlide, pttl);
ZEND_METHOD(ValkeyGlide, publish);
ZEND_METHOD(ValkeyGlide, pubsub);
ZEND_METHOD(ValkeyGlide, punsubscribe);
ZEND_METHOD(ValkeyGlide, rPop);
ZEND_METHOD(ValkeyGlide, randomKey);
ZEND_METHOD(ValkeyGlide, rawcommand);
ZEND_METHOD(ValkeyGlide, rename);
ZEND_METHOD(ValkeyGlide, renameNx);
ZEND_METHOD(ValkeyGlide, reset);
ZEND_METHOD(ValkeyGlide, restore);
ZEND_METHOD(ValkeyGlide, role);
ZEND_METHOD(ValkeyGlide, rpoplpush);
ZEND_METHOD(ValkeyGlide, sAdd);
ZEND_METHOD(ValkeyGlide, sAddArray);
ZEND_METHOD(ValkeyGlide, sDiff);
ZEND_METHOD(ValkeyGlide, sDiffStore);
ZEND_METHOD(ValkeyGlide, sInter);
ZEND_METHOD(ValkeyGlide, sintercard);
ZEND_METHOD(ValkeyGlide, sInterStore);
ZEND_METHOD(ValkeyGlide, sMembers);
ZEND_METHOD(ValkeyGlide, sMisMember);
ZEND_METHOD(ValkeyGlide, sMove);
ZEND_METHOD(ValkeyGlide, sPop);
ZEND_METHOD(ValkeyGlide, sRandMember);
ZEND_METHOD(ValkeyGlide, sUnion);
ZEND_METHOD(ValkeyGlide, sUnionStore);
ZEND_METHOD(ValkeyGlide, scan);
ZEND_METHOD(ValkeyGlide, scard);
ZEND_METHOD(ValkeyGlide, script);
ZEND_METHOD(ValkeyGlide, select);
ZEND_METHOD(ValkeyGlide, set);
ZEND_METHOD(ValkeyGlide, setBit);
ZEND_METHOD(ValkeyGlide, setRange);
ZEND_METHOD(ValkeyGlide, setex);
ZEND_METHOD(ValkeyGlide, setnx);
ZEND_METHOD(ValkeyGlide, sismember);
ZEND_METHOD(ValkeyGlide, touch);
ZEND_METHOD(ValkeyGlide, sort);
ZEND_METHOD(ValkeyGlide, sort_ro);
ZEND_METHOD(ValkeyGlide, sortAsc);
ZEND_METHOD(ValkeyGlide, sortAscAlpha);
ZEND_METHOD(ValkeyGlide, sortDesc);
ZEND_METHOD(ValkeyGlide, sortDescAlpha);
ZEND_METHOD(ValkeyGlide, srem);
ZEND_METHOD(ValkeyGlide, sscan);
ZEND_METHOD(ValkeyGlide, ssubscribe);
ZEND_METHOD(ValkeyGlide, strlen);
ZEND_METHOD(ValkeyGlide, subscribe);
ZEND_METHOD(ValkeyGlide, sunsubscribe);
ZEND_METHOD(ValkeyGlide, swapdb);
ZEND_METHOD(ValkeyGlide, time);
ZEND_METHOD(ValkeyGlide, ttl);
ZEND_METHOD(ValkeyGlide, type);
ZEND_METHOD(ValkeyGlide, unlink);
ZEND_METHOD(ValkeyGlide, unsubscribe);
ZEND_METHOD(ValkeyGlide, unwatch);
ZEND_METHOD(ValkeyGlide, watch);
ZEND_METHOD(ValkeyGlide, wait);
ZEND_METHOD(ValkeyGlide, xack);
ZEND_METHOD(ValkeyGlide, xadd);
ZEND_METHOD(ValkeyGlide, xautoclaim);
ZEND_METHOD(ValkeyGlide, xclaim);
ZEND_METHOD(ValkeyGlide, xdel);
ZEND_METHOD(ValkeyGlide, xgroup);
ZEND_METHOD(ValkeyGlide, xinfo);
ZEND_METHOD(ValkeyGlide, xlen);
ZEND_METHOD(ValkeyGlide, xpending);
ZEND_METHOD(ValkeyGlide, xrange);
ZEND_METHOD(ValkeyGlide, xread);
ZEND_METHOD(ValkeyGlide, xreadgroup);
ZEND_METHOD(ValkeyGlide, xrevrange);
ZEND_METHOD(ValkeyGlide, xtrim);
ZEND_METHOD(ValkeyGlide, zAdd);
ZEND_METHOD(ValkeyGlide, zCard);
ZEND_METHOD(ValkeyGlide, zCount);
ZEND_METHOD(ValkeyGlide, zIncrBy);
ZEND_METHOD(ValkeyGlide, zLexCount);
ZEND_METHOD(ValkeyGlide, zMscore);
ZEND_METHOD(ValkeyGlide, zPopMax);
ZEND_METHOD(ValkeyGlide, zPopMin);
ZEND_METHOD(ValkeyGlide, zRange);
ZEND_METHOD(ValkeyGlide, zRangeByLex);
ZEND_METHOD(ValkeyGlide, zRangeByScore);
ZEND_METHOD(ValkeyGlide, zrangestore);
ZEND_METHOD(ValkeyGlide, zRandMember);
ZEND_METHOD(ValkeyGlide, zRank);
ZEND_METHOD(ValkeyGlide, zRem);
ZEND_METHOD(ValkeyGlide, zRemRangeByLex);
ZEND_METHOD(ValkeyGlide, zRemRangeByRank);
ZEND_METHOD(ValkeyGlide, zRemRangeByScore);
ZEND_METHOD(ValkeyGlide, zRevRange);
ZEND_METHOD(ValkeyGlide, zRevRangeByLex);
ZEND_METHOD(ValkeyGlide, zRevRangeByScore);
ZEND_METHOD(ValkeyGlide, zRevRank);
ZEND_METHOD(ValkeyGlide, zScore);
ZEND_METHOD(ValkeyGlide, zdiff);
ZEND_METHOD(ValkeyGlide, zdiffstore);
ZEND_METHOD(ValkeyGlide, zinter);
ZEND_METHOD(ValkeyGlide, zintercard);
ZEND_METHOD(ValkeyGlide, zinterstore);
ZEND_METHOD(ValkeyGlide, zscan);
ZEND_METHOD(ValkeyGlide, zunion);
ZEND_METHOD(ValkeyGlide, zunionstore);


static const zend_function_entry class_ValkeyGlide_methods[] = {
	ZEND_ME(ValkeyGlide, __construct, arginfo_class_ValkeyGlide___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, __destruct, arginfo_class_ValkeyGlide___destruct, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, append, arginfo_class_ValkeyGlide_append, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, auth, arginfo_class_ValkeyGlide_auth, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, bitcount, arginfo_class_ValkeyGlide_bitcount, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, bitop, arginfo_class_ValkeyGlide_bitop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, bitpos, arginfo_class_ValkeyGlide_bitpos, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, blPop, arginfo_class_ValkeyGlide_blPop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, brPop, arginfo_class_ValkeyGlide_brPop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, brpoplpush, arginfo_class_ValkeyGlide_brpoplpush, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, bzPopMax, arginfo_class_ValkeyGlide_bzPopMax, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, bzPopMin, arginfo_class_ValkeyGlide_bzPopMin, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, bzmpop, arginfo_class_ValkeyGlide_bzmpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zmpop, arginfo_class_ValkeyGlide_zmpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, blmpop, arginfo_class_ValkeyGlide_blmpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, lmpop, arginfo_class_ValkeyGlide_lmpop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, client, arginfo_class_ValkeyGlide_client, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, close, arginfo_class_ValkeyGlide_close, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, config, arginfo_class_ValkeyGlide_config, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, copy, arginfo_class_ValkeyGlide_copy, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, dbSize, arginfo_class_ValkeyGlide_dbSize, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, decr, arginfo_class_ValkeyGlide_decr, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, decrBy, arginfo_class_ValkeyGlide_decrBy, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, del, arginfo_class_ValkeyGlide_del, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ValkeyGlide, delete, del, arginfo_class_ValkeyGlide_delete, ZEND_ACC_PUBLIC|ZEND_ACC_DEPRECATED)
	ZEND_ME(ValkeyGlide, discard, arginfo_class_ValkeyGlide_discard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, dump, arginfo_class_ValkeyGlide_dump, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, echo, arginfo_class_ValkeyGlide_echo, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, eval, arginfo_class_ValkeyGlide_eval, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, eval_ro, arginfo_class_ValkeyGlide_eval_ro, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, evalsha, arginfo_class_ValkeyGlide_evalsha, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, evalsha_ro, arginfo_class_ValkeyGlide_evalsha_ro, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, exec, arginfo_class_ValkeyGlide_exec, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, exists, arginfo_class_ValkeyGlide_exists, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, expire, arginfo_class_ValkeyGlide_expire, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, expireAt, arginfo_class_ValkeyGlide_expireAt, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, expiretime, arginfo_class_ValkeyGlide_expiretime, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, pexpiretime, arginfo_class_ValkeyGlide_pexpiretime, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, fcall, arginfo_class_ValkeyGlide_fcall, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, fcall_ro, arginfo_class_ValkeyGlide_fcall_ro, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, flushAll, arginfo_class_ValkeyGlide_flushAll, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, flushDB, arginfo_class_ValkeyGlide_flushDB, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, function, arginfo_class_ValkeyGlide_function, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, geoadd, arginfo_class_ValkeyGlide_geoadd, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, geodist, arginfo_class_ValkeyGlide_geodist, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, geohash, arginfo_class_ValkeyGlide_geohash, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, geopos, arginfo_class_ValkeyGlide_geopos, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, georadius, arginfo_class_ValkeyGlide_georadius, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, georadius_ro, arginfo_class_ValkeyGlide_georadius_ro, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, georadiusbymember, arginfo_class_ValkeyGlide_georadiusbymember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, georadiusbymember_ro, arginfo_class_ValkeyGlide_georadiusbymember_ro, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, geosearch, arginfo_class_ValkeyGlide_geosearch, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, geosearchstore, arginfo_class_ValkeyGlide_geosearchstore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, get, arginfo_class_ValkeyGlide_get, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, getWithMeta, arginfo_class_ValkeyGlide_getWithMeta, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, getAuth, arginfo_class_ValkeyGlide_getAuth, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, getBit, arginfo_class_ValkeyGlide_getBit, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, getEx, arginfo_class_ValkeyGlide_getEx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, getDel, arginfo_class_ValkeyGlide_getDel, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, getPersistentID, arginfo_class_ValkeyGlide_getPersistentID, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, serverName, arginfo_class_ValkeyGlide_serverName, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, serverVersion, arginfo_class_ValkeyGlide_serverVersion, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, getRange, arginfo_class_ValkeyGlide_getRange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, lcs, arginfo_class_ValkeyGlide_lcs, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, getset, arginfo_class_ValkeyGlide_getset, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hDel, arginfo_class_ValkeyGlide_hDel, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hExists, arginfo_class_ValkeyGlide_hExists, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hGet, arginfo_class_ValkeyGlide_hGet, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hGetAll, arginfo_class_ValkeyGlide_hGetAll, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hIncrBy, arginfo_class_ValkeyGlide_hIncrBy, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hIncrByFloat, arginfo_class_ValkeyGlide_hIncrByFloat, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hKeys, arginfo_class_ValkeyGlide_hKeys, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hLen, arginfo_class_ValkeyGlide_hLen, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hMget, arginfo_class_ValkeyGlide_hMget, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hMset, arginfo_class_ValkeyGlide_hMset, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hRandField, arginfo_class_ValkeyGlide_hRandField, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hSet, arginfo_class_ValkeyGlide_hSet, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hSetNx, arginfo_class_ValkeyGlide_hSetNx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hStrLen, arginfo_class_ValkeyGlide_hStrLen, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hVals, arginfo_class_ValkeyGlide_hVals, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, hscan, arginfo_class_ValkeyGlide_hscan, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, expiremember, arginfo_class_ValkeyGlide_expiremember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, expirememberat, arginfo_class_ValkeyGlide_expirememberat, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, incr, arginfo_class_ValkeyGlide_incr, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, incrBy, arginfo_class_ValkeyGlide_incrBy, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, incrByFloat, arginfo_class_ValkeyGlide_incrByFloat, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, info, arginfo_class_ValkeyGlide_info, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, keys, arginfo_class_ValkeyGlide_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, lInsert, arginfo_class_ValkeyGlide_lInsert, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, lLen, arginfo_class_ValkeyGlide_lLen, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, lMove, arginfo_class_ValkeyGlide_lMove, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, blmove, arginfo_class_ValkeyGlide_blmove, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, lPop, arginfo_class_ValkeyGlide_lPop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, lPos, arginfo_class_ValkeyGlide_lPos, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, lPush, arginfo_class_ValkeyGlide_lPush, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, rPush, arginfo_class_ValkeyGlide_rPush, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, lPushx, arginfo_class_ValkeyGlide_lPushx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, rPushx, arginfo_class_ValkeyGlide_rPushx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, lSet, arginfo_class_ValkeyGlide_lSet, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, lindex, arginfo_class_ValkeyGlide_lindex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, lrange, arginfo_class_ValkeyGlide_lrange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, lrem, arginfo_class_ValkeyGlide_lrem, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, ltrim, arginfo_class_ValkeyGlide_ltrim, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, mget, arginfo_class_ValkeyGlide_mget, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, move, arginfo_class_ValkeyGlide_move, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, mset, arginfo_class_ValkeyGlide_mset, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, msetnx, arginfo_class_ValkeyGlide_msetnx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, multi, arginfo_class_ValkeyGlide_multi, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, object, arginfo_class_ValkeyGlide_object, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, persist, arginfo_class_ValkeyGlide_persist, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, pexpire, arginfo_class_ValkeyGlide_pexpire, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, pexpireAt, arginfo_class_ValkeyGlide_pexpireAt, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, pfadd, arginfo_class_ValkeyGlide_pfadd, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, pfcount, arginfo_class_ValkeyGlide_pfcount, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, pfmerge, arginfo_class_ValkeyGlide_pfmerge, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, ping, arginfo_class_ValkeyGlide_ping, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, pipeline, arginfo_class_ValkeyGlide_pipeline, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, psetex, arginfo_class_ValkeyGlide_psetex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, psubscribe, arginfo_class_ValkeyGlide_psubscribe, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, pttl, arginfo_class_ValkeyGlide_pttl, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, publish, arginfo_class_ValkeyGlide_publish, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, pubsub, arginfo_class_ValkeyGlide_pubsub, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, punsubscribe, arginfo_class_ValkeyGlide_punsubscribe, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, rPop, arginfo_class_ValkeyGlide_rPop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, randomKey, arginfo_class_ValkeyGlide_randomKey, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, rawcommand, arginfo_class_ValkeyGlide_rawcommand, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, rename, arginfo_class_ValkeyGlide_rename, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, renameNx, arginfo_class_ValkeyGlide_renameNx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, reset, arginfo_class_ValkeyGlide_reset, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, restore, arginfo_class_ValkeyGlide_restore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, role, arginfo_class_ValkeyGlide_role, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, rpoplpush, arginfo_class_ValkeyGlide_rpoplpush, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sAdd, arginfo_class_ValkeyGlide_sAdd, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sAddArray, arginfo_class_ValkeyGlide_sAddArray, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sDiff, arginfo_class_ValkeyGlide_sDiff, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sDiffStore, arginfo_class_ValkeyGlide_sDiffStore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sInter, arginfo_class_ValkeyGlide_sInter, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sintercard, arginfo_class_ValkeyGlide_sintercard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sInterStore, arginfo_class_ValkeyGlide_sInterStore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sMembers, arginfo_class_ValkeyGlide_sMembers, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sMisMember, arginfo_class_ValkeyGlide_sMisMember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sMove, arginfo_class_ValkeyGlide_sMove, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sPop, arginfo_class_ValkeyGlide_sPop, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sRandMember, arginfo_class_ValkeyGlide_sRandMember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sUnion, arginfo_class_ValkeyGlide_sUnion, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sUnionStore, arginfo_class_ValkeyGlide_sUnionStore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, scan, arginfo_class_ValkeyGlide_scan, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, scard, arginfo_class_ValkeyGlide_scard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, script, arginfo_class_ValkeyGlide_script, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, select, arginfo_class_ValkeyGlide_select, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, set, arginfo_class_ValkeyGlide_set, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, setBit, arginfo_class_ValkeyGlide_setBit, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, setRange, arginfo_class_ValkeyGlide_setRange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, setex, arginfo_class_ValkeyGlide_setex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, setnx, arginfo_class_ValkeyGlide_setnx, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sismember, arginfo_class_ValkeyGlide_sismember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, touch, arginfo_class_ValkeyGlide_touch, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sort, arginfo_class_ValkeyGlide_sort, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sort_ro, arginfo_class_ValkeyGlide_sort_ro, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sortAsc, arginfo_class_ValkeyGlide_sortAsc, ZEND_ACC_PUBLIC|ZEND_ACC_DEPRECATED)
	ZEND_ME(ValkeyGlide, sortAscAlpha, arginfo_class_ValkeyGlide_sortAscAlpha, ZEND_ACC_PUBLIC|ZEND_ACC_DEPRECATED)
	ZEND_ME(ValkeyGlide, sortDesc, arginfo_class_ValkeyGlide_sortDesc, ZEND_ACC_PUBLIC|ZEND_ACC_DEPRECATED)
	ZEND_ME(ValkeyGlide, sortDescAlpha, arginfo_class_ValkeyGlide_sortDescAlpha, ZEND_ACC_PUBLIC|ZEND_ACC_DEPRECATED)
	ZEND_ME(ValkeyGlide, srem, arginfo_class_ValkeyGlide_srem, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sscan, arginfo_class_ValkeyGlide_sscan, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, ssubscribe, arginfo_class_ValkeyGlide_ssubscribe, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, strlen, arginfo_class_ValkeyGlide_strlen, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, subscribe, arginfo_class_ValkeyGlide_subscribe, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, sunsubscribe, arginfo_class_ValkeyGlide_sunsubscribe, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, swapdb, arginfo_class_ValkeyGlide_swapdb, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, time, arginfo_class_ValkeyGlide_time, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, ttl, arginfo_class_ValkeyGlide_ttl, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, type, arginfo_class_ValkeyGlide_type, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, unlink, arginfo_class_ValkeyGlide_unlink, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, unsubscribe, arginfo_class_ValkeyGlide_unsubscribe, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, unwatch, arginfo_class_ValkeyGlide_unwatch, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, watch, arginfo_class_ValkeyGlide_watch, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, wait, arginfo_class_ValkeyGlide_wait, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, xack, arginfo_class_ValkeyGlide_xack, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, xadd, arginfo_class_ValkeyGlide_xadd, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, xautoclaim, arginfo_class_ValkeyGlide_xautoclaim, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, xclaim, arginfo_class_ValkeyGlide_xclaim, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, xdel, arginfo_class_ValkeyGlide_xdel, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, xgroup, arginfo_class_ValkeyGlide_xgroup, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, xinfo, arginfo_class_ValkeyGlide_xinfo, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, xlen, arginfo_class_ValkeyGlide_xlen, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, xpending, arginfo_class_ValkeyGlide_xpending, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, xrange, arginfo_class_ValkeyGlide_xrange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, xread, arginfo_class_ValkeyGlide_xread, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, xreadgroup, arginfo_class_ValkeyGlide_xreadgroup, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, xrevrange, arginfo_class_ValkeyGlide_xrevrange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, xtrim, arginfo_class_ValkeyGlide_xtrim, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zAdd, arginfo_class_ValkeyGlide_zAdd, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zCard, arginfo_class_ValkeyGlide_zCard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zCount, arginfo_class_ValkeyGlide_zCount, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zIncrBy, arginfo_class_ValkeyGlide_zIncrBy, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zLexCount, arginfo_class_ValkeyGlide_zLexCount, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zMscore, arginfo_class_ValkeyGlide_zMscore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zPopMax, arginfo_class_ValkeyGlide_zPopMax, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zPopMin, arginfo_class_ValkeyGlide_zPopMin, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zRange, arginfo_class_ValkeyGlide_zRange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zRangeByLex, arginfo_class_ValkeyGlide_zRangeByLex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zRangeByScore, arginfo_class_ValkeyGlide_zRangeByScore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zrangestore, arginfo_class_ValkeyGlide_zrangestore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zRandMember, arginfo_class_ValkeyGlide_zRandMember, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zRank, arginfo_class_ValkeyGlide_zRank, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zRem, arginfo_class_ValkeyGlide_zRem, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zRemRangeByLex, arginfo_class_ValkeyGlide_zRemRangeByLex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zRemRangeByRank, arginfo_class_ValkeyGlide_zRemRangeByRank, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zRemRangeByScore, arginfo_class_ValkeyGlide_zRemRangeByScore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zRevRange, arginfo_class_ValkeyGlide_zRevRange, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zRevRangeByLex, arginfo_class_ValkeyGlide_zRevRangeByLex, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zRevRangeByScore, arginfo_class_ValkeyGlide_zRevRangeByScore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zRevRank, arginfo_class_ValkeyGlide_zRevRank, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zScore, arginfo_class_ValkeyGlide_zScore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zdiff, arginfo_class_ValkeyGlide_zdiff, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zdiffstore, arginfo_class_ValkeyGlide_zdiffstore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zinter, arginfo_class_ValkeyGlide_zinter, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zintercard, arginfo_class_ValkeyGlide_zintercard, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zinterstore, arginfo_class_ValkeyGlide_zinterstore, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zscan, arginfo_class_ValkeyGlide_zscan, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zunion, arginfo_class_ValkeyGlide_zunion, ZEND_ACC_PUBLIC)
	ZEND_ME(ValkeyGlide, zunionstore, arginfo_class_ValkeyGlide_zunionstore, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ValkeyGlideException_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_ValkeyGlide(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ValkeyGlide", class_ValkeyGlide_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zval const_VALKEY_GLIDE_NOT_FOUND_value;
	ZVAL_LONG(&const_VALKEY_GLIDE_NOT_FOUND_value, VALKEY_GLIDE_NOT_FOUND);
	zend_string *const_VALKEY_GLIDE_NOT_FOUND_name = zend_string_init_interned("VALKEY_GLIDE_NOT_FOUND", sizeof("VALKEY_GLIDE_NOT_FOUND") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_VALKEY_GLIDE_NOT_FOUND_name, &const_VALKEY_GLIDE_NOT_FOUND_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_VALKEY_GLIDE_NOT_FOUND_name);

	zval const_VALKEY_GLIDE_STRING_value;
	ZVAL_LONG(&const_VALKEY_GLIDE_STRING_value, VALKEY_GLIDE_STRING);
	zend_string *const_VALKEY_GLIDE_STRING_name = zend_string_init_interned("VALKEY_GLIDE_STRING", sizeof("VALKEY_GLIDE_STRING") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_VALKEY_GLIDE_STRING_name, &const_VALKEY_GLIDE_STRING_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_VALKEY_GLIDE_STRING_name);

	zval const_VALKEY_GLIDE_SET_value;
	ZVAL_LONG(&const_VALKEY_GLIDE_SET_value, VALKEY_GLIDE_SET);
	zend_string *const_VALKEY_GLIDE_SET_name = zend_string_init_interned("VALKEY_GLIDE_SET", sizeof("VALKEY_GLIDE_SET") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_VALKEY_GLIDE_SET_name, &const_VALKEY_GLIDE_SET_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_VALKEY_GLIDE_SET_name);

	zval const_VALKEY_GLIDE_LIST_value;
	ZVAL_LONG(&const_VALKEY_GLIDE_LIST_value, VALKEY_GLIDE_LIST);
	zend_string *const_VALKEY_GLIDE_LIST_name = zend_string_init_interned("VALKEY_GLIDE_LIST", sizeof("VALKEY_GLIDE_LIST") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_VALKEY_GLIDE_LIST_name, &const_VALKEY_GLIDE_LIST_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_VALKEY_GLIDE_LIST_name);

	zval const_VALKEY_GLIDE_ZSET_value;
	ZVAL_LONG(&const_VALKEY_GLIDE_ZSET_value, VALKEY_GLIDE_ZSET);
	zend_string *const_VALKEY_GLIDE_ZSET_name = zend_string_init_interned("VALKEY_GLIDE_ZSET", sizeof("VALKEY_GLIDE_ZSET") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_VALKEY_GLIDE_ZSET_name, &const_VALKEY_GLIDE_ZSET_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_VALKEY_GLIDE_ZSET_name);

	zval const_VALKEY_GLIDE_HASH_value;
	ZVAL_LONG(&const_VALKEY_GLIDE_HASH_value, VALKEY_GLIDE_HASH);
	zend_string *const_VALKEY_GLIDE_HASH_name = zend_string_init_interned("VALKEY_GLIDE_HASH", sizeof("VALKEY_GLIDE_HASH") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_VALKEY_GLIDE_HASH_name, &const_VALKEY_GLIDE_HASH_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_VALKEY_GLIDE_HASH_name);

	zval const_VALKEY_GLIDE_STREAM_value;
	ZVAL_LONG(&const_VALKEY_GLIDE_STREAM_value, VALKEY_GLIDE_STREAM);
	zend_string *const_VALKEY_GLIDE_STREAM_name = zend_string_init_interned("VALKEY_GLIDE_STREAM", sizeof("VALKEY_GLIDE_STREAM") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_VALKEY_GLIDE_STREAM_name, &const_VALKEY_GLIDE_STREAM_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_VALKEY_GLIDE_STREAM_name);

	zval const_BEFORE_value;
	zend_string *const_BEFORE_value_str = zend_string_init("before", strlen("before"), 1);
	ZVAL_STR(&const_BEFORE_value, const_BEFORE_value_str);
	zend_string *const_BEFORE_name = zend_string_init_interned("BEFORE", sizeof("BEFORE") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_BEFORE_name, &const_BEFORE_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_BEFORE_name);

	zval const_AFTER_value;
	zend_string *const_AFTER_value_str = zend_string_init("after", strlen("after"), 1);
	ZVAL_STR(&const_AFTER_value, const_AFTER_value_str);
	zend_string *const_AFTER_name = zend_string_init_interned("AFTER", sizeof("AFTER") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_AFTER_name, &const_AFTER_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_AFTER_name);

	zval const_LEFT_value;
	zend_string *const_LEFT_value_str = zend_string_init("left", strlen("left"), 1);
	ZVAL_STR(&const_LEFT_value, const_LEFT_value_str);
	zend_string *const_LEFT_name = zend_string_init_interned("LEFT", sizeof("LEFT") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_LEFT_name, &const_LEFT_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_LEFT_name);

	zval const_RIGHT_value;
	zend_string *const_RIGHT_value_str = zend_string_init("right", strlen("right"), 1);
	ZVAL_STR(&const_RIGHT_value, const_RIGHT_value_str);
	zend_string *const_RIGHT_name = zend_string_init_interned("RIGHT", sizeof("RIGHT") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_RIGHT_name, &const_RIGHT_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_RIGHT_name);
#if (PHP_VERSION_ID >= 80200)


	zend_add_parameter_attribute(zend_hash_str_find_ptr(&class_entry->function_table, "auth", sizeof("auth") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif

	return class_entry;
}

static zend_class_entry *register_class_ValkeyGlideException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ValkeyGlideException", class_ValkeyGlideException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RuntimeException);

	return class_entry;
}
