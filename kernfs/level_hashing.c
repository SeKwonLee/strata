#include "extents.h"
#include <libpmem.h>

/*
Function: F_HASH()
        Compute the first hash value of a key-value item
*/
uint64_t F_HASH(level_hash *level, const uint8_t *key) {
    return (hash((void *)key, strlen(key), level->f_seed));
}

/*
Function: S_HASH() 
        Compute the second hash value of a key-value item
*/
uint64_t S_HASH(level_hash *level, const uint8_t *key) {
    return (hash((void *)key, strlen(key), level->s_seed));
}

/*
Function: F_IDX() 
        Compute the second hash location
*/
uint64_t F_IDX(uint64_t hashKey, uint64_t capacity) {
    return hashKey % (capacity / 2);
}

/*
Function: S_IDX() 
        Compute the second hash location
*/
uint64_t S_IDX(uint64_t hashKey, uint64_t capacity) {
    return hashKey % (capacity / 2) + capacity / 2;
}

/*
Function: generate_seeds() 
        Generate two randomized seeds for hash functions
*/
void generate_seeds(level_hash *level)
{
    srand(time(NULL));
    do
    {
        level->f_seed = rand();
        level->s_seed = rand();
        level->f_seed = level->f_seed << (rand() % 63);
        level->s_seed = level->s_seed << (rand() % 63);
    } while (level->f_seed == level->s_seed);
}

/*
Function: level_init() 
        Initialize a level hash table
*/
level_hash *level_init(uint64_t level_size)
{
    level_hash *level = malloc(sizeof(level_hash));
    if (!level)
    {
        printf("The level hash table initialization fails:1\n");
        exit(1);
    }

    level->level_size = level_size;
    level->addr_capacity = pow(2, level_size);
    level->total_capacity = pow(2, level_size) + pow(2, level_size - 1);
    generate_seeds(level);
    level->buckets[0] = calloc(pow(2, level_size), sizeof(level_bucket));
    level->buckets[1] = calloc(pow(2, level_size - 1), sizeof(level_bucket));
    level->level_item_num[0] = 0;
    level->level_item_num[1] = 0;
    level->level_resize = 0;
    
    if (!level->buckets[0] || !level->buckets[1])
    {
        printf("The level hash table initialization fails:2\n");
        exit(1);
    }

    printf("Level hashing: ASSOC_NUM %d, KEY_LEN %d, VALUE_LEN %d \n", ASSOC_NUM, KEY_LEN, VALUE_LEN);
    printf("The number of top-level buckets: %d\n", level->addr_capacity);
    printf("The number of all buckets: %d\n", level->total_capacity);
    printf("The number of all entries: %d\n", level->total_capacity*ASSOC_NUM);
    printf("The level hash table initialization succeeds!\n");
    return level;
}

/*
Function: level_resize()
        Expand a level hash table in place;
        Put a new level on the top of the old hash table and only rehash the
        items in the bottom level of the old hash table;
*/
void level_resize(level_hash *level) 
{
    if (!level)
    {
        printf("The resizing fails: 1\n");
        exit(1);
    }

    level->addr_capacity = pow(2, level->level_size + 1);
    level_bucket *newBuckets = calloc(level->addr_capacity, sizeof(level_bucket));
    if (!newBuckets) {
        printf("The resizing fails: 2\n");
        exit(1);
    }
    uint64_t new_level_item_num = 0;
    
    uint64_t old_idx;
    for (old_idx = 0; old_idx < pow(2, level->level_size - 1); old_idx ++) {
        uint64_t i, j;
        for(i = 0; i < ASSOC_NUM; i ++){
            if (level->buckets[1][old_idx].token[i] == 1)
            {
                uint8_t *key = level->buckets[1][old_idx].slot[i].key;
                uint8_t *value = level->buckets[1][old_idx].slot[i].value;

                uint64_t f_idx = F_IDX(F_HASH(level, key), level->addr_capacity);
                uint64_t s_idx = S_IDX(S_HASH(level, key), level->addr_capacity);

                uint8_t insertSuccess = 0;
                for(j = 0; j < ASSOC_NUM; j ++){                            
                    /*  The rehashed item is inserted into the less-loaded bucket between 
                        the two hash locations in the new level
                    */
                    if (newBuckets[f_idx].token[j] == 0)
                    {
                        memcpy(newBuckets[f_idx].slot[j].key, key, KEY_LEN);
                        memcpy(newBuckets[f_idx].slot[j].value, value, VALUE_LEN);
                        newBuckets[f_idx].token[j] = 1;
                        insertSuccess = 1;
                        new_level_item_num ++;
                        break;
                    }
                    if (newBuckets[s_idx].token[j] == 0)
                    {
                        memcpy(newBuckets[s_idx].slot[j].key, key, KEY_LEN);
                        memcpy(newBuckets[s_idx].slot[j].value, value, VALUE_LEN);
                        newBuckets[s_idx].token[j] = 1;
                        insertSuccess = 1;
                        new_level_item_num ++;
                        break;
                    }
                }
                if(!insertSuccess){
                    printf("The resizing fails: 3\n");
                    exit(1);                    
                }
                
                level->buckets[1][old_idx].token[i] == 0;
            }
        }
    }

    level->level_size ++;
    level->total_capacity = pow(2, level->level_size) + pow(2, level->level_size - 1);

    free(level->buckets[1]);
    level->buckets[1] = level->buckets[0];
    level->buckets[0] = newBuckets;
    newBuckets = NULL;
    
    level->level_item_num[1] = level->level_item_num[0];
    level->level_item_num[0] = new_level_item_num;
    level->level_resize ++;
}

