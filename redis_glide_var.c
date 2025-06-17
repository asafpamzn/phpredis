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
#include "valkey_glide_core_common.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * Function to execute the OBJECT command using Glide client
 * Returns:
 *  1 on success
 *  0 when key doesn't exist
 * -1 on error
 */
int execute_object_command(const void *glide_client,
                           const char *subcommand, size_t subcommand_len,
                           const char *key, size_t key_len,
                           zval *return_value)
{
    CommandResult *result = NULL;
    int ret_val = -1; /* Default to error */

    /* Create command array: ["OBJECT", subcommand, key] */
    uintptr_t args[1];
    unsigned long args_len[1];

    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Select appropriate request type based on subcommand */
    enum RequestType req_type = CustomCommand; /* Default to CustomCommand */

    if (strncasecmp(subcommand, "REFCOUNT", subcommand_len) == 0)
    {
        req_type = ObjectRefCount;
    }
    else if (strncasecmp(subcommand, "IDLETIME", subcommand_len) == 0)
    {
        req_type = ObjectIdleTime;
    }
    else if (strncasecmp(subcommand, "FREQ", subcommand_len) == 0)
    {
        req_type = ObjectFreq;
    }
    else if (strncasecmp(subcommand, "ENCODING", subcommand_len) == 0)
    {
        req_type = ObjectEncoding;
    }
    /* For HELP and other subcommands, use CustomCommand (default) */

    /* Execute the command */
    result = execute_command(glide_client, req_type, 1, args, args_len);
    if (result == NULL)
    {
        return -1;
    }

    /* Handle different result types based on the subcommand */
    if (strncasecmp(subcommand, "REFCOUNT", subcommand_len) == 0 ||
        strncasecmp(subcommand, "IDLETIME", subcommand_len) == 0 ||
        strncasecmp(subcommand, "FREQ", subcommand_len) == 0)
    {
        /* These subcommands return integers */
        if (result->response && result->response->response_type == Int)
        {
            /* Success, set return value */
            ZVAL_LONG(return_value, (long)result->response->int_value);
            ret_val = 1;
        }
        else if (result->response && result->response->response_type == Null)
        {
            /* Key doesn't exist */
            ZVAL_FALSE(return_value);
            ret_val = 0;
        }
    }
    else if (strncasecmp(subcommand, "ENCODING", subcommand_len) == 0)
    {
        /* ENCODING returns a string */
        if (result->response && result->response->response_type == String)
        {
            /* Success, set return value */
            ZVAL_STRINGL(return_value, result->response->string_value, result->response->string_value_len);
            ret_val = 1;
        }
        else if (result->response && result->response->response_type == Null)
        {
            /* Key doesn't exist */
            ZVAL_FALSE(return_value);
            ret_val = 0;
        }
    }
    else if (strncasecmp(subcommand, "HELP", subcommand_len) == 0)
    {
        /* HELP returns an array of strings */
        if (result->response && result->response->response_type == Array)
        {
            if (command_response_to_zval(result->response, return_value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false) == 1)
            {
                ret_val = 1;
            }
            else
            {
                ret_val = -1;
            }
        }
        else
        {
            ret_val = -1;
        }
    }
    else
    {
        /* Unsupported subcommand */
        ret_val = -1;
    }

    /* Clean up */
    free_command_result(result);

    return ret_val;
}
