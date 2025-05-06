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
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Create a connection request in protobuf format */
static uint8_t *create_connection_request(const char *host, int port, const char *user, const char *pass, size_t *len)
{
    /* Create a connection request */
    ConnectionRequest__ConnectionRequest conn_req = CONNECTION_REQUEST__CONNECTION_REQUEST__INIT;

    /* Set up the node address */
    ConnectionRequest__NodeAddress node_addr = CONNECTION_REQUEST__NODE_ADDRESS__INIT;
    node_addr.host = (char *)host;
    node_addr.port = port;

    /* Add the node address to the connection request */
    ConnectionRequest__NodeAddress *addresses[1] = {&node_addr};
    conn_req.n_addresses = 1;
    conn_req.addresses = addresses;

    /* Set up authentication if provided */
    ConnectionRequest__AuthenticationInfo auth_info = CONNECTION_REQUEST__AUTHENTICATION_INFO__INIT;
    if (user && pass)
    {
        auth_info.username = (char *)user;
        auth_info.password = (char *)pass;
        conn_req.authentication_info = &auth_info;
    }

    /* Set default values */
    conn_req.tls_mode = CONNECTION_REQUEST__TLS_MODE__NoTls;
    conn_req.cluster_mode_enabled = false;
    conn_req.request_timeout = 5000; /* 5 seconds */
    conn_req.read_from = CONNECTION_REQUEST__READ_FROM__Primary;
    conn_req.database_id = 0;
    conn_req.protocol = CONNECTION_REQUEST__PROTOCOL_VERSION__RESP3;

    /* Set client name if needed */
    conn_req.client_name = "phpredis-glide";

    /* Calculate the size of the serialized message */
    *len = connection_request__connection_request__get_packed_size(&conn_req);

    /* Allocate memory for the serialized message */
    uint8_t *buffer = (uint8_t *)malloc(*len);
    if (!buffer)
    {
        *len = 0;
        return NULL;
    }

    /* Serialize the message */
    connection_request__connection_request__pack(&conn_req, buffer);

    return buffer;
}

/* Create a Valkey Glide client */
const void *create_glide_client(ClientConfig *config)
{
    /* Create a connection request */
    size_t len;
    uint8_t *request_bytes = create_connection_request(
        "localhost", /* Default host */
        6379,        /* Default port */
        NULL,        /* No username by default */
        NULL,        /* No password by default */
        &len);

    if (!request_bytes)
    {
        return NULL;
    }

    /* Set up client type for synchronous operation */
    ClientType client_type;
    client_type.tag = SyncClient;

    /* Create the client */
    const ConnectionResponse *conn_resp = create_client(
        request_bytes,
        len,
        &client_type,
        NULL /* No PubSub callback */
    );

    /* Free the request bytes as they're no longer needed */
    free(request_bytes);

    /* Check if there was an error */
    if (conn_resp->connection_error_message)
    {
        printf("Error creating client: %s\n", conn_resp->connection_error_message);
        free_connection_response((ConnectionResponse *)conn_resp);
        return NULL;
    }

    /* Get the client pointer */
    const void *client = conn_resp->conn_ptr;

    /* Free the connection response (but not the client) */
    free_connection_response((ConnectionResponse *)conn_resp);

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
long execute_bitcount_command(const void *glide_client, const char *key, size_t key_len, long start, long end, int bybit)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = bybit ? 4 : 3;
    uintptr_t args[4];
    unsigned long args_len[4];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: start */
    size_t start_len;
    char *start_str = long_to_string(start, &start_len);
    if (!start_str)
    {
        return -1;
    }
    args[1] = (uintptr_t)start_str;
    args_len[1] = start_len;

    /* Third argument: end */
    size_t end_len;
    char *end_str = long_to_string(end, &end_len);
    if (!end_str)
    {
        free(start_str);
        return -1;
    }
    args[2] = (uintptr_t)end_str;
    args_len[2] = end_len;

    /* Fourth argument: BYTE|BIT (if bybit is true) */
    if (bybit)
    {
        args[3] = (uintptr_t)"BIT";
        args_len[3] = 3;
    }

    /* Execute the command */
    CommandResult *result = command(
        glide_client,
        0,         /* channel */
        BitCount,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len,  /* argument lengths */
        NULL,      /* route bytes */
        0          /* route bytes length */
    );

    /* Free the argument strings */
    free(start_str);
    free(end_str);

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
    long value = -1;
    if (result->response && result->response->response_type == Int)
    {
        value = result->response->int_value;
    }

    /* Free the result */
    free_command_result(result);

    return value;
}

/* Close a Valkey Glide client */
void close_glide_client(const void *glide_client)
{
    if (glide_client)
    {
        close_client(glide_client);
    }
}
