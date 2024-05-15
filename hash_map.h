#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "linked_list.h"
#include <string.h>

typedef struct map_entry_t {
	void *value;
	void *key;
	list_node_t node;
} map_entry_t;

typedef struct map_t {
	linked_list_t **buckets;
	size_t max_buckets;
	size_t key_size;
	size_t value_size;

	int (*keycmp)(void *, void *);
	uint (*hash)(void *);
} map_t;

/**
 * map_create() - Creates a new hash map.
 * @param max_buckets: Maximum number of buckets the map should have.
 * @param key_size: Size of the keys used by the map (in bytes).
 * @param value_size: Size of the values to be inserted in the map (in bytes).
 * @param destructor: Function that destroys a map entry.
 * @param hash: Function that hashes a key.
 * @param keycmp: Function that compares 2 keys.
 * 
 * @return - Pointer to the new map.
 */
map_t *map_create(size_t max_buckets, size_t key_size, size_t value_size,
				  void (*destructor)(list_node_t *),
				  uint (*hash)(void *), int (*keycmp)(void *, void *));

/**
 * simple_entry_destroy() - Destroy a simple hash map entry.
 * 							(simple data types & arrays)
 * @param node: Node of the bucket list.
 */
void simple_entry_destroy(list_node_t *node);

/**
 * strkeycmp() - strcmp() : Used to compare 2 string keys in a map.
 * @param key1
 * @param key2
 * 
 * @return - See return of strcmp()
 */
int strkeycmp(void *key1, void *key2);

/**
 * map_destroy() - Destroys a hash map.
 * @param map - Pointer to the map to be destroyed.
 */
void map_destroy(map_t *map);

/**
 * map_free() - map_destroy()
 * @param map - Pointer to the map to be freed.
 */
void map_free(map_t *map);

/**
 * map_add() - Adds a value in the hash map at the specified key.
 * @param map
 * @param key
 * @param value
 */
void map_add(map_t *map, void *key, void *value);

/**
 * map_remove() - Removes from the map the entry at the specified key.
 * @param map
 * @param key
 */
void map_remove(map_t *map, void *key);

/**
 * map_has_key() - Checks the existence of an entry at the specified key.
 * @param map
 * @param key
 * 
 * @return - 1 if the map contains an entry for key.
 * 			 0 if the map does not contain an entry for key.
 */
char map_has_key(map_t *map, void *key);

/**
 * map_get_value() - Gets only the value that exists in the map at key.
 * @param map
 * @param key
 * 
 * @return - The value at key.
 */
void *map_get_value(map_t *map, void *key);

/**
 * map_get_entry() - Gets the whole entry that exists in the map at key.
 * 						()
 */
map_entry_t *map_get_entry(map_t *map, void *key);

#endif