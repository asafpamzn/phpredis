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

/* Bit operations */
extern int execute_bitcount_command(const void *glide_client, const char *key, size_t key_len, long start, long end, int bybit, long *output_value);
extern int execute_bitop_command(const void *glide_client, const char *op, size_t op_len, const char *dst, size_t dst_len, zval *keys, int keys_count, long *output_value);
extern int execute_bitpos_command(const void *glide_client, const char *key, size_t key_len, long bit, long start, long end, int bybit, long *output_value);

/* String operations */
extern int execute_set_command(const void *glide_client, const char *key, size_t key_len, const char *val, size_t val_len, long expire, zval *opts, char **old_val, size_t *old_val_len);
extern int execute_setex_command(const void *glide_client, const char *key, size_t key_len, long expire, const char *val, size_t val_len);
extern int execute_psetex_command(const void *glide_client, const char *key, size_t key_len, long expire, const char *val, size_t val_len);
extern int execute_setnx_command(const void *glide_client, const char *key, size_t key_len, const char *val, size_t val_len);
extern int execute_get_command(const void *glide_client, const char *key, size_t key_len, char **result, size_t *result_len);
extern int execute_getwithmeta_command(const void *glide_client, const char *key, size_t key_len, char **result, size_t *result_len);
extern int execute_getdel_command(const void *glide_client, const char *key, size_t key_len, char **result, size_t *result_len);
extern int execute_getex_command(const void *glide_client, const char *key, size_t key_len, zval *opts, char **result, size_t *result_len);

/* Key operations */
extern int execute_randomkey_command(const void *glide_client, char **result, size_t *result_len);
extern int execute_rename_command(const void *glide_client, const char *src, size_t src_len, const char *dst, size_t dst_len);
extern int execute_renamenx_command(const void *glide_client, const char *src, size_t src_len, const char *dst, size_t dst_len);

/* Server operations */
extern int execute_echo_command(const void *glide_client, const char *msg, size_t msg_len, char **result, size_t *result_len);
extern int execute_ping_command(const void *glide_client, const char *msg, size_t msg_len, char **result, size_t *result_len);
extern int execute_reset_command(const void *glide_client);
extern int execute_info_command(const void *glide_client, const char *section, size_t section_len, char **result, size_t *result_len);
extern int execute_info_sections_command(const void *glide_client, zval *sections, int sections_count, char **result, size_t *result_len);

/* Increment operations */
long execute_incr_command(const void *glide_client, const char *key, size_t key_len);
extern long execute_incrby_command(const void *glide_client, const char *key, size_t key_len, long value);
extern int execute_incrbyfloat_command(const void *glide_client, const char *key, size_t key_len, double value, double *result);

int execute_decr_command(const void *glide_client, const char *key, size_t key_len, long *output_value);
int execute_decrby_command(const void *glide_client, const char *key, size_t key_len, long decrement, long *output_value);

/* Additional operations */
extern int execute_getbit_command(const void *glide_client, const char *key, size_t key_len, long offset, long *output_value);
extern int execute_setbit_command(const void *glide_client, const char *key, size_t key_len, long offset, int value, long *output_value);
extern int execute_del_command(const void *glide_client, zval *keys, int keys_count, long *output_value);
int execute_del_array(const void *glide_client, HashTable *keys_hash, long *output_value);
int execute_unlink_array(const void *glide_client, HashTable *keys_hash, long *output_value);
extern int execute_strlen_command(const void *glide_client, const char *key, size_t key_len, long *output_value);
extern int execute_setrange_command(const void *glide_client, const char *key, size_t key_len, long offset, const char *value, size_t value_len, long *output_value);
extern int execute_lcs_command(const void *glide_client, const char *key1, size_t key1_len, const char *key2, size_t key2_len, zval *options, zval *result);

/* Time to live operations */
extern int execute_ttl_command(const void *glide_client, const char *key, size_t key_len, long *output_value);
extern int execute_pttl_command(const void *glide_client, const char *key, size_t key_len, long *output_value);
extern int execute_expire_command(const void *glide_client, const char *key, size_t key_len, long seconds, const char *mode, size_t mode_len);
extern int execute_expireat_command(const void *glide_client, const char *key, size_t key_len, long timestamp, const char *mode, size_t mode_len);
extern int execute_pexpire_command(const void *glide_client, const char *key, size_t key_len, long milliseconds, const char *mode, size_t mode_len);
extern int execute_pexpireat_command(const void *glide_client, const char *key, size_t key_len, long timestamp_ms, const char *mode, size_t mode_len);
extern int execute_persist_command(const void *glide_client, const char *key, size_t key_len);
extern int execute_expiretime_command(const void *glide_client, const char *key, size_t key_len, long *output_value);
extern int execute_pexpiretime_command(const void *glide_client, const char *key, size_t key_len, long *output_value);

/* Sorted set operations */

/* Hash operations */

int execute_mget_command(const void *glide_client, zval *keys, zval *return_value);
int execute_exists_command(const void *glide_client, zval *keys, int keys_count, long *output_value);
int execute_touch_command(const void *glide_client, zval *keys, int keys_count, long *output_value);
int execute_unlink_command(const void *glide_client, zval *keys, int keys_count, long *output_value);

/* New commands added in redis_glide_commands.c */
int execute_setOption_command(const void *glide_client, zend_long option, zval *value);
int execute_mset_command(const void *glide_client, zval *arr);
int execute_msetnx_command(const void *glide_client, zval *arr, int *output_value);
int execute_brpoplpush_command(const void *glide_client, const char *src, size_t src_len, const char *dst, size_t dst_len, zend_long timeout, char **result, size_t *result_len);

/* Object operations */
int execute_object_command(const void *glide_client,
                           const char *subcommand, size_t subcommand_len,
                           const char *key, size_t key_len,
                           zval *return_value);

/* Import the execute functions from redis_glide_keys.c */
int execute_keys_command(const void *glide_client, const char *pattern, size_t pattern_len, zval *return_value);

/* Don't include command_response.h here to avoid circular dependencies */

#endif /* REDIS_GLIDE_H */
