#include "linked_list.h"

linked_list_t *list_create(void (*destructor)(list_node_t *))
{
	linked_list_t *list = malloc(sizeof(*list));
	DIE(!list, "Malloc failed\n");
	list->size = 0;
	list->head = NULL;
	list->tail = NULL;
	list->destructor = destructor;
	return list;
}

void list_remove_all(linked_list_t *list)
{
	while (!list_empty(list)) {
		list_node_t *node = list_remove(list, 0);
		if (list->destructor)
			list->destructor(node);
	}
}

void list_destroy(linked_list_t *list)
{
	list_remove_all(list);
	free(list);
}

void list_free(linked_list_t *list)
{
	list_destroy(list);
}

char list_empty(linked_list_t *list)
{
	return (list->size == 0);
}

size_t list_get_node_index(linked_list_t *list, list_node_t *node)
{
	size_t index = 0;
	list_node_t *current = list->head;
	while (current != node && current) {
		current = current->next;
		index++;
	}
	return index;
}

list_node_t *list_get_node(linked_list_t *list, size_t index)
{
	if (!list)
		return NULL;

	if (list->size == 0)
		return NULL;

	index = (index >= list->size) ? list->size - 1 : index;

	list_node_t *current;
	// Check if it's closer to the end of the list rather than the beginning.
	if (index <= list->size / 2) {
		current = list->head;
		while (index > 0) {
			current = current->next;
			index--;
		}
	} else {
		current = list->tail;
		index = list->size - index - 1;
		while (index > 0) {
			current = current->prev;
			index--;
		}
	}
	return current;
}

list_node_t *list_remove_node(linked_list_t *list, list_node_t *node)
{
	if (!list || !node)
		return NULL;

	if (list->size == 0)
		return NULL;

	list_node_t *prev = node->prev;
	list_node_t *next = node->next;
	// If node is only the node
	if (!prev && !next) {
		list->head = NULL;
		list->tail = NULL;
		list->size--;
		return node;
	}
	// Node is head
	if (!prev) {
		list->head = next;
		next->prev = NULL;
	} else {
		prev->next = next;
	}
	// Node is tail
	if (!next) {
		list->tail = prev;
		prev->next = NULL;
	} else {
		next->prev = prev;
	}
	list->size--;
	return node;
}

list_node_t *list_remove(linked_list_t *list, size_t index)
{
	list_node_t *removed = list_get_node(list, index);
	return list_remove_node(list, removed);
}

void list_purge(linked_list_t *list, size_t index)
{
	list_node_t *removed = list_remove(list, index);
	if (!removed)
		return;

	if (list->destructor)
		list->destructor(removed);
}

void list_purge_node(linked_list_t *list, list_node_t *node)
{
	list_node_t *removed = list_remove_node(list, node);
	if (!removed)
		return;

	if (list->destructor)
		list->destructor(removed);
}

void list_purge_all(linked_list_t *list)
{
	list_remove_all(list);
}

void list_push(linked_list_t *list, list_node_t *node)
{
	list_add(list, list->size, node);
}

void list_add(linked_list_t *list, size_t index, list_node_t *node)
{
	if (!list || !node)
		return;

	index = (index > list->size) ? list->size : index;
	if (list->size == 0) {
		node->next = NULL;
		node->prev = NULL;
		list->head = node;
		list->tail = node;
		list->size++;
		return;
	}
	if (index == 0) {
		node->next = list->head;
		node->prev = NULL;
		list->head = node;
		list->size++;
		return;
	}
	if (index == list->size) {
		list->tail->next = node;
		node->next = NULL;
		node->prev = list->tail;
		list->tail = node;
		list->size++;
		return;
	}

	list_node_t *prev = list_get_node(list, index - 1);
	list_node_t *next = prev->next;
	node->next = next;
	node->prev = prev;
	prev->next = node;
	next->prev = node;
	list->size++;
}

void list_add_after(linked_list_t *list, list_node_t *prev, list_node_t *node)
{
	if (!list || !node)
		return;

	// Functionality for adding node as head.
	if (!prev) {
		if (list->size == 0) {
			node->next = NULL;
			node->prev = NULL;
			list->head = node;
			list->tail = node;
			list->size++;
			return;
		}
		node->prev = NULL;
		node->next = list->head;
		list->head->prev = node;
		list->head = node;
		list->size++;
		return;
	}
	if (prev == list->tail) {
		list->tail->next = node;
		node->next = NULL;
		node->prev = list->tail;
		list->tail = node;
		list->size++;
		return;
	}
	list_node_t *next = prev->next;
	node->next = next;
	node->prev = prev;
	prev->next = node;
	next->prev = node;
	list->size++;
}

void list_add_before(linked_list_t *list, list_node_t *next, list_node_t *node)
{
	if (!list || !node)
		return;

	// Functionality for adding node as tail.
	if (!next) {
		if (list->size == 0) {
			node->next = NULL;
			node->prev = NULL;
			list->head = node;
			list->tail = node;
			list->size++;
			return;
		}
		node->next = NULL;
		node->prev = list->tail;
		list->tail->next = node;
		list->tail = node;
		list->size++;
		return;
	}
	if (next == list->head) {
		node->prev = NULL;
		node->next = list->head;
		list->head->prev = node;
		list->head = node;
		list->size++;
		return;
	}
	list_node_t *prev = next->prev;
	node->next = next;
	node->prev = prev;
	prev->next = node;
	next->prev = node;
	list->size++;
}
