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

/* These functions are now defined in command_response.c */

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
    CommandResult *result = execute_command(
        glide_client,
        BitCount,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument strings */
    free(start_str);
    free(end_str);

    /* Use the generic handler to process the result */
    return handle_int_response(result);
}

/* Execute a BITOP command using the Valkey Glide client */
long execute_bitop_command(const void *glide_client, const char *op, size_t op_len, const char *dst, size_t dst_len, zval *keys, int keys_count)
{
    /* Check if client, op, dst, and keys are valid */
    if (!glide_client || !op || !dst || !keys || keys_count <= 0)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2 + keys_count; /* op + dst + keys */
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return -1;
    }

    /* First argument: operation (AND, OR, XOR, NOT) */
    args[0] = (uintptr_t)op;
    args_len[0] = op_len;

    /* Second argument: destination key */
    args[1] = (uintptr_t)dst;
    args_len[1] = dst_len;

    /* Remaining arguments: source keys */
    int i;
    for (i = 0; i < keys_count; i++)
    {
        zval *key = &keys[i];
        if (Z_TYPE_P(key) != IS_STRING)
        {
            free(args);
            free(args_len);
            return -1;
        }
        args[2 + i] = (uintptr_t)Z_STRVAL_P(key);
        args_len[2 + i] = Z_STRLEN_P(key);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        BitOp,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    free(args);
    free(args_len);

    /* Use the generic handler to process the result */
    return handle_int_response(result);
}

/* Execute a BITPOS command using the Valkey Glide client */
long execute_bitpos_command(const void *glide_client, const char *key, size_t key_len, long bit, long start, long end, int bybit)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = bybit ? 5 : 4;
    uintptr_t args[5];
    unsigned long args_len[5];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: bit (0 or 1) */
    size_t bit_len;
    char *bit_str = long_to_string(bit, &bit_len);
    if (!bit_str)
    {
        return -1;
    }
    args[1] = (uintptr_t)bit_str;
    args_len[1] = bit_len;

    /* Third argument: start */
    size_t start_len;
    char *start_str = long_to_string(start, &start_len);
    if (!start_str)
    {
        free(bit_str);
        return -1;
    }
    args[2] = (uintptr_t)start_str;
    args_len[2] = start_len;

    /* Fourth argument: end */
    size_t end_len;
    char *end_str = long_to_string(end, &end_len);
    if (!end_str)
    {
        free(bit_str);
        free(start_str);
        return -1;
    }
    args[3] = (uintptr_t)end_str;
    args_len[3] = end_len;

    /* Fifth argument: BYTE|BIT (if bybit is true) */
    if (bybit)
    {
        args[4] = (uintptr_t)"BIT";
        args_len[4] = 3;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        BitPos,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument strings */
    free(bit_str);
    free(start_str);
    free(end_str);

    /* Use the generic handler to process the result */
    return handle_int_response(result);
}

