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

int execute_zrandmember_command(const void *glide_client, const char *key, size_t key_len, long count, int withscores, zval *return_value)
{
    CommandRequestType cmd_type = REDIS_COMMAND_ZRANDMEMBER;
    CommandRequest *request = create_command_request(cmd_type);

    // Add the key parameter
    command_request_add_string(request, key, key_len);

    // Add count parameter
    char count_str[32];
    int count_str_len = snprintf(count_str, sizeof(count_str), "%ld", count);
    command_request_add_string(request, count_str, count_str_len);

    // Add WITHSCORES if required
    if (withscores)
    {
        command_request_add_string(request, "WITHSCORES", 10);
    }

    // Execute the command
    CommandResponse *response = execute_command(glide_client, request);

    // Free the request
    command_request_free(request);

    // Process response
    if (!response)
    {
        return 0;
    }

    int success = 0;

    CommandResult *result = response->result;
    if (result && (result->type == Array))
    {
        parse_array_response_to_zval(result, return_value);
        success = 1;
    }

    free_command_response(response);

    return success;
}

int execute_zscore_command(const void *glide_client, const char *key, size_t key_len,
                           const char *member, size_t member_len, double *output_value)
{
    CommandRequestType cmd_type = REDIS_COMMAND_ZSCORE;
    CommandRequest *request = create_command_request(cmd_type);

    // Add the key parameter
    command_request_add_string(request, key, key_len);

    // Add member parameter
    command_request_add_string(request, member, member_len);

    // Execute the command
    CommandResponse *response = execute_command(glide_client, request);

    // Free the request
    command_request_free(request);

    // Process response
    if (!response)
    {
        return -1; // Error
    }

    CommandResult *result = response->result;
    int success = 0;

    if (result)
    {
        if (result->type == Null)
        {
            // Member doesn't exist in the sorted set
            success = 0;
        }
        else if (result->type == String)
        {
            // Parse string as double
            if (safe_strtod(result->string_value, result->string_length, output_value))
            {
                success = 1;
            }
        }
    }

    free_command_response(response);

    return success;
}

int execute_zmscore_command(const void *glide_client, const char *key, size_t key_len,
                            zval *members, int members_count, zval *return_value)
{
    CommandRequestType cmd_type = REDIS_COMMAND_ZMSCORE;
    CommandRequest *request = create_command_request(cmd_type);

    // Add the key parameter
    command_request_add_string(request, key, key_len);

    // Add all member parameters
    int i;
    for (i = 0; i < members_count; i++)
    {
        zval *z_member = &members[i];
        convert_to_string(z_member);
        command_request_add_string(request, Z_STRVAL_P(z_member), Z_STRLEN_P(z_member));
    }

    // Execute the command
    CommandResponse *response = execute_command(glide_client, request);

    // Free the request
    command_request_free(request);

    // Process response
    if (!response)
    {
        return 0; // Error
    }

    int success = 0;

    CommandResult *result = response->result;
    if (result && (result->type == Array))
    {
        parse_array_response_to_zval(result, return_value);
        success = 1;
    }

    free_command_response(response);

    return success;
}

int execute_zrank_command(const void *glide_client, const char *key, size_t key_len,
                          const char *member, size_t member_len, int withscore,
                          long *rank_value, double *score_value)
{
    CommandRequestType cmd_type = REDIS_COMMAND_ZRANK;
    CommandRequest *request = create_command_request(cmd_type);

    // Add the key parameter
    command_request_add_string(request, key, key_len);

    // Add member parameter
    command_request_add_string(request, member, member_len);

    // Add WITHSCORE if required
    if (withscore)
    {
        command_request_add_string(request, "WITHSCORE", 9);
    }

    // Execute the command
    CommandResponse *response = execute_command(glide_client, request);

    // Free the request
    command_request_free(request);

    // Process response
    if (!response)
    {
        return -1; // Error
    }

    CommandResult *result = response->result;
    int success = 0;

    if (result)
    {
        if (result->type == Null)
        {
            // Member doesn't exist in the sorted set
            success = 0;
        }
        else if (result->type == Integer)
        {
            // Integer rank
            *rank_value = result->int_value;
            success = 1;
        }
        else if (result->type == Array && withscore)
        {
            // Array with rank and score [rank, score]
            Array *rank_array = result->array_value;
            if (rank_array && rank_array->length == 2 &&
                rank_array->items[0]->type == Integer &&
                (rank_array->items[1]->type == String || rank_array->items[1]->type == Double))
            {

                *rank_value = rank_array->items[0]->int_value;

                if (rank_array->items[1]->type == String)
                {
                    safe_strtod(rank_array->items[1]->string_value, rank_array->items[1]->string_length, score_value);
                }
                else
                {
                    *score_value = rank_array->items[1]->double_value;
                }

                success = 1;
            }
        }
    }

    free_command_response(response);

    return success;
}

