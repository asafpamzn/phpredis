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

/* Execute an SADD command using the Valkey Glide client */
int execute_sadd_command(const void *glide_client, const char *key, size_t key_len,
                         zval *members, int members_count, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || key_len == 0 || !members || members_count <= 0 || !output_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = members_count + 1; /* key + members */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* First argument is the key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add each member to the arguments */
    int i;
    zval temp;
    for (i = 0; i < members_count; i++)
    {
        zval *member = &members[i];

        /* Convert to string if needed */
        if (Z_TYPE_P(member) != IS_STRING)
        {
            ZVAL_COPY(&temp, member);
            convert_to_string(&temp);
            args[i + 1] = (uintptr_t)Z_STRVAL(temp);
            args_len[i + 1] = Z_STRLEN(temp);
            zval_dtor(&temp);
        }
        else
        {
            args[i + 1] = (uintptr_t)Z_STRVAL_P(member);
            args_len[i + 1] = Z_STRLEN_P(member);
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SAdd,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Handle the response */
    return handle_int_response(result, output_value);
}

/* Execute an SADD command with array input using the Valkey Glide client */
int execute_sadd_array_command(const void *glide_client, const char *key, size_t key_len,
                               HashTable *members_ht, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || key_len == 0 || !members_ht || !output_value)
    {
        return 0;
    }

    /* Count number of members */
    int members_count = zend_hash_num_elements(members_ht);
    if (members_count == 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = members_count + 1; /* key + members */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* First argument is the key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add each member to the arguments */
    int i = 1;
    zend_ulong idx;
    zend_string *str_key;
    zval *val, temp;

    ZEND_HASH_FOREACH_KEY_VAL(members_ht, idx, str_key, val)
    {
        /* Convert to string if needed */
        if (Z_TYPE_P(val) != IS_STRING)
        {
            ZVAL_COPY(&temp, val);
            convert_to_string(&temp);
            args[i] = (uintptr_t)Z_STRVAL(temp);
            args_len[i] = Z_STRLEN(temp);
            zval_dtor(&temp);
        }
        else
        {
            args[i] = (uintptr_t)Z_STRVAL_P(val);
            args_len[i] = Z_STRLEN_P(val);
        }
        i++;
    }
    ZEND_HASH_FOREACH_END();

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SAdd,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Handle the response */
    return handle_int_response(result, output_value);
}

/* Execute an SCARD command using the Valkey Glide client */
int execute_scard_command(const void *glide_client, const char *key, size_t key_len,
                          long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || key_len == 0 || !output_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1; /* key */
    uintptr_t args[1];
    unsigned long args_len[1];

    /* Set up arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SCard,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Handle the response */
    return handle_int_response(result, output_value);
}

/* Execute an SREM command using the Valkey Glide client */
int execute_srem_command(const void *glide_client, const char *key, size_t key_len,
                         zval *members, int members_count, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || key_len == 0 || !members || members_count <= 0 || !output_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = members_count + 1; /* key + members */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* First argument is the key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add each member to the arguments */
    int i;
    zval temp;
    for (i = 0; i < members_count; i++)
    {
        zval *member = &members[i];

        /* Convert to string if needed */
        if (Z_TYPE_P(member) != IS_STRING)
        {
            ZVAL_COPY(&temp, member);
            convert_to_string(&temp);
            args[i + 1] = (uintptr_t)Z_STRVAL(temp);
            args_len[i + 1] = Z_STRLEN(temp);
            zval_dtor(&temp);
        }
        else
        {
            args[i + 1] = (uintptr_t)Z_STRVAL_P(member);
            args_len[i + 1] = Z_STRLEN_P(member);
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SRem,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Handle the response */
    return handle_int_response(result, output_value);
}

/* Execute an SMOVE command using the Valkey Glide client */
int execute_smove_command(const void *glide_client, const char *src, size_t src_len,
                          const char *dst, size_t dst_len, const char *member,
                          size_t member_len, int *output_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !src || src_len == 0 || !dst || dst_len == 0 || !member || member_len == 0 || !output_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3; /* source, destination, member */
    uintptr_t args[3];
    unsigned long args_len[3];

    /* Set up arguments */
    args[0] = (uintptr_t)src;
    args_len[0] = src_len;
    args[1] = (uintptr_t)dst;
    args_len[1] = dst_len;
    args[2] = (uintptr_t)member;
    args_len[2] = member_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SMove,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Handle the response as integer (1 if moved, 0 if not) */
    long int_result;
    int status = handle_int_response(result, &int_result);
    if (status)
    {
        *output_value = (int)int_result;
    }
    return status;
}

/* Execute an SPOP command using the Valkey Glide client */
int execute_spop_command(const void *glide_client, const char *key, size_t key_len,
                         long count, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || key_len == 0 || !return_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = count > 1 ? 2 : 1; /* key [count] */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* First argument is the key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add count argument if needed */
    char count_str[32];
    if (count > 1)
    {
        snprintf(count_str, sizeof(count_str), "%ld", count);
        args[1] = (uintptr_t)count_str;
        args_len[1] = strlen(count_str);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SPop,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Handle the response */
    int status = 0;
    if (result)
    {
        if (result->command_error)
        {
            /* Command failed */
            free_command_result(result);
            return 0;
        }

        if (result->response)
        {
            /* SPOP returns a string (one element) or array (multiple elements) */
            status = command_response_to_zval(result->response, return_value, 0);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute an SRANDMEMBER command using the Valkey Glide client */
int execute_srandmember_command(const void *glide_client, const char *key, size_t key_len,
                                long count, zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || key_len == 0 || !return_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = count != 1 ? 2 : 1; /* key [count] */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* First argument is the key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add count argument if provided and not 1 (default) */
    char count_str[32];
    if (count != 1)
    {
        snprintf(count_str, sizeof(count_str), "%ld", count);
        args[1] = (uintptr_t)count_str;
        args_len[1] = strlen(count_str);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SRandMember, /* command type */
        arg_count,   /* number of arguments */
        args,        /* arguments */
        args_len     /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Handle the response */
    int status = 0;
    if (result)
    {
        if (result->command_error)
        {
            /* Command failed */
            free_command_result(result);
            return 0;
        }

        if (result->response)
        {
            /* SRANDMEMBER returns a string (one element) or array (multiple elements) */
            status = command_response_to_zval(result->response, return_value, 0);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}

/* Execute an SISMEMBER command using the Valkey Glide client */
int execute_sismember_command(const void *glide_client, const char *key, size_t key_len,
                              const char *member, size_t member_len, int *output_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || key_len == 0 || !member || !output_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2; /* key, member */
    uintptr_t args[2];
    unsigned long args_len[2];

    /* Set up arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;
    args[1] = (uintptr_t)member;
    args_len[1] = member_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SIsMember, /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Handle the response as integer (1 if member, 0 if not) */
    long int_result;
    int status = handle_int_response(result, &int_result);
    if (status)
    {
        *output_value = (int)int_result;
    }
    return status;
}

/* Execute an SMEMBERS command using the Valkey Glide client */
int execute_smembers_command(const void *glide_client, const char *key, size_t key_len,
                             zval *return_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key || key_len == 0 || !return_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1; /* key */
    uintptr_t args[1];
    unsigned long args_len[1];

    /* Set up arguments */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SMembers,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Handle the response directly with sets handler */
    return handle_set_response(result, return_value);
}

/* Execute an SMISMEMBER command using the Valkey Glide client */
int execute_smismember_command(const void *glide_client, const char *key, size_t key_len,
                               zval *members, int members_count, zval *return_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !key || key_len == 0 || !members || members_count <= 0 || !return_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = members_count + 1; /* key + members */
    uintptr_t *args = (uintptr_t *)emalloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)emalloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            efree(args);
        if (args_len)
            efree(args_len);
        return 0;
    }

    /* First argument is the key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add each member to the arguments */
    int i;
    zval temp;
    for (i = 0; i < members_count; i++)
    {
        zval *member = &members[i];

        /* Convert to string if needed */
        if (Z_TYPE_P(member) != IS_STRING)
        {
            ZVAL_COPY(&temp, member);
            convert_to_string(&temp);
            args[i + 1] = (uintptr_t)Z_STRVAL(temp);
            args_len[i + 1] = Z_STRLEN(temp);
            zval_dtor(&temp);
        }
        else
        {
            args[i + 1] = (uintptr_t)Z_STRVAL_P(member);
            args_len[i + 1] = Z_STRLEN_P(member);
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SMIsMember, /* command type */
        arg_count,  /* number of arguments */
        args,       /* arguments */
        args_len    /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Initialize an array for the result */
    array_init(return_value);

    /* Handle the response */
    int status = 0;
    if (result)
    {
        if (result->command_error)
        {
            /* Command failed */
            free_command_result(result);
            return 0;
        }

        if (result->response)
        {
            /* SMISMEMBER returns an array of 0/1 integers */
            status = command_response_to_zval(result->response, return_value, 0);
            free_command_result(result);
            return status;
        }
        free_command_result(result);
    }

    return 0;
}
