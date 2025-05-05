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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Create a connection request in protobuf format */
static uint8_t *create_connection_request(const char *host, int port, const char *user, const char *pass, size_t *len)
{

    return 0;
}

/* Create a Valkey Glide client */
void *create_glide_client(const char *host, int port, const char *user, const char *pass)
{
    /* Check if host is provided */
    if (!host)
    {
        return NULL;
    }

    /* Create a synchronous client type */
    ClientType client_type;
    client_type.tag = SyncClient; /* SyncClient */

    /* Create a connection request with host, port, user, and pass */
    size_t request_len;
    uint8_t *request_bytes = create_connection_request(host, port, user, pass, &request_len);
    if (!request_bytes)
    {
        return NULL;
    }

    printf("Creating Valkey Glide client for %s:%d\n", host, port);

    /* Call the FFI function to create a client */
    ConnectionResponse *response = create_client(request_bytes, request_len, &client_type, NULL);

    /* Free the request bytes */
    free(request_bytes);

    /* Check if the connection was successful */
    if (!response)
    {
        return NULL;
    }

    /* Check if there was an error */
    if (response->connection_error_message)
    {
        printf("Error creating Valkey Glide client: %s\n", response->connection_error_message);
        free_connection_response(response);
        return NULL;
    }

    /* Get the client pointer */
    void *client = response->conn_ptr;

    /* Free the response */
    free_connection_response(response);

    return client;
}

/* Convert a long value to a string */
static char *long_to_string(long value, size_t *len)
{
    char buffer[32];
    *len = snprintf(buffer, sizeof(buffer), "%ld", value);
    char *str = (char *)malloc(*len + 1);
    if (str)
    {
        memcpy(str, buffer, *len);
        str[*len] = '\0';
    }
    return str;
}

/* Execute a BITCOUNT command using the Valkey Glide client */
long execute_bitcount_command(void *glide_client, const char *key, size_t key_len, long start, long end, int bybit)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return -1;
    }

    printf("Executing BITCOUNT command on key '%.*s' with range [%ld,%ld]\n", (int)key_len, key, start, end);

    /* Execute the command */
    CommandResult *result = NULL;
    ; // command(glide_client, 0, BitCount, arg_count, args, args_len, NULL, 0);

    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing BITCOUNT command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Get the result value */
    /* long value = -1;
     if (result->response && result->response->response_type == RESPONSE_INT)
     {
         value = result->response->int_value;
     }
 */
    /* Free the result */
    /* free_command_result(result);*/

    return 42;
}