/* Execute a SET command using the Valkey Glide client */
int execute_set_command(const void *glide_client, const char *key, size_t key_len, const char *val, size_t val_len, long expire, zval *opts)
{
    /* Check if client, key, and value are valid */
    if (!glide_client || !key || !val)
    {
        return -1;
    }

    /* Count the number of arguments */
    unsigned long arg_count = 2; /* key + value */
    int has_ex = 0, has_px = 0, has_nx = 0, has_xx = 0, has_get = 0, has_keepttl = 0;

    /* Check if we have an expiry time */
    if (expire > 0)
    {
        arg_count += 2; /* EX/PX + seconds/milliseconds */
        has_ex = 1;     /* Default to EX (seconds) */
    }

    /* Check if we have options */
    if (opts && Z_TYPE_P(opts) == IS_ARRAY)
    {
        zval *z_nx = zend_hash_str_find(Z_ARRVAL_P(opts), "nx", sizeof("nx") - 1);
        zval *z_xx = zend_hash_str_find(Z_ARRVAL_P(opts), "xx", sizeof("xx") - 1);
        zval *z_get = zend_hash_str_find(Z_ARRVAL_P(opts), "get", sizeof("get") - 1);
        zval *z_ex = zend_hash_str_find(Z_ARRVAL_P(opts), "ex", sizeof("ex") - 1);
        zval *z_px = zend_hash_str_find(Z_ARRVAL_P(opts), "px", sizeof("px") - 1);
        zval *z_keepttl = zend_hash_str_find(Z_ARRVAL_P(opts), "keepttl", sizeof("keepttl") - 1);

        /* NX option */
        if (z_nx && Z_TYPE_P(z_nx) == IS_TRUE)
        {
            arg_count++;
            has_nx = 1;
        }

        /* XX option */
        if (z_xx && Z_TYPE_P(z_xx) == IS_TRUE)
        {
            arg_count++;
            has_xx = 1;
        }

        /* GET option */
        if (z_get && Z_TYPE_P(z_get) == IS_TRUE)
        {
            arg_count++;
            has_get = 1;
        }

        /* EX option */
        if (z_ex && Z_TYPE_P(z_ex) == IS_LONG)
        {
            arg_count += 2;
            has_ex = 1;
            expire = Z_LVAL_P(z_ex);
        }

        /* PX option */
        if (z_px && Z_TYPE_P(z_px) == IS_LONG)
        {
            arg_count += 2;
            has_px = 1;
            expire = Z_LVAL_P(z_px);
        }

        /* KEEPTTL option */
        if (z_keepttl && Z_TYPE_P(z_keepttl) == IS_TRUE)
        {
            arg_count++;
            has_keepttl = 1;
        }
    }

    /* Allocate memory for arguments */
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return -1;
    }

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: value */
    args[1] = (uintptr_t)val;
    args_len[1] = val_len;

    /* Current argument index */
    int arg_idx = 2;

    /* Add EX/PX option if we have an expiry time */
    if (has_ex || has_px)
    {
        /* Add EX/PX keyword */
        args[arg_idx] = (uintptr_t)(has_ex ? "EX" : "PX");
        args_len[arg_idx] = 2;
        arg_idx++;

        /* Add expiry time */
        size_t expire_len;
        char *expire_str = long_to_string(expire, &expire_len);
        if (!expire_str)
        {
            free(args);
            free(args_len);
            return -1;
        }
        args[arg_idx] = (uintptr_t)expire_str;
        args_len[arg_idx] = expire_len;
        arg_idx++;
    }

    /* Add NX option */
    if (has_nx)
    {
        args[arg_idx] = (uintptr_t)"NX";
        args_len[arg_idx] = 2;
        arg_idx++;
    }

    /* Add XX option */
    if (has_xx)
    {
        args[arg_idx] = (uintptr_t)"XX";
        args_len[arg_idx] = 2;
        arg_idx++;
    }

    /* Add GET option */
    if (has_get)
    {
        args[arg_idx] = (uintptr_t)"GET";
        args_len[arg_idx] = 3;
        arg_idx++;
    }

    /* Add KEEPTTL option */
    if (has_keepttl)
    {
        args[arg_idx] = (uintptr_t)"KEEPTTL";
        args_len[arg_idx] = 7;
        arg_idx++;
    }

    /* Execute the command */
    CommandResult *result = command(
        glide_client,
        0,         /* channel */
        Set,       /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len,  /* argument lengths */
        NULL,      /* route bytes */
        0          /* route bytes length */
    );

    /* Free the argument arrays */
    if (has_ex || has_px)
    {
        free((void *)args[3]); /* Free the expire string */
    }
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing SET command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (result->response)
    {
        switch (result->response->response_type)
        {
        case Ok:
            ret_val = 1; /* Success */
            break;
        case Null:
            ret_val = 0; /* Not set (NX/XX condition not met) */
            break;
        case String:
            ret_val = 2; /* GET option returned a value */
            break;
        default:
            ret_val = -1; /* Error */
            break;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Execute a SETEX command using the Valkey Glide client */
int execute_setex_command(const void *glide_client, const char *key, size_t key_len, long expire, const char *val, size_t val_len)
{
    printf("Deprectaed\n");
    return -1;
}

/* Execute a PSETEX command using the Valkey Glide client */
int execute_psetex_command(const void *glide_client, const char *key, size_t key_len, long expire, const char *val, size_t val_len)
{
    printf("Deprectaed\n");
    return -1;
}

/* Execute a SETNX command using the Valkey Glide client */
int execute_setnx_command(const void *glide_client, const char *key, size_t key_len, const char *val, size_t val_len)
{
    printf("Deprectaed\n");
    return -1;
}

/* Function to close a Valkey Glide client */
void close_glide_client(const void *glide_client)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return;
    }

    /* Close the client using the close_client function from glide_bindings.h */
    close_client(glide_client);
}

