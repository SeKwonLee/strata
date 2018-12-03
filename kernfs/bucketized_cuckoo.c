#ifdef MLFS_HASH
#ifndef LEVEL_HASH
#include "extents.h"
#include <libpmem.h>

#define FIRST_HASH(hash, capacity) (hash % (capacity / 2))
#define SECOND_HASH(hash, capacity) ((hash % (capacity / 2)) + (capacity / 2))

uint32_t F_IDX(cuckoo_hash *cuckoo, const uint8_t *key, uint32_t capacity) {
    return (hash((void *)key, strlen(key), cuckoo->f_seed)) % (capacity / 2);
}
uint32_t S_IDX(cuckoo_hash *cuckoo, const uint8_t *key, uint32_t capacity) {
    return (hash((void *)key, strlen(key), cuckoo->s_seed)) % (capacity / 2) + (capacity / 2);
}

uint32_t F_IDX_(const uint32_t key, uint32_t capacity) {
    return (key) % (capacity / 2);
}
uint32_t S_IDX_(const uint32_t key, uint32_t capacity) {
    return (key) % (capacity / 2) + (capacity / 2);
}

void generate_seeds(cuckoo_hash *cuckoo)
{
    srand(time(NULL));

    do
    {
        cuckoo->f_seed = rand();
        cuckoo->s_seed = rand();
        cuckoo->f_seed = cuckoo->f_seed << (rand() % 63);
        cuckoo->s_seed = cuckoo->s_seed << (rand() % 63);
    } while (cuckoo->f_seed == cuckoo->s_seed);
}

cuckoo_hash *cuckoo_init(uint32_t levels)
{
    cuckoo_hash *cuckoo = malloc(sizeof(cuckoo_hash));
    if (!cuckoo)
    {
        printf("The Initialization Fails1!\n");
        exit(1);
    }

    cuckoo->levels = levels;
    cuckoo->addr_capacity = pow(2, levels) + pow(2, levels - 1);
    cuckoo->total_capacity = pow(2, levels) + pow(2, levels - 1);

    cuckoo->occupied = 0;
    generate_seeds(cuckoo);
    cuckoo->buckets = calloc(cuckoo->total_capacity, sizeof(cuckoo_node));
    cuckoo->bucket_num = cuckoo->total_capacity;
    cuckoo->entry_num = cuckoo->bucket_num*ASSOC_NUM;
    cuckoo->resize_num = 0;

    if (!cuckoo->buckets)
    {
        printf("The Initialization Fails2!\n");
        exit(1);
    }

    printf("The Initialization succeeds!\n");
    printf("The number of addressable cells in cuckoo hashing: %d\n", cuckoo->addr_capacity);
    printf("The total number of cells in cuckoo hashing: %d\n", cuckoo->total_capacity);
    printf("Buckets: %d\n", cuckoo->bucket_num);
    printf("Entries: %d\n", cuckoo->entry_num);
    return cuckoo;
}

extern cuckoo_hash *mlfs_cuckoo_init(uint32_t levels, handle_t *handle, 
        struct inode *inode, unsigned int flags)
{
    int goal, err = 0;
    mlfs_lblk_t allocated = 0;
    mlfs_fsblk_t newblock;

	if (sizeof(cuckoo_hash) < g_block_size_bytes)
		allocated = 1;
	else {
		allocated = (sizeof(cuckoo_hash) >> g_block_size_shift);
		if (sizeof(cuckoo_hash) % g_block_size_bytes != 0) 
			allocated++;
	}

    newblock = mlfs_new_data_blocks(handle, inode, goal, flags, &allocated, &err);
    cuckoo_hash *cuckoo = g_bdev[handle->dev]->map_base_addr + (newblock << g_block_size_shift);
    inode->l1.addrs[5] = newblock;
    if (newblock == 0)
    {
        printf("The cuckoo hash table initialization fails:1\n");
        return NULL;
    }

    cuckoo->levels = levels;
    cuckoo->addr_capacity = pow(2, levels) + pow(2, levels - 1);
    cuckoo->total_capacity = pow(2, levels) + pow(2, levels - 1);

    cuckoo->occupied = 0;
    generate_seeds(cuckoo);

