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

#ifndef COMMAND_RESPONSE_H
#define COMMAND_RESPONSE_H

#include "php_redis.h"
#include "include/glide_bindings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* No need to forward declare zval or ClientConfig as they are already defined in php.h and redis_glide.h */

/*
 * Execute a command and handle common error checking
 * Returns NULL if there was an error, otherwise returns the CommandResult
 * The caller is responsible for freeing the CommandResult using free_command_result()
 */
CommandResult *execute_command(
    const void *glide_client,
    enum RequestType command_type,
    unsigned long arg_count,
    const uintptr_t *args,
    const unsigned long *args_len);

/*
 * Handle an integer response
 * Returns 0 on error, 1 on success
 * The output_value parameter is set to the integer value on success
 * This function frees the CommandResult
 */
long handle_int_response(CommandResult *result, long *output_value);

/*
 * Handle a string response
 * Returns 1 on success, 0 if the key doesn't exist, -1 on error
 * The output and output_len parameters are set to the string value and length
 * The caller is responsible for freeing the output string using free()
 * This function frees the CommandResult
 */
int handle_string_response(CommandResult *result, char **output, size_t *output_len);

/*
 * Handle a boolean response
 * Returns 1 for true, 0 for false, -1 on error
 * This function frees the CommandResult
 */
int handle_bool_response(CommandResult *result);

/*
 * Handle an OK response
 * Returns 1 on success, -1 on error
 * This function frees the CommandResult
 */
int handle_ok_response(CommandResult *result);

/*
 * Handle a null or string response
 * Returns 1 if string, 0 if null, -1 on error
 * The output and output_len parameters are set to the string value and length if not null
 * The caller is responsible for freeing the output string using free()
 * This function frees the CommandResult
 */
int handle_null_or_string_response(CommandResult *result, char **output, size_t *output_len);

/*
 * Handle a double response
 * Returns 1 on success, -1 on error
 * The output parameter is set to the double value
 * This function frees the CommandResult
 */
int handle_double_response(CommandResult *result, double *output);

/*
 * Handle an array response
 * Returns 1 on success, 0 if null, -1 on error
 * The output parameter is set to a PHP array
 * This function frees the CommandResult
 */
int handle_array_response(CommandResult *result, zval *output);

/*
 * Handle a map response
 * Returns 1 on success, 0 if null, -1 on error
 * The output parameter is set to a PHP associative array
 * This function frees the CommandResult
 */
int handle_map_response(CommandResult *result, zval *output);

/*
 * Handle a set response
 * Returns 1 on success, 0 if null, -1 on error
 * The output parameter is set to a PHP array (with unique values)
 * This function frees the CommandResult
 */
int handle_set_response(CommandResult *result, zval *output);

/*
 * Helper function to convert a CommandResponse to a PHP value
 * Returns 1 on success, 0 if null, -1 on error
 * The output parameter is set to the PHP value
 */
int command_response_to_zval(CommandResponse *response, zval *output);

/*
 * Helper function to convert a long value to a string
 * Returns a newly allocated string or NULL on error
 * The caller is responsible for freeing the string using free()
 */
char *long_to_string(long value, size_t *len);

/*
 * Helper function to convert a double value to a string
 * Returns a newly allocated string or NULL on error
 * The caller is responsible for freeing the string using free()
 */
char *double_to_string(double value, size_t *len);

#endif /* COMMAND_RESPONSE_H */
