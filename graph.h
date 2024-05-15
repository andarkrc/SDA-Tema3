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

// CHECK README FOR AN EXAMPLE OF HOW TO USE THE GRAPH.

/**
 * graph_create() - Creates a new graph.
 * @param graph_node_des: Destructor function that should destroy the 
 * 						  whole structure in which a graph node resides. 
 * 
 * @return - Pointer to the new graph.
 */
graph_t *graph_create(void (*graph_node_des)(list_node_t *));

/*
	typedef struct example_t {
		int some_data;
		char *some_string;
		...

		graph_node_t gnode; // The graph node is a staticly declared member.
	} example_t;

	static void example_destructor(list_node_t *node)
	{
		// Also check the declaration of STRUCT_FROM_MEMBER to see how it works.
		// Your data struct should have a graph node encapsulated in it.
		// (and the graph node has a linked list node encapsulated in it).
		// The only real thing you should worry about is a way to keep track
		// of what data is placed around what graph node.

		graph_node_t *gnode = STRUCT_FROM_MEMBER(graph_node_t, node, node);
		example_t *example = STRUCT_FROM_MEMBER(example_t, gnode, gnode);
		
		// Free the allocated memory in your struct.
		free(example->some_string);
		...
		

		graph_node_destroy(example->gnode); // Destroy the graph node.

		free(example); // Free the memory allocated for your struct.
	}

	graph_t *my_graph = graph_create(example_destructor);

	// The graph nodes provide a basic ID system: the highway.
	// If you want to use another one, feel free.
	// Just don't overwrite the graph's system.

	// Allocate your struct.
	example_t *new_example = malloc(sizeof(example_t)); // Implicit cast ik.
	new_example->some_data = 56;
	new_example->some_string = another_string;
	...
	
	// Now, add the new node to the graph.
	new_example->gnode = graph_node_create();
	size_t new_id = graph_add_node(my_graph, &new_example->gnode);

	// You can also keep track of where you data is in the graph.
	// But you have to do this on your own ;).
	keep_track_of_the_data(data_tracker, new_example, new_id);

	...
	// Free the graph when you don't need it anymore.
	graph_destroy(my_graph);
*/

/**
 * graph_destroy() - Destroys a graph (frees the memory)
 * @param graph: Pointer to the graph to be destroyed.
 */
void graph_destroy(graph_t *graph);

/**
 * graph_node_create() - Creates (initializes) a new 'Graph Node'.
 * 
 * @return - The newly created node (the entire struct, NOT A POINTER).
 */
graph_node_t graph_node_create(void);

/**
 * graph_node_destroy() - Destroys (frees the allocated memory for) agraph node.
 * @param gnode: 'Graph Node' to be destroyed(the entire struct, NOT A POINTER). 
 */
void graph_node_destroy(graph_node_t gnode);

/**
 * graph_add_node() - Adds a previously created 'Graph Node' to the graph.
 * @param graph: Pointer to the graph where the node will be added to.
 * @param gnode: POINTER to the node to be added.
 * 
 * @return - ID of the node in the graph.
 */
size_t graph_add_node(graph_t *graph, graph_node_t *gnode);

/**
 * graph_link_nodes() - Function that adds a link from node 1 to node 2.
 * @param graph: Pointer to the graph.
 * @param gnode1: POINTER to the 'source' node: source--->destination.
 * @param gnode2: POINTER to the 'destination' node.
 */
void graph_link_nodes(graph_t *graph, graph_node_t *gnode1, graph_node_t *gnode2);

/**
 * graph_link_by_id() - Function that adds a link from node with id1 to
 * 					    node with id2.
 * @param graph: Pointer to the graph.
 * @param id1: ID of the 'source' node.
 * @param id2: ID of the 'destination' node.
 */
void graph_link_by_id(graph_t *graph, size_t id1, size_t id2);

/**
 * graph_blink_nodes() - Same as graph_link_nodes() but the link is both ways.
 * @param graph: Pointer to the graph.
 * @param gnode1: POINTER to the first graph node.
 * @param gnode2: POINTER to the second graph node.
 */
void graph_blink_nodes(graph_t *graph, graph_node_t *gnode1, graph_node_t *gnode2);

/**
 * graph_blink_by_id() - Same as graph_link_by_id() but the link is both ways.
 * @param graph: Pointer to the graph.
 * @param id1: ID of the first graph node.
 * @param id2: ID of the second graph node.
 */
void graph_blink_by_id(graph_t *graph, size_t id1, size_t id2);

/**
 * graph_remove_node() - Completely remove a node from a graph.
 * @param graph: Pointer to the graph.
 * @param gnode: POINTER to the graph node to be removed.
 */
void graph_remove_node(graph_t *graph, graph_node_t *gnode);

/**
 * graph_remove_node_by_id() - Completely remove the node with the specified
 * 							   ID from a graph.
 * @param graph: Pointer to the graph.
 * @param id: ID of the graph node to be removed.
 */
void graph_remove_node_by_id(graph_t *graph, size_t id);

/**
 * graph_get_node() - Find the graph node with the specified ID in a graph.
 * @param graph: Pointer to the graph.
 * @param id: ID of the graph node to be found.
 * 
 * @return - NULL if there is no node with the specifed ID in the graph.
 * 			 POINTER to the desired node if it exists in the graph.
 */
graph_node_t *graph_get_node(graph_t *graph, size_t id);

/**
 * graph_print_ids() - Prints the graph (using the node's id).
 * @param graph: Pointer to the graph to be printed.
 */
void graph_print_ids(graph_t *graph);

#endif