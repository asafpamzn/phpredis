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
#include "valkey_glide_list_common.h"
#include "valkey_glide_core_common.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <ext/spl/spl_exceptions.h>
#include <zend_exceptions.h>
#include <ext/standard/info.h>
#include <ext/hash/php_hash.h>

extern zend_class_entry *redis_ce;
extern zend_class_entry *redis_exception_ce;

/* Import the string conversion functions from command_response.c */
extern char *long_to_string(long value, size_t *len);
extern char *double_to_string(double value, size_t *len);

/* Create a connection request in protobuf format */
static uint8_t *create_connection_request(const char *host, int port, const char *user, const char *pass, size_t *len, ClientConfig *config)
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
    conn_req.cluster_mode_enabled = config->is_cluster;
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
        &len,
        config);

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

/* Custom result processor for SET commands with GET option support */
struct set_result_data
{
    char **old_val;
    size_t *old_val_len;
    int has_get;
};

static int process_set_result(CommandResult *result, void *output)
{
    struct set_result_data *data = (struct set_result_data *)output;

    if (!result || !result->response)
    {
        return 0;
    }

    switch (result->response->response_type)
    {
    case Ok:
        return 1; /* Success */
    case Null:
        return 0; /* Not set (NX/XX condition not met) */
    case String:
        /* GET option returned a value */
        if (data->has_get && data->old_val && data->old_val_len && result->response->string_value)
        {
            *data->old_val = emalloc(result->response->string_value_len + 1);
            if (*data->old_val)
            {
                memcpy(*data->old_val, result->response->string_value, result->response->string_value_len);
                (*data->old_val)[result->response->string_value_len] = '\0';
                *data->old_val_len = result->response->string_value_len;
            }
        }
        return 2; /* GET option returned a value */
    default:
        return 0; /* Error */
    }
}

/* Custom result processor for PING command */
static int process_ping_result(CommandResult *result, void *output)
{
    struct
    {
        char **result;
        size_t *result_len;
    } *string_output = output;

    if (!result || !result->response || !string_output)
    {
        return 0;
    }

    if (result->response->response_type == Ok)
    {
        /* PONG response with no message */
        *string_output->result = estrdup("PONG");
        *string_output->result_len = 4;
        return 1;
    }
    else if (result->response->response_type == String)
    {
        /* PING with message - echo the message back */
        if (result->response->string_value_len == 0)
        {
            *string_output->result = emalloc(1);
            if (*string_output->result)
            {
                (*string_output->result)[0] = '\0';
            }
            *string_output->result_len = 0;
        }
        else
        {
            *string_output->result = emalloc(result->response->string_value_len + 1);
            if (*string_output->result)
            {
                memcpy(*string_output->result, result->response->string_value,
                       result->response->string_value_len);
                (*string_output->result)[result->response->string_value_len] = '\0';
            }
            *string_output->result_len = result->response->string_value_len;
        }
        return *string_output->result ? 1 : 0;
    }
    else if (result->response->response_type == Null)
    {
        *string_output->result = NULL;
        *string_output->result_len = 0;
        return 0;
    }

    return 0;
}

/* These functions are now defined in command_response.c */

/* Execute a BITCOUNT command using the Valkey Glide client - UNIFIED IMPLEMENTATION */
int execute_bitcount_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long start = 0, end = -1;
    zend_bool bybit = 0;
    long result_value;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Os|llb",
                                     &object, redis_ce, &key, &key_len,
                                     &start, &end, &bybit) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    if (!redis || !redis->glide_client)
    {
        return 0;
    }

    /* Execute using core framework */
    core_command_args_t args = {0};
    args.glide_client = redis->glide_client;
    args.cmd_type = BitCount;
    args.key = key;
    args.key_len = key_len;

    /* Set range options */
    args.options.start = start;
    args.options.end = end;
    args.options.has_range = 1;
    args.options.bybit = bybit;

    if (execute_core_command(&args, &result_value, process_core_int_result))
    {
        ZVAL_LONG(return_value, result_value);
        return 1;
    }

    return 0;
}