/* Execute a PING command using the Valkey Glide client */
int execute_ping_command(const void *glide_client, const char *msg, size_t msg_len, char **result, size_t *result_len)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = msg ? 1 : 0;
    uintptr_t args[1];
    unsigned long args_len[1];

    /* Add message argument if provided */
    if (msg)
    {
        args[0] = (uintptr_t)msg;
        args_len[0] = msg_len;
    }

    /* Execute the command */
    CommandResult *cmd_result = execute_command(
        glide_client,
        Ping,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Special handling for PING command */
    if (cmd_result && cmd_result->response && cmd_result->response->response_type == Ok)
    {
        /* PONG response with no message */
        *result = strdup("PONG");
        *result_len = 4;
        free_command_result(cmd_result);
        return 1;
    }
    else
    {
        /* Use the generic handler for string responses */
        return handle_string_response(cmd_result, result, result_len);
    }
}

/* Execute an INFO command using the Valkey Glide client */
int execute_info_command(const void *glide_client, const char *section, size_t section_len, char **result, size_t *result_len)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = section ? 1 : 0;
    uintptr_t args[1];
    unsigned long args_len[1];

    /* Add section argument if provided */
    if (section)
    {
        args[0] = (uintptr_t)section;
        args_len[0] = section_len;
    }

    /* Execute the command */
    CommandResult *cmd_result = execute_command(
        glide_client,
        Info,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_string_response(cmd_result, result, result_len);
}

/* Execute a GETSET command using the Valkey Glide client */
int execute_getset_command(const void *glide_client, const char *key, size_t key_len, const char *val, size_t val_len, char **result, size_t *result_len)
{
    /* Check if client, key, and value are valid */
    if (!glide_client || !key || !val)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2; /* key + value */
    uintptr_t args[2];
    unsigned long args_len[2];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: value */
    args[1] = (uintptr_t)val;
    args_len[1] = val_len;

    /* Execute the command */
    CommandResult *cmd_result = execute_command(
        glide_client,
        GetSet,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_string_response(cmd_result, result, result_len);
}

/* Execute a GET command using the Valkey Glide client */
int execute_get_command(const void *glide_client, const char *key, size_t key_len, char **result, size_t *result_len)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1; /* key */
    uintptr_t args[1];
    unsigned long args_len[1];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Execute the command */
    CommandResult *cmd_result = execute_command(
        glide_client,
        Get,       /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_string_response(cmd_result, result, result_len);
}

/* Execute a RANDOMKEY command using the Valkey Glide client */
int execute_randomkey_command(const void *glide_client, char **result, size_t *result_len)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return -1;
    }

    /* Execute the command (no arguments needed) */
    CommandResult *cmd_result = execute_command(
        glide_client,
        RandomKey, /* command type */
        0,         /* number of arguments */
        NULL,      /* arguments */
        NULL       /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_string_response(cmd_result, result, result_len);
}

/* Execute a GETBIT command using the Valkey Glide client */
long execute_getbit_command(const void *glide_client, const char *key, size_t key_len, long offset)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2;
    uintptr_t args[2];
    unsigned long args_len[2];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: offset */
    size_t offset_len;
    char *offset_str = long_to_string(offset, &offset_len);
    if (!offset_str)
    {
        return -1;
    }
    args[1] = (uintptr_t)offset_str;
    args_len[1] = offset_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        GetBit,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument strings */
    free(offset_str);

    /* Use the generic handler to process the result */
    return handle_int_response(result);
}

