/*
  +----------------------------------------------------------------------+
  | ValkeyGlide Glide FFI integration for phpredis                             |
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

#include "valkey_glide_commands_common.h"
#include "command_response.h"

/* Execute a command and handle common error checking */
CommandResult *execute_command(
    const void *glide_client,
    enum RequestType command_type,
    unsigned long arg_count,
    const uintptr_t *args,
    const unsigned long *args_len)
{
    /* Check if client is valid */
    if (!glide_client)
    {
        return NULL;
    }

    /* Execute the command */
    CommandResult *result = command(
        glide_client,
        0,            /* channel */
        command_type, /* command type */
        arg_count,    /* number of arguments */
        args,         /* arguments */
        args_len,     /* argument lengths */
        NULL,         /* route bytes */
        0             /* route bytes length */
    );

    return result;
}

/* Handle an integer response */
long handle_int_response(CommandResult *result, long *output_value)
{
    /* Check if the command was successful */
    if (!result)
    {
        return 0; /* False - failure */
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        // printf("%s:%d - Error executing command: %s\n", __FILE__, __LINE__, result->command_error->command_error_message);
        free_command_result(result);
        return 0; /* False - failure */
    }

    /* Get the result value */
    if (result->response && result->response->response_type == Int)
    {
        *output_value = result->response->int_value;

        /* Free the result */
        free_command_result(result);
        return 1; /* True - success */
    }
    else
    {
        // printf("%s:%d - Unexpected response type for integer command\n", __FILE__, __LINE__);
        assert(0); /* This should never happen */
    }

    /* Unexpected response type */
    free_command_result(result);
    return 0; /* False - failure */
}

