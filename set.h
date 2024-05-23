#ifndef SET_H
#define SET_H

#include "linked_list.h"

typedef struct set_element_t {
	void *data;
	list_node_t node;
} set_element_t;

typedef struct set_t {
	linked_list_t *list;
	size_t data_size;
	int (*datacmp)(void *, void *);
} set_t;

/**
 * set_create() - Create a new set (list of unique elements).
 * @param destructor: Function that frees the memory for a set_element_t.
 * @param datacmp: Function that compares the elements of the set.
 * @param data_size: Size of the data stored in a set_element_t (in bytes).
 * @return - Pointer to the new set.
 */
set_t *set_create(void (*destructor)(list_node_t *),
				  int (*datacmp)(void *, void *), size_t data_size);

/**
 * set_simple_destructor() - Function that destroys a simple set_element_t.
 * @param node: Pointer to the node member of the set_element_t.
 */
void simple_set_destructor(list_node_t *node);

/**
 * set_destroy() - Function that destroys a set(frees the memory).
 * @param set: Pointer to the set to be destroyed.  
 */
void set_destroy(set_t *set);

/**
 * set_contains() - Whether or not the set contains the data.
 * @param set: The set to search the data in.
 * @param data: The data to be searched.
 * @return - 1 if the set contains the data,
 * 			 0 otherwise.
 */
char set_contains(set_t *set, void *data);

/**
 * set_add() - Add the given data in the set.
 * @param set
 * @param data
 */
void set_add(set_t *set, void *data);

/**
 * set_remove() - Remove the given data from the set.
 * @param set
 * @param data
 */
void set_remove(set_t *set, void *data);

/**
 * set_intersect() - Creates a new set as the intersection between set1 & set2.
 * @param set1
 * @param set2
 * @return - Pointer to the new intersection set.
 */
set_t *set_intersect(set_t *set1, set_t *set2);

/**
 * set_union() - Creates a new set as the union between set1 & set2.
 * @param set1
 * @param set2
 * @return - Pointer to the new union set.
 */
set_t *set_union(set_t *set1, set_t *set2);

/**
 * set_empty() Verify if a ste is empty or not,
 * @param set
 * @return - 1 if the set is empty,
 * 			 0 otherwise.
 */
char set_empty(set_t *set);
#endif