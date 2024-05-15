#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

typedef struct test_t {
	uint data;
	graph_node_t gnode;
} test_t;

static void test_destructor(list_node_t *node)
{
	graph_node_t *gnode;
	gnode = STRUCT_FROM_MEMBER(graph_node_t, node, node);
	test_t *test;
	test = STRUCT_FROM_MEMBER(test_t, gnode, gnode);
	graph_node_destroy(test->gnode);
	free(test);
}

int main(void)
{
	test_t *test;
	graph_t *graph = graph_create(test_destructor);

	test = (test_t *)malloc(sizeof(test_t));
	test->data = 0;
	test->gnode = graph_node_create();
	graph_add_node(graph, &test->gnode);

	test = (test_t *)malloc(sizeof(test_t));
	test->data = 1;
	test->gnode = graph_node_create();
	graph_add_node(graph, &test->gnode);

	test = (test_t *)malloc(sizeof(test_t));
	test->data = 2;
	test->gnode = graph_node_create();
	graph_add_node(graph, &test->gnode);

	test = (test_t *)malloc(sizeof(test_t));
	test->data = 3;
	test->gnode = graph_node_create();
	graph_add_node(graph, &test->gnode);

	graph_link_by_id(graph, 0, 1);
	graph_link_by_id(graph, 0, 2);
	graph_link_by_id(graph, 2, 1);
	graph_blink_by_id(graph, 2, 3);
	graph_link_by_id(graph, 3, 1);
	graph_print_ids(graph);
	graph_remove_node_by_id(graph, 1);
	graph_remove_node_by_id(graph, 2);

	graph_print_ids(graph);

	test = (test_t *)malloc(sizeof(test_t));
	test->data = 1;
	test->gnode = graph_node_create();
	graph_add_node(graph, &test->gnode);
	
	graph_print_ids(graph);

	test = (test_t *)malloc(sizeof(test_t));
	test->data = 2;
	test->gnode = graph_node_create();
	graph_add_node(graph, &test->gnode);
	
	graph_print_ids(graph);

	graph_destroy(graph);
	return 0;
}