#include <stdio.h>
#include "graph.h"

static void old_id_destroy(list_node_t *node)
{
	old_id_t *old_id;
	old_id = STRUCT_FROM_MEMBER(old_id_t, node, node);
	free(old_id);	
}

graph_t *graph_create(void (*graph_node_des)(list_node_t *))
{
	graph_t *graph = malloc(sizeof(*graph));
	DIE(!graph, "Malloc failed!");
	graph->nodes = list_create(graph_node_des);
	graph->highway = map_create(HIGHWAY_BUCKETS, sizeof(size_t),
								sizeof(graph_node_t *), simple_entry_destroy,
								hash_size_t, sizetcmp);
	graph->old_ids = list_create(old_id_destroy);
	return graph;
}

void graph_destroy(graph_t *graph)
{
	map_destroy(graph->highway);
	list_destroy(graph->nodes);
	list_destroy(graph->old_ids);
	free(graph);
}

static void graph_link_destroy(list_node_t *node)
{
	graph_link_t *link = STRUCT_FROM_MEMBER(graph_link_t, node, node);
	free(link);
}

static graph_link_t *graph_link_create(graph_node_t *gnode)
{
	graph_link_t *glink = malloc(sizeof(*glink));
	DIE(!glink, "Malloc failed!");
	glink->link = gnode;
	glink->node.prev = NULL;
	glink->node.next = NULL;
	return glink;
}

graph_node_t graph_node_create(void)
{
	graph_node_t gnode;
	gnode.out_links = list_create(graph_link_destroy);
	gnode.in_links = list_create(graph_link_destroy);
	gnode.node.prev = NULL;
	gnode.node.next = NULL;
	gnode.id = 0;
	return gnode;
}

void graph_node_destroy(graph_node_t gnode)
{
	list_destroy(gnode.out_links);
	list_destroy(gnode.in_links);
}

size_t graph_add_node(graph_t *graph, graph_node_t *gnode)
{
	// Do I need to make so it takes the first available id or nah?
	// I don't think it's a pressing issue right now, but ye
	// it's more intuitive if it takes the first available id.
	// Buuut, it's not like the list is sorted anyway, so yeah.
	// I'm just going to leave this as is.
	if (graph_get_node(graph, graph->nodes->size) == NULL) {
		gnode->id = graph->nodes->size;
		list_push(graph->nodes, &gnode->node);
		map_add(graph->highway, &gnode->id, &gnode);
		return gnode->id;
	} else {
		old_id_t *old_id;
		old_id = STRUCT_FROM_MEMBER(old_id_t, graph->old_ids->head, node);
		gnode->id = old_id->id;
		list_push(graph->nodes, &gnode->node);
		map_add(graph->highway, &gnode->id, &gnode);
		old_id->id = old_id->id + 1;
		if (graph_get_node(graph, old_id->id) != NULL) {
			list_purge(graph->old_ids, 0);
		}
		return gnode->id;
	}
}

void graph_link_nodes(graph_t *graph, graph_node_t *gnode1, graph_node_t *gnode2)
{
	// Seems like 'graph' is unused, but it may be in the future
	graph = graph;
	if (gnode1 == NULL || gnode2 == NULL) {
		return;
	}
	// Check if the link already exists
	list_node_t *current;
	current = gnode1->out_links->head;
	while (current != NULL) {
		graph_link_t *link;
		link = STRUCT_FROM_MEMBER(graph_link_t, current, node);
		if (link->link == gnode2) {
			return;
		}
		current = current->next;
	}
	// Add the new link
	graph_link_t *new_link;
	new_link = graph_link_create(gnode2);
	list_push(gnode1->out_links, &new_link->node);
	new_link = graph_link_create(gnode1);
	list_push(gnode2->in_links, &new_link->node);
}

void graph_link_by_id(graph_t *graph, size_t id1, size_t id2)
{
	// It can be better
	if (map_get_entry(graph->highway, &id1) == NULL) {
		return;
	}
	if (map_get_entry(graph->highway, &id2) == NULL) {
		return;
	}

	graph_node_t *gnode1 = *(graph_node_t **)map_get_value(graph->highway, &id1);
	graph_node_t *gnode2 = *(graph_node_t **)map_get_value(graph->highway, &id2);

	graph_link_nodes(graph, gnode1, gnode2);
}

void graph_blink_by_id(graph_t *graph, size_t id1, size_t id2)
{
	graph_link_by_id(graph, id1, id2);
	graph_link_by_id(graph, id2, id1);
}

void graph_blink_nodes(graph_t *graph, graph_node_t *gnode1, graph_node_t *gnode2)
{
	graph_link_nodes(graph, gnode1, gnode2);
	graph_link_nodes(graph, gnode2, gnode1);
}

