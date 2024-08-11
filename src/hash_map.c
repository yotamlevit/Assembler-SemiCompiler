
#include <stdio.h>
#include <string.h>

#include "../include/hash_map.h"

#define HASH_PRIME 109951

/**
 * Default hash function for strings.
 *
 * @param key The key to hash.
 * @return The hash value.
 */
size_t defaultHashFunction(void *key) {
    char *str = (char *)key;
    char* ch;
    size_t hash = 0;
    for (ch = str; *ch; ch++) {
        hash ^= (size_t)(unsigned char)(*ch);
        hash *= HASH_PRIME;
    }

    return hash;
}


/**
 * Default key comparison function for strings.
 *
 * @param key1 The first key to compare.
 * @param key2 The second key to compare.
 * @return 0 if the keys are equal, non-zero otherwise.
 */
int defaultKeyCompareFunction(void *key1, void *key2) {
    return strcmp((char *)key1, (char *)key2);
}


/**
 * Creates a hash map with the specified number of buckets, hash function, and key comparison function.
 *
 * @param bucketCount The number of buckets in the hash map.
 * @param hashFunction The hash function to use (can be NULL for default).
 * @param keyCompareFunction The key comparison function to use (can be NULL for default).
 * @return A pointer to the created hash map, or NULL on failure.
 */
HashMapPtr createHashMap(size_t bucketCount, size_t (*hashFunction)(void *), int (*keyCompareFunction)(void *, void *)) {
    HashMapPtr map = (HashMapPtr)malloc(sizeof(HashMap));
    if(!map)
        return NULL;

    map->entries = (HashMapEntryPtr *)calloc(bucketCount, sizeof(HashMapEntryPtr));
    if(!map->entries){
        free(map);
        return NULL;
    }

    map->bucketCount = bucketCount;
    map->hashFunction = hashFunction ? hashFunction : defaultHashFunction;
    map->keyCompareFunction = keyCompareFunction ? keyCompareFunction : defaultKeyCompareFunction;
    return map;
}


/**
 * Inserts a key-value pair into the hash map.
 *
 * @param map The hash map to insert into.
 * @param key The key to insert.
 * @param value The value to insert.
 */
void hashMapInsert(HashMapPtr map, void *key, void *value) {
    size_t bucket_index = map->hashFunction(key) % map->bucketCount;
    HashMapEntry *entry = map->entries[bucket_index];

    while (entry != NULL) {
        if (map->keyCompareFunction(entry->key, key) == 0) {
            entry->value = value;
            return;
        }
        entry = entry->next;
    }

    entry = (HashMapEntry *)malloc(sizeof(HashMapEntry));
    entry->key = key;
    entry->value = value;
    entry->next = map->entries[bucket_index];
    map->entries[bucket_index] = entry;
}


/**
 * Finds a value in the hash map by its key.
 *
 * @param map The hash map to search.
 * @param key The key to search for.
 * @return The value associated with the key, or NULL if not found.
 */
void *hashMapFind(HashMapPtr map, void *key) {
    HashMapEntry *entry;
    size_t bucket_index = map->hashFunction(key) % map->bucketCount;

    if(bucket_index > map->bucketCount)
        return NULL;

    entry = map->entries[bucket_index];

    while (entry != NULL) {
        if (map->keyCompareFunction(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}


/**
 * Frees the memory allocated for the hash map.
 *
 * @param map The hash map to free.
 */
void hashMapFree(HashMapPtr map) {
    size_t i;
    for ( i = 0; i < map->bucketCount; i++) {
        HashMapEntry *entry = map->entries[i];
        while (entry != NULL) {
            HashMapEntry *next = entry->next;
            free(entry);
            entry = next;
        }
    }
    free(map->entries);
    free(map);
}