    unsigned long bucket_size = cuckoo->total_capacity * sizeof(cuckoo_node);
	if (bucket_size < g_block_size_bytes)
		allocated = 1;
	else {
		allocated = (bucket_size >> g_block_size_shift);
		if ((bucket_size) % g_block_size_bytes != 0)
			allocated++;
	}

    newblock = mlfs_new_data_blocks(handle, inode, goal, flags, &allocated, &err);
    cuckoo->buckets = g_bdev[handle->dev]->map_base_addr + (newblock << g_block_size_shift);
    pmem_memset_persist(cuckoo->buckets, 0, cuckoo->total_capacity * sizeof(cuckoo_node));
    inode->l1.addrs[6] = newblock;

    //cuckoo->buckets = calloc(cuckoo->total_capacity, sizeof(cuckoo_node));
    cuckoo->bucket_num = cuckoo->total_capacity;
    cuckoo->entry_num = cuckoo->bucket_num*ASSOC_NUM;
    cuckoo->resize_num = 0;

    if (!cuckoo->buckets)
    {
        printf("The Initialization Fails2!\n");
        exit(1);
    }

    pmem_persist(cuckoo, sizeof(cuckoo_hash));

    //printf("The Initialization succeeds!\n");
    //printf("The number of addressable cells in cuckoo hashing: %d\n", cuckoo->addr_capacity);
    //printf("The total number of cells in cuckoo hashing: %d\n", cuckoo->total_capacity);
    //printf("Buckets: %d\n", cuckoo->bucket_num);
    //printf("Entries: %d\n", cuckoo->entry_num);
    return cuckoo;
}

int cuckoo_resize(cuckoo_hash *cuckoo)
{
    if (!cuckoo)
    {
        printf("The Resize Fails1!\n");
        exit(1);
    }

    uint32_t newCapacity = pow(2, cuckoo->levels + 1) + pow(2, cuckoo->levels);
    uint32_t oldCapacity = pow(2, cuckoo->levels) + pow(2, cuckoo->levels - 1);

    cuckoo->addr_capacity = newCapacity;                      
    cuckoo_node *newBucket = calloc(cuckoo->addr_capacity, sizeof(cuckoo_node));
    
    cuckoo_node *oldBucket = cuckoo->buckets;
    cuckoo->buckets = newBucket;
    
    cuckoo->levels++;
    cuckoo->total_capacity = newCapacity;
    
    cuckoo->bucket_num = cuckoo->total_capacity;
    cuckoo->entry_num = cuckoo->bucket_num*ASSOC_NUM;
    generate_seeds(cuckoo);

    if (!newBucket) {
        printf("The Resize Failed in calloc newBucket!\n");
        exit(1);
    }

    uint32_t old_idx;
    for (old_idx = 0; old_idx < oldCapacity; old_idx ++) {

        int i, j;
        for(i = 0; i < ASSOC_NUM; i ++){
            if (oldBucket[old_idx].slot[i].token == 1)
            {
                uint8_t *key = oldBucket[old_idx].slot[i].key;
                uint8_t *value = oldBucket[old_idx].slot[i].value;

                uint32_t f_idx = F_IDX(cuckoo, key, cuckoo->addr_capacity);
                uint32_t s_idx = S_IDX(cuckoo, key, cuckoo->addr_capacity);
                int insertSuccess = 0;

                for(;;){
                    for(j = 0; j < ASSOC_NUM; j ++){
                        if (cuckoo->buckets[f_idx].slot[j].token == 0)
                        {
                            memcpy(cuckoo->buckets[f_idx].slot[j].key, key, KEY_LEN);
                            memcpy(cuckoo->buckets[f_idx].slot[j].value, value, VALUE_LEN);
                            cuckoo->buckets[f_idx].slot[j].token = 1;
                            insertSuccess = 1;
                            break;
                        }
                        else if (cuckoo->buckets[s_idx].slot[j].token == 0)
                        {
                            memcpy(cuckoo->buckets[s_idx].slot[j].key, key, KEY_LEN);
                            memcpy(cuckoo->buckets[s_idx].slot[j].value, value, VALUE_LEN);
                            cuckoo->buckets[s_idx].slot[j].token = 1;
                            insertSuccess = 1;
                            break;
                        }
                    }
                    if (insertSuccess)
                        break;
                    if(rehash(cuckoo, f_idx, CUCKOO_MAX_DEP, key) == 2){
                        printf("Rehash fails when resizing!\n");
                        if(cuckoo_query(cuckoo, key))
                            printf("key: %s\n", key);
                        exit(1);
                    }
                }
                oldBucket[old_idx].slot[i].token == 0;
            }
        }
    }

    free(oldBucket);
    return 1;
}