int execute_zrevrank_command(const void *glide_client, const char *key, size_t key_len,
                             const char *member, size_t member_len, int withscore,
                             long *rank_value, double *score_value)
{
    CommandRequestType cmd_type = REDIS_COMMAND_ZREVRANK;
    CommandRequest *request = create_command_request(cmd_type);

    // Add the key parameter
    command_request_add_string(request, key, key_len);

    // Add member parameter
    command_request_add_string(request, member, member_len);

    // Add WITHSCORE if required
    if (withscore)
    {
        command_request_add_string(request, "WITHSCORE", 9);
    }

    // Execute the command
    CommandResponse *response = execute_command(glide_client, request);

    // Free the request
    command_request_free(request);

    // Process response
    if (!response)
    {
        return -1; // Error
    }

    CommandResult *result = response->result;
    int success = 0;

    if (result)
    {
        if (result->type == Null)
        {
            // Member doesn't exist in the sorted set
            success = 0;
        }
        else if (result->type == Integer)
        {
            // Integer rank
            *rank_value = result->int_value;
            success = 1;
        }
        else if (result->type == Array && withscore)
        {
            // Array with rank and score [rank, score]
            Array *rank_array = result->array_value;
            if (rank_array && rank_array->length == 2 &&
                rank_array->items[0]->type == Integer &&
                (rank_array->items[1]->type == String || rank_array->items[1]->type == Double))
            {

                *rank_value = rank_array->items[0]->int_value;

                if (rank_array->items[1]->type == String)
                {
                    safe_strtod(rank_array->items[1]->string_value, rank_array->items[1]->string_length, score_value);
                }
                else
                {
                    *score_value = rank_array->items[1]->double_value;
                }

                success = 1;
            }
        }
    }

    free_command_response(response);

    return success;
}

int execute_zincrby_command(const void *glide_client, const char *key, size_t key_len,
                            double increment, const char *member, size_t member_len,
                            double *output_value)
{
    CommandRequestType cmd_type = REDIS_COMMAND_ZINCRBY;
    CommandRequest *request = create_command_request(cmd_type);

    // Add the key parameter
    command_request_add_string(request, key, key_len);

    // Add increment parameter
    char increment_str[64];
    int increment_str_len = snprintf(increment_str, sizeof(increment_str), "%f", increment);
    command_request_add_string(request, increment_str, increment_str_len);

    // Add member parameter
    command_request_add_string(request, member, member_len);

    // Execute the command
    CommandResponse *response = execute_command(glide_client, request);

    // Free the request
    command_request_free(request);

    // Process response
    if (!response)
    {
        return 0;
    }

    CommandResult *result = response->result;
    int success = 0;

    if (result)
    {
        if (result->type == String)
        {
            // Parse string as double
            success = safe_strtod(result->string_value, result->string_length, output_value);
        }
        else if (result->type == Double)
        {
            *output_value = result->double_value;
            success = 1;
        }
    }

    free_command_response(response);

    return success;
}

int execute_zcount_command(const void *glide_client, const char *key, size_t key_len,
                           const char *min, size_t min_len, const char *max, size_t max_len,
                           long *output_value)
{
    CommandRequestType cmd_type = REDIS_COMMAND_ZCOUNT;
    CommandRequest *request = create_command_request(cmd_type);

    // Add the key parameter
    command_request_add_string(request, key, key_len);

    // Add min and max parameters
    command_request_add_string(request, min, min_len);
    command_request_add_string(request, max, max_len);

    // Execute the command
    CommandResponse *response = execute_command(glide_client, request);

    // Free the request
    command_request_free(request);

    // Process response
    if (!response)
    {
        return 0;
    }

    CommandResult *result = response->result;
    int success = 0;

    if (result && result->type == Integer)
    {
        *output_value = result->int_value;
        success = 1;
    }

    free_command_response(response);

    return success;
}

int execute_zlexcount_command(const void *glide_client, const char *key, size_t key_len,
                              const char *min, size_t min_len, const char *max, size_t max_len,
                              long *output_value)
{
    CommandRequestType cmd_type = REDIS_COMMAND_ZLEXCOUNT;
    CommandRequest *request = create_command_request(cmd_type);

    // Add the key parameter
    command_request_add_string(request, key, key_len);

    // Add min and max parameters
    command_request_add_string(request, min, min_len);
    command_request_add_string(request, max, max_len);

    // Execute the command
    CommandResponse *response = execute_command(glide_client, request);

    // Free the request
    command_request_free(request);

    // Process response
    if (!response)
    {
        return 0;
    }

    CommandResult *result = response->result;
    int success = 0;

    if (result && result->type == Integer)
    {
        *output_value = result->int_value;
        success = 1;
    }

    free_command_response(response);

    return success;
}

