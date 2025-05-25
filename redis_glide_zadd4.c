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
#include "command_response.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Execute a ZREVRANGE command using the Valkey Glide client */
int execute_zrevrange_command(const void *glide_client, const char *key, size_t key_len,
                              zval *z_start, zval *z_end, zval *options, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !z_start || !z_end || !return_value)
    {
        return 0;
    }

    /* Parse range options */
    int has_withscores = 0, has_byscore = 0, has_bylex = 0;
    int has_limit = 0;
    long offset = 0, count = 0;

    /* ZREVRANGE is always REV */
    int has_rev = 1;

    if (options)
    {
        parse_range_options(options, &has_withscores, &has_byscore, &has_bylex, NULL,
                            &has_limit, &offset, &count);
    }

    /* Calculate the maximum number of arguments:
     * 1 (key) + 2 (start/end) + 7 (all options) */
    unsigned long max_args = 10;
    uintptr_t *args = (uintptr_t *)emalloc(max_args * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(max_args * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Track allocated strings that need to be freed after command execution */
    char **allocated_strings = (char **)ecalloc(4, sizeof(char *));
    int allocated_count = 0;

    if (!allocated_strings)
    {
        efree(args);
        efree(args_len);
        return 0;
    }

    /* Add key */
    unsigned long arg_idx = 0;
    args[arg_idx] = (uintptr_t)key;
    args_len[arg_idx++] = key_len;

    /* Add start and end parameters */
    int need_free = 0;
    size_t len = 0;
    char *str = NULL;

    /* Start parameter */
    str = zval_to_string(z_start, &len, &need_free);
    if (!str)
    {
        for (int j = 0; j < allocated_count; j++)
            efree(allocated_strings[j]);
        efree(allocated_strings);
        efree(args);
        efree(args_len);
        return 0;
    }
    args[arg_idx] = (uintptr_t)str;
    args_len[arg_idx++] = len;
    if (need_free)
        allocated_strings[allocated_count++] = str;

    /* End parameter */
    str = zval_to_string(z_end, &len, &need_free);
    if (!str)
    {
        for (int j = 0; j < allocated_count; j++)
            efree(allocated_strings[j]);
        efree(allocated_strings);
        efree(args);
        efree(args_len);
        return 0;
    }
    args[arg_idx] = (uintptr_t)str;
    args_len[arg_idx++] = len;
    if (need_free)
        allocated_strings[allocated_count++] = str;

    /* Add options */
    if (has_bylex)
    {
        args[arg_idx] = (uintptr_t)"BYLEX";
        args_len[arg_idx++] = 5;
    }
    else if (has_byscore)
    {
        args[arg_idx] = (uintptr_t)"BYSCORE";
        args_len[arg_idx++] = 7;
    }

    /* ZREVRANGE is always REV */
    args[arg_idx] = (uintptr_t)"REV";
    args_len[arg_idx++] = 3;

    if (has_withscores)
    {
        args[arg_idx] = (uintptr_t)"WITHSCORES";
        args_len[arg_idx++] = 10;
    }

    if (has_limit)
    {
        args[arg_idx] = (uintptr_t)"LIMIT";
        args_len[arg_idx++] = 5;

        /* Add offset parameter */
        str = long_to_string(offset, &len);
        if (!str)
        {
            for (int j = 0; j < allocated_count; j++)
                efree(allocated_strings[j]);
            efree(allocated_strings);
            efree(args);
            efree(args_len);
            return 0;
        }
        args[arg_idx] = (uintptr_t)str;
        args_len[arg_idx++] = len;
        allocated_strings[allocated_count++] = str;

        /* Add count parameter */
        str = long_to_string(count, &len);
        if (!str)
        {
            for (int j = 0; j < allocated_count; j++)
                efree(allocated_strings[j]);
            efree(allocated_strings);
            efree(args);
            efree(args_len);
            return 0;
        }
        args[arg_idx] = (uintptr_t)str;
        args_len[arg_idx++] = len;
        allocated_strings[allocated_count++] = str;
    }

    /* Execute the ZRANGE command with REV option */
    CommandResult *result = execute_command(
        glide_client,
        ZRange,  /* command type */
        arg_idx, /* number of arguments */
        args,    /* arguments */
        args_len /* argument lengths */
    );

    /* Free allocated strings */
    for (int j = 0; j < allocated_count; j++)
    {
        efree(allocated_strings[j]);
    }
    efree(allocated_strings);
    efree(args);
    efree(args_len);

    /* Process the result */
    return process_zrange_response(result, return_value);
}

/* Execute a ZRANGEBYSCORE command using the Valkey Glide client */
int execute_zrangebyscore_command(const void *glide_client, const char *key, size_t key_len,
                                  zval *z_min, zval *z_max, zval *options, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !z_min || !z_max || !return_value)
    {
        return 0;
    }

    /* Parse range options */
    int has_withscores = 0, has_bylex = 0;
    int has_limit = 0, has_rev = 0;
    long offset = 0, count = 0;

    /* ZRANGEBYSCORE always has BYSCORE option */
    int has_byscore = 1;

    if (options)
    {
        parse_range_options(options, &has_withscores, NULL, &has_bylex, &has_rev,
                            &has_limit, &offset, &count);
    }

    /* Calculate the maximum number of arguments:
     * 1 (key) + 2 (min/max) + 7 (all options) */
    unsigned long max_args = 10;
    uintptr_t *args = (uintptr_t *)emalloc(max_args * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(max_args * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Track allocated strings that need to be freed after command execution */
    char **allocated_strings = (char **)ecalloc(4, sizeof(char *));
    int allocated_count = 0;

    if (!allocated_strings)
    {
        efree(args);
        efree(args_len);
        return 0;
    }

    /* Add key */
    unsigned long arg_idx = 0;
    args[arg_idx] = (uintptr_t)key;
    args_len[arg_idx++] = key_len;

    /* Add min and max parameters */
    int need_free = 0;
    size_t len = 0;
    char *str = NULL;

    /* Min parameter */
    str = zval_to_string(z_min, &len, &need_free);
    if (!str)
    {
        for (int j = 0; j < allocated_count; j++)
            efree(allocated_strings[j]);
        efree(allocated_strings);
        efree(args);
        efree(args_len);
        return 0;
    }
    args[arg_idx] = (uintptr_t)str;
    args_len[arg_idx++] = len;
    if (need_free)
        allocated_strings[allocated_count++] = str;

    /* Max parameter */
    str = zval_to_string(z_max, &len, &need_free);
    if (!str)
    {
        for (int j = 0; j < allocated_count; j++)
            efree(allocated_strings[j]);
        efree(allocated_strings);
        efree(args);
        efree(args_len);
        return 0;
    }
    args[arg_idx] = (uintptr_t)str;
    args_len[arg_idx++] = len;
    if (need_free)
        allocated_strings[allocated_count++] = str;

    /* Add BYSCORE option */
    args[arg_idx] = (uintptr_t)"BYSCORE";
    args_len[arg_idx++] = 7;

    /* Add REV option if specified */
    if (has_rev)
    {
        args[arg_idx] = (uintptr_t)"REV";
        args_len[arg_idx++] = 3;
    }

    if (has_withscores)
    {
        args[arg_idx] = (uintptr_t)"WITHSCORES";
        args_len[arg_idx++] = 10;
    }

    if (has_limit)
    {
        args[arg_idx] = (uintptr_t)"LIMIT";
        args_len[arg_idx++] = 5;

        /* Add offset parameter */
        str = long_to_string(offset, &len);
        if (!str)
        {
            for (int j = 0; j < allocated_count; j++)
                efree(allocated_strings[j]);
            efree(allocated_strings);
            efree(args);
            efree(args_len);
            return 0;
        }
        args[arg_idx] = (uintptr_t)str;
        args_len[arg_idx++] = len;
        allocated_strings[allocated_count++] = str;

        /* Add count parameter */
        str = long_to_string(count, &len);
        if (!str)
        {
            for (int j = 0; j < allocated_count; j++)
                efree(allocated_strings[j]);
            efree(allocated_strings);
            efree(args);
            efree(args_len);
            return 0;
        }
        args[arg_idx] = (uintptr_t)str;
        args_len[arg_idx++] = len;
        allocated_strings[allocated_count++] = str;
    }

    /* Execute the ZRANGE command with BYSCORE option */
    CommandResult *result = execute_command(
        glide_client,
        ZRange,  /* command type */
        arg_idx, /* number of arguments */
        args,    /* arguments */
        args_len /* argument lengths */
    );

    /* Free allocated strings */
    for (int j = 0; j < allocated_count; j++)
    {
        efree(allocated_strings[j]);
    }
    efree(allocated_strings);
    efree(args);
    efree(args_len);

    /* Process the result */
    return process_zrange_response(result, return_value);
}

/* Execute a ZREVRANGEBYSCORE command using the Valkey Glide client */
int execute_zrevrangebyscore_command(const void *glide_client, const char *key, size_t key_len,
                                     zval *z_max, zval *z_min, zval *options, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !z_min || !z_max || !return_value)
    {
        return 0;
    }

    /* Parse range options */
    int has_withscores = 0, has_bylex = 0, has_limit = 0;
    long offset = 0, count = 0;

    /* ZREVRANGEBYSCORE always uses BYSCORE and REV options */
    int has_byscore = 1;
    int has_rev = 1;

    if (options)
    {
        parse_range_options(options, &has_withscores, NULL, &has_bylex, NULL,
                            &has_limit, &offset, &count);
    }

    /* Calculate the maximum number of arguments:
     * 1 (key) + 2 (max/min) + 7 (all options) */
    unsigned long max_args = 10;
    uintptr_t *args = (uintptr_t *)emalloc(max_args * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(max_args * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Track allocated strings that need to be freed after command execution */
    char **allocated_strings = (char **)ecalloc(4, sizeof(char *));
    int allocated_count = 0;

    if (!allocated_strings)
    {
        efree(args);
        efree(args_len);
        return 0;
    }

    /* Add key */
    unsigned long arg_idx = 0;
    args[arg_idx] = (uintptr_t)key;
    args_len[arg_idx++] = key_len;

    /* Add max and min parameters (in that order for ZREVRANGEBYSCORE) */
    int need_free = 0;
    size_t len = 0;
    char *str = NULL;

    /* Max parameter */
    str = zval_to_string(z_max, &len, &need_free);
    if (!str)
    {
        for (int j = 0; j < allocated_count; j++)
            efree(allocated_strings[j]);
        efree(allocated_strings);
        efree(args);
        efree(args_len);
        return 0;
    }
    args[arg_idx] = (uintptr_t)str;
    args_len[arg_idx++] = len;
    if (need_free)
        allocated_strings[allocated_count++] = str;

    /* Min parameter */
    str = zval_to_string(z_min, &len, &need_free);
    if (!str)
    {
        for (int j = 0; j < allocated_count; j++)
            efree(allocated_strings[j]);
        efree(allocated_strings);
        efree(args);
        efree(args_len);
        return 0;
    }
    args[arg_idx] = (uintptr_t)str;
    args_len[arg_idx++] = len;
    if (need_free)
        allocated_strings[allocated_count++] = str;

    /* Add BYSCORE option */
    args[arg_idx] = (uintptr_t)"BYSCORE";
    args_len[arg_idx++] = 7;

    /* Add REV option */
    args[arg_idx] = (uintptr_t)"REV";
    args_len[arg_idx++] = 3;

    if (has_withscores)
    {
        args[arg_idx] = (uintptr_t)"WITHSCORES";
        args_len[arg_idx++] = 10;
    }

    if (has_limit)
    {
        args[arg_idx] = (uintptr_t)"LIMIT";
        args_len[arg_idx++] = 5;

        /* Add offset parameter */
        str = long_to_string(offset, &len);
        if (!str)
        {
            for (int j = 0; j < allocated_count; j++)
                efree(allocated_strings[j]);
            efree(allocated_strings);
            efree(args);
            efree(args_len);
            return 0;
        }
        args[arg_idx] = (uintptr_t)str;
        args_len[arg_idx++] = len;
        allocated_strings[allocated_count++] = str;

        /* Add count parameter */
        str = long_to_string(count, &len);
        if (!str)
        {
            for (int j = 0; j < allocated_count; j++)
                efree(allocated_strings[j]);
            efree(allocated_strings);
            efree(args);
            efree(args_len);
            return 0;
        }
        args[arg_idx] = (uintptr_t)str;
        args_len[arg_idx++] = len;
        allocated_strings[allocated_count++] = str;
    }

    /* Execute the ZRANGE command with BYSCORE and REV options */
    CommandResult *result = execute_command(
        glide_client,
        ZRange,  /* command type */
        arg_idx, /* number of arguments */
        args,    /* arguments */
        args_len /* argument lengths */
    );

    /* Free allocated strings */
    for (int j = 0; j < allocated_count; j++)
    {
        efree(allocated_strings[j]);
    }
    efree(allocated_strings);
    efree(args);
    efree(args_len);

    /* Process the result */
    return process_zrange_response(result, return_value);
}

/* Execute a ZRANGEBYLEX command using the Valkey Glide client */
int execute_zrangebylex_command(const void *glide_client, const char *key, size_t key_len,
                                zval *z_min, zval *z_max, zval *options, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || !z_min || !z_max || !return_value)
    {
        return 0;
    }

    /* Parse range options */
    int has_withscores = 0, has_byscore = 0;
    int has_limit = 0, has_rev = 0;
    long offset = 0, count = 0;

    /* ZRANGEBYLEX always uses BYLEX */
    int has_bylex = 1;

    if (options)
    {
        parse_range_options(options, &has_withscores, &has_byscore, NULL, &has_rev,
                            &has_limit, &offset, &count);
    }

    /* Calculate the maximum number of arguments:
     * 1 (key) + 2 (min/max) + 7 (all options) */
    unsigned long max_args = 10;
    uintptr_t *args = (uintptr_t *)emalloc(max_args * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(max_args * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* Track allocated strings that need to be freed after command execution */
    char **allocated_strings = (char **)ecalloc(4, sizeof(char *));
    int allocated_count = 0;

    if (!allocated_strings)
    {
        efree(args);
        efree(args_len);
        return 0;
    }

    /* Add key */
    unsigned long arg_idx = 0;
    args[arg_idx] = (uintptr_t)key;
    args_len[arg_idx++] = key_len;

    /* Add min and max parameters */
    int need_free = 0;
    size_t len = 0;
    char *str = NULL;

    /* Min parameter */
    str = zval_to_string(z_min, &len, &need_free);
    if (!str)
    {
        for (int j = 0; j < allocated_count; j++)
            efree(allocated_strings[j]);
        efree(allocated_strings);
        efree(args);
        efree(args_len);
        return 0;
    }
    args[arg_idx] = (uintptr_t)str;
    args_len[arg_idx++] = len;
    if (need_free)
        allocated_strings[allocated_count++] = str;

    /* Max parameter */
    str = zval_to_string(z_max, &len, &need_free);
    if (!str)
    {
        for (int j = 0; j < allocated_count; j++)
            efree(allocated_strings[j]);
        efree(allocated_strings);
        efree(args);
        efree(args_len);
        return 0;
    }
    args[arg_idx] = (uintptr_t)str;
    args_len[arg_idx++] = len;
    if (need_free)
        allocated_strings[allocated_count++] = str;

    /* Add BYLEX option */
    args[arg_idx] = (uintptr_t)"BYLEX";
    args_len[arg_idx++] = 5;

    /* Add REV option if specified */
    if (has_rev)
    {
        args[arg_idx] = (uintptr_t)"REV";
        args_len[arg_idx++] = 3;
    }

    if (has_limit)
    {
        args[arg_idx] = (uintptr_t)"LIMIT";
        args_len[arg_idx++] = 5;

        /* Add offset parameter */
        str = long_to_string(offset, &len);
        if (!str)
        {
            for (int j = 0; j < allocated_count; j++)
                efree(allocated_strings[j]);
            efree(allocated_strings);
            efree(args);
            efree(args_len);
            return 0;
        }
        args[arg_idx] = (uintptr_t)str;
        args_len[arg_idx++] = len;
        allocated_strings[allocated_count++] = str;

        /* Add count parameter */
        str = long_to_string(count, &len);
        if (!str)
        {
            for (int j = 0; j < allocated_count; j++)
                efree(allocated_strings[j]);
            efree(allocated_strings);
            efree(args);
            efree(args_len);
            return 0;
        }
        args[arg_idx] = (uintptr_t)str;
        args_len[arg_idx++] = len;
        allocated_strings[allocated_count++] = str;
    }

    /* Execute the ZRANGE command with BYLEX option */
    CommandResult *result = execute_command(
        glide_client,
        ZRange,  /* command type */
        arg_idx, /* number of arguments */
        args,    /* arguments */
        args_len /* argument lengths */
    );

    /* Free allocated strings */
    for (int j = 0; j < allocated_count; j++)
    {
        efree(allocated_strings[j]);
    }
    efree(allocated_strings);
    efree(args);
    efree(args_len);

    /* Process the result */
    return process_zrange_response(result, return_value);
}
