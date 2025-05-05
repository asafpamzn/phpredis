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

#ifndef REDIS_GLIDE_H
#define REDIS_GLIDE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Include the generated bindings */
#include "include/glide_bindings.h"

/* Forward declaration for ClientAdapter */
typedef struct ClientAdapter ClientAdapter;

/* Function declarations for Valkey Glide FFI */
extern void close_client_ffi(void *client_adapter_ptr);
extern void free_command_response(CommandResponse *command_response_ptr);
extern void free_command_result(CommandResult *command_result_ptr);

/* Helper functions for Valkey Glide integration */
extern void *create_glide_client(const char *host, int port, const char *user, const char *pass);
extern long execute_bitcount_command(void *glide_client, const char *key, size_t key_len, long start, long end, int bybit);

#endif /* REDIS_GLIDE_H */
