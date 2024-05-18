#include "hash_map.h"

map_t *map_create(size_t max_buckets, size_t key_size, size_t value_size,
				  void (*destructor)(list_node_t *),
				  uint (*hash)(void *), int (*keycmp)(void *, void *))
{
	map_t *map = malloc(sizeof(*map));
	DIE(!map, "Malloc failed\n");
	map->max_buckets = max_buckets;
	map->hash = hash;
	map->keycmp = keycmp;
	map->key_size = key_size;
	map->value_size = value_size;
	size_t buckets_size = max_buckets * sizeof(*(map->buckets));
	map->buckets = malloc(buckets_size);
	DIE(!map->buckets, "Malloc failed\n");
	for (size_t i = 0; i < max_buckets; i++) {
		map->buckets[i] = list_create(destructor);
	}
	return map;
}

void map_destroy(map_t *map)
{
	for (size_t i = 0; i < map->max_buckets; i++) {
		linked_list_t *list = map->buckets[i];
		list_destroy(list);
	}
	free(map->buckets);
	free(map);
}

void map_free(map_t *map)
{
	map_destroy(map);
}

void simple_entry_destroy(list_node_t *node)
{
	map_entry_t *entry = STRUCT_FROM_MEMBER(map_entry_t, node, node);
	free(entry->key);
	free(entry->value);
	free(entry);
}

int strkeycmp(void *key1, void *key2)
{
	return strcmp((const char *)key1, (const char *)key2);
}

void map_add(map_t *map, void *key, void *value)
{
	map_entry_t *entry = map_get_entry(map, key);
	if (entry != NULL) {
		memcpy(entry->value, value, map->value_size);
		return;
	}

	unsigned int bucket = map->hash(key) % map->max_buckets;
	linked_list_t *list = map->buckets[bucket];
	// Create the new entry & place it in the list.
	entry = malloc(sizeof(*entry));
	DIE(!entry, "Malloc failed\n");

	entry->key = malloc(map->key_size);
	DIE(!entry->key, "Malloc failed\n");
	entry->value = malloc(map->value_size);
	DIE(!entry->value, "Malloc failed\n");

	memcpy(entry->key, key, map->key_size);
	memcpy(entry->value, value, map->value_size);

	list_push(list, &entry->node);
}

void map_remove(map_t *map, void *key)
{
	map_entry_t *entry = map_get_entry(map, key);
	if (entry == NULL) {
		return;
	}

	uint bucket = map->hash(key) % map->max_buckets;
	linked_list_t *list = map->buckets[bucket];
	list_purge_node(list, &entry->node);
}

map_entry_t *map_get_entry(map_t *map, void *key)
{
	uint bucket = map->hash(key) % map->max_buckets;
	linked_list_t *list = map->buckets[bucket];
	list_node_t *current = list->head;
	while (current) {
		map_entry_t *entry = STRUCT_FROM_MEMBER(map_entry_t, current, node);
		// Stop when you find the correct entry.
		if (map->keycmp(entry->key, key) == 0) {
			return entry;
		}
		current = current->next;
	}
	return NULL;
}

char map_has_key(map_t *map, void *key)
{
	return (map_get_entry(map, key) != NULL);
}

void *map_get_value(map_t *map, void *key)
{
	map_entry_t *entry = map_get_entry(map, key);
	return (entry != NULL) ? entry->value : NULL;
}