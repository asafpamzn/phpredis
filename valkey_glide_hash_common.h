/*
  +----------------------------------------------------------------------+
  | Valkey Glide Hash Commands Common Utilities                          |
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

#ifndef VALKEY_GLIDE_HASH_COMMON_H
#define VALKEY_GLIDE_HASH_COMMON_H

#include "php_redis.h"
#include "redis_glide.h"
#include "command_response.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ====================================================================
 * STRUCTURES AND TYPES
 * ==================================================================== */

/**
 * Generic hash command arguments structure
 */
typedef struct _h_command_args_t
{
    /* Core fields */
    const void *glide_client; /* GlideClient instance */
    const char *key;          /* Hash key */
    size_t key_len;           /* Hash key length */

    /* Single field operations (HGET, HEXISTS, HSTRLEN, HSETNX) */
    char *field;      /* Field name */
    size_t field_len; /* Field name length */
    char *value;      /* Field value */
    size_t value_len; /* Field value length */

    /* Multi-field operations (HDEL, HMGET, HKEYS, HVALS) */
    zval *fields;    /* Array of field names */
    int field_count; /* Number of fields */

    /* Field-value operations (HSET, HMSET) */
    zval *field_values; /* Associative array or alternating field/value array */
    int fv_count;       /* Number of field-value pairs */
    int is_array_arg;   /* Whether using associative array format */

    /* Increment operations (HINCRBY, HINCRBYFLOAT) */
    long increment;    /* Integer increment value */
    double float_incr; /* Float increment value */

    /* HRANDFIELD specific */
    long count;     /* Number of fields to return */
    int withvalues; /* Whether to return values with fields */
} h_command_args_t;

/**
 * Function pointer types for result processing
 */
typedef int (*h_result_processor_t)(CommandResult *result, void *output);

/**
 * Argument preparation function type
 */
typedef int (*h_arg_preparer_t)(h_command_args_t *args, uintptr_t **args_out,
                                unsigned long **args_len_out,
                                char ***allocated_strings, int *allocated_count);

/* ====================================================================
 * CORE FRAMEWORK FUNCTIONS
 * ==================================================================== */

/**
 * Generic hash command execution framework
 */
int execute_h_generic_command(const void *glide_client,
                              enum RequestType cmd_type,
                              h_command_args_t *args,
                              void *result_ptr,
                              h_result_processor_t process_result);

/**
 * Simplified execution for commands using standard response handlers
 */
int execute_h_simple_command(const void *glide_client,
                             enum RequestType cmd_type,
                             h_command_args_t *args,
                             void *result_ptr,
                             int response_type);

/* ====================================================================
 * ARGUMENT PREPARATION FUNCTIONS
 * ==================================================================== */

/**
 * Prepare arguments for single-key commands (HLEN)
 */
int prepare_h_key_only_args(h_command_args_t *args, uintptr_t **args_out,
                            unsigned long **args_len_out,
                            char ***allocated_strings, int *allocated_count);

/**
 * Prepare arguments for single-field commands (HGET, HEXISTS, HSTRLEN)
 */
int prepare_h_single_field_args(h_command_args_t *args, uintptr_t **args_out,
                                unsigned long **args_len_out,
                                char ***allocated_strings, int *allocated_count);

/**
 * Prepare arguments for field-value commands (HSETNX)
 */
int prepare_h_field_value_args(h_command_args_t *args, uintptr_t **args_out,
                               unsigned long **args_len_out,
                               char ***allocated_strings, int *allocated_count);

/**
 * Prepare arguments for multi-field commands (HDEL, HMGET)
 */
int prepare_h_multi_field_args(h_command_args_t *args, uintptr_t **args_out,
                               unsigned long **args_len_out,
                               char ***allocated_strings, int *allocated_count);

/**
 * Prepare arguments for HSET command (handles both formats)
 */
int prepare_h_set_args(h_command_args_t *args, uintptr_t **args_out,
                       unsigned long **args_len_out,
                       char ***allocated_strings, int *allocated_count);

/**
 * Prepare arguments for HMSET command
 */
int prepare_h_mset_args(h_command_args_t *args, uintptr_t **args_out,
                        unsigned long **args_len_out,
                        char ***allocated_strings, int *allocated_count);

/**
 * Prepare arguments for increment commands (HINCRBY, HINCRBYFLOAT)
 */
int prepare_h_incr_args(h_command_args_t *args, uintptr_t **args_out,
                        unsigned long **args_len_out,
                        char ***allocated_strings, int *allocated_count);

/**
 * Prepare arguments for HRANDFIELD command
 */
int prepare_h_randfield_args(h_command_args_t *args, uintptr_t **args_out,
                             unsigned long **args_len_out,
                             char ***allocated_strings, int *allocated_count);

