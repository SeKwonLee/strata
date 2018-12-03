#ifdef MLFS_HASH
#ifndef LEVEL_HASH
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include "hash_bch.h"

#define ASSOC_NUM 4
#define KEY_LEN 16
#define VALUE_LEN 15
#define CUCKOO_MAX_DEP 500

typedef struct entry{
    uint8_t token;                  
    uint32_t key;
    uint64_t value;
} entry;

typedef struct cuckoo_node
{
    entry slot[ASSOC_NUM];
} cuckoo_node;

typedef struct cuckoo_hash {
    cuckoo_node *buckets;
    uint32_t entry_num;
    uint32_t bucket_num;
    
    uint32_t levels;
    uint32_t addr_capacity;         
    uint32_t total_capacity;        
    uint32_t occupied;                 
    uint64_t f_seed;
    uint64_t s_seed;
    uint32_t resize_num;
} cuckoo_hash;

cuckoo_hash *cuckoo_init(uint32_t levels);       

uint8_t cuckoo_insert(cuckoo_hash *cuckoo, uint32_t key, uint64_t value);

uint64_t cuckoo_query(cuckoo_hash *cuckoo, uint32_t key);

int cuckoo_delete(cuckoo_hash *cuckoo, uint8_t*key);        

static uint8_t rehash(cuckoo_hash *cuckoo, uint32_t idx, uint32_t loop, uint32_t insert_key);

int cuckoo_resize(cuckoo_hash *cuckoo);
#endif
#endif