/*
Function: level_dynamic_query() 
        Lookup a key-value item in level hash table via danamic search scheme;
        First search the level with more items;
*/
uint64_t level_dynamic_query(level_hash *level, uint8_t *key)
{
    
    uint64_t f_hash = F_HASH(level, key);
    uint64_t s_hash = S_HASH(level, key);

    uint32_t int_key = atoi(key);

    uint64_t i, j, f_idx, s_idx;
    if(level->level_item_num[0] > level->level_item_num[1]){
        f_idx = F_IDX(f_hash, level->addr_capacity);
        s_idx = S_IDX(s_hash, level->addr_capacity); 

        for(i = 0; i < 2; i ++){
            for(j = 0; j < ASSOC_NUM; j ++){
                if (level->buckets[i][f_idx].token[j] == 1&&level->buckets[i][f_idx].slot[j].key == int_key)
                {
                    return level->buckets[i][f_idx].slot[j].value;
                }
            }
            for(j = 0; j < ASSOC_NUM; j ++){
                if (level->buckets[i][s_idx].token[j] == 1&&level->buckets[i][s_idx].slot[j].key == int_key)
                {
                    return level->buckets[i][s_idx].slot[j].value;
                }
            }
            f_idx = F_IDX(f_hash, level->addr_capacity / 2);
            s_idx = S_IDX(s_hash, level->addr_capacity / 2);
        }
    }
    else{
        f_idx = F_IDX(f_hash, level->addr_capacity/2);
        s_idx = S_IDX(s_hash, level->addr_capacity/2);

        for(i = 2; i > 0; i --){
            for(j = 0; j < ASSOC_NUM; j ++){
                if (level->buckets[i-1][f_idx].token[j] == 1&&level->buckets[i-1][f_idx].slot[j].key == int_key)
                {
                    return level->buckets[i-1][f_idx].slot[j].value;
                }
            }
            for(j = 0; j < ASSOC_NUM; j ++){
                if (level->buckets[i-1][s_idx].token[j] == 1&&level->buckets[i-1][s_idx].slot[j].key == int_key)
                {
                    return level->buckets[i-1][s_idx].slot[j].value;
                }
            }
            f_idx = F_IDX(f_hash, level->addr_capacity);
            s_idx = S_IDX(s_hash, level->addr_capacity);
        }
    }
    return 0;
}