/* Execute a BITOP command using the Valkey Glide client - UNIFIED IMPLEMENTATION */
int execute_bitop_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    char *op = NULL, *key = NULL;
    size_t op_len, key_len;
    zval *keys = NULL;
    int keys_count = 0;
    long result_value;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Oss*",
                                     &object, redis_ce, &op, &op_len,
                                     &key, &key_len, &keys, &keys_count) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    if (!redis || !redis->glide_client)
    {
        return 0;
    }

    /* Execute using core framework */
    core_command_args_t args = {0};
    args.glide_client = redis->glide_client;
    args.cmd_type = BitOp;
    args.key = key; /* destination key */
    args.key_len = key_len;

    /* Add operation as first argument */
    args.args[0].type = CORE_ARG_TYPE_STRING;
    args.args[0].data.string_arg.value = op;
    args.args[0].data.string_arg.len = op_len;

    /* Add source keys as remaining arguments */
    for (int i = 0; i < keys_count && i < 7; i++)
    {
        args.args[i + 1].type = CORE_ARG_TYPE_STRING;
        args.args[i + 1].data.string_arg.value = Z_STRVAL(keys[i]);
        args.args[i + 1].data.string_arg.len = Z_STRLEN(keys[i]);
    }
    args.arg_count = 1 + keys_count; /* operation + source keys */

    if (execute_core_command(&args, &result_value, process_core_int_result))
    {
        ZVAL_LONG(return_value, result_value);
        return 1;
    }

    return 0;
}

/* Execute a BITPOS command using the Valkey Glide client - UNIFIED IMPLEMENTATION */
int execute_bitpos_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long bit, start = 0, end = -1;
    zend_bool bybit = 0;
    long result_value;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Osl|llb",
                                     &object, redis_ce, &key, &key_len, &bit,
                                     &start, &end, &bybit) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    if (!redis || !redis->glide_client)
    {
        return 0;
    }

    /* Execute using core framework */
    core_command_args_t args = {0};
    args.glide_client = redis->glide_client;
    args.cmd_type = BitPos;
    args.key = key;
    args.key_len = key_len;

    /* Add bit value argument */
    args.args[0].type = CORE_ARG_TYPE_LONG;
    args.args[0].data.long_arg.value = bit;
    args.arg_count = 1;

    /* Set range options */
    args.options.start = start;
    args.options.end = end;
    args.options.has_range = 1;
    args.options.bybit = bybit;

    if (execute_core_command(&args, &result_value, process_core_int_result))
    {
        ZVAL_LONG(return_value, result_value);
        return 1;
    }

    return 0;
}

/* Execute a SET command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_set_command(const void *glide_client, const char *key, size_t key_len, const char *val, size_t val_len, long expire, zval *opts, char **old_val, size_t *old_val_len)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Set;
    args.key = key;
    args.key_len = key_len;
    args.raw_options = opts;

    /* Add value argument */
    args.args[0].type = CORE_ARG_TYPE_STRING;
    args.args[0].data.string_arg.value = val;
    args.args[0].data.string_arg.len = val_len;
    args.arg_count = 1;

    /* Parse options */
    if (opts)
    {
        if (parse_set_options(opts, &args.options) == 0)
        {
            /* If parsing failed, return error */
            return 0;
        }
    }

    /* Set expire if provided */
    if (expire > 0)
    {
        args.options.expire_seconds = expire;
        args.options.has_expire = 1;
    }

    /* Prepare result data for GET option */
    struct set_result_data result_data = {old_val, old_val_len, args.options.get_old_value};

    return execute_core_command(&args, &result_data, process_set_result);
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

/* Execute an ECHO command using the Valkey Glide client - UNIFIED IMPLEMENTATION */
int execute_echo_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    char *msg = NULL;
    size_t msg_len;
    char *response = NULL;
    size_t response_len = 0;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Os",
                                     &object, redis_ce, &msg, &msg_len) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    if (!redis || !redis->glide_client)
    {
        return 0;
    }

    /* Execute using core framework */
    core_command_args_t args = {0};
    args.glide_client = redis->glide_client;
    args.cmd_type = Echo;

    /* Add message argument */
    args.args[0].type = CORE_ARG_TYPE_STRING;
    args.args[0].data.string_arg.value = msg;
    args.args[0].data.string_arg.len = msg_len;
    args.arg_count = 1;

    /* Use string result processor */
    struct
    {
        char **result;
        size_t *result_len;
    } output = {&response, &response_len};

    if (execute_core_command(&args, &output, process_core_string_result))
    {
        if (response != NULL)
        {
            ZVAL_STRINGL(return_value, response, response_len);
            efree(response);
            return 1;
        }
    }

    return 0;
}

/* Execute a PING command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_ping_command(const void *glide_client, const char *msg, size_t msg_len, char **result, size_t *result_len)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Ping;

    /* Add optional message argument */
    if (msg)
    {
        args.args[0].type = CORE_ARG_TYPE_STRING;
        args.args[0].data.string_arg.value = msg;
        args.args[0].data.string_arg.len = msg_len;
        args.arg_count = 1;
    }

    /* Custom result processor to handle PONG response */
    struct
    {
        char **result;
        size_t *result_len;
    } output = {result, result_len};

    return execute_core_command(&args, &output, process_ping_result);
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