extern int mlfs_cuckoo_resize(cuckoo_hash *cuckoo, 
        handle_t *handle, struct inode *inode, unsigned int flags)
{
    int goal, err = 0;
    mlfs_lblk_t allocated = 0;
    mlfs_fsblk_t newblock;

    if (!cuckoo)
    {
        printf("The Resize Fails1!\n");
        exit(1);
    }

    uint32_t newCapacity = pow(2, cuckoo->levels + 1) + pow(2, cuckoo->levels);
    uint32_t oldCapacity = pow(2, cuckoo->levels) + pow(2, cuckoo->levels - 1);

    cuckoo->addr_capacity = newCapacity;

    uint64_t new_bucket_size = cuckoo->addr_capacity * sizeof(cuckoo_node);
	if (new_bucket_size < g_block_size_bytes)
		allocated = 1;
	else {
		allocated = (new_bucket_size >> g_block_size_shift);
		if ((new_bucket_size) % g_block_size_bytes != 0)
			allocated++;
	}

    newblock = mlfs_new_data_blocks(handle, inode, goal, flags, &allocated, &err);
    cuckoo_node *newBucket = g_bdev[handle->dev]->map_base_addr + (newblock << g_block_size_shift);
    pmem_memset_persist(newBucket, 0, cuckoo->addr_capacity * sizeof(cuckoo_node));
    //cuckoo_node *newBucket = calloc(cuckoo->addr_capacity, sizeof(cuckoo_node));
    if (newblock == 0) {
        printf("The new cuckoo node allocation fail\n");
        exit(1);
    }

    cuckoo_node *oldBucket = cuckoo->buckets;
    cuckoo->buckets = newBucket;

    cuckoo->levels++;
    cuckoo->total_capacity = newCapacity;
    
    cuckoo->bucket_num = cuckoo->total_capacity;
    cuckoo->entry_num = cuckoo->bucket_num*ASSOC_NUM;
    generate_seeds(cuckoo);

    if (!newBucket) {
        printf("The Resize Failed in calloc newBucket!\n");
        exit(1);
    }

    uint32_t old_idx;
    for (old_idx = 0; old_idx < oldCapacity; old_idx ++) {
        int i, j;
        for(i = 0; i < ASSOC_NUM; i ++){
            if (oldBucket[old_idx].slot[i].token == 1)
            {
                uint32_t key = oldBucket[old_idx].slot[i].key;
                uint64_t value = oldBucket[old_idx].slot[i].value;

                uint32_t f_idx = F_IDX_(key, cuckoo->addr_capacity);
                uint32_t s_idx = S_IDX_(key, cuckoo->addr_capacity);
                int insertSuccess = 0;

                for(;;){
                    for(j = 0; j < ASSOC_NUM; j ++){
                        if (cuckoo->buckets[f_idx].slot[j].token == 0)
                        {
                            cuckoo->buckets[f_idx].slot[j].key = key;
                            cuckoo->buckets[f_idx].slot[j].value = value;
                            cuckoo->buckets[f_idx].slot[j].token = 1;
                            pmem_persist(&cuckoo->buckets[f_idx].slot[j].key, sizeof(uint32_t));
                            pmem_persist(&cuckoo->buckets[f_idx].slot[j].value, sizeof(uint64_t));
                            pmem_persist(&cuckoo->buckets[f_idx].slot[j].token, sizeof(int8_t));
                            insertSuccess = 1;
                            break;
                        }
                        else if (cuckoo->buckets[s_idx].slot[j].token == 0)
                        {
                            cuckoo->buckets[s_idx].slot[j].key = key;
                            cuckoo->buckets[s_idx].slot[j].value = value;
                            cuckoo->buckets[s_idx].slot[j].token = 1;
                            pmem_persist(&cuckoo->buckets[s_idx].slot[j].key, sizeof(uint32_t));
                            pmem_persist(&cuckoo->buckets[s_idx].slot[j].value, sizeof(uint64_t));
                            pmem_persist(&cuckoo->buckets[s_idx].slot[j].token, sizeof(uint8_t));
                            insertSuccess = 1;
                            break;
                        }
                    }
                    if (insertSuccess)
                        break;
                    if(rehash(cuckoo, f_idx, CUCKOO_MAX_DEP, key) == 2){
                        printf("Rehash fails when resizing!\n");
                        if(cuckoo_query(cuckoo, key))
                            printf("key: %d\n", key);
                        exit(1);
                    }
                }
                oldBucket[old_idx].slot[i].token == 0;
            }
        }
    }

    new_bucket_size = oldCapacity * sizeof(cuckoo_node);
	if (new_bucket_size < g_block_size_bytes)
		allocated = 1;
	else {
		allocated = (new_bucket_size >> g_block_size_shift);
		if ((new_bucket_size) % g_block_size_bytes != 0)
			allocated++;
	}

    mlfs_free_blocks(handle, inode, NULL, inode->l1.addrs[6], allocated, 0);
    inode->l1.addrs[6] = newblock;
    //free(oldBucket);
    return 1;
}

