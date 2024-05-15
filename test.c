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

void graph_print_data(graph_t *graph)
{
	test_t *test;
	for (size_t i = 0; i < graph->nodes->size; i++) {
		graph_node_t *gnode;
		gnode = *(graph_node_t **)map_get_value(graph->highway, &i);
		test = STRUCT_FROM_MEMBER(test_t, gnode, gnode);
		printf("%u: ", test->data);
		
		list_node_t *current;
		current = gnode->out_links->head;
		while (current != NULL) {
			graph_link_t *link = STRUCT_FROM_MEMBER(graph_link_t, current, node);
			test = STRUCT_FROM_MEMBER(test_t, link->link, gnode);
			printf("%u, ", test->data);
			current = current->next;
		}
		printf("\n");
	}
	printf("\n");
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
	graph_print_data(graph);
	graph_remove_node_by_id(graph, 3);

	graph_print_data(graph);
	graph_destroy(graph);
	return 0;
}