/* Execute an INFO command with multiple sections using the Valkey Glide client */
int execute_info_sections_command(const void *glide_client, zval *sections, int sections_count, char **result, size_t *result_len)
{
    /* Check if client is valid */
    if (!glide_client || !sections || sections_count <= 0)
    {
        return 0;
    }

    /* Prepare command arguments - each section is one argument */
    unsigned long arg_count = sections_count;
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

    /* Process each section argument */
    int i;
    for (i = 0; i < sections_count; i++)
    {
        zval *section = &sections[i];

        /* Check if the section is a string */
        if (Z_TYPE_P(section) != IS_STRING)
        {
            /* Convert to string if needed */
            zval temp;
            ZVAL_COPY(&temp, section);
            convert_to_string(&temp);

            args[i] = (uintptr_t)Z_STRVAL(temp);
            args_len[i] = Z_STRLEN(temp);

            /* Free the temporary zval after the command executes */
            zval_dtor(&temp);
        }
        else
        {
            /* It's already a string */
            args[i] = (uintptr_t)Z_STRVAL_P(section);
            args_len[i] = Z_STRLEN_P(section);
        }
    }

    /* Execute the command */
    CommandResult *cmd_result = execute_command(
        glide_client,
        Info,      /* command type */
        arg_count, /* number of arguments */
        args,      /* arguments */
        args_len   /* argument lengths */
    );

    /* Free the argument arrays */
    efree(args);
    efree(args_len);

    /* Use the generic handler to process the result */
    return handle_string_response(cmd_result, result, result_len);
}

/* Execute a GET command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_get_command(const void *glide_client, const char *key, size_t key_len, char **result, size_t *result_len)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Get;
    args.key = key;
    args.key_len = key_len;

    /* Use string result processor */
    struct
    {
        char **result;
        size_t *result_len;
    } output = {result, result_len};

    return execute_core_command(&args, &output, process_core_string_result);
}

/* Execute a RANDOMKEY command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_randomkey_command(const void *glide_client, char **result, size_t *result_len)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = RandomKey;

    /* Use string result processor */
    struct
    {
        char **result;
        size_t *result_len;
    } output = {result, result_len};

    return execute_core_command(&args, &output, process_core_string_result);
}

/* Execute a GETBIT command using the Valkey Glide client - UNIFIED IMPLEMENTATION */
int execute_getbit_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long offset;
    long result_value;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Osl",
                                     &object, redis_ce, &key, &key_len,
                                     &offset) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    if (!redis || !redis->glide_client)
    {
        return 0;
    }

    /* Execute using core framework */
    core_command_args_t args = {0};
    args.glide_client = redis->glide_client;
    args.cmd_type = GetBit;
    args.key = key;
    args.key_len = key_len;

    /* Add offset argument */
    args.args[0].type = CORE_ARG_TYPE_LONG;
    args.args[0].data.long_arg.value = offset;
    args.arg_count = 1;

    if (execute_core_command(&args, &result_value, process_core_int_result))
    {
        ZVAL_LONG(return_value, result_value);
        return 1;
    }

    return 0;
}

/* Execute a SETBIT command using the Valkey Glide client - UNIFIED IMPLEMENTATION */
int execute_setbit_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    char *key = NULL;
    size_t key_len;
    zend_long offset;
    zend_bool value;
    long result_value;

    /* Parse parameters */
    if (zend_parse_method_parameters(argc, object, "Oslb",
                                     &object, redis_ce, &key, &key_len,
                                     &offset, &value) == FAILURE)
    {
        return 0;
    }

    /* Get Redis object */
    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    if (!redis || !redis->glide_client)
    {
        return 0;
    }

    /* Execute using core framework */
    core_command_args_t args = {0};
    args.glide_client = redis->glide_client;
    args.cmd_type = SetBit;
    args.key = key;
    args.key_len = key_len;

    /* Add offset argument */
    args.args[0].type = CORE_ARG_TYPE_LONG;
    args.args[0].data.long_arg.value = offset;

    /* Add value argument (0 or 1) */
    args.args[1].type = CORE_ARG_TYPE_LONG;
    args.args[1].data.long_arg.value = value ? 1 : 0;
    args.arg_count = 2;

    if (execute_core_command(&args, &result_value, process_core_int_result))
    {
        ZVAL_LONG(return_value, result_value);
        return 1;
    }

    return 0;
}

