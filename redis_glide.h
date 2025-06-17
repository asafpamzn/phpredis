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
#include "include/glide_bindings.h"
#include "include/glide/connection_request.pb-c.h"

/* Forward declarations for types defined in glide_bindings.h */
typedef struct CommandResponse CommandResponse;
typedef struct CommandResult CommandResult;
typedef struct CommandError CommandError;
typedef struct ConnectionResponse ConnectionResponse;

enum ReadFrom
{
    /**
     * Primary: Read data from the primary node in the cluster.
     */
    Primary = 0,

    /**
     * PreferReplica: Prefer reading data from a replica node in the cluster, if
     * available.
     */
    PreferReplica = 1,

    /**
     * LowestLatency: Read data from the node with the lowest latency in the
     * cluster.
     */
    LowestLatency = 2,

    /**
     * AZAffinity: Read data from a node in the same availability zone as the
     * client, if possible.
     */
    AZAffinity = 3,
};

enum TLSMode
{
    /**
     * No TLS encryption is used for the connection.
     */
    NoTLS = 0,

    /**
     * TLS encryption is used for the connection with certificate verification.
     */
    SecureTLS = 1,

    /**
     * TLS encryption is used for the connection without certificate verification.
     */
    InsecureTLS = 2,
};

typedef struct
{
    // Credential credential_;
    enum TLSMode tls_mode_;
    uint32_t database_;
    uint32_t request_timeout_;
    char *client_name_;
    enum ReadFrom read_from_;
    bool is_cluster;
} ClientConfig;
/* Forward declaration for ClientAdapter */
typedef struct ClientAdapter ClientAdapter;

/* Function to close a Valkey Glide client */
extern void close_glide_client(const void *glide_client);
extern void free_command_response(CommandResponse *command_response_ptr);
extern void free_command_result(CommandResult *command_result_ptr);

/* Helper functions for Valkey Glide integration */
extern const void *create_glide_client(ClientConfig *config);

/* Bit operations - UNIFIED SIGNATURES */
extern int execute_bitcount_command(zval *object, int argc, zval *return_value);
extern int execute_bitop_command(zval *object, int argc, zval *return_value);
extern int execute_bitpos_command(zval *object, int argc, zval *return_value);

/* String operations */
extern int execute_set_command_internal(const void *glide_client, const char *key, size_t key_len, const char *val, size_t val_len, long expire, zval *opts, char **old_val, size_t *old_val_len);
extern int execute_set_command(zval *object, int argc, zval *return_value);
extern int execute_setex_command(zval *object, int argc, zval *return_value);
extern int execute_psetex_command(zval *object, int argc, zval *return_value);
extern int execute_setnx_command(zval *object, int argc, zval *return_value);
extern int execute_get_command(zval *object, int argc, zval *return_value);

/* Key operations */
extern int execute_randomkey_command(zval *object, int argc, zval *return_value);

/* Server operations */
extern int execute_echo_command(zval *object, int argc, zval *return_value);
extern int execute_ping_command(zval *object, int argc, zval *return_value);
extern int execute_reset_command(const void *glide_client);
extern int execute_info_command(zval *object, int argc, zval *return_value);
extern int execute_info_sections_command(const void *glide_client, zval *sections, int sections_count, char **result, size_t *result_len);

/* Additional operations */
extern int execute_getbit_command(zval *object, int argc, zval *return_value);
extern int execute_setbit_command(zval *object, int argc, zval *return_value);
int execute_del_command(zval *object, int argc, zval *return_value);
int execute_del_array(const void *glide_client, HashTable *keys_hash, long *output_value);
int execute_unlink_array(const void *glide_client, HashTable *keys_hash, long *output_value);
extern int execute_strlen_command(zval *object, int argc, zval *return_value);
extern int execute_setrange_command(zval *object, int argc, zval *return_value);
extern int execute_getset_command(zval *object, int argc, zval *return_value);
int execute_lcs_command(zval *object, int argc, zval *return_value);

/* Time to live operations */
extern int execute_ttl_command(zval *object, int argc, zval *return_value);
extern int execute_pttl_command(zval *object, int argc, zval *return_value);

/* Sorted set operations */

/* Hash operations */

/* New commands added in redis_glide_commands.c */
int execute_mset_command(const void *glide_client, zval *arr);
int execute_msetnx_command(const void *glide_client, zval *arr, long *output_value);
int execute_brpoplpush_command(const void *glide_client, const char *src, size_t src_len, const char *dst, size_t dst_len, zend_long timeout, char **result, size_t *result_len);

/* Object operations */
int execute_object_command(const void *glide_client,
                           const char *subcommand, size_t subcommand_len,
                           const char *key, size_t key_len,
                           zval *return_value);

/* Import the execute functions from redis_glide_keys.c */
int execute_keys_command(const void *glide_client, const char *pattern, size_t pattern_len, zval *return_value);

/* Unified command functions */
extern int execute_watch_command(zval *object, int argc, zval *return_value);
extern int execute_unwatch_command(zval *object, int argc, zval *return_value);
extern int execute_acl_command(zval *object, int argc, zval *return_value);
extern int execute_flushdb_command(zval *object, int argc, zval *return_value);
extern int execute_flushall_command(zval *object, int argc, zval *return_value);
extern int execute_time_command(zval *object, int argc, zval *return_value);
extern int execute_role_command(zval *object, int argc, zval *return_value);
extern int execute_servername_command(zval *object, int argc, zval *return_value);
extern int execute_serverversion_command(zval *object, int argc, zval *return_value);
extern int execute_scan_command(zval *object, int argc, zval *return_value);
extern int execute_sscan_command(zval *object, int argc, zval *return_value);
extern int execute_copy_command(zval *object, int argc, zval *return_value);
extern int execute_hscan_command(zval *object, int argc, zval *return_value);
extern int execute_pfadd_command(zval *object, int argc, zval *return_value);
extern int execute_pfcount_command(zval *object, int argc, zval *return_value);
extern int execute_pfmerge_command(zval *object, int argc, zval *return_value);
extern int execute_gettimeout_command(zval *object, int argc, zval *return_value);
extern int execute_getreadtimeout_command(zval *object, int argc, zval *return_value);
extern int execute_client_command(zval *object, int argc, zval *return_value);
extern int execute_rawcommand_command(zval *object, int argc, zval *return_value);
extern int execute_dbsize_command(zval *object, int argc, zval *return_value);
extern int execute_select_command(zval *object, int argc, zval *return_value);
extern int execute_swapdb_command(zval *object, int argc, zval *return_value);
extern int execute_move_command(zval *object, int argc, zval *return_value);

/* Don't include command_response.h here to avoid circular dependencies */

#endif /* REDIS_GLIDE_H */
