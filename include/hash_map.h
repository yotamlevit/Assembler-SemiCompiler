/*
Created by Yotam Levit on 26/04/2024.
*/

#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct HashMapEntry {
    void *key;
    void *value;
    struct HashMapEntry *next;
} HashMapEntry,* HashMapEntryPtr;


typedef struct HashMap {
    HashMapEntryPtr *entries;
    size_t bucketCount;
    size_t (*hashFunction)(void *key);
    int (*keyCompareFunction)(void *key1, void *key2);
} HashMap,* HashMapPtr;


/* Function Headers */
size_t defaultHashFunction(void *key);
int defaultKeyCompareFunction(void *key1, void *key2);
HashMapPtr createHashMap(size_t bucketCount, size_t (*hashFunction)(void *), int (*keyCompareFunction)(void *, void *));
void hashMapInsert(HashMapPtr map, void *key, void *value);
void *hashMapFind(HashMapPtr map, void *key);
void hashMapFree(HashMapPtr map);



#endif
