/*
  +----------------------------------------------------------------------+
  | Redis Glide FFI integration for phpredis                             |
  +----------------------------------------------------------------------+
  | Copyright (c) 2023-2025 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
*/

#include "php_redis.h"
#include "redis_glide.h"
#include "redis_glide_zadd.h"
#include "command_response.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Helper function to parse range options */
int parse_range_options(zval *options, int *has_withscores,
                        int *has_byscore, int *has_bylex, int *has_rev,
                        int *has_limit, long *offset, long *count)
{
    if (!options || Z_TYPE_P(options) != IS_ARRAY)
    {
        return 0;
    }

    HashTable *options_ht = Z_ARRVAL_P(options);
    zval *z_tmp;

    /* First iterate through array values to check for flat format options */
    ZEND_HASH_FOREACH_VAL(options_ht, z_tmp)
    {
        if (Z_TYPE_P(z_tmp) == IS_STRING)
        {
            if (strcasecmp(Z_STRVAL_P(z_tmp), "WITHSCORES") == 0)
            {
                *has_withscores = 1;
            }
            else if (strcasecmp(Z_STRVAL_P(z_tmp), "BYSCORE") == 0)
            {
                *has_byscore = 1;
            }
            else if (strcasecmp(Z_STRVAL_P(z_tmp), "BYLEX") == 0)
            {
                *has_bylex = 1;
            }
            else if (strcasecmp(Z_STRVAL_P(z_tmp), "REV") == 0)
            {
                *has_rev = 1;
            }
        }
    }
    ZEND_HASH_FOREACH_END();

    /* Look for WITHSCORES option as key-value */
    if ((z_tmp = zend_hash_str_find(options_ht, "WITHSCORES", sizeof("WITHSCORES") - 1)) != NULL ||
        (z_tmp = zend_hash_str_find(options_ht, "withscores", sizeof("withscores") - 1)) != NULL)
    {
        if (Z_TYPE_P(z_tmp) == IS_TRUE ||
            (Z_TYPE_P(z_tmp) == IS_LONG && Z_LVAL_P(z_tmp) == 1))
        {
            *has_withscores = 1;
        }
    }

    /* Look for BY option as key-value */
    if ((z_tmp = zend_hash_str_find(options_ht, "BY", sizeof("BY") - 1)) != NULL ||
        (z_tmp = zend_hash_str_find(options_ht, "by", sizeof("by") - 1)) != NULL)
    {
        if (Z_TYPE_P(z_tmp) == IS_STRING)
        {
            if (strcasecmp(Z_STRVAL_P(z_tmp), "SCORE") == 0)
            {
                *has_byscore = 1;
            }
            else if (strcasecmp(Z_STRVAL_P(z_tmp), "LEX") == 0)
            {
                *has_bylex = 1;
            }
        }
    }

    /* Look for REV option as key-value */
    if ((z_tmp = zend_hash_str_find(options_ht, "REV", sizeof("REV") - 1)) != NULL ||
        (z_tmp = zend_hash_str_find(options_ht, "rev", sizeof("rev") - 1)) != NULL)
    {
        if (Z_TYPE_P(z_tmp) == IS_TRUE ||
            (Z_TYPE_P(z_tmp) == IS_LONG && Z_LVAL_P(z_tmp) == 1))
        {
            *has_rev = 1;
        }
    }

    /* Look for LIMIT option */
    if ((z_tmp = zend_hash_str_find(options_ht, "LIMIT", sizeof("LIMIT") - 1)) != NULL ||
        (z_tmp = zend_hash_str_find(options_ht, "limit", sizeof("limit") - 1)) != NULL)
    {
        if (Z_TYPE_P(z_tmp) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(z_tmp)) == 2)
        {
            zval *z_offset, *z_count;
            HashTable *ht_limit = Z_ARRVAL_P(z_tmp);

            /* Get offset and count from LIMIT array */
            if ((z_offset = zend_hash_index_find(ht_limit, 0)) != NULL &&
                (z_count = zend_hash_index_find(ht_limit, 1)) != NULL)
            {
                *offset = zval_get_long(z_offset);
                *count = zval_get_long(z_count);
                *has_limit = 1;
            }
        }
    }

    return 1;
}