int execute_zrem_command(const void *glide_client, const char *key, size_t key_len,
                         zval *members, int members_count, long *output_value)
{
    CommandRequestType cmd_type = REDIS_COMMAND_ZREM;
    CommandRequest *request = create_command_request(cmd_type);

    // Add the key parameter
    command_request_add_string(request, key, key_len);

    // Add all member parameters
    int i;
    for (i = 0; i < members_count; i++)
    {
        zval *z_member = &members[i];
        convert_to_string(z_member);
        command_request_add_string(request, Z_STRVAL_P(z_member), Z_STRLEN_P(z_member));
    }

    // Execute the command
    CommandResponse *response = execute_command(glide_client, request);

    // Free the request
    command_request_free(request);

    // Process response
    if (!response)
    {
        return 0;
    }

    CommandResult *result = response->result;
    int success = 0;

    if (result && result->type == Integer)
    {
        *output_value = result->int_value;
        success = 1;
    }

    free_command_response(response);

    return success;
}

int execute_zremrangebylex_command(const void *glide_client, const char *key, size_t key_len,
                                   const char *min, size_t min_len, const char *max, size_t max_len,
                                   long *output_value)
{
    CommandRequestType cmd_type = REDIS_COMMAND_ZREMRANGEBYLEX;
    CommandRequest *request = create_command_request(cmd_type);

    // Add the key parameter
    command_request_add_string(request, key, key_len);

    // Add min and max parameters
    command_request_add_string(request, min, min_len);
    command_request_add_string(request, max, max_len);

    // Execute the command
    CommandResponse *response = execute_command(glide_client, request);

    // Free the request
    command_request_free(request);

    // Process response
    if (!response)
    {
        return 0;
    }

    CommandResult *result = response->result;
    int success = 0;

    if (result && result->type == Integer)
    {
        *output_value = result->int_value;
        success = 1;
    }

    free_command_response(response);

    return success;
}

int execute_zremrangebyrank_command(const void *glide_client, const char *key, size_t key_len,
                                    long start, long end, long *output_value)
{
    CommandRequestType cmd_type = REDIS_COMMAND_ZREMRANGEBYRANK;
    CommandRequest *request = create_command_request(cmd_type);

    // Add the key parameter
    command_request_add_string(request, key, key_len);

    // Add start and end parameters
    char start_str[32], end_str[32];
    int start_str_len = snprintf(start_str, sizeof(start_str), "%ld", start);
    int end_str_len = snprintf(end_str, sizeof(end_str), "%ld", end);

    command_request_add_string(request, start_str, start_str_len);
    command_request_add_string(request, end_str, end_str_len);

    // Execute the command
    CommandResponse *response = execute_command(glide_client, request);

    // Free the request
    command_request_free(request);

    // Process response
    if (!response)
    {
        return 0;
    }

    CommandResult *result = response->result;
    int success = 0;

    if (result && result->type == Integer)
    {
        *output_value = result->int_value;
        success = 1;
    }

    free_command_response(response);

    return success;
}

int execute_zremrangebyscore_command(const void *glide_client, const char *key, size_t key_len,
                                     const char *min, size_t min_len, const char *max, size_t max_len,
                                     long *output_value)
{
    CommandRequestType cmd_type = REDIS_COMMAND_ZREMRANGEBYSCORE;
    CommandRequest *request = create_command_request(cmd_type);

    // Add the key parameter
    command_request_add_string(request, key, key_len);

    // Add min and max parameters
    command_request_add_string(request, min, min_len);
    command_request_add_string(request, max, max_len);

    // Execute the command
    CommandResponse *response = execute_command(glide_client, request);

    // Free the request
    command_request_free(request);

    // Process response
    if (!response)
    {
        return 0;
    }

    CommandResult *result = response->result;
    int success = 0;

    if (result && result->type == Integer)
    {
        *output_value = result->int_value;
        success = 1;
    }

    free_command_response(response);

    return success;
}

int execute_zcard_command(const void *glide_client, const char *key, size_t key_len, long *output_value)
{
    CommandRequestType cmd_type = REDIS_COMMAND_ZCARD;
    CommandRequest *request = create_command_request(cmd_type);

    // Add the key parameter
    command_request_add_string(request, key, key_len);

    // Execute the command
    CommandResponse *response = execute_command(glide_client, request);

    // Free the request
    command_request_free(request);

    // Process response
    if (!response)
    {
        return 0;
    }

    CommandResult *result = response->result;
    int success = 0;

    if (result && result->type == Integer)
    {
        *output_value = result->int_value;
        success = 1;
    }

    free_command_response(response);

    return success;
}
