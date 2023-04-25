// Headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CACHE_SIZE 32
#define BLOCK_SIZE 4

typedef struct cache {
    int **Tag_Array;
    int **valid_array;    // Valid array
    char **data_array;    // Data array
    int *lru;   // LRU array
    int size;   // cache size
    int assoc;  // cache associativity
    int num_sets;   // Number of sets in the cache
    int block_size; // Size of a block in the cache
    int replacement; //(0 for LRU, 1 for Random)
    int hit;    // cache hits count
    int miss;   // cache misses count
} Cache;

// function phototype
int hex_to_int(char *hex);
void CacheInit(Cache *cache, int assoc, int rp);
void AccessCache(Cache *cache, int addr);

// convert hexadecimal strings to integers
int hex_to_int(char *hex_str)
{
    int len = strlen(hex_str);
    int base = 1;
    int result = 0;

    for (int i = len - 1; i >= 0; i--)
    {
        if (hex_str[i] >= '0' && hex_str[i] <= '9')
        {
            result += (hex_str[i] - '0') * base;
            base *= 16;
        }
        else if (hex_str[i] >= 'a' && hex_str[i] <= 'f')
        {
            result += (hex_str[i] - 'a' + 10) * base;
            base *= 16;
        }
        else if (hex_str[i] >= 'A' && hex_str[i] <= 'F')
        {
            result += (hex_str[i] - 'A' + 10) * base;
            base *= 16;
        }
        else
        {
            printf("Error: Invalid hexadecimal string\n");
            exit(1);
        }
    }
    return result;
}

// initialize the cache
void CacheInit(Cache *cache, int asso, int rp) {
    // memory allocation
    cache->Tag_Array = (int **) malloc(cache->num_sets * sizeof(int *));
    cache->valid_array = (int **) malloc(cache->num_sets * sizeof(int *));
    cache->data_array = (char **) malloc(cache->num_sets * sizeof(char *));
    cache->assoc = asso;
    cache->num_sets = cache->size / (asso * cache->block_size);
    cache->replacement = rp;
    cache->hit = 0;
    cache->miss = 0;

    if (rp == 0) {
        cache->lru = (int *) malloc(cache->num_sets * sizeof(int));
        memset(cache->lru, 0, cache->num_sets * sizeof(int));
    }
    for (int i = 0; i < cache->num_sets; i++) {
        cache->Tag_Array[i] = (int *) malloc(asso * sizeof(int));
        cache->valid_array[i] = (int *) malloc(asso * sizeof(int));
        cache->data_array[i] = (char *) malloc(cache->block_size * asso * sizeof(char));
        for (int j = 0; j < asso; j++) {
            cache->Tag_Array[i][j] = -1;
            cache->valid_array[i][j] = 0;
            memset(&cache->data_array[i][j * cache->block_size], 0, cache->block_size * sizeof(char));
        }
    }
}
// access the cache to store to memory
void AccessCache(Cache *cache, int addr) {
    int set_index = (addr / cache->block_size) % cache->num_sets;
    int tag = (addr / cache->block_size) / cache->num_sets;
    int lru_index, random_index;

    for (int i = 0; i < cache->assoc; i++) {
        if (cache->valid_array[set_index][i] && cache->Tag_Array[set_index][i] == tag) {
            cache->hit++;
            if (cache->replacement == 0) {
                cache->lru[set_index] = i;
            }
            return;
        }
    }
    // add "miss" count when store new cache into memory
    cache->miss++;

    // condition when cache replace
    if (cache->replacement == 0) {
        lru_index = cache->lru[set_index];
        for (int i = 0; i < cache->assoc; i++) {
            if (!cache->valid_array[set_index][i]) {
                lru_index = i;
                break;
            }
            if (cache->lru[set_index] > cache->lru[set_index + i]) {
                lru_index = i;
            }
        }
        cache->lru[set_index] = cache->lru[set_index] + 1;
        // condition then lry equal associative
        if (lru_index == cache->assoc) {
            lru_index = 0;
            cache->lru[set_index] = 0;
        }
        cache->Tag_Array[set_index][lru_index] = tag;
        cache->valid_array[set_index][lru_index] = 1;
        memcpy(&cache->data_array[set_index][lru_index * cache->block_size], &addr, cache->block_size * sizeof(char));
    }
    else {
        random_index = rand() % cache->assoc;
        cache->Tag_Array[set_index][random_index] = tag;
        cache->valid_array[set_index][random_index] = 1;
        memcpy(&cache->data_array[set_index][random_index * cache->block_size], &addr, cache->block_size * sizeof(char));
    }
}

int main(int argc, char *argv[]) {

    Cache cache;
    char hex[9];
    int addr;
    int i;
    cache.size = CACHE_SIZE; // cache size 32
    cache.block_size = BLOCK_SIZE; // block size 4
    CacheInit(&cache, 1, 0);
    // loop for accessing addresses
    for (i = 0; i < 16; i++) {
        // Read hexadecimal input
        scanf("%s", hex);
        // Convert hexadecimal to integer
        addr = hex_to_int(hex);
        // call function for accessing cache
        AccessCache(&cache, addr);
    }

    // Print outcome
    printf("Hits: %d\n", cache.hit);
    printf("Misses: %d\n", cache.miss);

    return 0;
}
