#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "posts.h"
#include "users.h"

// I am sorry, but you will see a lot of 'id' variables.
// The ID system of the posts is different than the graph's.

// I will also go by the assumption that every post referenced
// exists in the app already (when it's referenced at least).
// The same for users.
// I won't check for (size_t)(-1).

static void _create(app_wrapper_t* app, char *username, char *title)
{
	app_add_user(app, username);
	size_t uid = *(size_t *)map_get_value(app->user_ids, username);
	app_create_post(app, title, uid);
	printf("Created %s for %s\n", title, username);
}

static void _repost(app_wrapper_t *app, char *username, size_t id)
{
	app_add_user(app, username);
	size_t uid = app_get_user_graph_id(app, username);
	// Get the id of the post in the graph.
	size_t graph_post_id = app_get_post_graph_id(app, id);

	graph_node_t *gnode = graph_get_node(app->posts, graph_post_id);
	post_t *post = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
	
	size_t repost_id = app_create_post(app, post->title, uid);
	// Get the id of the repost in the graph.
	size_t graph_repost_id = app_get_post_graph_id(app, repost_id);

	gnode = graph_get_node(app->posts, graph_repost_id);
	post_t *repost = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
	repost->post_level = post->post_level + 1;

	graph_link_by_id(app->posts, graph_post_id, graph_repost_id);
	printf("Created Repost #%lu for %s\n", repost_id, username);
}

static void _common_repost(app_wrapper_t *app, size_t id1, size_t id2)
{
	size_t graph_id1 = app_get_post_graph_id(app, id1);
	size_t graph_id2 = app_get_post_graph_id(app, id2);
	graph_node_t *gnode;

	gnode = graph_get_node(app->posts, graph_id1);
	post_t *post1 = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
	gnode = graph_get_node(app->posts, graph_id2);
	post_t *post2 = STRUCT_FROM_MEMBER(post_t, gnode, gnode);

	// Get the 2 reposts on the same 'level'.
	while (post1->post_level < post2->post_level) {
		gnode = graph_node_get_first_inlink(&post2->gnode);
		post2 = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
	}
	while (post2->post_level < post1->post_level) {
		gnode = graph_node_get_first_inlink(&post1->gnode);
		post1 = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
	}

	if (post1 == post2) {
		// One of the reposts is the original post.
		printf("The first common repost of %lu and %lu is %lu\n",
			   id1, id2, post1->id);
		return;
	}

	graph_node_t *parent1;
	graph_node_t *parent2;
	do {
		parent1 = graph_node_get_first_inlink(&post1->gnode);
		parent2 = graph_node_get_first_inlink(&post2->gnode);
		if (parent1 == NULL) {
			post1 = NULL;
			post2 = NULL;
			break;
		}
		post1 = STRUCT_FROM_MEMBER(post_t, parent1, gnode);
		post2 = STRUCT_FROM_MEMBER(post_t, parent2, gnode);
	} while (post1 != post2);
	if (post1 == NULL) {
		// The reposts don't have a common repost.
		printf("I don't know why you got here. Good luck\n");
		return;
	}
	printf("The first common repost of %lu and %lu is %lu\n",
		   id1, id2, post1->id);
}

void handle_input_posts(char *input, app_wrapper_t *app)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd) {
		free(commands);
		return;
	}

	if (!strcmp(cmd, "create")) {
		char *username = strtok(NULL, "\n ");
		char *title = strtok(NULL, "\n");
		_create(app, username, title);
	}
	else if (!strcmp(cmd, "repost")) {
		char *username = strtok(NULL, "\n ");
		char *post_id_str = strtok(NULL, "\n ");
		char *repost_id_str = strtok(NULL, "\n");
		size_t id;
		id = (size_t)atoi(post_id_str);
		if (repost_id_str != NULL) {
			// Repost of repost
			id = (size_t)atoi(repost_id_str);
		}
		_repost(app, username, id);
	}
	else if (!strcmp(cmd, "common-repost")) {
		char *post_id = strtok(NULL, "\n ");
		post_id = post_id;
		char *repost_id1 = strtok(NULL, "\n ");
		char *repost_id2 = strtok(NULL, "\n");
		size_t id1 = (size_t)atoi(repost_id1);
		size_t id2 = (size_t)atoi(repost_id2);
		_common_repost(app, id1, id2);
	}
	else if (!strcmp(cmd, "like"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "ratio"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "delete"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "get-likes"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "get-reposts"))
		(void)cmd;
		// TODO: Add function
	
	free(commands);
}