/*
Function: level_static_query() 
        Lookup a key-value item in level hash table via static search scheme;
        Always first search the top level and then search the bottom level;
*/
uint8_t* level_static_query(level_hash *level, uint8_t *key)
{
    uint64_t f_hash = F_HASH(level, key);
    uint64_t s_hash = S_HASH(level, key);
    uint64_t f_idx = F_IDX(f_hash, level->addr_capacity);
    uint64_t s_idx = S_IDX(s_hash, level->addr_capacity);

    uint64_t i, j;
    for(i = 0; i < 2; i ++){
        for(j = 0; j < ASSOC_NUM; j ++){
            if (level->buckets[i][f_idx].token[j] == 1&&strcmp(level->buckets[i][f_idx].slot[j].key, key) == 0)
            {
                return level->buckets[i][f_idx].slot[j].value;
            }
        }
        for(j = 0; j < ASSOC_NUM; j ++){
            if (level->buckets[i][s_idx].token[j] == 1&&strcmp(level->buckets[i][s_idx].slot[j].key, key) == 0)
            {
                return level->buckets[i][s_idx].slot[j].value;
            }
        }
        f_idx = F_IDX(f_hash, level->addr_capacity / 2);
        s_idx = S_IDX(s_hash, level->addr_capacity / 2);
    }

    return NULL;
}


/*
Function: level_delete() 
        Remove a key-value item from level hash table;
        The function can be optimized by using the dynamic search scheme
*/
uint8_t level_delete(level_hash *level, uint8_t *key)
{
    uint64_t f_hash = F_HASH(level, key);
    uint64_t s_hash = S_HASH(level, key);
    uint64_t f_idx = F_IDX(f_hash, level->addr_capacity);
    uint64_t s_idx = S_IDX(s_hash, level->addr_capacity);
    
    uint64_t i, j;
    for(i = 0; i < 2; i ++){
        for(j = 0; j < ASSOC_NUM; j ++){
            if (level->buckets[i][f_idx].token[j] == 1&&strcmp(level->buckets[i][f_idx].slot[j].key, key) == 0)
            {
                level->buckets[i][f_idx].token[j] = 0;
                level->level_item_num[i] --;
                return 0;
            }
        }
        for(j = 0; j < ASSOC_NUM; j ++){
            if (level->buckets[i][s_idx].token[j] == 1&&strcmp(level->buckets[i][s_idx].slot[j].key, key) == 0)
            {
                level->buckets[i][s_idx].token[j] = 0;
                level->level_item_num[i] --;
                return 0;
            }
        }
        f_idx = F_IDX(f_hash, level->addr_capacity / 2);
        s_idx = S_IDX(s_hash, level->addr_capacity / 2);
    }

    return 1;
}

/*
Function: level_update() 
        Update the value of a key-value item in level hash table;
        The function can be optimized by using the dynamic search scheme
*/
uint8_t level_update(level_hash *level, uint8_t *key, uint8_t *new_value)
{
    uint64_t f_hash = F_HASH(level, key);
    uint64_t s_hash = S_HASH(level, key);
    uint64_t f_idx = F_IDX(f_hash, level->addr_capacity);
    uint64_t s_idx = S_IDX(s_hash, level->addr_capacity);
    
    uint64_t i, j;
    for(i = 0; i < 2; i ++){
        for(j = 0; j < ASSOC_NUM; j ++){
            if (level->buckets[i][f_idx].token[j] == 1&&strcmp(level->buckets[i][f_idx].slot[j].key, key) == 0)
            {
                memcpy(level->buckets[i][f_idx].slot[j].value, new_value, VALUE_LEN);
                return 0;
            }
        }
        for(j = 0; j < ASSOC_NUM; j ++){
            if (level->buckets[i][s_idx].token[j] == 1&&strcmp(level->buckets[i][s_idx].slot[j].key, key) == 0)
            {
                memcpy(level->buckets[i][s_idx].slot[j].value, new_value, VALUE_LEN);
                return 0;
            }
        }
        f_idx = F_IDX(f_hash, level->addr_capacity / 2);
        s_idx = S_IDX(s_hash, level->addr_capacity / 2);
    }

    return 1;
}

