/*
  +----------------------------------------------------------------------+
  | Valkey Glide Z-Commands Common Utilities Header                      |
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

#ifndef VALKEY_GLIDE_Z_COMMON_H
#define VALKEY_GLIDE_Z_COMMON_H

#include "php_redis.h"
#include "include/glide_bindings.h"

/* ====================================================================
 * STRUCTURE DEFINITIONS
 * ==================================================================== */

/**
 * Range command options structure
 * Used for ZRANGE, ZREVRANGE, ZRANGEBYSCORE, etc.
 */
typedef struct
{
    int withscores;    /* WITHSCORES option */
    int byscore;       /* BYSCORE option */
    int bylex;         /* BYLEX option */
    int rev;           /* REV option */
    int has_limit;     /* Whether LIMIT is specified */
    long limit_offset; /* LIMIT offset value */
    long limit_count;  /* LIMIT count value */
} range_options_t;

/**
 * ZADD command options structure
 */
typedef struct
{
    int xx;   /* XX option - only update existing elements */
    int nx;   /* NX option - only add new elements */
    int lt;   /* LT option - only update if new score is less than current */
    int gt;   /* GT option - only update if new score is greater than current */
    int ch;   /* CH option - return number of changed elements */
    int incr; /* INCR option - increment the score */
} zadd_options_t;

/**
 * Store command options structure
 * Used for ZUNIONSTORE, ZINTERSTORE, etc.
 */
typedef struct
{
    zval *weights;     /* Weights array */
    int has_weights;   /* Whether weights are specified */
    zval *aggregate;   /* Aggregate option (SUM, MIN, MAX) */
    int has_aggregate; /* Whether aggregate is specified */
    int withscores;    /* WITHSCORES option */
} store_options_t;

/**
 * Generic Z-command arguments structure
 */
typedef struct
{
    /* Key arguments */
    const char *key;
    size_t key_len;

    /* Member arguments */
    const char *member;
    size_t member_len;

    /* Multiple members */
    zval *members;
    int member_count;

    /* Range arguments */
    const char *min;
    size_t min_len;
    const char *max;
    size_t max_len;

    /* Numeric range arguments */
    long start;
    long end;

    /* Score/increment arguments */
    double score;
    double increment;

    /* Options */
    zval *z_start;
    zval *z_end;
    zval *options;
    zval *weights;

    /* Command-specific flags */
    int withscores;

    /* Result destinations */
    long *long_result;
    double *double_result;
    zval *zval_result;
} z_command_args_t;

/**
 * Result processing callback type
 */
typedef int (*z_result_processor_t)(CommandResult *result, void *output);

/* ====================================================================
 * COMMON EXECUTION FRAMEWORK
 * ==================================================================== */

/**
 * Generic Z-command execution framework
 */
int execute_z_generic_command(
    const void *glide_client,
    enum RequestType cmd_type,
    z_command_args_t *args,
    void *result_ptr,
    z_result_processor_t process_result);

/**
 * Process integer result (for commands returning count)
 */
int process_z_int_result(CommandResult *result, void *output);

/**
 * Process double result (for commands returning scores)
 */
int process_z_double_result(CommandResult *result, void *output);

/**
 * Process null/exists result (for exists-type commands)
 */
int process_z_exists_result(CommandResult *result, void *output);

/**
 * Process array result (for commands returning arrays)
 */
int process_z_array_result(CommandResult *result, void *output);

/**
 * Process rank result with optional score
 */
int process_z_rank_result(CommandResult *result, void *output);

/* ====================================================================
 * ARGUMENT PREPARATION UTILITIES
 * ==================================================================== */

/**
 * Prepare basic Z-command arguments (just key)
 */
int prepare_z_key_args(z_command_args_t *args, uintptr_t **args_out,
                       unsigned long **args_len_out);

/**
 * Prepare member-based Z-command arguments (key + member)
 */
