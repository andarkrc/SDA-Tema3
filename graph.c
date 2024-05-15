#include <stdio.h>
#include "graph.h"

graph_t *graph_create(void (*graph_node_des)(list_node_t *))
{
	graph_t *graph = (graph_t *)malloc(sizeof(graph_t));
	DIE(!graph, "Malloc failed!");
	graph->nodes = list_create(graph_node_des);
	graph->highway = map_create(HIGHWAY_BUCKETS, sizeof(size_t),
								sizeof(graph_node_t *), simple_entry_destroy,
								hash_size_t, sizetcmp);
	return graph;
}

void graph_destroy(graph_t *graph)
{
	map_destroy(graph->highway);
	list_destroy(graph->nodes);
	free(graph);
}

static void graph_link_destroy(list_node_t *node)
{
	graph_link_t *link = STRUCT_FROM_MEMBER(graph_link_t, node, node);
	free(link);
}

static graph_link_t *graph_link_create(graph_node_t *gnode)
{
	graph_link_t *glink = (graph_link_t *)malloc(sizeof(graph_link_t));
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
	gnode.node_id = 0;
	return gnode;
}

void graph_node_destroy(graph_node_t gnode)
{
	list_destroy(gnode.out_links);
	list_destroy(gnode.in_links);
}

void graph_add_node(graph_t *graph, graph_node_t *gnode)
{
	gnode->node_id = graph->nodes->size;
	list_push(graph->nodes, &gnode->node);
	map_add(graph->highway, &gnode->node_id, &gnode);
}

void graph_link_nodes(graph_t *graph, graph_node_t *gnode1, graph_node_t *gnode2)
{
	graph_link_t *new_link;
	new_link = graph_link_create(gnode2);
	list_push(gnode1->out_links, &new_link->node);
	new_link = graph_link_create(gnode1);
	list_push(gnode2->in_links, &new_link->node);
}

void graph_link_by_id(graph_t *graph, size_t id1, size_t id2)
{
	if (id1 >= graph->nodes->size) {
		return;
	}
	if (id2 >= graph->nodes->size) {
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

void graph_remove_node(graph_t *graph, graph_node_t *gnode)
{
	// Remove all links that point to this node.
	list_node_t *current;
	list_node_t *current2;
	list_node_t *removed;
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
				removed = list_remove_node(linked_node->out_links, &out_link->node);
				linked_node->out_links->destructor(removed);
				break;
			}
			current2 = current2->next;
		}
		current = current->next;
	}
	// Remove the node
	if (graph->nodes->size == 1) {
		removed = list_remove_node(graph->nodes, graph->nodes->head);
		map_remove(graph->highway, &gnode->node_id);
		graph->nodes->destructor(removed);
	} else if (graph->nodes->tail == &gnode->node) {
		removed = list_remove_node(graph->nodes, &gnode->node);
		map_remove(graph->highway, &gnode->node_id);
		graph->nodes->destructor(removed);
	} else {
		graph_node_t *last_node;
		last_node = STRUCT_FROM_MEMBER(graph_node_t, graph->nodes->tail, node);
		list_remove_node(graph->nodes, graph->nodes->tail);
		map_remove(graph->highway, &last_node->node_id);
		last_node->node_id = gnode->node_id;
		map_add(graph->highway, &last_node->node_id, &last_node);
		list_add_after(graph->nodes, &gnode->node, &last_node->node);
		removed = list_remove_node(graph->nodes, &gnode->node);
		graph->nodes->destructor(removed);
	}
}

void graph_remove_node_by_id(graph_t *graph, size_t id)
{
	graph_node_t *gnode;
	gnode = *(graph_node_t **)map_get_value(graph->highway, &id);
	graph_remove_node(graph, gnode);
}

void graph_print_ids(graph_t *graph)
{
	for (size_t i = 0; i < graph->nodes->size; i++) {
		graph_node_t *gnode;
		gnode = *(graph_node_t **)map_get_value(graph->highway, &i);
		printf("%lu: ", gnode->node_id);
		
		list_node_t *current;
		current = gnode->out_links->head;
		while (current != NULL) {
			graph_link_t *link = STRUCT_FROM_MEMBER(graph_link_t, current, node);
			printf("%lu, ", link->link->node_id);
			current = current->next;
		}
		printf("\n");
	}
	printf("\n");
}