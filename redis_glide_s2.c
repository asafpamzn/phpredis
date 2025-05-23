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

/* Execute an SINTER command using the Valkey Glide client */
int execute_sinter_command(const void *glide_client, zval *keys, int keys_count, zval *return_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys || keys_count <= 0 || !return_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = keys_count; /* Just keys */
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

    /* Add each key to the arguments */
    int i;
    zval temp;
    for (i = 0; i < keys_count; i++)
    {
        zval *key = &keys[i];

        /* Convert to string if needed */
        if (Z_TYPE_P(key) != IS_STRING)
        {
            ZVAL_COPY(&temp, key);
            convert_to_string(&temp);
            args[i] = (uintptr_t)Z_STRVAL(temp);
            args_len[i] = Z_STRLEN(temp);
            zval_dtor(&temp);
        }
        else
        {
            args[i] = (uintptr_t)Z_STRVAL_P(key);
            args_len[i] = Z_STRLEN_P(key);
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SInter,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Handle the result as a set response */
    return handle_set_response(result, return_value);
}

/* Execute an SINTERCARD command using the Valkey Glide client */
int execute_sintercard_command(const void *glide_client, zval *keys, int keys_count, long limit, zval *return_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys || keys_count <= 0 || !return_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    int use_limit = (limit > 0);
    unsigned long arg_count = keys_count + 1 + (use_limit ? 2 : 0); /* numkeys + keys + [LIMIT limit] */
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

    /* First argument is the number of keys as a string */
    char numkeys_str[32];
    snprintf(numkeys_str, sizeof(numkeys_str), "%d", keys_count);
    args[0] = (uintptr_t)numkeys_str;
    args_len[0] = strlen(numkeys_str);

    /* Add each key to the arguments */
    int i;
    zval temp;
    for (i = 0; i < keys_count; i++)
    {
        zval *key = &keys[i];

        /* Convert to string if needed */
        if (Z_TYPE_P(key) != IS_STRING)
        {
            ZVAL_COPY(&temp, key);
            convert_to_string(&temp);
            args[i + 1] = (uintptr_t)Z_STRVAL(temp);
            args_len[i + 1] = Z_STRLEN(temp);
            zval_dtor(&temp);
        }
        else
        {
            args[i + 1] = (uintptr_t)Z_STRVAL_P(key);
            args_len[i + 1] = Z_STRLEN_P(key);
        }
    }

    /* Add LIMIT argument if needed */
    if (use_limit)
    {
        /* Add "LIMIT" keyword */
        args[keys_count + 1] = (uintptr_t)"LIMIT";
        args_len[keys_count + 1] = 5;

        /* Add limit value */
        char limit_str[32];
        snprintf(limit_str, sizeof(limit_str), "%ld", limit);
        args[keys_count + 2] = (uintptr_t)limit_str;
        args_len[keys_count + 2] = strlen(limit_str);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SInterCard, /* command type */
        arg_count,  /* number of arguments */
        args,       /* arguments */
        args_len    /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Handle the response as integer (cardinality of intersection) */
    long cardinality;
    int status = handle_int_response(result, &cardinality);
    if (status)
    {
        ZVAL_LONG(return_value, cardinality);
        return 1;
    }
    return 0;
}

/* Execute an SINTERSTORE command using the Valkey Glide client */
int execute_sinterstore_command(const void *glide_client, const char *dst, size_t dst_len,
                                zval *keys, int keys_count, long *output_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !dst || dst_len == 0 || !keys || keys_count <= 0 || !output_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = keys_count + 1; /* destination + keys */
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

    /* First argument is the destination key */
    args[0] = (uintptr_t)dst;
    args_len[0] = dst_len;

    /* Add each source key to the arguments */
    int i;
    zval temp;
    for (i = 0; i < keys_count; i++)
    {
        zval *key = &keys[i];

        /* Convert to string if needed */
        if (Z_TYPE_P(key) != IS_STRING)
        {
            ZVAL_COPY(&temp, key);
            convert_to_string(&temp);
            args[i + 1] = (uintptr_t)Z_STRVAL(temp);
            args_len[i + 1] = Z_STRLEN(temp);
            zval_dtor(&temp);
        }
        else
        {
            args[i + 1] = (uintptr_t)Z_STRVAL_P(key);
            args_len[i + 1] = Z_STRLEN_P(key);
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SInterStore, /* command type */
        arg_count,   /* number of arguments */
        args,        /* arguments */
        args_len     /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Handle the response as integer (cardinality of result set) */
    return handle_int_response(result, output_value);
}

/* Execute an SUNION command using the Valkey Glide client */
int execute_sunion_command(const void *glide_client, zval *keys, int keys_count, zval *return_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys || keys_count <= 0 || !return_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = keys_count; /* Just keys */
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

    /* Add each key to the arguments */
    int i;
    zval temp;
    for (i = 0; i < keys_count; i++)
    {
        zval *key = &keys[i];

        /* Convert to string if needed */
        if (Z_TYPE_P(key) != IS_STRING)
        {
            ZVAL_COPY(&temp, key);
            convert_to_string(&temp);
            args[i] = (uintptr_t)Z_STRVAL(temp);
            args_len[i] = Z_STRLEN(temp);
            zval_dtor(&temp);
        }
        else
        {
            args[i] = (uintptr_t)Z_STRVAL_P(key);
            args_len[i] = Z_STRLEN_P(key);
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SUnion,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Handle the result as a set response */
    return handle_set_response(result, return_value);
}

/* Execute an SUNIONSTORE command using the Valkey Glide client */
int execute_sunionstore_command(const void *glide_client, const char *dst, size_t dst_len,
                                zval *keys, int keys_count, long *output_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !dst || dst_len == 0 || !keys || keys_count <= 0 || !output_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = keys_count + 1; /* destination + keys */
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

    /* First argument is the destination key */
    args[0] = (uintptr_t)dst;
    args_len[0] = dst_len;

    /* Add each source key to the arguments */
    int i;
    zval temp;
    for (i = 0; i < keys_count; i++)
    {
        zval *key = &keys[i];

        /* Convert to string if needed */
        if (Z_TYPE_P(key) != IS_STRING)
        {
            ZVAL_COPY(&temp, key);
            convert_to_string(&temp);
            args[i + 1] = (uintptr_t)Z_STRVAL(temp);
            args_len[i + 1] = Z_STRLEN(temp);
            zval_dtor(&temp);
        }
        else
        {
            args[i + 1] = (uintptr_t)Z_STRVAL_P(key);
            args_len[i + 1] = Z_STRLEN_P(key);
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SUnionStore, /* command type */
        arg_count,   /* number of arguments */
        args,        /* arguments */
        args_len     /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Handle the response as integer (cardinality of result set) */
    return handle_int_response(result, output_value);
}

/* Execute an SDIFF command using the Valkey Glide client */
int execute_sdiff_command(const void *glide_client, zval *keys, int keys_count, zval *return_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys || keys_count <= 0 || !return_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = keys_count; /* Just keys */
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

    /* Add each key to the arguments */
    int i;
    zval temp;
    for (i = 0; i < keys_count; i++)
    {
        zval *key = &keys[i];

        /* Convert to string if needed */
        if (Z_TYPE_P(key) != IS_STRING)
        {
            ZVAL_COPY(&temp, key);
            convert_to_string(&temp);
            args[i] = (uintptr_t)Z_STRVAL(temp);
            args_len[i] = Z_STRLEN(temp);
            zval_dtor(&temp);
        }
        else
        {
            args[i] = (uintptr_t)Z_STRVAL_P(key);
            args_len[i] = Z_STRLEN_P(key);
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SDiff,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Handle the result as a set response */
    return handle_set_response(result, return_value);
}

/* Execute an SDIFFSTORE command using the Valkey Glide client */
int execute_sdiffstore_command(const void *glide_client, const char *dst, size_t dst_len,
                               zval *keys, int keys_count, long *output_value)
{
    /* Check if client and parameters are valid */
    if (!glide_client || !dst || dst_len == 0 || !keys || keys_count <= 0 || !output_value)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = keys_count + 1; /* destination + keys */
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

    /* First argument is the destination key */
    args[0] = (uintptr_t)dst;
    args_len[0] = dst_len;

    /* Add each source key to the arguments */
    int i;
    zval temp;
    for (i = 0; i < keys_count; i++)
    {
        zval *key = &keys[i];

        /* Convert to string if needed */
        if (Z_TYPE_P(key) != IS_STRING)
        {
            ZVAL_COPY(&temp, key);
            convert_to_string(&temp);
            args[i + 1] = (uintptr_t)Z_STRVAL(temp);
            args_len[i + 1] = Z_STRLEN(temp);
            zval_dtor(&temp);
        }
        else
        {
            args[i + 1] = (uintptr_t)Z_STRVAL_P(key);
            args_len[i + 1] = Z_STRLEN_P(key);
        }
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SDiffStore, /* command type */
        arg_count,  /* number of arguments */
        args,       /* arguments */
        args_len    /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Handle the response as integer (cardinality of result set) */
    return handle_int_response(result, output_value);
}

/* Execute a command to get server name from INFO using the Valkey Glide client */
int execute_servername_command(const void *glide_client, char **output, size_t *output_len)
{
    /* Check if client and output parameters are valid */
    if (!glide_client || !output || !output_len)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1; /* "server" section */
    uintptr_t args[1];
    unsigned long args_len[1];

    /* Set up arguments */
    const char *section = "server";
    args[0] = (uintptr_t)section;
    args_len[0] = strlen(section);

    /* Execute the INFO command */
    CommandResult *result = execute_command(
        glide_client,
        Info,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Check if the command was successful */
    if (!result || result->command_error || !result->response)
    {
        if (result)
            free_command_result(result);
        return 0;
    }

    /* Get the result string */
    if (result->response->response_type != String)
    {
        free_command_result(result);
        return 0;
    }

    /* Parse the INFO response to extract the server name */
    const char *info_str = result->response->string_value;
    size_t info_len = result->response->string_value_len;

    /* Look for the line with redis_version */
    const char *redis_version_line = strstr(info_str, "redis_version:");
    if (!redis_version_line)
    {
        free_command_result(result);
        return 0;
    }

    /* Skip past "redis_version:" */
    redis_version_line += 14; // Length of "redis_version:"

    /* Find the end of the line */
    const char *line_end = strchr(redis_version_line, '\r');
    if (!line_end)
        line_end = strchr(redis_version_line, '\n');
    if (!line_end)
        line_end = info_str + info_len;

    /* Extract the server name: before the version number */
    const char *version_number = redis_version_line;
    while (version_number < line_end && *version_number && !isdigit(*version_number))
    {
        version_number++;
    }

    /* If we found a digit, back up to the last space */
    const char *name_end = version_number;
    while (name_end > redis_version_line && *(name_end - 1) == ' ')
    {
        name_end--;
    }

    /* Copy the server name */
    if (name_end > redis_version_line)
    {
        size_t name_len = name_end - redis_version_line;
        *output = (char *)emalloc(name_len + 1);
        if (*output)
        {
            memcpy(*output, redis_version_line, name_len);
            (*output)[name_len] = '\0';
            *output_len = name_len;
        }
    }
    else
    {
        /* Default to "Redis" if we couldn't extract a name */
        const char *default_name = "Redis";
        size_t default_len = strlen(default_name);
        *output = (char *)emalloc(default_len + 1);
        if (*output)
        {
            memcpy(*output, default_name, default_len);
            (*output)[default_len] = '\0';
            *output_len = default_len;
        }
    }

    /* Free the result */
    free_command_result(result);

    /* Check if we successfully allocated memory for the output */
    return (*output != NULL);
}

/* Execute a command to get server version from INFO using the Valkey Glide client */
int execute_serverversion_command(const void *glide_client, char **output, size_t *output_len)
{
    /* Check if client and output parameters are valid */
    if (!glide_client || !output || !output_len)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1; /* "server" section */
    uintptr_t args[1];
    unsigned long args_len[1];

    /* Set up arguments */
    const char *section = "server";
    args[0] = (uintptr_t)section;
    args_len[0] = strlen(section);

    /* Execute the INFO command */
    CommandResult *result = execute_command(
        glide_client,
        Info,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Check if the command was successful */
    if (!result || result->command_error || !result->response)
    {
        if (result)
            free_command_result(result);
        return 0;
    }

    /* Get the result string */
    if (result->response->response_type != String)
    {
        free_command_result(result);
        return 0;
    }

    /* Parse the INFO response to extract the version */
    const char *info_str = result->response->string_value;
    size_t info_len = result->response->string_value_len;

    /* Look for the line with redis_version */
    const char *redis_version_line = strstr(info_str, "redis_version:");
    if (!redis_version_line)
    {
        free_command_result(result);
        return 0;
    }

    /* Skip past "redis_version:" */
    redis_version_line += 14; // Length of "redis_version:"

    /* Find the end of the line */
    const char *line_end = strchr(redis_version_line, '\r');
    if (!line_end)
        line_end = strchr(redis_version_line, '\n');
    if (!line_end)
        line_end = info_str + info_len;

    /* Extract the version number */
    const char *version_number = redis_version_line;
    while (version_number < line_end && *version_number && !isdigit(*version_number))
    {
        version_number++;
    }

    /* Copy the version number */
    if (version_number < line_end)
    {
        size_t version_len = line_end - version_number;
        *output = (char *)emalloc(version_len + 1);
        if (*output)
        {
            memcpy(*output, version_number, version_len);
            (*output)[version_len] = '\0';
            *output_len = version_len;
        }
    }
    else
    {
        /* Default to "unknown" if we couldn't extract a version */
        const char *default_version = "unknown";
        size_t default_len = strlen(default_version);
        *output = (char *)emalloc(default_len + 1);
        if (*output)
        {
            memcpy(*output, default_version, default_len);
            (*output)[default_len] = '\0';
            *output_len = default_len;
        }
    }

    /* Free the result */
    free_command_result(result);

    /* Check if we successfully allocated memory for the output */
    return (*output != NULL);
}

/* Execute a SCAN command using the Valkey Glide client */
int execute_scan_command(const void *glide_client, long *it, const char *pattern, size_t pattern_len,
                         long count, zval *return_value)
{
    /* Check if client is valid */
    if (!glide_client || !it || !return_value)
    {
        return 0;
    }

    /* Determine number of arguments based on options */
    int has_pattern = (pattern && pattern_len > 0);
    int has_count = (count > 0);
    unsigned long arg_count = 1 + (has_pattern ? 2 : 0) + (has_count ? 2 : 0); /* cursor [MATCH pattern] [COUNT count] */

    /* Prepare command arguments */
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

    /* First argument is the cursor */
    char cursor_str[32];
    snprintf(cursor_str, sizeof(cursor_str), "%ld", *it);
    args[0] = (uintptr_t)cursor_str;
    args_len[0] = strlen(cursor_str);

    /* Add MATCH pattern if provided */
    int arg_idx = 1;
    if (has_pattern)
    {
        args[arg_idx] = (uintptr_t)"MATCH";
        args_len[arg_idx] = 5;
        arg_idx++;

        args[arg_idx] = (uintptr_t)pattern;
        args_len[arg_idx] = pattern_len;
        arg_idx++;
    }

    /* Add COUNT if provided */
    if (has_count)
    {
        args[arg_idx] = (uintptr_t)"COUNT";
        args_len[arg_idx] = 5;
        arg_idx++;

        char count_str[32];
        snprintf(count_str, sizeof(count_str), "%ld", count);
        args[arg_idx] = (uintptr_t)count_str;
        args_len[arg_idx] = strlen(count_str);
        arg_idx++;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Scan,      /* command type */
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

        if (result->response && result->response->response_type == Array)
        {
            /* SCAN returns an array with [cursor, [elements]] */
            if (result->response->array_value_len >= 2)
            {
                /* Get the new cursor */
                CommandResponse *cursor_resp = &result->response->array_value[0];
                if (cursor_resp->response_type == String)
                {
                    /* Convert cursor string to long */
                    *it = atol(cursor_resp->string_value);
                }

                /* Initialize return array */
                array_init(return_value);

                /* Add cursor to return array */
                add_next_index_long(return_value, *it);

                /* Get the elements array */
                CommandResponse *elements_resp = &result->response->array_value[1];
                if (elements_resp->response_type == Array)
                {
                    /* Add elements to return array */
                    zval elements_array;
                    array_init(&elements_array);
                    for (int i = 0; i < elements_resp->array_value_len; i++)
                    {
                        CommandResponse *element = &elements_resp->array_value[i];
                        if (element->response_type == String)
                        {
                            add_next_index_stringl(&elements_array,
                                                   element->string_value,
                                                   element->string_value_len);
                        }
                    }

                    /* Add the elements array to return value */
                    add_next_index_zval(return_value, &elements_array);
                }

                status = 1;
            }
        }
        free_command_result(result);
    }

    return status;
}

/* Execute an SSCAN command using the Valkey Glide client */
int execute_sscan_command(const void *glide_client, const char *key, size_t key_len,
                          long *it, const char *pattern, size_t pattern_len,
                          long count, zval *return_value)
{
    /* Check if client and key are valid */
    return 0;
}