/* ====================================================================
 * RESULT PROCESSING FUNCTIONS
 * ==================================================================== */

/**
 * Process results for HMGET (associative field mapping)
 */
int process_h_mget_result(CommandResult *result, void *output);

/**
 * Process results for HRANDFIELD
 */
int process_h_randfield_result(CommandResult *result, void *output);

/**
 * Process results for HINCRBYFLOAT
 */
int process_h_incrbyfloat_result(CommandResult *result, void *output);

/* ====================================================================
 * UTILITY FUNCTIONS
 * ==================================================================== */

/**
 * Convert zval array to command arguments with proper string conversion
 */
int convert_zval_array_to_args(zval *z_array, int start_index,
                               uintptr_t *args, unsigned long *args_len,
                               char **allocated_strings, int *allocated_count,
                               int max_allocations);

/**
 * Process field-value pairs from associative array
 */
int process_field_value_pairs(zval *field_values, uintptr_t *args, unsigned long *args_len,
                              int start_index, char **allocated_strings, int *allocated_count);

/**
 * Safe cleanup for allocated argument strings
 */
void cleanup_h_command_args(char **allocated_strings, int allocated_count,
                            uintptr_t *args, unsigned long *args_len);

/* ====================================================================
 * RESPONSE TYPE CONSTANTS
 * ==================================================================== */

#define H_RESPONSE_INT 1
#define H_RESPONSE_STRING 2
#define H_RESPONSE_BOOL 3
#define H_RESPONSE_ARRAY 4
#define H_RESPONSE_MAP 5
#define H_RESPONSE_OK 6
#define H_RESPONSE_CUSTOM 7

/* ====================================================================
 * HASH COMMAND EXECUTION FUNCTIONS
 * ==================================================================== */

/* Basic hash operations */
int execute_h_get_command(const void *glide_client, const char *key, size_t key_len,
                          char *field, size_t field_len,
                          char **result, size_t *result_len);

int execute_h_len_command(const void *glide_client, const char *key, size_t key_len,
                          long *output_value);

int execute_h_exists_command(const void *glide_client, const char *key, size_t key_len,
                             char *field, size_t field_len, int *output_value);

int execute_h_del_command(const void *glide_client, const char *key, size_t key_len,
                          zval *fields, int fields_count, long *output_value);

/* Field modification operations */
int execute_h_set_command(const void *glide_client, const char *key, size_t key_len,
                          zval *z_args, int argc, long *output_value, int is_array_arg);

int execute_h_setnx_command(const void *glide_client, const char *key, size_t key_len,
                            char *field, size_t field_len,
                            char *value, size_t value_len, int *output_value);

int execute_h_mset_command(const void *glide_client, const char *key, size_t key_len,
                           zval *keyvals, int keyvals_count);

/* Increment operations */
int execute_h_incrby_command(const void *glide_client, const char *key, size_t key_len,
                             char *field, size_t field_len,
                             long increment, long *output_value);

int execute_h_incrbyfloat_command(const void *glide_client, const char *key, size_t key_len,
                                  char *field, size_t field_len,
                                  double increment, double *output_value);

/* Multi-field operations */
int execute_h_mget_command(const void *glide_client, const char *key, size_t key_len,
                           zval *fields, int fields_count, zval *return_value);

int execute_h_keys_command(const void *glide_client, const char *key, size_t key_len,
                           zval *return_value);

int execute_h_vals_command(const void *glide_client, const char *key, size_t key_len,
                           zval *return_value);

int execute_h_getall_command(const void *glide_client, const char *key, size_t key_len,
                             zval *return_value);

/* Advanced operations */
int execute_h_strlen_command(const void *glide_client, const char *key, size_t key_len,
                             char *field, size_t field_len, long *output_value);

int execute_h_randfield_command(const void *glide_client, const char *key, size_t key_len,
                                long count, int withvalues, zval *return_value);

/* ====================================================================
 * CONVENIENCE MACROS
 * ==================================================================== */

/**
 * Macro to define PHP method implementations for hash commands
 */
#define HASH_METHOD_IMPL(class_name, method_name, execute_func, ...) \
    PHP_METHOD(class_name, method_name)                              \
    {                                                                \
        if (execute_func(__VA_ARGS__))                               \
        {                                                            \
            return;                                                  \
        }                                                            \
        zval_dtor(return_value);                                     \
        RETURN_FALSE;                                                \
    }

/**
 * Helper macro for basic validation
 */
#define VALIDATE_HASH_ARGS(glide_client, key) \
    do                                        \
    {                                         \
        if (!(glide_client) || !(key))        \
        {                                     \
            return 0;                         \
        }                                     \
    } while (0)

#endif /* VALKEY_GLIDE_HASH_COMMON_H */
