#include <stdlib.h>
#include <string.h>
#include "set.h"
#include "utils.h"

set_t *set_create(void (*destructor)(list_node_t *),
				  int (*datacmp)(void *, void *), size_t data_size)
{
	set_t *set = malloc(sizeof(*set));
	DIE(!set, "Malloc failed!\n");
	set->data_size = data_size;
	set->list = list_create(destructor);
	set->datacmp = datacmp;
	return set;
}

void simple_set_destructor(list_node_t *node)
{
	set_element_t *elem = STRUCT_FROM_MEMBER(set_element_t, node, node);
	free(elem->data);
	free(elem);
}

void set_destroy(set_t *set)
{
	list_destroy(set->list);
	free(set);
}

char set_contains(set_t *set, void *data)
{
	list_node_t *current = set->list->head;
	while (current) {
		set_element_t *elem = STRUCT_FROM_MEMBER(set_element_t, current, node);
		if (set->datacmp(elem->data, data) == 0)
			return 1;

		current = current->next;
	}
	return 0;
}

void set_add(set_t *set, void *data)
{
	if (set_contains(set, data))
		return;

	set_element_t *new_elem = malloc(sizeof(*new_elem));
	DIE(!new_elem, "Malloc failed\n");
	new_elem->data = malloc(set->data_size);
	DIE(!new_elem, "Malloc failed\n");
	memcpy(new_elem->data, data, set->data_size);
	list_push(set->list, &new_elem->node);
}

void set_remove(set_t *set, void *data)
{
	list_node_t *current = set->list->head;
	while (current) {
		set_element_t *elem = STRUCT_FROM_MEMBER(set_element_t, current, node);
		if (set->datacmp(elem->data, data) == 0) {
			list_purge_node(set->list, current);
			return;
		}
		current = current->next;
	}
}

set_t *set_intersect(set_t *set1, set_t *set2)
{
	set_t *intersect = set_create(set1->list->destructor, set1->datacmp,
								  set1->data_size);
	list_node_t *current = set1->list->head;
	while (current) {
		set_element_t *elem = STRUCT_FROM_MEMBER(set_element_t, current, node);
		if (set_contains(set2, elem->data))
			set_add(intersect, elem->data);

		current = current->next;
	}

	return intersect;
}

set_t *set_union(set_t *set1, set_t *set2)
{
	set_t *sunion = set_create(set1->list->destructor, set1->datacmp,
							   set1->data_size);
	list_node_t *current = set1->list->head;
	while (current) {
		set_element_t *elem = STRUCT_FROM_MEMBER(set_element_t, current, node);
		set_add(sunion, elem->data);
		current = current->next;
	}

	current = set2->list->head;
	while (current) {
		set_element_t *elem = STRUCT_FROM_MEMBER(set_element_t, current, node);
		set_add(sunion, elem->data);
		current = current->next;
	}
	return sunion;
}

char set_empty(set_t *set)
{
	return (set->list->size == 0);
}