void graph_unlink_nodes(graph_t *graph, graph_node_t *gnode1, graph_node_t *gnode2)
{
	// Seems like 'graph' is unused, but it may be in the future
	graph = graph;
	if (gnode1 == NULL || gnode2 == NULL) {
		return;
	}
	list_node_t *current;

	current = gnode1->out_links->head;
	while (current != NULL) {
		graph_link_t *link;
		link = STRUCT_FROM_MEMBER(graph_link_t, current, node);
		if (link->link == gnode2) {
			list_purge_node(gnode1->out_links, &link->node);
			break;
		}
		current = current->next;
	}

	current = gnode2->in_links->head;
	while (current != NULL) {
		graph_link_t *link;
		link = STRUCT_FROM_MEMBER(graph_link_t, current, node);
		if (link->link == gnode1) {
			list_purge_node(gnode2->in_links, &link->node);
			break;
		}
		current = current->next;
	}
}

void graph_unlink_by_id(graph_t *graph, size_t id1, size_t id2)
{
	if (map_get_entry(graph->highway, &id1) == NULL) {
		return;
	}
	if (map_get_entry(graph->highway, &id2) == NULL) {
		return;
	}

	graph_node_t *gnode1 = *(graph_node_t **)map_get_value(graph->highway, &id1);
	graph_node_t *gnode2 = *(graph_node_t **)map_get_value(graph->highway, &id2);

	graph_unlink_nodes(graph, gnode1, gnode2);
}

void graph_unblink_nodes(graph_t *graph, graph_node_t *gnode1, graph_node_t *gnode2)
{
	graph_unlink_nodes(graph, gnode1, gnode2);
	graph_unlink_nodes(graph, gnode2, gnode2);
}

void graph_unblink_by_id(graph_t *graph, size_t id1, size_t id2)
{
	graph_unlink_by_id(graph, id1, id2);
	graph_unlink_by_id(graph, id2, id1);
}

void graph_remove_node(graph_t *graph, graph_node_t *gnode)
{
	// Remove all links that point to this node.
	list_node_t *current;
	list_node_t *current2;
	current = gnode->in_links->head;
	while (current != NULL) {
		graph_link_t *link;
		link = STRUCT_FROM_MEMBER(graph_link_t, current, node);
		graph_node_t *linked_node = link->link;
		// Search for the link to gnode
		current2 = linked_node->out_links->head;
		while (current2 != NULL) {
			graph_link_t *out_link;
			out_link = STRUCT_FROM_MEMBER(graph_link_t, current2, node);
			if (out_link->link == gnode) {
				list_purge_node(linked_node->out_links, &out_link->node);
				break;
			}
			current2 = current2->next;
		}
		current = current->next;
	}
	// Remove the node
	if (graph->nodes->size == 1) {
		map_remove(graph->highway, &gnode->id);
		list_purge_node(graph->nodes, graph->nodes->head);
	} else if (graph->nodes->tail == &gnode->node) {
		map_remove(graph->highway, &gnode->id);
		list_purge_node(graph->nodes, &gnode->node);
	} else {
		size_t id = gnode->id;
		old_id_t *old_id;
		if (id == 0) {
			old_id = malloc(sizeof(*old_id));
			DIE(!old_id, "Malloc failed!\n");
			old_id->id = 0;
			list_push(graph->old_ids, &old_id->node);
		} else {
			size_t prev_id = id - 1;
			if (graph_get_node(graph, prev_id) != NULL) {
				old_id = malloc(sizeof(*old_id));
				DIE(!old_id, "Malloc failed!\n");
				old_id->id = id;
				list_push(graph->old_ids, &old_id->node);
			}
		}
		map_remove(graph->highway, &id);
		list_purge_node(graph->nodes, &gnode->node);
	}
	current = graph->old_ids->head;
	while (current != NULL) {
		current2 = current->next;
		old_id_t *old_id = STRUCT_FROM_MEMBER(old_id_t, current, node);
		if (old_id->id >= graph->nodes->size) {
			list_purge_node(graph->old_ids, &old_id->node);
		}
		current = current2;
	}
}

void graph_remove_node_by_id(graph_t *graph, size_t id)
{
	if (map_get_entry(graph->highway, &id) == NULL) {
		return;
	}
	graph_node_t *gnode;
	gnode = *(graph_node_t **)map_get_value(graph->highway, &id);
	graph_remove_node(graph, gnode);
}

graph_node_t *graph_get_node(graph_t *graph, size_t id)
{
	graph_node_t **node;
	node = (graph_node_t **)map_get_value(graph->highway, &id);
	return (node == NULL) ? NULL : *node;
}

graph_node_t *graph_node_get_first_inlink(graph_node_t *gnode)
{
	list_node_t *node;
	node = gnode->in_links->head;
	if (node == NULL) {
		return NULL;
	}
	graph_link_t *link = STRUCT_FROM_MEMBER(graph_link_t, node, node);
	return link->link;
}

void graph_print_ids(graph_t *graph)
{
	list_node_t *current = graph->nodes->head;
	while (current != NULL) {
		graph_node_t *gnode;
		gnode = STRUCT_FROM_MEMBER(graph_node_t, current, node);
		printf("%lu: ", gnode->id);
		
		list_node_t *current2;
		current2 = gnode->out_links->head;
		while (current2 != NULL) {
			graph_link_t *link = STRUCT_FROM_MEMBER(graph_link_t, current2, node);
			printf("%lu, ", link->link->id);
			current2 = current2->next;
		}
		printf("\n");
		current = current->next;
	}
	printf("\n");
}