/*
Function: level_insert() 
        Insert a key-value item into level hash table;
*/
uint8_t level_insert(level_hash *level, uint8_t *key, uint8_t *value)
{
    uint64_t f_hash = F_HASH(level, key);
    uint64_t s_hash = S_HASH(level, key);
    uint64_t f_idx = F_IDX(f_hash, level->addr_capacity);
    uint64_t s_idx = S_IDX(s_hash, level->addr_capacity);

    uint64_t i, j;
    int empty_location;

    uint32_t int_key = atoi(key);
    uint64_t int_val = atoi(value);

    for(i = 0; i < 2; i ++){
        for(j = 0; j < ASSOC_NUM; j ++){        
            /*  The new item is inserted into the less-loaded bucket between 
                the two hash locations in each level           
            */      
            if (level->buckets[i][f_idx].token[j] == 0)
            {
                level->buckets[i][f_idx].slot[j].key = int_key;
                level->buckets[i][f_idx].slot[j].value = int_val;
                level->buckets[i][f_idx].token[j] = 1;
                pmem_persist(&level->buckets[i][f_idx].slot[j], sizeof(entry));
                pmem_persist(&level->buckets[i][f_idx].token[j], sizeof(uint8_t));

                level->level_item_num[i] ++;
                return 0;
            }
            if (level->buckets[i][s_idx].token[j] == 0) 
            {
                level->buckets[i][s_idx].slot[j].key = int_key;
                level->buckets[i][s_idx].slot[j].value = int_val;
                level->buckets[i][s_idx].token[j] = 1;
                pmem_persist(&level->buckets[i][s_idx].slot[j], sizeof(entry));
                pmem_persist(&level->buckets[i][s_idx].token[j], sizeof(uint8_t));

                level->level_item_num[i] ++;
                return 0;
            }
        }

        empty_location = try_movement(level, f_idx, i);
        if(empty_location != -1){
            level->buckets[i][f_idx].slot[empty_location].key = int_key;
            level->buckets[i][f_idx].slot[empty_location].value = int_val;
            level->buckets[i][f_idx].token[empty_location] = 1;
            pmem_persist(&level->buckets[i][f_idx].slot[empty_location], sizeof(entry));
            pmem_persist(&level->buckets[i][f_idx].token[empty_location], sizeof(uint8_t));
            level->level_item_num[i] ++;
            return 0;

        }
        
        if(i == 1){
            empty_location = try_movement(level, s_idx, i);
            if(empty_location != -1){
                level->buckets[i][s_idx].slot[empty_location].key = int_key;
                level->buckets[i][s_idx].slot[empty_location].value = int_val;
                level->buckets[i][s_idx].token[empty_location] = 1;
                pmem_persist(&level->buckets[i][s_idx].slot[empty_location], sizeof(entry));
                pmem_persist(&level->buckets[i][s_idx].token[empty_location], sizeof(uint8_t));
                level->level_item_num[i] ++;
                return 0;
            }
        }
        
        f_idx = F_IDX(f_hash, level->addr_capacity / 2);
        s_idx = S_IDX(s_hash, level->addr_capacity / 2);
    }

    if(level->level_resize > 0){
        empty_location = b2t_movement(level, f_idx);
        if(empty_location != -1){
            level->buckets[1][f_idx].slot[empty_location].key = int_key;
            level->buckets[1][f_idx].slot[empty_location].value = int_val;
            level->buckets[1][f_idx].token[empty_location] = 1;
            pmem_persist(&level->buckets[1][f_idx].slot[empty_location], sizeof(entry));
            pmem_persist(&level->buckets[1][f_idx].token[empty_location], sizeof(uint8_t));
            level->level_item_num[1] ++;
            return 0;
        }

        empty_location = b2t_movement(level, s_idx);
        if(empty_location != -1){
            level->buckets[1][s_idx].slot[empty_location].key = int_key;
            level->buckets[1][s_idx].slot[empty_location].value = int_val;
            level->buckets[1][s_idx].token[empty_location] = 1;
            pmem_persist(&level->buckets[1][s_idx].slot[empty_location], sizeof(entry));
            pmem_persist(&level->buckets[1][s_idx].token[empty_location], sizeof(uint8_t));
            level->level_item_num[1] ++;
            return 0;
        }
    }

    return 1;
}