uint64_t cuckoo_query(cuckoo_hash *cuckoo, uint32_t key)
{
    uint32_t f_idx = F_IDX_(key, cuckoo->addr_capacity);
    uint32_t s_idx = S_IDX_(key, cuckoo->addr_capacity);
    int j;

    for(j = 0; j < ASSOC_NUM; j ++){
        if (cuckoo->buckets[f_idx].slot[j].token &&
            cuckoo->buckets[f_idx].slot[j].key == key) {
            return cuckoo->buckets[f_idx].slot[j].value;
        }
        else if (cuckoo->buckets[s_idx].slot[j].token &&
            cuckoo->buckets[s_idx].slot[j].key == key) {
            return cuckoo->buckets[s_idx].slot[j].value;
        }
    }
    return 0;
}

int cuckoo_delete(cuckoo_hash *cuckoo, uint8_t*key) {
    uint32_t f_idx = F_IDX(cuckoo, key, cuckoo->addr_capacity);
    uint32_t s_idx = S_IDX(cuckoo, key, cuckoo->addr_capacity);
    int j;
    for(j = 0; j < ASSOC_NUM; j ++){
        if (cuckoo->buckets[f_idx].slot[j].token &&
            strcmp(cuckoo->buckets[f_idx].slot[j].key, key) == 0) {
            cuckoo->buckets[f_idx].slot[j].token = 0;
            cuckoo->occupied--;
            return 0;
        }
        else if (cuckoo->buckets[s_idx].slot[j].token &&
            strcmp(cuckoo->buckets[s_idx].slot[j].key, key) == 0) {
            cuckoo->buckets[s_idx].slot[j].token = 0;
            cuckoo->occupied--;
            return 0;
        }
    }
    return 1;
}

extern int mlfs_cuckoo_delete(cuckoo_hash *cuckoo, uint32_t key, 
        handle_t *handle, struct inode *inode) {
    uint32_t f_idx = F_IDX_(key, cuckoo->addr_capacity);
    uint32_t s_idx = S_IDX_(key, cuckoo->addr_capacity);
    int j;
    for(j = 0; j < ASSOC_NUM; j ++){
        if (cuckoo->buckets[f_idx].slot[j].token &&
            cuckoo->buckets[f_idx].slot[j].key == key) {
            cuckoo->buckets[f_idx].slot[j].token = 0;
            mlfs_free_blocks(handle, inode, NULL, 
                    cuckoo->buckets[f_idx].slot[j].value, 1, 0);
            pmem_persist(&cuckoo->buckets[f_idx].slot[j].token, sizeof(uint8_t));
            cuckoo->occupied--;
            return 0;
        }
        else if (cuckoo->buckets[s_idx].slot[j].token &&
            cuckoo->buckets[s_idx].slot[j].key == key) {
            cuckoo->buckets[s_idx].slot[j].token = 0;
            mlfs_free_blocks(handle, inode, NULL, 
                    cuckoo->buckets[s_idx].slot[j].value, 1, 0);
            pmem_persist(&cuckoo->buckets[s_idx].slot[j].token, sizeof(uint8_t));
            cuckoo->occupied--;
            return 0;
        }
    }
    return 1;
}