/* Execute a SETBIT command using the Valkey Glide client */
long execute_setbit_command(const void *glide_client, const char *key, size_t key_len, long offset, int value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3;
    uintptr_t args[3];
    unsigned long args_len[3];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: offset */
    size_t offset_len;
    char *offset_str = long_to_string(offset, &offset_len);
    if (!offset_str)
    {
        return -1;
    }
    args[1] = (uintptr_t)offset_str;
    args_len[1] = offset_len;

    /* Third argument: value (0 or 1) */
    char value_str[2] = {'0', '\0'};
    if (value)
        value_str[0] = '1';
    args[2] = (uintptr_t)value_str;
    args_len[2] = 1;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SetBit,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument strings */
    free(offset_str);

    /* Use the generic handler to process the result */
    return handle_int_response(result);
}

/* Execute a DEL command using the Valkey Glide client */
long execute_del_command(const void *glide_client, zval *keys, int keys_count)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys || keys_count <= 0)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = keys_count;
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return -1;
    }

    /* Add keys as arguments */
    int i;
    for (i = 0; i < keys_count; i++)
    {
        zval *key = &keys[i];
        if (Z_TYPE_P(key) != IS_STRING)
        {
            free(args);
            free(args_len);
            return -1;
        }
        args[i] = (uintptr_t)Z_STRVAL_P(key);
        args_len[i] = Z_STRLEN_P(key);
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Del,       /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    free(args);
    free(args_len);

    /* Use the generic handler to process the result */
    return handle_int_response(result);
}

/* Execute a STRLEN command using the Valkey Glide client */
long execute_strlen_command(const void *glide_client, const char *key, size_t key_len)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1;
    uintptr_t args[1];
    unsigned long args_len[1];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Strlen,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_int_response(result);
}

/* Execute a SETRANGE command using the Valkey Glide client */
long execute_setrange_command(const void *glide_client, const char *key, size_t key_len, long offset, const char *value, size_t value_len)
{
    /* Check if client, key, and value are valid */
    if (!glide_client || !key || !value)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 3;
    uintptr_t args[3];
    unsigned long args_len[3];

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Second argument: offset */
    size_t offset_len;
    char *offset_str = long_to_string(offset, &offset_len);
    if (!offset_str)
    {
        return -1;
    }
    args[1] = (uintptr_t)offset_str;
    args_len[1] = offset_len;

    /* Third argument: value */
    args[2] = (uintptr_t)value;
    args_len[2] = value_len;

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        SetRange,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument strings */
    free(offset_str);

    /* Use the generic handler to process the result */
    return handle_int_response(result);
}