/*
Function: try_movement() 
        Try to move an item from the current bucket to its same-level alternative bucket;
*/
int try_movement(level_hash *level, uint64_t idx, uint64_t level_num)
{
    uint64_t i, j, jdx;
    uint8_t str_key[KEY_LEN];

    for(i = 0; i < ASSOC_NUM; i ++){
        uint32_t key = level->buckets[level_num][idx].slot[i].key;
        uint64_t value = level->buckets[level_num][idx].slot[i].value;

        snprintf(str_key, KEY_LEN, "%lu", key);

        uint64_t f_hash = F_HASH(level, str_key);
        uint64_t s_hash = S_HASH(level, str_key);
        uint64_t f_idx = F_IDX(f_hash, level->addr_capacity/(1+level_num));
        uint64_t s_idx = S_IDX(s_hash, level->addr_capacity/(1+level_num));
        
        if(f_idx == idx)
            jdx = s_idx;
        else
            jdx = f_idx;

        for(j = 0; j < ASSOC_NUM; j ++){
            if (level->buckets[level_num][jdx].token[j] == 0)
            {
                level->buckets[level_num][jdx].slot[j].key = key;
                level->buckets[level_num][jdx].slot[j].value = value;
                level->buckets[level_num][jdx].token[j] = 1;
                level->buckets[level_num][idx].token[i] = 0;
                pmem_persist(&level->buckets[level_num][jdx].slot[j], sizeof(entry));
                pmem_persist(&level->buckets[level_num][jdx].token[j], sizeof(uint8_t));
                pmem_persist(&level->buckets[level_num][idx].token[i], sizeof(uint8_t));
                return i;
            }
        }       
    }
    
    return -1;
}

/*
Function: b2t_movement() 
        Try to move a bottom-level item to its top-level alternative buckets;
*/
int b2t_movement(level_hash *level, uint64_t idx)
{
    uint32_t key;
    uint64_t value;
    uint64_t s_hash, f_hash;
    uint64_t s_idx, f_idx;
    
    uint64_t i, j;
    uint8_t str_key[KEY_LEN];
    for(i = 0; i < ASSOC_NUM; i ++){
        key = level->buckets[1][idx].slot[i].key;
        value = level->buckets[1][idx].slot[i].value;

        snprintf(str_key, KEY_LEN, "%lu", key);

        f_hash = F_HASH(level, str_key);
        s_hash = S_HASH(level, str_key);
        f_idx = F_IDX(f_hash, level->addr_capacity);
        s_idx = S_IDX(s_hash, level->addr_capacity);
    
        for(j = 0; j < ASSOC_NUM; j ++){
            if (level->buckets[0][f_idx].token[j] == 0)
            {
                level->buckets[0][f_idx].slot[j].key = key;
                level->buckets[0][f_idx].slot[j].value = value;
                level->buckets[0][f_idx].token[j] = 1;
                level->buckets[1][idx].token[i] = 0;
                pmem_persist(&level->buckets[0][f_idx].slot[j], sizeof(entry));
                pmem_persist(&level->buckets[0][f_idx].token[j], sizeof(uint8_t));
                pmem_persist(&level->buckets[1][idx].token[i], sizeof(uint8_t));
                level->level_item_num[0] ++;
                level->level_item_num[1] --;
                return i;
            }
            else if (level->buckets[0][s_idx].token[j] == 0)
            {
                level->buckets[0][s_idx].slot[j].key = key;
                level->buckets[0][s_idx].slot[j].value = value;
                level->buckets[0][s_idx].token[j] = 1;
                level->buckets[1][idx].token[i] = 0;
                pmem_persist(&level->buckets[0][s_idx].slot[j], sizeof(entry));
                pmem_persist(&level->buckets[0][s_idx].token[j], sizeof(uint8_t));
                pmem_persist(&level->buckets[1][idx].token[i], sizeof(uint8_t));
                level->level_item_num[0] ++;
                level->level_item_num[1] --;
                return i;
            }
        }
    }

    return -1;
}

/*
Function: level_destroy() 
        Destroy a level hash table
*/
void level_destroy(level_hash *level)
{
    free(level->buckets[0]);
    free(level->buckets[1]);
    level = NULL;
}

static void mlfs_buffer_sync(uint8_t to_dev, uint8_t *data,
        mlfs_fsblk_t blk_num, uint32_t size, uint32_t offset) {
    int ret;
    struct buffer_head *bh_data;
    mlfs_lblk_t nr_blocks = 0;

	if (size < g_block_size_bytes)
		nr_blocks = 1;
	else {
		nr_blocks = (size >> g_block_size_shift);
		if (size % g_block_size_bytes != 0) 
			nr_blocks++;
	}

    // update data block
    bh_data = bh_get_sync_IO(to_dev, blk_num, BH_NO_DATA_ALLOC);

    bh_data->b_data = data;
    bh_data->b_size = nr_blocks * g_block_size_bytes;
    bh_data->b_offset = offset;

    ret = mlfs_write(bh_data);
    mlfs_assert(!ret);
    clear_buffer_uptodate(bh_data);
    bh_release(bh_data);
}

