#ifndef SOCIAL_MEDIA_H
#define SOCIAL_MEDIA_H

#include "graph.h"

#define USERNAME_LENGTH 100

typedef struct app_wrapper_t {
	graph_t *users;
	map_t *user_ids;
	graph_t *posts;
} app_wrapper_t;

typedef struct user_t {
	char *username;
	graph_node_t gnode;
} user_t;

typedef struct post_t {
	// NEEDS IMPLEMENTATION
	graph_node_t gnode;
} post_t;

app_wrapper_t *app_create(void);

void app_destroy(app_wrapper_t *app);

void user_destructor(list_node_t *node);

void post_destructor(list_node_t *node);

#endif