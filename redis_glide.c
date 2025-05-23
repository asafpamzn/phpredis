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
    uint8_t *buffer = (uint8_t *)emalloc(*len);
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
    efree(request_bytes);

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
int execute_bitcount_command(const void *glide_client, const char *key, size_t key_len, long start, long end, int bybit, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
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
        return 0;
    }
    args[1] = (uintptr_t)start_str;
    args_len[1] = start_len;

    /* Third argument: end */
    size_t end_len;
    char *end_str = long_to_string(end, &end_len);
    if (!end_str)
    {
        efree(start_str);
        return 0;
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
    efree(start_str);
    efree(end_str);

    /* Use the generic handler to process the result using the new signature */
    return handle_int_response(result, output_value);
}

/* Execute a BITOP command using the Valkey Glide client */
int execute_bitop_command(const void *glide_client, const char *op, size_t op_len, const char *dst, size_t dst_len, zval *keys, int keys_count, long *output_value)
{
    /* Check if client, op, dst, and keys are valid */
    if (!glide_client || !op || !dst || !keys || keys_count <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 2 + keys_count; /* op + dst + keys */
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
            efree(args);
            efree(args_len);
            return 0;
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
    efree(args);
    efree(args_len);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute a BITPOS command using the Valkey Glide client */
int execute_bitpos_command(const void *glide_client, const char *key, size_t key_len, long bit, long start, long end, int bybit, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0; /* False - failure */
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
        return 0; /* False - failure */
    }
    args[1] = (uintptr_t)bit_str;
    args_len[1] = bit_len;

    /* Third argument: start */
    size_t start_len;
    char *start_str = long_to_string(start, &start_len);
    if (!start_str)
    {
        efree(bit_str);
        return 0; /* False - failure */
    }
    args[2] = (uintptr_t)start_str;
    args_len[2] = start_len;

    /* Fourth argument: end */
    size_t end_len;
    char *end_str = long_to_string(end, &end_len);
    if (!end_str)
    {
        efree(bit_str);
        efree(start_str);
        return 0; /* False - failure */
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
    efree(bit_str);
    efree(start_str);
    efree(end_str);

    /* Use the generic handler to process the result with output parameter */
    return handle_int_response(result, output_value);
}

/* Execute a SET command using the Valkey Glide client */
int execute_set_command(const void *glide_client, const char *key, size_t key_len, const char *val, size_t val_len, long expire, zval *opts, char **old_val, size_t *old_val_len)
{
    /* Check if client, key, and value are valid */
    if (!glide_client || !key || !val)
    {
        return 0;
    }

    /* Initialize output parameters */
    if (old_val)
    {
        *old_val = NULL;
    }
    if (old_val_len)
    {
        *old_val_len = 0;
    }

    /* Count the number of arguments */
    unsigned long arg_count = 2; /* key + value */
    int has_ex = 0, has_px = 0, has_exat = 0, has_pxat = 0;
    int has_nx = 0, has_xx = 0, has_get = 0, has_keepttl = 0, has_ifeq = 0;
    char *ifeq_value = NULL;
    size_t ifeq_len = 0;

    /* Check if we have an expiry time */
    if (expire > 0)
    {
        arg_count += 2; /* EX/PX + seconds/milliseconds */
        has_ex = 1;     /* Default to EX (seconds) */
    }

    /* Check if we have options */
    if (opts && Z_TYPE_P(opts) == IS_ARRAY)
    {
        HashTable *options_ht = Z_ARRVAL_P(opts);
        zval *z_option;
        zend_string *option_key;
        zend_ulong num_key;

        /* Iterate through all options */
        ZEND_HASH_FOREACH_KEY_VAL(options_ht, num_key, option_key, z_option)
        {
            if (option_key == NULL)
            {
                /* Handle numeric keys - these are option flags without values */
                if (Z_TYPE_P(z_option) == IS_STRING)
                {
                    zend_string *opt_str = Z_STR_P(z_option);
                    char *opt = ZSTR_VAL(opt_str);
                    size_t opt_len = ZSTR_LEN(opt_str);

                    /* NX option */
                    if (strcasecmp(opt, "NX") == 0)
                    {
                        arg_count++;
                        has_nx = 1;
                    }
                    /* XX option */
                    else if (strcasecmp(opt, "XX") == 0)
                    {
                        arg_count++;
                        has_xx = 1;
                    }
                    /* GET option */
                    else if (strcasecmp(opt, "GET") == 0)
                    {
                        arg_count++;
                        has_get = 1;
                    }
                    /* KEEPTTL option */
                    else if (strcasecmp(opt, "KEEPTTL") == 0)
                    {
                        arg_count++;
                        has_keepttl = 1;
                    }
                }
            }
            else
            {
                /* Handle string keys - these are options with values */
                char *opt = ZSTR_VAL(option_key);

                /* Check for time-based options */
                if (strcasecmp(opt, "EX") == 0)
                {
                    /* EX option - seconds */
                    if (Z_TYPE_P(z_option) == IS_LONG || Z_TYPE_P(z_option) == IS_DOUBLE)
                    {
                        arg_count += 2;
                        has_ex = 1;
                        expire = zval_get_long(z_option);
                        /* Reset other time options */
                        has_px = has_exat = has_pxat = 0;
                    }
                    else
                    {
                        /* Invalid value type for EX option - should be numeric */
                        return 0;
                    }
                }
                else if (strcasecmp(opt, "PX") == 0)
                {
                    /* PX option - milliseconds */
                    if (Z_TYPE_P(z_option) == IS_LONG || Z_TYPE_P(z_option) == IS_DOUBLE)
                    {
                        arg_count += 2;
                        has_px = 1;
                        expire = zval_get_long(z_option);
                        /* Reset other time options */
                        has_ex = has_exat = has_pxat = 0;
                    }
                    else
                    {
                        /* Invalid value type for PX option - should be numeric */
                        return 0;
                    }
                }
                else if (strcasecmp(opt, "EXAT") == 0)
                {
                    /* EXAT option - unix time in seconds */
                    if (Z_TYPE_P(z_option) == IS_LONG || Z_TYPE_P(z_option) == IS_DOUBLE)
                    {
                        arg_count += 2;
                        has_exat = 1;
                        expire = zval_get_long(z_option);
                        /* Reset other time options */
                        has_ex = has_px = has_pxat = 0;
                    }
                    else
                    {
                        /* Invalid value type for EXAT option - should be numeric */
                        return 0;
                    }
                }
                else if (strcasecmp(opt, "PXAT") == 0)
                {
                    /* PXAT option - unix time in milliseconds */
                    if (Z_TYPE_P(z_option) == IS_LONG || Z_TYPE_P(z_option) == IS_DOUBLE)
                    {
                        arg_count += 2;
                        has_pxat = 1;
                        expire = zval_get_long(z_option);
                        /* Reset other time options */
                        has_ex = has_px = has_exat = 0;
                    }
                    else
                    {
                        /* Invalid value type for PXAT option - should be numeric */
                        return 0;
                    }
                }
                /* IFEQ option */
                else if (strcasecmp(opt, "IFEQ") == 0)
                {
                    /* IFEQ option - comparison value */
                    if (Z_TYPE_P(z_option) == IS_STRING)
                    {
                        arg_count += 2;
                        has_ifeq = 1;
                        ifeq_value = Z_STRVAL_P(z_option);
                        ifeq_len = Z_STRLEN_P(z_option);
                    }
                }
            }
        }
        ZEND_HASH_FOREACH_END();
    }

    /* Allocate memory for arguments */
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
            efree(args);
            efree(args_len);
            return 0;
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

    /* Add IFEQ option and value */
    if (has_ifeq)
    {
        args[arg_idx] = (uintptr_t)"IFEQ";
        args_len[arg_idx] = 4;
        arg_idx++;

        args[arg_idx] = (uintptr_t)ifeq_value;
        args_len[arg_idx] = ifeq_len;
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
        efree((void *)args[3]); /* Free the expire string */
    }
    efree(args);
    efree(args_len);

    /* Check if the command was successful */
    if (!result)
    {
        return 0;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        printf("Error executing SET command: %s\n", result->command_error->command_error_message);
        free_command_result(result);
        return 0;
    }

    /* Process the result */
    int ret_val = 0;
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
            /* GET option returned a value */
            ret_val = 2; /* GET option returned a value */

            /* Extract the string value for the caller if requested */
            if (has_get && old_val != NULL && old_val_len != NULL && result->response->string_value != NULL)
            {
                *old_val = emalloc(result->response->string_value_len + 1);
                if (*old_val)
                {
                    memcpy(*old_val, result->response->string_value, result->response->string_value_len);
                    (*old_val)[result->response->string_value_len] = '\0'; /* Null-terminate for safety */
                    *old_val_len = result->response->string_value_len;
                }
            }
            break;
        default:
            ret_val = 0; /* Error */
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
    /* Call execute_set_command with expire in seconds (EX) and no special options */
    /* No need to pass options since execute_set_command uses EX by default when expire > 0 */
    return execute_set_command(glide_client, key, key_len, val, val_len, expire, NULL, NULL, NULL);
}

/* Execute a PSETEX command using the Valkey Glide client */
int execute_psetex_command(const void *glide_client, const char *key, size_t key_len, long expire, const char *val, size_t val_len)
{
    /* Create options array for PX option */
    zval options;
    array_init(&options);

    /* Add PX option with expire value */
    add_assoc_long_ex(&options, "PX", sizeof("PX") - 1, expire);

    /* Call execute_set_command with the PX option */
    int result = execute_set_command(glide_client, key, key_len, val, val_len, 0, &options, NULL, NULL);

    /* Clean up options array */
    zval_dtor(&options);

    return result;
}

/* Execute a SETNX command using the Valkey Glide client */
int execute_setnx_command(const void *glide_client, const char *key, size_t key_len, const char *val, size_t val_len)
{
    /* Create options array for NX option */
    zval options;
    array_init(&options);

    /* Add NX option as a numeric index */
    zval nx_option;
    ZVAL_STRING(&nx_option, "NX");
    add_next_index_zval(&options, &nx_option);

    /* Call execute_set_command with the NX option and no expiration */
    int result = execute_set_command(glide_client, key, key_len, val, val_len, 0, &options, NULL, NULL);

    /* Clean up options array */
    zval_dtor(&options);

    return result;
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

/* Execute an ECHO command using the Valkey Glide client */
int execute_echo_command(const void *glide_client, const char *msg, size_t msg_len, char **result, size_t *result_len)
{
    /* Check if client is valid */
    if (!glide_client || !msg)
    {
        return -1;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1;
    uintptr_t args[1];
    unsigned long args_len[1];

    /* First argument: message */
    args[0] = (uintptr_t)msg;
    args_len[0] = msg_len;

    /* Execute the command */
    CommandResult *cmd_result = execute_command(
        glide_client,
        Echo,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_string_response(cmd_result, result, result_len);
}

/* Execute a PING command using the Valkey Glide client */
int execute_ping_command(const void *glide_client, const char *msg, size_t msg_len, char **result, size_t *result_len)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
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
        *result = estrdup("PONG");
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
        return 0;
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
int execute_getbit_command(const void *glide_client, const char *key, size_t key_len, long offset, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
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
        return 0;
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
    efree(offset_str);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute a SETBIT command using the Valkey Glide client */
int execute_setbit_command(const void *glide_client, const char *key, size_t key_len, long offset, int value, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
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
        return 0;
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
    efree(offset_str);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Helper function to execute del_command with arrays */
int execute_del_array(const void *glide_client, HashTable *keys_hash, long *output_value)
{
    /* Check if client and hash are valid */
    if (!glide_client || !keys_hash || zend_hash_num_elements(keys_hash) <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = zend_hash_num_elements(keys_hash);
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

    /* Add keys from hash table as arguments */
    zval *key;
    unsigned long idx = 0;
    ZEND_HASH_FOREACH_VAL(keys_hash, key)
    {
        /* Convert to string if needed */
        zval tmp;
        ZVAL_NULL(&tmp);
        if (Z_TYPE_P(key) != IS_STRING)
        {
            /* Convert to string */
            ZVAL_COPY(&tmp, key);
            convert_to_string(&tmp);
            args[idx] = (uintptr_t)Z_STRVAL(tmp);
            args_len[idx] = Z_STRLEN(tmp);
        }
        else
        {
            args[idx] = (uintptr_t)Z_STRVAL_P(key);
            args_len[idx] = Z_STRLEN_P(key);
        }
        idx++;
    }
    ZEND_HASH_FOREACH_END();

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Del,       /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute a DEL command using the Valkey Glide client */
int execute_del_command(const void *glide_client, zval *keys, int keys_count, long *output_value)
{
    /* Check if client and keys are valid */
    if (!glide_client || !keys || keys_count <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = keys_count;
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

    /* Add keys as arguments */
    int i;
    for (i = 0; i < keys_count; i++)
    {
        zval *key = &keys[i];
        if (Z_TYPE_P(key) != IS_STRING)
        {
            efree(args);
            efree(args_len);
            return 0;
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
    efree(args);
    efree(args_len);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Helper function to execute unlink_command with arrays */
int execute_unlink_array(const void *glide_client, HashTable *keys_hash, long *output_value)
{
    /* Check if client and hash are valid */
    if (!glide_client || !keys_hash || zend_hash_num_elements(keys_hash) <= 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = zend_hash_num_elements(keys_hash);
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

    /* Add keys from hash table as arguments */
    zval *key;
    unsigned long idx = 0;
    ZEND_HASH_FOREACH_VAL(keys_hash, key)
    {
        /* Convert to string if needed */
        zval tmp;
        ZVAL_NULL(&tmp);
        if (Z_TYPE_P(key) != IS_STRING)
        {
            /* Convert to string */
            ZVAL_COPY(&tmp, key);
            convert_to_string(&tmp);
            args[idx] = (uintptr_t)Z_STRVAL(tmp);
            args_len[idx] = Z_STRLEN(tmp);
        }
        else
        {
            args[idx] = (uintptr_t)Z_STRVAL_P(key);
            args_len[idx] = Z_STRLEN_P(key);
        }
        idx++;
    }
    ZEND_HASH_FOREACH_END();

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        Unlink,    /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute a STRLEN command using the Valkey Glide client */
int execute_strlen_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
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
    return handle_int_response(result, output_value);
}

/* Execute a SETRANGE command using the Valkey Glide client */
int execute_setrange_command(const void *glide_client, const char *key, size_t key_len, long offset, const char *value, size_t value_len, long *output_value)
{
    /* Check if client, key, and value are valid */
    if (!glide_client || !key || !value)
    {
        return 0;
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
        return 0;
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
    efree(offset_str);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
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

/* Execute an MPOP command (LMPOP, BLMPOP, ZMPOP, BZMPOP) using the Valkey Glide client */
int execute_mpop_command(const void *glide_client, const char *cmd, double timeout, zval *keys, const char *from, size_t from_len, long count, zval *result)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return 0;
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
        return 0;
    }
}

/* Execute a TTL command using the Valkey Glide client */
int execute_ttl_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
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
        TTL,       /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute a PTTL command using the Valkey Glide client */
int execute_pttl_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    /* Check if client and key are valid */
    if (!glide_client || !key)
    {
        return 0;
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
        PTTL,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}

/* Execute a ZADD command using the Valkey Glide client */
int execute_zadd_command(const void *glide_client, const char *key, size_t key_len, zval *z_args, int argc, int flags, long *output_value)
{
    /* Check if client, key, and args are valid */
    if (!glide_client || !key || !z_args || argc < 2 || argc % 2 != 0)
    {
        return 0;
    }

    /* Prepare command arguments */
    unsigned long arg_count = 1 + argc; /* key + (score, member) pairs */
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

    /* First argument: key */
    args[0] = (uintptr_t)key;
    args_len[0] = key_len;

    /* Add score/member pairs */
    int i;
    size_t score_len;
    char *score_str = NULL;
    int arg_idx = 1;

    for (i = 0; i < argc; i += 2)
    {
        /* Score */
        zval *score = &z_args[i];
        if (Z_TYPE_P(score) == IS_DOUBLE)
        {
            score_str = double_to_string(Z_DVAL_P(score), &score_len);
        }
        else if (Z_TYPE_P(score) == IS_LONG)
        {
            score_str = long_to_string(Z_LVAL_P(score), &score_len);
        }
        else if (Z_TYPE_P(score) == IS_STRING)
        {
            score_str = Z_STRVAL_P(score);
            score_len = Z_STRLEN_P(score);
        }
        else
        {
            /* Unsupported type for score */
            efree(args);
            efree(args_len);
            return 0;
        }

        args[arg_idx] = (uintptr_t)score_str;
        args_len[arg_idx] = score_len;
        arg_idx++;

        /* Member */
        zval *member = &z_args[i + 1];
        if (Z_TYPE_P(member) != IS_STRING)
        {
            /* Free score string if we allocated it */
            if (Z_TYPE_P(score) == IS_DOUBLE || Z_TYPE_P(score) == IS_LONG)
            {
                efree((void *)score_str);
            }
            efree(args);
            efree(args_len);
            return 0;
        }
        args[arg_idx] = (uintptr_t)Z_STRVAL_P(member);
        args_len[arg_idx] = Z_STRLEN_P(member);
        arg_idx++;
    }

    /* Execute the command */
    CommandResult *result = execute_command(
        glide_client,
        ZAdd,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free any allocated score strings */
    for (i = 0; i < argc; i += 2)
    {
        zval *score = &z_args[i];
        if (Z_TYPE_P(score) == IS_DOUBLE || Z_TYPE_P(score) == IS_LONG)
        {
            efree((void *)args[1 + i]);
        }
    }

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Use the generic handler to process the result */
    return handle_int_response(result, output_value);
}