int prepare_z_member_args(z_command_args_t *args, uintptr_t **args_out,
                          unsigned long **args_len_out);

/**
 * Prepare range-based Z-command arguments (key + min + max)
 */
int prepare_z_range_args(z_command_args_t *args, uintptr_t **args_out,
                         unsigned long **args_len_out);

/**
 * Prepare multi-member Z-command arguments (key + multiple members)
 */
int prepare_z_members_args(z_command_args_t *args, uintptr_t **args_out,
                           unsigned long **args_len_out,
                           char ***allocated_strings, int *allocated_count);

/**
 * Prepare complex range Z-command arguments with options
 */
int prepare_z_complex_range_args(z_command_args_t *args, uintptr_t **args_out,
                                 unsigned long **args_len_out,
                                 char ***allocated_strings, int *allocated_count);

/* ====================================================================
 * OPTIONS PARSING HELPERS
 * ==================================================================== */

/**
 * Parse range command options (withscores, byscore, bylex, rev, limit)
 * Returns 1 on success, 0 on failure
 */
int parse_range_options(zval *options, range_options_t *opts);

/**
 * Parse ZADD command options (XX, NX, LT, GT, CH, INCR)
 * Returns 1 on success, 0 on failure
 */
int parse_zadd_options(zval *options, zadd_options_t *opts);

/**
 * Parse store command options (weights, aggregate) for ZUNIONSTORE-style commands
 * Returns 1 on success, 0 on failure
 */
int parse_store_options(zval *weights, zval *options, store_options_t *opts);

/* ====================================================================
 * CONVERSION & UTILITY HELPERS
 * ==================================================================== */

/**
 * Safe zval to string conversion with memory management
 * Returns allocated string that must be freed, or NULL on error
 * Sets need_free to 1 if returned string must be freed
 */
char *zval_to_string_safe(zval *z, size_t *len, int *need_free);

/**
 * Prepare keys array from zval for multi-key operations
 * Returns keys count on success, 0 on failure
 */
int prepare_keys_array(zval *keys, uintptr_t **args, unsigned long **args_len);

/**
 * Create LIMIT arguments (offset, count)
 * Returns number of arguments added (0 or 3)
 */
int create_limit_args(range_options_t *opts, uintptr_t *args, unsigned long *args_len,
                      int start_idx, char **allocated_strings, int *allocated_count);

/* ====================================================================
 * RESPONSE PROCESSING HELPERS
 * ==================================================================== */

/**
 * Handle score response (for ZSCORE, ZINCRBY)
 * Returns: 1 = success with score, 0 = member not found, -1 = error
 */
int handle_score_response(CommandResult *result, double *score);

/**
 * Handle rank response (for ZRANK, ZREVRANK)
 * Returns: 1 = success with rank, 0 = member not found, -1 = error
 */
int handle_rank_response(CommandResult *result, long *rank, double *score, int withscore);

/**
 * Flatten withscores array from [[member, score]] to [member => score]
 * Returns 1 on success, 0 on failure
 */
int flatten_withscores_array(zval *return_value);

/**
 * Free array of allocated strings
 */
void free_allocated_strings(char **strings, int count);

/* ====================================================================
 * Z COMMAND IMPLEMENTATION FUNCTIONS (THIN WRAPPERS)
 * ==================================================================== */