extern level_hash *mlfs_level_init(uint64_t level_size,
        handle_t *handle, struct inode *inode, unsigned int flags)
{
    int goal, err = 0;
    mlfs_lblk_t allocated = 0;
    mlfs_fsblk_t newblock;

	if (sizeof(level_hash) < g_block_size_bytes)
		allocated = 1;
	else {
		allocated = (sizeof(level_hash) >> g_block_size_shift);
		if (sizeof(level_hash) % g_block_size_bytes != 0) 
			allocated++;
	}

    newblock = mlfs_new_data_blocks(handle, inode, goal, flags, &allocated, &err);
    level_hash *level = g_bdev[handle->dev]->map_base_addr + (newblock << g_block_size_shift);
    inode->l1.addrs[5] = newblock;
    if (!level)
    {
        printf("The level hash table initialization fails:1\n");
        exit(1);
    }

    level->level_size = level_size;
    level->addr_capacity = pow(2, level_size);
    level->total_capacity = pow(2, level_size) + pow(2, level_size - 1);
    generate_seeds(level);
    
    unsigned long bucket_size = sizeof(level_bucket) * pow(2, level_size);
	if (bucket_size < g_block_size_bytes)
		allocated = 1;
	else {
		allocated = (bucket_size >> g_block_size_shift);
		if ((bucket_size) % g_block_size_bytes != 0)
			allocated++;
	}

    newblock = mlfs_new_data_blocks(handle, inode, goal, flags, &allocated, &err);
    level->buckets[0] = g_bdev[handle->dev]->map_base_addr + (newblock << g_block_size_shift);
    pmem_memset_persist(level->buckets[0], 0, sizeof(level_bucket) * pow(2, level_size));
    inode->l1.addrs[6] = newblock;

    bucket_size = sizeof(level_bucket) * pow(2, level_size - 1);
	if (bucket_size < g_block_size_bytes)
		allocated = 1;
	else {
		allocated = (bucket_size >> g_block_size_shift);
		if ((bucket_size) % g_block_size_bytes != 0)
			allocated++;
	}

    newblock = mlfs_new_data_blocks(handle, inode, goal, flags, &allocated, &err);
    level->buckets[1] = g_bdev[handle->dev]->map_base_addr + (newblock << g_block_size_shift);
    pmem_memset_persist(level->buckets[1], 0, sizeof(level_bucket) * pow(2, level_size - 1));
    inode->l1.addrs[7] = newblock;

    level->level_item_num[0] = 0;
    level->level_item_num[1] = 0;
    level->level_resize = 0;
    
    if (!level->buckets[0] || !level->buckets[1])
    {
        printf("The level hash table initialization fails:2\n");
        exit(1);
    }

    pmem_persist(g_bdev[handle->dev]->map_base_addr + 
            ((inode->l1.addrs[5]) << g_block_size_shift), sizeof(level_hash));

//    printf("Level hashing: ASSOC_NUM %d, KEY_LEN %d, VALUE_LEN %d \n", ASSOC_NUM, KEY_LEN, VALUE_LEN);
//    printf("The number of top-level buckets: %d\n", level->addr_capacity);
//    printf("The number of all buckets: %d\n", level->total_capacity);
//    printf("The number of all entries: %d\n", level->total_capacity*ASSOC_NUM);
//    printf("The level hash table initialization succeeds!\n");

    return level;
}

extern void mlfs_level_resize(level_hash *level, handle_t *handle, 
        struct inode *inode, unsigned int flags) 
{
    int goal, err = 0;
    mlfs_lblk_t allocated = 0;
    mlfs_fsblk_t newblock;

    if (!level)
    {
        printf("The resizing fails: 1\n");
        exit(1);
    }