/* Helper function to prepare arguments for MPOP commands */
static int prepare_mpop_arguments(
    const void *glide_client,
    int is_blocking,
    double timeout,
    zval *keys,
    const char *from,
    size_t from_len,
    long count,
    unsigned long *arg_count_ptr,
    uintptr_t **args_ptr,
    unsigned long **args_len_ptr,
    char **numkeys_str_ptr,
    char **timeout_str_ptr,
    char **count_str_ptr)
{
    /* Get the number of keys */
    int keys_count = 0;
    if (Z_TYPE_P(keys) == IS_ARRAY)
    {
        keys_count = zend_hash_num_elements(Z_ARRVAL_P(keys));
    }
    else
    {
        return -1; /* Keys must be an array */
    }

    /* Check if we have at least one key */
    if (keys_count <= 0)
    {
        return -1;
    }

    /* Calculate the number of arguments */
    unsigned long arg_count = keys_count + 2; /* numkeys + keys + direction */
    if (is_blocking)
    {
        arg_count++; /* Add timeout for blocking commands */
    }

    /* Allocate memory for arguments */
    uintptr_t *args = (uintptr_t *)malloc(arg_count * sizeof(uintptr_t));
    unsigned long *args_len = (unsigned long *)malloc(arg_count * sizeof(unsigned long));

    if (!args || !args_len)
    {
        if (args)
            free(args);
        if (args_len)
            free(args_len);
        return -1;
    }

    /* Current argument index */
    int arg_idx = 0;

    /* Add timeout for blocking commands */
    if (is_blocking)
    {
        /* Convert timeout to string */
        size_t timeout_len;
        char *timeout_str = double_to_string(timeout, &timeout_len);
        if (!timeout_str)
        {
            free(args);
            free(args_len);
            return -1;
        }
        args[arg_idx] = (uintptr_t)timeout_str;
        args_len[arg_idx] = timeout_len;
        *timeout_str_ptr = timeout_str;
        arg_idx++;
    }

    /* Add numkeys first (this should be the first argument after timeout for blocking commands) */
    size_t numkeys_len;
    char *numkeys_str = long_to_string(keys_count, &numkeys_len);
    if (!numkeys_str)
    {
        free(args);
        free(args_len);
        if (is_blocking)
        {
            free(*timeout_str_ptr);
            *timeout_str_ptr = NULL;
        }
        return -1;
    }
    args[arg_idx] = (uintptr_t)numkeys_str;
    args_len[arg_idx] = numkeys_len;
    *numkeys_str_ptr = numkeys_str;
    arg_idx++;

    /* Add keys */
    HashTable *ht = Z_ARRVAL_P(keys);
    zval *z_key;
    ZEND_HASH_FOREACH_VAL(ht, z_key)
    {
        if (Z_TYPE_P(z_key) != IS_STRING)
        {
            free(args);
            free(args_len);
            free(numkeys_str);
            *numkeys_str_ptr = NULL;
            if (is_blocking)
            {
                free(*timeout_str_ptr);
                *timeout_str_ptr = NULL;
            }
            return -1;
        }
        args[arg_idx] = (uintptr_t)Z_STRVAL_P(z_key);
        args_len[arg_idx] = Z_STRLEN_P(z_key);
        arg_idx++;
    }
    ZEND_HASH_FOREACH_END();

    /* Add direction (LEFT or RIGHT) directly */
    args[arg_idx] = (uintptr_t)from;
    args_len[arg_idx] = from_len;
    arg_idx++;

    /* Add COUNT if count > 1 */
    if (count > 1)
    {
        /* Increase arg_count for COUNT and its value */
        arg_count += 2;

        /* Reallocate args and args_len arrays */
        uintptr_t *new_args = (uintptr_t *)realloc(args, arg_count * sizeof(uintptr_t));
        unsigned long *new_args_len = (unsigned long *)realloc(args_len, arg_count * sizeof(unsigned long));

        if (!new_args || !new_args_len)
        {
            free(args);
            free(args_len);
            free(numkeys_str);
            *numkeys_str_ptr = NULL;
            if (is_blocking)
            {
                free(*timeout_str_ptr);
                *timeout_str_ptr = NULL;
            }
            return -1;
        }

        args = new_args;
        args_len = new_args_len;

        /* Add COUNT keyword */
        args[arg_idx] = (uintptr_t)"COUNT";
        args_len[arg_idx] = 5;
        arg_idx++;

        /* Add count value */
        size_t count_len;
        char *count_str = long_to_string(count, &count_len);
        if (!count_str)
        {
            free(args);
            free(args_len);
            free(numkeys_str);
            *numkeys_str_ptr = NULL;
            if (is_blocking)
            {
                free(*timeout_str_ptr);
                *timeout_str_ptr = NULL;
            }
            return -1;
        }
        args[arg_idx] = (uintptr_t)count_str;
        args_len[arg_idx] = count_len;
        *count_str_ptr = count_str;
        arg_idx++;
    }

    /* Set output parameters */
    *arg_count_ptr = arg_count;
    *args_ptr = args;
    *args_len_ptr = args_len;

    return keys_count;
}

/* Process list elements from LMPOP/BLMPOP response */
static void process_list_elements(struct CommandResponse *elements_resp, zval *elements_array)
{
    /* For lists, elements are just values */
    for (int i = 0; i < elements_resp->array_value_len; i++)
    {
        struct CommandResponse *element = &elements_resp->array_value[i];
        if (element->response_type == String)
        {
            add_next_index_stringl(elements_array,
                                   element->string_value,
                                   element->string_value_len);
        }
    }
}