/* Handle a string response */
int handle_string_response(CommandResult *result, char **output, size_t *output_len)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        // printf("%s:%d - Error executing command: %s\n", __FILE__, __LINE__, result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (result->response)
    {
        switch (result->response->response_type)
        {
        case String:
            /* Command returns a string/binary data */
            if (result->response->string_value_len == 0)
            {
                *output = emalloc(1); // Allocate at least one byte using PHP's memory manager
                if (*output)
                {
                    (*output)[0] = '\0'; // Empty string is still null-terminated
                }
                *output_len = 0;
            }
            else
            {
                // Allocate exact size needed for binary data using PHP's memory manager
                *output = emalloc(result->response->string_value_len + 1);
                (*output)[result->response->string_value_len] = '\0';
                if (*output)
                {
                    // Copy binary data without assuming null-termination
                    memcpy(*output, result->response->string_value, result->response->string_value_len);
                }
                *output_len = result->response->string_value_len;
            }
            // Check if allocation failed
            if (!*output)
            {
                ret_val = -1; // Memory allocation failed
            }
            else
            {
                ret_val = 1; // Success
            }
            break;
        case Null:
            /* Key didn't exist, return NULL */
            *output = NULL;
            *output_len = 0;
            ret_val = 0;
            break;
        default:
            ret_val = -1;
            break;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Handle a boolean response */
int handle_bool_response(CommandResult *result)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        // printf("%s:%d - Error executing command: %s\n", __FILE__, __LINE__, result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Get the result value */
    int ret_val = -1;
    if (result->response && result->response->response_type == Bool)
    {
        ret_val = result->response->bool_value ? 1 : 0;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Handle an OK response */
int handle_ok_response(CommandResult *result)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        // printf("%s:%d - Error executing command: %s\n", __FILE__, __LINE__, result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Get the result value */
    int ret_val = -1;
    if (result->response && result->response->response_type == Ok)
    {
        ret_val = 1;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Handle a null or string response */
int handle_null_or_string_response(CommandResult *result, char **output, size_t *output_len)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        // printf("%s:%d - Error executing command: %s\n", __FILE__, __LINE__, result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (result->response)
    {
        switch (result->response->response_type)
        {
        case String:
            /* Command returns a string/binary data */
            if (result->response->string_value_len == 0)
            {
                *output = emalloc(1); // Allocate at least one byte using PHP's memory manager
                if (*output)
                {
                    (*output)[0] = '\0'; // Empty string is still null-terminated
                }
                *output_len = 0;
            }
            else
            {
                // Allocate exact size needed for binary data using PHP's memory manager
                *output = emalloc(result->response->string_value_len);
                if (*output)
                {
                    // Copy binary data without assuming null-termination
                    memcpy(*output, result->response->string_value, result->response->string_value_len);
                }
                *output_len = result->response->string_value_len;
            }
            // Check if allocation failed
            if (!*output)
            {
                ret_val = -1; // Memory allocation failed
            }
            else
            {
                ret_val = 1; // Success
            }
            break;
        case Null:
            /* Key didn't exist, return NULL */
            *output = NULL;
            *output_len = 0;
            ret_val = 0;
            break;
        default:
            ret_val = -1;
            break;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Handle a double response */
int handle_double_response(CommandResult *result, double *output)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        // printf("%s:%d - Error executing command: %s\n", __FILE__, __LINE__, result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Get the result value */
    int ret_val = -1;
    if (result->response && result->response->response_type == Float)
    {
        *output = result->response->float_value;
        ret_val = 1;
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Helper function to convert a CommandResponse to a PHP value
 * use_associative_array:
 * - 0: regular array processing
 * - 1: convert Map elements to associative array format (for ZMPOP/sorted sets)
 */
int command_response_to_zval(CommandResponse *response, zval *output, int use_associative_array, bool use_false_if_null)
{

    if (!response)
    {
        ZVAL_NULL(output);
        return 0;
    }
    // printf("response_type = %d\n", response->response_type);
    switch (response->response_type)
    {
    case Null:
        // printf("%s:%d - CommandResponse is NULL\n", __FILE__, __LINE__);
        if (use_false_if_null)
        {
            // printf("%s:%d - CommandResponse is converted to false\n", __FILE__, __LINE__);
            ZVAL_FALSE(output);
        }
        else
        {
            ZVAL_NULL(output);
        }

        return 0;
    case Int:
        // printf("%s:%d - CommandResponse is Int: %ld\n", __FILE__, __LINE__, response->int_value);
        ZVAL_LONG(output, response->int_value);
        return 1;
    case Float:
        // printf("%s:%d - CommandResponse is Float: %f\n", __FILE__, __LINE__, response->float_value);
        ZVAL_DOUBLE(output, response->float_value);
        ////printf("%s:%d - Converted CommandResponse to double: %f\n", __FILE__, __LINE__, Z_DVAL_P(output));
        return 1;
    case Bool:
        ////printf("%s:%d - CommandResponse is Bool: %d\n", __FILE__, __LINE__, response->bool_value);
        ZVAL_BOOL(output, response->bool_value);
        return 1;
    case String:
        // printf("%s:%d - CommandResponse is String with length: %ld string = %s\n", __FILE__, __LINE__, response->string_value_len, response->string_value);
        ZVAL_STRINGL(output, response->string_value, response->string_value_len);
        return 1;
    case Array:
        // printf("%s:%d - CommandResponse is Array with length: %ld, use_associative_array = %d\n", __FILE__, __LINE__, response->array_value_len, use_associative_array);
        array_init(output);
        if (response->array_value_len == 2 && use_associative_array == COMMAND_RESPONSE_STREAM_ARRAY_ASSOCIATIVE)
        {
            zval field, value;
            // printf("%s:%d - response->array_value[0]->command_response_type = %d, response->array_value[1]->command_response_type = %d\n",
            //       __FILE__, __LINE__, response->array_value[0].response_type, response->array_value[1].response_type);
            command_response_to_zval(&response->array_value[0], &field, use_associative_array, use_false_if_null);
            command_response_to_zval(&response->array_value[1], &value, use_associative_array, use_false_if_null);
            // printf("%s:%d - DEBUG: Adding field \n", __FILE__, __LINE__);
            // php_var_dump(&field, 2);
            // printf("%s:%d - DEBUG: Adding value \n", __FILE__, __LINE__);
            // php_var_dump(&value, 2);

            if (Z_TYPE(field) == IS_STRING)
            {
                // printf("%s:%d - DEBUG: Adding field %s with value %s\n", __FILE__, __LINE__, Z_STRVAL(field), Z_STRVAL(value));
                add_assoc_zval(output, Z_STRVAL(field), &value);
                zval_dtor(&field);
            }
            else if (Z_TYPE(value) == IS_ARRAY && Z_TYPE(field) == IS_ARRAY)
            {
                {
                    // Iterate through the field array and add each key-value pair to output
                    HashTable *field_ht = Z_ARRVAL(field);
                    zend_string *key;
                    zval *val;
                    ZEND_HASH_FOREACH_STR_KEY_VAL(field_ht, key, val)
                    {
                        zval copy;
                        ZVAL_COPY(&copy, val);
                        add_assoc_str(output, ZSTR_VAL(key), Z_STR(copy));
                    }
                    ZEND_HASH_FOREACH_END();
                }

                // Do the same for the value array

                {
                    HashTable *value_ht = Z_ARRVAL(value);
                    zend_string *key;
                    zval *val;
                    ZEND_HASH_FOREACH_STR_KEY_VAL(value_ht, key, val)
                    {
                        zval copy;
                        ZVAL_COPY(&copy, val);
                        add_assoc_str(output, ZSTR_VAL(key), Z_STR(copy));
                    }
                    ZEND_HASH_FOREACH_END();
                }
            }
            else
            {
                zval_dtor(&field);
                zval_dtor(&value);
            }
            // php_var_dump(output, 2);
        }
        else
        {
            for (int64_t i = 0; i < response->array_value_len; i++)
            {

                zval value;

                command_response_to_zval(&response->array_value[i], &value, use_associative_array, use_false_if_null);
                // printf("%s:%d - DEBUG: Adding array value %d\n", __FILE__, __LINE__, i);
                //   php_var_dump(&value, 2); // No need to modify this as it's not printf

                add_next_index_zval(output, &value);
                // printf("%s:%d - DEBUG: Added array value %d\n", __FILE__, __LINE__, i);
                // php_var_dump(output, 2); // No need to modify this as it's not printf
            }
        }
        // printf("%s:%d - DEBUG: Finished processing array response\n", __FILE__, __LINE__);
        return 1;
#if 1
    case Map:
        // printf("%s:%d - CommandResponse is Map with length: %ld\n", __FILE__, __LINE__, response->array_value_len);
        array_init(output);
        for (int i = 0; i < response->array_value_len; i++)
        {
            zval key, value;
            CommandResponse *element = &response->array_value[i];

            // Process the key
            if (element->map_key != NULL)
            {
                command_response_to_zval(element->map_key, &key, use_associative_array, use_false_if_null);
            }
            else
            {
                ZVAL_NULL(&key);
            }

            // Process the value
            if (element->map_value != NULL)
            {
                // printf("%s:%d - DEBUG: Processing map value %d\n", __FILE__, __LINE__, i);
                command_response_to_zval(element->map_value, &value, use_associative_array, use_false_if_null);
                // printf("%s:%d - DEBUG: Map value %d processed\n", __FILE__, __LINE__, i);
            }
            else
            {
                ZVAL_NULL(&value);
            }

            if (use_associative_array != COMMAND_RESPONSE_NOT_ASSOSIATIVE && Z_TYPE(key) == IS_STRING)
            {
                // printf("%s:%d - DEBUG: Adding key %s \n", __FILE__, __LINE__, Z_STRVAL(key));
                // php_var_dump(&value, 2); // No need to modify this as it's not printf
                add_assoc_zval(output, Z_STRVAL(key), &value);
                zval_dtor(&key); // Clean up the key since we're using it as an index
            }
            else
            {
                // Add the key as a separate array element (original behavior)
                add_next_index_zval(output, &key);
                // Add the value as the next array element
                add_next_index_zval(output, &value);
            }
        }
        // php_var_dump(output, 2); // No need to modify this as it's not printf
        return 1;
#endif
    case Sets:
        array_init(output);
        for (int i = 0; i < response->sets_value_len; i++)
        {
            zval value;
            CommandResponse *set_item = &response->sets_value[i];

            if (set_item->response_type == String)
            {
                ZVAL_STRINGL(&value, set_item->string_value, set_item->string_value_len);
                add_next_index_zval(output, &value);
            }
        }
        return 1;
    case Ok:
        // ZVAL_STRING(output, "OK");
        ZVAL_BOOL(output, true);
        return 1;
    default:
        ZVAL_NULL(output);
        return -1;
    }
}

/* Handle an array response */
int handle_array_response(CommandResult *result, zval *output)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        // printf("%s:%d - Error executing command: %s\n", __FILE__, __LINE__, result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (result->response)
    {
        if (result->response->response_type == Null)
        {
            ZVAL_NULL(output);
            ret_val = 0;
        }
        else if (result->response->response_type == Array)
        {
            ret_val = command_response_to_zval(result->response, output, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
        }
        else
        {
            ret_val = -1;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Handle a map response */
int handle_map_response(CommandResult *result, zval *output)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        // printf("%s:%d - Error executing command: %s\n", __FILE__, __LINE__, result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (result->response)
    {
        if (result->response->response_type == Null)
        {
            ZVAL_NULL(output);
            ret_val = 0;
        }
        else if (result->response->response_type == Map)
        {
            ret_val = command_response_to_zval(result->response, output, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
        }
        else
        {
            ret_val = -1;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Handle a set response */
int handle_set_response(CommandResult *result, zval *output)
{
    /* Check if the command was successful */
    if (!result)
    {
        return -1;
    }

    /* Check if there was an error */
    if (result->command_error)
    {
        // printf("%s:%d - Error executing command: %s\n", __FILE__, __LINE__, result->command_error->command_error_message);
        free_command_result(result);
        return -1;
    }

    /* Process the result */
    int ret_val = -1;
    if (result->response)
    {
        if (result->response->response_type == Null)
        {
            ZVAL_NULL(output);
            ret_val = 0;
        }
        else if (result->response->response_type == Sets)
        {
            ret_val = command_response_to_zval(result->response, output, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
        }
        else
        {
            ret_val = -1;
        }
    }

    /* Free the result */
    free_command_result(result);

    return ret_val;
}

/* Convert a long value to a string */
char *long_to_string(long value, size_t *len)
{
    char buffer[32];
    *len = snprintf(buffer, sizeof(buffer), "%ld", value);
    char *str = (char *)emalloc(*len + 1);
    if (str)
    {
        memcpy(str, buffer, *len);
        str[*len] = '\0';
    }
    return str;
}

/* Convert a double value to a string */
char *double_to_string(double value, size_t *len)
{
    char buffer[64];
    /* Use %.6g format to get a more user-friendly representation */
    *len = snprintf(buffer, sizeof(buffer), "%.6g", value);
    char *str = (char *)emalloc(*len + 1);
    if (str)
    {
        memcpy(str, buffer, *len);
        str[*len] = '\0';
    }
    return str;
}

/* Helper function to recursively extract field-value pairs from a stream entry */
void extract_stream_field_values(CommandResponse *response, zval *field_array)
{

    if (!response)
    {
        return;
    }
    CommandResponse *field_resp1 = &response->array_value[0];
    zval field, value;
    command_response_to_zval(&field_resp1->array_value[0], &field, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
    command_response_to_zval(&field_resp1->array_value[1], &value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
    add_assoc_zval(field_array, Z_STRVAL(field), &value);
    zval_dtor(&field); // Clean up the field since we're using it as an index
}

/* Helper function to convert a CommandResponse to a PHP stream format
 * This is specifically for XRANGE/XREVRANGE commands that return stream entries
 * We need to handle both Array and Map response types
 * The output should be: ["stream_id" => ["field1" => "value1", "field2" => "value2", ...]]
 */
int command_response_to_stream_zval(CommandResponse *response, zval *output)
{
    // printf("%s:%d - ------------------------------------------------\n", __FILE__, __LINE__);
    if (!response)
    {
        // printf("%s:%d - DEBUG: Response is NULL\n", __FILE__, __LINE__);
        ZVAL_NULL(output);
        return 0;
    }
    array_init(output);

    /* Handle different response types */
    // printf("%s:%d - DEBUG: Processing command response of type %d\n", __FILE__, __LINE__, response->response_type);
    switch (response->response_type)
    {
    case Map:
        /* Process map response where keys are stream IDs and values are field-value pairs */
        for (int i = 0; i < response->array_value_len; i++)
        {
            CommandResponse *element = &response->array_value[i];

            /* Skip if we don't have both key and value */
            if (!element->map_key || !element->map_value)
            {
                continue;
            }

            /* Extract stream ID from key */
            if (element->map_key->response_type != String)
            {
                continue;
            }

            char *stream_id = element->map_key->string_value;
            size_t stream_id_len = element->map_key->string_value_len;
            // printf("%s:%d - NEW STREAM ID: %.*s\n", __FILE__, __LINE__, (int)stream_id_len, stream_id);

            /* Create associative array for field-value pairs */
            zval field_array;
            array_init(&field_array);
            // printf("%s:%d - DEBUG: Processing stream ID: %.*s, element->map_value->response_type = %d\n", __FILE__, __LINE__, (int)stream_id_len, stream_id, element->map_value->response_type);
            /* Process nested field-value pairs - add safety check */
            if (element->map_value->response_type == Array)
            {
                /* Safe version that checks array bounds */
                if (element->map_value->array_value_len > 0)
                {
                    // printf("%s:%d - DEBUG: Processing Array response for stream ID: %.*s, array_value_len = %ld\n", __FILE__, __LINE__, (int)stream_id_len, stream_id, element->map_value->array_value_len);
                    CommandResponse *field_resp1 = &element->map_value->array_value[0];

                    if (field_resp1->response_type == Array && field_resp1->array_value_len == 2)
                    {

                        zval field, value;
                        command_response_to_zval(&field_resp1->array_value[0], &field, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);
                        command_response_to_zval(&field_resp1->array_value[1], &value, COMMAND_RESPONSE_NOT_ASSOSIATIVE, false);

                        if (Z_TYPE(field) == IS_STRING)
                        {
                            // printf("%s:%d - DEBUG: Adding field %s with value %s\n", __FILE__, __LINE__, Z_STRVAL(field), Z_STRVAL(value));
                            add_assoc_zval(&field_array, Z_STRVAL(field), &value);
                            zval_dtor(&field);
                        }
                        else
                        {
                            zval_dtor(&field);
                            zval_dtor(&value);
                        }
                    }
                }
                /* Add the stream entry to the output array */
                add_assoc_zval_ex(output, stream_id, stream_id_len, &field_array);
            }
            else if (element->map_value->response_type == Map)
            {
                CommandResponse *map = element->map_value;
                // printf("%s:%d - DEBUG: Processing Map response for stream ID: %.*s, map->array_value_len = %ld\n", __FILE__, __LINE__, (int)stream_id_len, stream_id, map->array_value_len);
                // printf("%s:%d - DEBUG: Map response type = %d\n", __FILE__, __LINE__, map->response_type);
                zval output1;
                command_response_to_zval(map, &output1, COMMAND_RESPONSE_STREAM_ARRAY_ASSOCIATIVE, false);
                add_assoc_zval_ex(output, stream_id, stream_id_len, &output1);
            }
            else
            {
                // printf("%s:%d - DEBUG: Unexpected response type for stream fields: %d\n", __FILE__, __LINE__, element->map_value->response_type);
            }
        }
        break;
    case Null:
        /* If the response is Null, set output to NULL */
        // printf("%s:%d - DEBUG: Response is Null\n", __FILE__, __LINE__);
        array_init(output);
        break;

    default:
        zval_dtor(output); /* Clean up the initialized array */
        ZVAL_NULL(output);
        return 0;
    }

    return 1;
}

/**
 * Safe zval to string conversion with memory management
 * Returns allocated string that must be freed, or NULL on error
 * Sets need_free to 1 if returned string must be freed
 */
char *zval_to_string_safe(zval *z, size_t *len, int *need_free)
{
    char *str = NULL;
    *need_free = 0;

    if (!z || !len)
    {
        return NULL;
    }

    switch (Z_TYPE_P(z))
    {
    case IS_STRING:
        str = Z_STRVAL_P(z);
        *len = Z_STRLEN_P(z);
        break;

    case IS_LONG:
        str = long_to_string(Z_LVAL_P(z), len);
        *need_free = 1;
        break;

    case IS_DOUBLE:
        str = double_to_string(Z_DVAL_P(z), len);
        *need_free = 1;
        break;

    case IS_TRUE:
        str = estrdup("1");
        *len = 1;
        *need_free = 1;
        break;

    case IS_FALSE:
        str = estrdup("0");
        *len = 1;
        *need_free = 1;
        break;

    default:
        /* Convert other types to string */
        zval copy;
        ZVAL_COPY(&copy, z);
        convert_to_string(&copy);
        str = estrndup(Z_STRVAL(copy), Z_STRLEN(copy));
        *len = Z_STRLEN(copy);
        zval_dtor(&copy);
        *need_free = 1;
        break;
    }

    return str;
}

/**
 * Free array of allocated strings
 */
void free_allocated_strings(char **strings, int count)
{
    if (!strings)
        return;

    for (int i = 0; i < count; i++)
    {
        if (strings[i])
        {
            efree(strings[i]);
        }
    }
}
