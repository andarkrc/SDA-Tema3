#ifndef GRAPH_H
#define GRAPH_H

#include "hash_map.h"

#define HIGHWAY_BUCKETS 100

typedef struct old_id_t {
	size_t id;
	list_node_t node;
} old_id_t;

typedef struct graph_node_t {
	size_t id;
	linked_list_t *out_links;
	linked_list_t *in_links;
	list_node_t node;
} graph_node_t;

typedef struct graph_link_t{
	graph_node_t *link;
	list_node_t node;
} graph_link_t;

typedef struct graph_t {
	linked_list_t *nodes;
	map_t *highway;
	linked_list_t *old_ids;
} graph_t;

graph_t *graph_create(void (*graph_node_des)(list_node_t *));

void graph_destroy(graph_t *graph);

graph_node_t graph_node_create(void);

void graph_node_destroy(graph_node_t gnode);

void graph_add_node(graph_t *graph, graph_node_t *gnode);

void graph_link_nodes(graph_t *graph, graph_node_t *gnode1, graph_node_t *gnode2);

void graph_link_by_id(graph_t *graph, size_t id1, size_t id2);

void graph_blink_by_id(graph_t *graph, size_t id1, size_t id2);

void graph_remove_node(graph_t *graph, graph_node_t *gnode);

void graph_remove_node_by_id(graph_t *graph, size_t id);

graph_node_t *graph_get_node(graph_t *graph, size_t id);

void graph_print_ids(graph_t *graph);

#endif