/* Process sorted set elements from ZMPOP/BZMPOP response */
static void process_sorted_set_elements(struct CommandResponse *elements_resp, zval *elements_array)
{
    /* For sorted sets, elements are pairs of member and score */
    for (int i = 0; i < elements_resp->array_value_len; i += 2)
    {
        if (i + 1 < elements_resp->array_value_len)
        {
            struct CommandResponse *member = &elements_resp->array_value[i];
            struct CommandResponse *score = &elements_resp->array_value[i + 1];

            if (member->response_type == String && score->response_type == String)
            {
                /* Convert score string to double */
                double score_val = atof(score->string_value);

                /* Add member => score pair to elements array */
                add_assoc_double_ex(elements_array,
                                    member->string_value,
                                    member->string_value_len,
                                    score_val);
            }
        }
    }
}

/* Execute an LMPOP or BLMPOP command (for list operations) using the Valkey Glide client */
int execute_lmpop_command(const void *glide_client, const char *cmd, double timeout, zval *keys, const char *from, size_t from_len, long count, zval *result)
{
    printf("Exectute lmpop command\n");
    /* Check if client, keys, and from are valid */
    if (!glide_client || !keys || !from)
    {
        return -1;
    }

    /* Determine if this is a blocking command */
    int is_blocking = (strncmp(cmd, "B", 1) == 0);

    /* Prepare for argument construction */
    unsigned long arg_count = 0;
    uintptr_t *args = NULL;
    unsigned long *args_len = NULL;
    char *numkeys_str = NULL;
    char *timeout_str = NULL;
    char *count_str = NULL;

    /* Prepare the arguments */
    int keys_count = prepare_mpop_arguments(
        glide_client, is_blocking, timeout, keys, from, from_len, count,
        &arg_count, &args, &args_len,
        &numkeys_str, &timeout_str, &count_str);

    if (keys_count < 0)
    {
        return -1;
    }

    /* Determine the command type */
    enum RequestType cmd_type = is_blocking ? BLMPop : LMPop;

    /* Execute the command */
    CommandResult *cmd_result = command(
        glide_client,
        0,         /* channel */
        cmd_type,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len,  /* argument lengths */
        NULL,      /* route bytes */
        0          /* route bytes length */
    );

    /* Free the argument strings */
    if (numkeys_str)
        free(numkeys_str);
    if (timeout_str)
        free(timeout_str);
    if (count_str)
        free(count_str);
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!cmd_result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (cmd_result->command_error)
    {
        printf("Error executing %s command: %s\n", cmd, cmd_result->command_error->command_error_message);
        free_command_result(cmd_result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (cmd_result->response)
    {
        printf("Response type: %d\n", cmd_result->response->response_type);
        switch (cmd_result->response->response_type)
        {
        case Null:
            /* No elements popped */
            ZVAL_NULL(result);
            ret_val = 0;
            break;
        case Map:
            /* Elements popped */
            array_init(result);

            /* Process the map response - we need to format it as a specific array structure */
            if (cmd_result->response->array_value_len >= 1)
            {
                printf("cmd_result->response->map_key = %p\n", cmd_result->response->map_key);
                if (cmd_result->response->map_key == NULL)
                {
                    ZVAL_NULL(result);
                    ret_val = -1;
                    break;
                }
                /* First element is the key */
                struct CommandResponse *key_resp = &cmd_result->response->map_key[0];
                if (key_resp->response_type == String)
                {
                    /* Add key to result array as first element */
                    add_next_index_stringl(result, key_resp->string_value, key_resp->string_value_len);

                    /* Second element is the array of popped elements */
                    struct CommandResponse *elements_resp = &cmd_result->response->map_value[0];
                    if (elements_resp->response_type == Array)
                    {
                        /* Create array for elements */
                        zval elements_array;
                        array_init(&elements_array);

                        /* Use command_response_to_zval to process each list element */
                        for (int i = 0; i < elements_resp->array_value_len; i++)
                        {
                            zval element;
                            struct CommandResponse *elem_resp = &elements_resp->array_value[i];
                            command_response_to_zval(elem_resp, &element);
                            add_next_index_zval(&elements_array, &element);
                        }

                        /* Add elements array to result */
                        add_next_index_zval(result, &elements_array);
                    }
                }
            }
            ret_val = 1;
            break;
        default:
            /* Unexpected response type */
            ZVAL_NULL(result);
            ret_val = -1;
            break;
        }
    }

    /* Free the result */
    free_command_result(cmd_result);

    return ret_val;
}

/* Execute a ZMPOP or BZMPOP command (for sorted set operations) using the Valkey Glide client */
int execute_zmpop_command(const void *glide_client, const char *cmd, double timeout, zval *keys, const char *from, size_t from_len, long count, zval *result)
{
    /* Check if client, keys, and from are valid */
    if (!glide_client || !keys || !from)
    {
        return -1;
    }

    /* Determine if this is a blocking command */
    int is_blocking = (strncmp(cmd, "B", 1) == 0);

    /* Prepare for argument construction */
    unsigned long arg_count = 0;
    uintptr_t *args = NULL;
    unsigned long *args_len = NULL;
    char *numkeys_str = NULL;
    char *timeout_str = NULL;
    char *count_str = NULL;

    /* Prepare the arguments */
    int keys_count = prepare_mpop_arguments(
        glide_client, is_blocking, timeout, keys, from, from_len, count,
        &arg_count, &args, &args_len,
        &numkeys_str, &timeout_str, &count_str);

    if (keys_count < 0)
    {
        return -1;
    }

    /* Determine the command type */
    enum RequestType cmd_type = is_blocking ? BZMPop : ZMPop;

    /* Execute the command */
    CommandResult *cmd_result = command(
        glide_client,
        0,         /* channel */
        cmd_type,  /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len,  /* argument lengths */
        NULL,      /* route bytes */
        0          /* route bytes length */
    );

    /* Free the argument strings */
    if (numkeys_str)
        free(numkeys_str);
    if (timeout_str)
        free(timeout_str);
    if (count_str)
        free(count_str);
    free(args);
    free(args_len);

    /* Check if the command was successful */
    if (!cmd_result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (cmd_result->command_error)
    {
        printf("Error executing %s command: %s\n", cmd, cmd_result->command_error->command_error_message);
        free_command_result(cmd_result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (cmd_result->response)
    {
        switch (cmd_result->response->response_type)
        {
        case Null:
            /* No elements popped */
            ZVAL_NULL(result);
            ret_val = 0;
            break;
        case Array:
            /* Elements popped */
            array_init(result);

            /* Process the array response */
            if (cmd_result->response->array_value_len >= 2)
            {
                /* First element is the key */
                struct CommandResponse *key_resp = &cmd_result->response->array_value[0];
                if (key_resp->response_type == String)
                {
                    /* Add key to result array */
                    add_next_index_stringl(result, key_resp->string_value, key_resp->string_value_len);

                    /* Second element is the array of popped elements */
                    struct CommandResponse *elements_resp = &cmd_result->response->array_value[1];
                    if (elements_resp->response_type == Array)
                    {
                        /* Create array for elements */
                        zval elements_array;
                        array_init(&elements_array);

                        /* Process sorted set elements */
                        process_sorted_set_elements(elements_resp, &elements_array);

                        /* Add elements array to result */
                        add_next_index_zval(result, &elements_array);
                    }
                }
            }
            ret_val = 1;
            break;
        default:
            /* Unexpected response type */
            ZVAL_NULL(result);
            ret_val = -1;
            break;
        }
    }

    /* Free the result */
    free_command_result(cmd_result);

    return ret_val;
}

/* Execute an MPOP command (LMPOP, BLMPOP, ZMPOP, BZMPOP) using the Valkey Glide client */
int execute_mpop_command(const void *glide_client, const char *cmd, double timeout, zval *keys, const char *from, size_t from_len, long count, zval *result)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return -1;
    }

    /* Check for list-based commands (LMPOP, BLMPOP) */
    if (strcmp(cmd, "LMPOP") == 0 || strcmp(cmd, "BLMPOP") == 0)
    {
        return execute_lmpop_command(glide_client, cmd, timeout, keys, from, from_len, count, result);
    }
    /* Check for sorted set-based commands (ZMPOP, BZMPOP) */
    else if (strcmp(cmd, "ZMPOP") == 0 || strcmp(cmd, "BZMPOP") == 0)
    {
        return execute_zmpop_command(glide_client, cmd, timeout, keys, from, from_len, count, result);
    }
    /* Unknown command type */
    else
    {
        return -1;
    }
}
