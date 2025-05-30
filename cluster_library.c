#include "php_redis.h"
#include "common.h"

#include "cluster_library.h"
#include "crc16.h"
#include <zend_exceptions.h>

extern zend_class_entry *redis_cluster_exception_ce;
int le_cluster_slot_cache;
