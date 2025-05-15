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

/* Convert a double value to a string */
static char *double_to_string(double value, size_t *len)
{
    char buffer[64];
    *len = snprintf(buffer, sizeof(buffer), "%.17g", value);
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
    CommandResult *result = command(
        glide_client,
        0,         /* channel */
        BitOp,     /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len,  /* argument lengths */
        NULL,      /* route bytes */
        0          /* route bytes length */
    );

    /* Free the argument arrays */
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
        printf("Error executing BITOP command: %s\n", result->command_error->command_error_message);
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
    CommandResult *result = command(
        glide_client,
        0,         /* channel */
        BitPos,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len,  /* argument lengths */
        NULL,      /* route bytes */
        0          /* route bytes length */
    );

    /* Free the argument strings */
    free(bit_str);
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
        printf("Error executing BITPOS command: %s\n", result->command_error->command_error_message);
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
    CommandResult *cmd_result = command(
        glide_client,
        0,         /* channel */
        Ping,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len,  /* argument lengths */
        NULL,      /* route bytes */
        0          /* route bytes length */
    );

    /* Check if the command was successful */
    if (!cmd_result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (cmd_result->command_error)
    {
        printf("Error executing PING command: %s\n", cmd_result->command_error->command_error_message);
        free_command_result(cmd_result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (cmd_result->response)
    {
        switch (cmd_result->response->response_type)
        {
        case Ok:
            /* PONG response with no message */
            *result = strdup("PONG");
            *result_len = 4;
            ret_val = 1;
            break;
        case String:
            /* PING with message returns the message */
            *result = strdup(cmd_result->response->string_value);
            *result_len = cmd_result->response->string_value_len;
            ret_val = 1;
            break;
        default:
            ret_val = -1;
            break;
        }
    }

    /* Free the result */
    free_command_result(cmd_result);

    return ret_val;
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
    CommandResult *cmd_result = command(
        glide_client,
        0,         /* channel */
        Info,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len,  /* argument lengths */
        NULL,      /* route bytes */
        0          /* route bytes length */
    );

    /* Check if the command was successful */
    if (!cmd_result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (cmd_result->command_error)
    {
        printf("Error executing INFO command: %s\n", cmd_result->command_error->command_error_message);
        free_command_result(cmd_result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (cmd_result->response && cmd_result->response->response_type == String)
    {
        /* INFO returns a string with the server information */
        *result = strdup(cmd_result->response->string_value);
        *result_len = cmd_result->response->string_value_len;
        ret_val = 1;
    }

    /* Free the result */
    free_command_result(cmd_result);

    return ret_val;
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
    CommandResult *cmd_result = command(
        glide_client,
        0,         /* channel */
        GetSet,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len,  /* argument lengths */
        NULL,      /* route bytes */
        0          /* route bytes length */
    );

    /* Check if the command was successful */
    if (!cmd_result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (cmd_result->command_error)
    {
        printf("Error executing GETSET command: %s\n", cmd_result->command_error->command_error_message);
        free_command_result(cmd_result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (cmd_result->response)
    {
        switch (cmd_result->response->response_type)
        {
        case String:
            /* GETSET returns the old value */
            *result = strdup(cmd_result->response->string_value);
            *result_len = cmd_result->response->string_value_len;
            ret_val = 1;
            break;
        case Null:
            /* Key didn't exist, return NULL */
            *result = NULL;
            *result_len = 0;
            ret_val = 0;
            break;
        default:
            ret_val = -1;
            break;
        }
    }

    /* Free the result */
    free_command_result(cmd_result);

    return ret_val;
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
    CommandResult *cmd_result = command(
        glide_client,
        0,         /* channel */
        RandomKey, /* command type */
        0,         /* number of arguments */
        NULL,      /* arguments */
        NULL,      /* argument lengths */
        NULL,      /* route bytes */
        0          /* route bytes length */
    );

    /* Check if the command was successful */
    if (!cmd_result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (cmd_result->command_error)
    {
        printf("Error executing RANDOMKEY command: %s\n", cmd_result->command_error->command_error_message);
        free_command_result(cmd_result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (cmd_result->response)
    {
        switch (cmd_result->response->response_type)
        {
        case String:
            /* RANDOMKEY returns a random key */
            *result = strdup(cmd_result->response->string_value);
            *result_len = cmd_result->response->string_value_len;
            ret_val = 1;
            break;
        case Null:
            /* No keys in the database */
            *result = NULL;
            *result_len = 0;
            ret_val = 0;
            break;
        default:
            ret_val = -1;
            break;
        }
    }

    /* Free the result */
    free_command_result(cmd_result);

    return ret_val;
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
    CommandResult *result = command(
        glide_client,
        0,         /* channel */
        GetBit,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len,  /* argument lengths */
        NULL,      /* route bytes */
        0          /* route bytes length */
    );

    /* Free the argument strings */
    free(offset_str);

    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing GETBIT command: %s\n", result->command_error->command_error_message);
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
    CommandResult *result = command(
        glide_client,
        0,         /* channel */
        SetBit,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len,  /* argument lengths */
        NULL,      /* route bytes */
        0          /* route bytes length */
    );

    /* Free the argument strings */
    free(offset_str);

    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing SETBIT command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Get the result value */
    long value_result = -1;
    if (result->response && result->response->response_type == Int)
    {
        value_result = result->response->int_value;
    }

    /* Free the result */
    free_command_result(result);

    return value_result;
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
    CommandResult *result = command(
        glide_client,
        0,         /* channel */
        Del,       /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len,  /* argument lengths */
        NULL,      /* route bytes */
        0          /* route bytes length */
    );

    /* Free the argument arrays */
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
        printf("Error executing DEL command: %s\n", result->command_error->command_error_message);
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