/* Traditional function signatures (original) */
int execute_zrandmember_command(const void *glide_client, const char *key, size_t key_len, long count, int withscores, zval *return_value);
int execute_zscore_command(const void *glide_client, const char *key, size_t key_len, const char *member, size_t member_len, double *score);
int execute_zmscore_command(const void *glide_client, const char *key, size_t key_len, zval *members, int member_count, zval *return_value);
int execute_zrank_command(const void *glide_client, const char *key, size_t key_len, const char *member, size_t member_len, int withscore, long *rank, double *score);
int execute_zrevrank_command(const void *glide_client, const char *key, size_t key_len, const char *member, size_t member_len, int withscore, long *rank, double *score);
int execute_zincrby_command(const void *glide_client, const char *key, size_t key_len, double increment, const char *member, size_t member_len, double *new_score);
int execute_zcount_command(const void *glide_client, const char *key, size_t key_len, const char *min, size_t min_len, const char *max, size_t max_len, long *count);
int execute_zlexcount_command(const void *glide_client, const char *key, size_t key_len, const char *min, size_t min_len, const char *max, size_t max_len, long *count);
int execute_zrem_command(const void *glide_client, const char *key, size_t key_len, zval *members, int member_count, long *removed_count);
int execute_zremrangebylex_command(const void *glide_client, const char *key, size_t key_len, const char *min, size_t min_len, const char *max, size_t max_len, long *removed_count);
int execute_zremrangebyrank_command(const void *glide_client, const char *key, size_t key_len, long start, long stop, long *removed_count);
int execute_zrange_command(const void *glide_client, const char *key, size_t key_len, zval *z_start, zval *z_end, zval *z_options, zval *return_value);
int execute_zcard_command(const void *glide_client, const char *key, size_t key_len, long *output_value);

/* New thin wrapper signatures (for PHP_METHOD implementations) */
int redis_zrandmember_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zscore_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zmscore_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zrank_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zrevrank_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zincrby_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zcount_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zlexcount_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zrem_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zremrangebylex_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zremrangebyrank_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zremrangebyscore_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zrange_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zcard_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zrevrange_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zrangebyscore_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zrevrangebyscore_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zrangebylex_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zrevrangebylex_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zadd_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);
int redis_zrangestore_cmd(const void *glide_client, int argc, zval *getThis, zval *return_value);

/* Z command implementations from redis_new14.c */
int execute_zintercard_command(const void *glide_client, int argc, zval *getThis, zval *return_value);
int execute_zunion_command(const void *glide_client, int argc, zval *getThis, zval *return_value);
int execute_zdiffstore_command(const void *glide_client, int argc, zval *getThis, zval *return_value);
int execute_zinterstore_command(const void *glide_client, int argc, zval *getThis, zval *return_value);
int execute_zunionstore_command(const void *glide_client, int argc, zval *getThis, zval *return_value);
int execute_zpopmax_command(const void *glide_client, int argc, zval *getThis, zval *return_value);
int execute_zpopmin_command(const void *glide_client, int argc, zval *getThis, zval *return_value);
int execute_zscan_command(const void *glide_client, int argc, zval *getThis, zval *return_value);
int execute_zdiff_command(const void *glide_client, int argc, zval *getThis, zval *return_value);
int execute_zinter_command(const void *glide_client, int argc, zval *getThis, zval *return_value);
int execute_zmpop_command(const void *glide_client, const char *cmd, double timeout, zval *keys, const char *from, size_t from_len, long count, zval *result);

/* ====================================================================
 * UTILITY MACROS
 * ==================================================================== */

/**
 * Check if glide client is available and return FALSE if not
 */
#define CHECK_GLIDE_CLIENT(redis)               \
    do                                          \
    {                                           \
        if (!(redis) || !(redis)->glide_client) \
        {                                       \
            RETURN_FALSE;                       \
        }                                       \
    } while (0)

/**
 * Initialize array return value and check for allocation success
 */
#define INIT_RETURN_ARRAY(return_value) \
    do                                  \
    {                                   \
        array_init(return_value);       \
    } while (0)

/**
 * Clean up and return FALSE on failure
 */
#define CLEANUP_AND_RETURN_FALSE(return_value)                  \
    do                                                          \
    {                                                           \
        if (return_value && Z_TYPE_P(return_value) == IS_ARRAY) \
        {                                                       \
            zval_dtor(return_value);                            \
        }                                                       \
        RETURN_FALSE;                                           \
    } while (0)

#endif /* VALKEY_GLIDE_Z_COMMON_H */