    level->addr_capacity = pow(2, level->level_size + 1);
    unsigned long new_bucket_size = level->addr_capacity * sizeof(level_bucket);
	if (new_bucket_size < g_block_size_bytes)
		allocated = 1;
	else {
		allocated = (new_bucket_size >> g_block_size_shift);
		if ((new_bucket_size) % g_block_size_bytes != 0)
			allocated++;
	}

    newblock = mlfs_new_data_blocks(handle, inode, goal, flags, &allocated, &err);
    level_bucket *newBuckets = g_bdev[g_root_dev]->map_base_addr + (newblock << g_block_size_shift);
    memset(newBuckets, 0, level->addr_capacity * pow(2, level->level_size + 1));

    if (!newBuckets) {
        printf("The resizing fails: 2\n");
        exit(1);
    }
    uint64_t new_level_item_num = 0;
    
    uint64_t old_idx;
    uint8_t str_key[KEY_LEN];
    for (old_idx = 0; old_idx < pow(2, level->level_size - 1); old_idx ++) {
        uint64_t i, j;
        for(i = 0; i < ASSOC_NUM; i ++){
            if (level->buckets[1][old_idx].token[i] == 1)
            {
                uint32_t key = level->buckets[1][old_idx].slot[i].key;
                uint64_t value = level->buckets[1][old_idx].slot[i].value;

                snprintf(str_key, KEY_LEN, "%lu", key);

                uint64_t f_idx = F_IDX(F_HASH(level, str_key), level->addr_capacity);
                uint64_t s_idx = S_IDX(S_HASH(level, str_key), level->addr_capacity);

                uint8_t insertSuccess = 0;
                for(j = 0; j < ASSOC_NUM; j ++){                            
                    /*  The rehashed item is inserted into the less-loaded bucket between 
                        the two hash locations in the new level
                    */
                    if (newBuckets[f_idx].token[j] == 0)
                    {
                        newBuckets[f_idx].slot[j].key = key;
                        newBuckets[f_idx].slot[j].value = value;
                        newBuckets[f_idx].token[j] = 1;
                        pmem_persist(&newBuckets[f_idx].slot[j].key, sizeof(uint32_t));
                        pmem_persist(&newBuckets[f_idx].slot[j].value, sizeof(uint64_t));
                        pmem_persist(&newBuckets[f_idx].token[j], sizeof(uint8_t));

                        insertSuccess = 1;
                        new_level_item_num ++;
                        break;
                    }
                    if (newBuckets[s_idx].token[j] == 0)
                    {
                        newBuckets[s_idx].slot[j].key = key;
                        newBuckets[s_idx].slot[j].value = value;
                        newBuckets[s_idx].token[j] = 1;
                        pmem_persist(&newBuckets[s_idx].slot[j].key, sizeof(uint32_t));
                        pmem_persist(&newBuckets[s_idx].slot[j].value, sizeof(uint64_t));
                        pmem_persist(&newBuckets[s_idx].token[j], sizeof(uint8_t));

                        insertSuccess = 1;
                        new_level_item_num ++;
                        break;
                    }
                }
                if(!insertSuccess){
                    printf("The resizing fails: 3\n");
                    exit(1);                    
                }
                
                level->buckets[1][old_idx].token[i] == 0;
                pmem_persist(&level->buckets[1][old_idx].token[i], sizeof(uint8_t));
            }
        }
    }

    level->level_size ++;
    level->total_capacity = pow(2, level->level_size) + pow(2, level->level_size - 1);

    //free(level->buckets[1]);
    new_bucket_size = ((uint64_t)pow(2, level->level_size - 2)) * sizeof(level_bucket);
	if (new_bucket_size < g_block_size_bytes)
		allocated = 1;
	else {
		allocated = (new_bucket_size >> g_block_size_shift);
		if ((new_bucket_size) % g_block_size_bytes != 0)
			allocated++;
	}

    mlfs_free_blocks(handle, inode, NULL,
        inode->l1.addrs[7], allocated, 0);

    level->buckets[1] = level->buckets[0];
    level->buckets[0] = newBuckets;
    newBuckets = NULL;

    inode->l1.addrs[7] = inode->l1.addrs[6];
    inode->l1.addrs[6] = newblock;
    
    level->level_item_num[1] = level->level_item_num[0];
    level->level_item_num[0] = new_level_item_num;
    level->level_resize ++;
}

