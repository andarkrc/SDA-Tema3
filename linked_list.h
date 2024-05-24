#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>
#include "utils.h"

typedef struct list_node_t {
	struct list_node_t *prev;
	struct list_node_t *next;
} list_node_t;

typedef struct linked_list_t {
	list_node_t *head;
	list_node_t *tail;
	size_t size;
	void (*destructor)(list_node_t *node);
} linked_list_t;

// Some of the functions can be macros.
// But I don't care. I am not creating os kernels.
// Also 'memory is cheap'. (actual comment from the linux kernel :/)

/**
 * list_create() - Creates a new linked list.
 * @param destructor: Function that destroys a node of the list.
 *					  (frees the memory)
 *
 * @return - Pointer to the list.
 */
linked_list_t *list_create(void (*destructor)(list_node_t *));

/**
 * list_remove_all() - Remove & destroy all nodes of a list.
 * @param list
 */
void list_remove_all(linked_list_t *list);

/**
 * list_destroy() - Destroys a link list.
 * @param list
 */
void list_destroy(linked_list_t *list);

/**
 * list_free() - list_destroy().
 * @param list
 */
void list_free(linked_list_t *list);

/**
 * list_add() - Adds a node at the specified index in the list.
 * @param list
 * @param index: Where the node should be placed in the list.
 * @param node: Pointer to the node.
 */
void list_add(linked_list_t *list, size_t index, list_node_t *node);

/**
 * list_push() - Adds a node at the end of the list.
 * @param list
 * @param node
 */
void list_push(linked_list_t *list, list_node_t *node);

/**
 * list_add_after() - Adds a node after a specified node.
 * @param list
 * @param prev: Node after we add. If NULL, node will be head of the list.
 * @param node
 */
void list_add_after(linked_list_t *list, list_node_t *prev, list_node_t *node);

/**
 * list_add_before() - Adds a node before a specified node.
 * @param list
 * @param next: Node before we add. If NULL, node will be tail of the list.
 * @param node
 */
void list_add_before(linked_list_t *list, list_node_t *next, list_node_t *node);

/**
 * list_remove() - Removes the node from the list from the specified index.
 * @param list
 * @param index
 *
 * @return - Pointer to the removed node.
 */
list_node_t *list_remove(linked_list_t *list, size_t index);

/**
 * list_remove_node() - Removes the specified node from the list.
 * @param list
 * @param node: Pointer to the node.
 *
 * @return - Pointer to the removed node.
 */
list_node_t *list_remove_node(linked_list_t *list, list_node_t *node);

/**
 * list_purge() - list_remove() but also frees the node.
 * @param list
 * @param index: Index of the node to be purged.
 */
void list_purge(linked_list_t *list, size_t index);

/**
 * list_purge_node() - list_remove_node() but also frees the node.
 * @param list
 * @param node
 */
void list_purge_node(linked_list_t *list, list_node_t *node);

/**
 * list_purge_all() - list_remove_all()
 * @param list
 */
void list_purge_all(linked_list_t *list);

/**
 * list_get_node() - Returns the node at a specified index in the list.
 * @param list
 * @param index
 *
 * @return - Pointer to the node.
 */
list_node_t *list_get_node(linked_list_t *list, size_t index);

/**
 * list_get_node_index() - Returns the index at where a specified node exists
 *						   in the list
 * @param list
 * @param node - Pointer to the node.
 *
 * @return - Index of the node.
 */
size_t list_get_node_index(linked_list_t *list, list_node_t *node);

/**
 * list_empty() - Verifies if the list is empty
 * @param list
 *
 * @return - 1 if the list is empty (size is 0)
 *			 0 if the list is not empty
 */
char list_empty(linked_list_t *list);

#endif