uint8_t cuckoo_insert(cuckoo_hash *cuckoo, uint32_t key, uint64_t value)
{
    uint32_t f_idx = F_IDX_(key, cuckoo->addr_capacity);
    uint32_t s_idx = S_IDX_(key, cuckoo->addr_capacity);

    int j;
    for (;;)
    {
        for(j = 0; j < ASSOC_NUM; j ++){
            if (!cuckoo->buckets[f_idx].slot[j].token)
            {
                cuckoo->buckets[f_idx].slot[j].key = key;
                cuckoo->buckets[f_idx].slot[j].value = value;
                cuckoo->buckets[f_idx].slot[j].token = 1;
                pmem_persist(&cuckoo->buckets[f_idx].slot[j].key, 
                        sizeof(uint32_t) + sizeof(uint64_t));
                pmem_persist(&cuckoo->buckets[f_idx].slot[j].token, 
                        sizeof(uint8_t));
                cuckoo->occupied ++;
                return 0;
            }
            else if (!cuckoo->buckets[s_idx].slot[j].token)
            {
                cuckoo->buckets[s_idx].slot[j].key = key;
                cuckoo->buckets[s_idx].slot[j].value = value;
                cuckoo->buckets[s_idx].slot[j].token = 1;
                pmem_persist(&cuckoo->buckets[s_idx].slot[j].key, 
                        sizeof(uint32_t) + sizeof(uint64_t));
                pmem_persist(&cuckoo->buckets[s_idx].slot[j].token, 
                        sizeof(uint8_t));
                cuckoo->occupied ++;
                return 0;
            }
        }
        if(rehash(cuckoo, f_idx, CUCKOO_MAX_DEP, key) == 2){
            return 0;
        }
    }
}

static uint8_t rehash(cuckoo_hash *cuckoo, uint32_t idx, uint32_t loop, uint32_t insert_key)
{
    srand(time(NULL));
    uint64_t rand_location = rand();
    uint32_t key = cuckoo->buckets[idx].slot[rand_location%ASSOC_NUM].key;
    uint32_t f_idx = F_IDX_(key, cuckoo->addr_capacity);
    uint32_t s_idx = S_IDX_(key, cuckoo->addr_capacity);
    uint32_t n_idx = s_idx;

    if (idx == s_idx)
    {
        n_idx = f_idx;
    }

    int j;
    for(j = 0; j < ASSOC_NUM; j ++){
        if (!cuckoo->buckets[n_idx].slot[j].token){
            cuckoo->buckets[n_idx].slot[j] = cuckoo->buckets[idx].slot[rand_location%ASSOC_NUM];
            cuckoo->buckets[idx].slot[rand_location%ASSOC_NUM].token = 0;
            pmem_persist(&cuckoo->buckets[n_idx].slot[j], sizeof(entry));
            pmem_persist(&cuckoo->buckets[idx].slot[rand_location%ASSOC_NUM].token, sizeof(uint8_t));
            return 0;
        }
    }

    if (loop == 0)
    {
        return 2;
    }
    else
    {
        if (rehash(cuckoo, n_idx, loop - 1, insert_key) == 1)
        {
            return 1;
        }
    }
}

extern void mlfs_cuckoo_destroy(cuckoo_hash *cuckoo, handle_t *handle, struct inode *inode)
{
    mlfs_lblk_t allocated = 0;
    unsigned long new_bucket_size;

    new_bucket_size = (cuckoo->total_capacity) * sizeof(cuckoo_node);
	if (new_bucket_size < g_block_size_bytes)
		allocated = 1;
	else {
		allocated = (new_bucket_size >> g_block_size_shift);
		if ((new_bucket_size) % g_block_size_bytes != 0)
			allocated++;
	}
    mlfs_free_blocks(handle, inode, NULL, inode->l1.addrs[6], allocated, 0);
    inode->l1.addrs[6] = 0;

    mlfs_free_blocks(handle, inode, NULL, inode->l1.addrs[5], 1, 0);
    inode->l1.addrs[5] = 0;
    cuckoo = NULL;
}
#endif
#endif