/* Helper function to execute del_command with arrays - MIGRATED TO CORE FRAMEWORK */
int execute_del_array(const void *glide_client, HashTable *keys_hash, long *output_value)
{
    /* Convert HashTable to zval array for core framework */
    if (!glide_client || !keys_hash || zend_hash_num_elements(keys_hash) <= 0)
    {
        return 0;
    }

    /* Create temporary zval array from HashTable */
    zval keys_array;
    array_init(&keys_array);

    zval *key;
    ZEND_HASH_FOREACH_VAL(keys_hash, key)
    {
        add_next_index_zval(&keys_array, key);
    }
    ZEND_HASH_FOREACH_END();

    /* Use core framework with converted array */
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Del;

    args.args[0].type = CORE_ARG_TYPE_ARRAY;
    args.args[0].data.array_arg.array = &keys_array;
    args.args[0].data.array_arg.count = zend_hash_num_elements(keys_hash);
    args.arg_count = 1;

    int result = execute_core_command(&args, output_value, process_core_int_result);

    return result;
}

/* Execute a DEL command using the Valkey Glide client - UNIFIED IMPLEMENTATION */
int execute_del_command(zval *object, int argc, zval *return_value)
{
    redis_object *redis;
    long result_value = 0;
    zval *keys = NULL;
    int keys_count = 0;

    if (zend_parse_method_parameters(argc, object, "O*",
                                     &object, redis_ce, &keys, &keys_count) == FAILURE)
    {
        return 0;
    }

    redis = PHPREDIS_ZVAL_GET_OBJECT(redis_object, object);
    if (!redis || !redis->glide_client)
    {
        return 0;
    }

    if (keys_count == 1 && Z_TYPE(keys[0]) == IS_ARRAY)
    {
        if (execute_del_array(redis->glide_client, Z_ARRVAL(keys[0]), &result_value))
        {
            ZVAL_LONG(return_value, result_value);
            return 1;
        }
    }
    else
    {
        if (execute_multi_key_command(redis->glide_client, Del, keys, keys_count, &result_value))
        {
            ZVAL_LONG(return_value, result_value);
            return 1;
        }
    }
    return 0;
}

/* Helper function to execute unlink_command with arrays - MIGRATED TO CORE FRAMEWORK */
int execute_unlink_array(const void *glide_client, HashTable *keys_hash, long *output_value)
{
    /* Convert HashTable to zval array for core framework */
    if (!glide_client || !keys_hash || zend_hash_num_elements(keys_hash) <= 0)
    {
        return 0;
    }

    /* Create temporary zval array from HashTable */
    zval keys_array;
    array_init(&keys_array);

    zval *key;
    ZEND_HASH_FOREACH_VAL(keys_hash, key)
    {
        add_next_index_zval(&keys_array, key);
    }
    ZEND_HASH_FOREACH_END();

    /* Use core framework with converted array */
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Unlink;

    args.args[0].type = CORE_ARG_TYPE_ARRAY;
    args.args[0].data.array_arg.array = &keys_array;
    args.args[0].data.array_arg.count = zend_hash_num_elements(keys_hash);
    args.arg_count = 1;

    return execute_core_command(&args, output_value, process_core_int_result);
}

/* Execute a STRLEN command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_strlen_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Strlen;
    args.key = key;
    args.key_len = key_len;

    return execute_core_command(&args, output_value, process_core_int_result);
}

/* Execute a SETRANGE command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_setrange_command(const void *glide_client, const char *key, size_t key_len, long offset, const char *value, size_t value_len, long *output_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = SetRange;
    args.key = key;
    args.key_len = key_len;

    /* Add offset argument */
    args.args[0].type = CORE_ARG_TYPE_LONG;
    args.args[0].data.long_arg.value = offset;

    /* Add value argument */
    args.args[1].type = CORE_ARG_TYPE_STRING;
    args.args[1].data.string_arg.value = value;
    args.args[1].data.string_arg.len = value_len;
    args.arg_count = 2;

    return execute_core_command(&args, output_value, process_core_int_result);
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

/* Execute a TTL command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_ttl_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = TTL;
    args.key = key;
    args.key_len = key_len;

    return execute_core_command(&args, output_value, process_core_int_result);
}

/* Execute a single-key DEL command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_del_single_key(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = Del;
    args.key = key;
    args.key_len = key_len;
    /* arg_count = 0 for single key mode */

    return execute_core_command(&args, output_value, process_core_int_result);
}

/* Execute a PTTL command using the Valkey Glide client - MIGRATED TO CORE FRAMEWORK */
int execute_pttl_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    core_command_args_t args = {0};
    args.glide_client = glide_client;
    args.cmd_type = PTTL;
    args.key = key;
    args.key_len = key_len;

    return execute_core_command(&args, output_value, process_core_int_result);
}
