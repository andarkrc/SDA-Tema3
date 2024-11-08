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
// Damn, my implementation uses some memory. Maybe it's not that much.
// For 500 posts it uses  ~1.3 MB of ram. Idk if it should be concerning.

static void _create(app_wrapper_t *app);

static void _repost(app_wrapper_t *app);

static void _common_repost(app_wrapper_t *app);

static void _like(app_wrapper_t *app);

static void _get_likes(app_wrapper_t *app);

static void _delete_post(app_wrapper_t *app);

static void _get_reposts(app_wrapper_t *app);

static void _ratio(app_wrapper_t *app);

void handle_input_posts(char *input, app_wrapper_t *app)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd) {
		free(commands);
		return;
	}

	// Using some strtok magic, but we can always change it
	// so we give the command as parameter.
	if (!strcmp(cmd, "create"))
		_create(app);
	else if (!strcmp(cmd, "repost"))
		_repost(app);
	else if (!strcmp(cmd, "common-repost"))
		_common_repost(app);
	else if (!strcmp(cmd, "like"))
		_like(app);
	else if (!strcmp(cmd, "ratio"))
		_ratio(app);
	else if (!strcmp(cmd, "delete"))
		_delete_post(app);
	else if (!strcmp(cmd, "get-likes"))
		_get_likes(app);
	else if (!strcmp(cmd, "get-reposts"))
		_get_reposts(app);

	free(commands);
}

static void _create(app_wrapper_t *app)
{
	char *username = strtok(NULL, "\n ");
	char *title = strtok(NULL, "\n");
	app_add_user(app, username);
	size_t uid = app_get_user_graph_id(app, username);
	app_create_post(app, title, uid);
	printf("Created %s for %s\n", title, username);
}

static void _repost(app_wrapper_t *app)
{
	char *username = strtok(NULL, "\n ");
	char *post_id_str = strtok(NULL, "\n ");
	char *repost_id_str = strtok(NULL, "\n");
	size_t id;
	id = strtosizet(post_id_str);
	if (repost_id_str) {
		// Repost of repost
		id = strtosizet(repost_id_str);
	}

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
	printf("Created repost #%lu for %s\n", repost_id, username);
}

static void _common_repost(app_wrapper_t *app)
{
	char *post_id = strtok(NULL, "\n ");
	post_id = post_id;
	char *repost_id1 = strtok(NULL, "\n ");
	char *repost_id2 = strtok(NULL, "\n");
	size_t id1 = strtosizet(repost_id1);
	size_t id2 = strtosizet(repost_id2);

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
		if (!parent1) {
			post1 = NULL;
			post2 = NULL;
			break;
		}
		post1 = STRUCT_FROM_MEMBER(post_t, parent1, gnode);
		post2 = STRUCT_FROM_MEMBER(post_t, parent2, gnode);
	} while (post1 != post2);
	if (!post1) {
		// The reposts don't have a common repost.
		printf("I don't know why you got here. Good luck\n");
		return;
	}
	printf("The first common repost of %lu and %lu is %lu\n",
		   id1, id2, post1->id);
}

static void _like(app_wrapper_t *app)
{
	char *username = strtok(NULL, "\n ");
	char *post_id = strtok(NULL, "\n ");
	char *repost_id = strtok(NULL, "\n");
	size_t id = strtosizet(post_id);
	if (repost_id)
		id = strtosizet(repost_id);

	app_add_user(app, username);
	size_t graph_post_id = app_get_post_graph_id(app, id);
	graph_node_t *gnode = graph_get_node(app->posts, graph_post_id);
	post_t *post = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
	size_t uid = app_get_user_graph_id(app, username);
	char exists = 69;
	if (map_has_key(post->likes, &uid)) {
		post->like_count--;
		map_remove(post->likes, &uid);

		if (!repost_id)
			printf("User %s unliked post %s\n", username, post->title);
		else
			printf("User %s unliked repost %s\n", username, post->title);

		return;
	}
	post->like_count++;
	map_add(post->likes, &uid, &exists);

	if (!repost_id)
		printf("User %s liked post %s\n", username, post->title);
	else
		printf("User %s liked repost %s\n", username, post->title);
}

static void _get_likes(app_wrapper_t *app)
{
	char *post_id = strtok(NULL, "\n ");
	char *repost_id = strtok(NULL, "\n");
	size_t id = strtosizet(post_id);
	if (repost_id)
		id = strtosizet(repost_id);

	size_t graph_post_id = app_get_post_graph_id(app, id);
	graph_node_t *gnode = graph_get_node(app->posts, graph_post_id);
	post_t *post = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
	if (post->post_level == 0)
		printf("Post %s has %u likes\n", post->title, post->like_count);
	else
		printf("Repost #%lu has %u likes\n", post->id, post->like_count);
}

static void _delete_post_rec(graph_t *graph, graph_node_t *gnode)
{
	linked_list_t *children = gnode->out_links;
	list_node_t *current = children->head;
	list_node_t *backup;
	while (current) {
		backup = current->next;
		graph_link_t *child = STRUCT_FROM_MEMBER(graph_link_t, current, node);
		_delete_post_rec(graph, child->link);
		current = backup;
	}
	graph_remove_node(graph, gnode);
}

static void _delete_post(app_wrapper_t *app)
{
	char *post_id = strtok(NULL, "\n ");
	char *repost_id = strtok(NULL, "\n");
	size_t id = strtosizet(post_id);
	if (repost_id)
		id = strtosizet(repost_id);

	size_t graph_post_id = app_get_post_graph_id(app, id);
	graph_node_t *gnode = graph_get_node(app->posts, graph_post_id);
	post_t *post = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
	char *title = malloc(POST_TITLE_LENGTH);
	DIE(!title, "Malloc failed\n");
	strcpy(title, post->title);
	_delete_post_rec(app->posts, gnode);
	if (repost_id) {
		printf("Deleted repost #%lu of post %s\n", id, title);
		free(title);
		return;
	}
	printf("Deleted %s\n", title);
	free(title);
}

static void _print_repost(graph_node_t *gnode, app_wrapper_t *app)
{
	post_t *post = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
	size_t uid = post->user_id;
	gnode = graph_get_node(app->users, uid);
	user_t *user = STRUCT_FROM_MEMBER(user_t, gnode, gnode);
	if (post->post_level == 0)
		printf("%s - Post by %s\n", post->title, user->username);
	else
		printf("Repost #%lu by %s\n", post->id, user->username);
}

static void _get_reposts_rec(graph_node_t *gnode, app_wrapper_t *app)
{
	linked_list_t *children = gnode->out_links;
	list_node_t *current = children->head;
	_print_repost(gnode, app);
	while (current) {
		graph_link_t *child = STRUCT_FROM_MEMBER(graph_link_t, current, node);
		_get_reposts_rec(child->link, app);
		current = current->next;
	}
}

static void _get_reposts(app_wrapper_t *app)
{
	char *post_id = strtok(NULL, "\n ");
	char *repost_id = strtok(NULL, "\n");
	size_t id = strtosizet(post_id);
	if (repost_id)
		id = strtosizet(repost_id);

	size_t graph_post_id = app_get_post_graph_id(app, id);
	graph_node_t *gnode = graph_get_node(app->posts, graph_post_id);

	_get_reposts_rec(gnode, app);
}

static void _ratio(app_wrapper_t *app)
{
	char *post_id = strtok(NULL, "\n");
	size_t id = strtosizet(post_id);
	size_t graph_post_id = app_get_post_graph_id(app, id);
	graph_node_t *gnode = graph_get_node(app->posts, graph_post_id);
	post_t *post = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
	post_t *ratio = post;
	list_node_t *current = gnode->out_links->head;
	graph_link_t *link;
	while (current) {
		link = STRUCT_FROM_MEMBER(graph_link_t, current, node);
		post_t *repost = STRUCT_FROM_MEMBER(post_t, link->link, gnode);
		if (repost->like_count > ratio->like_count)
			ratio = repost;

		current = current->next;
	}
	if (ratio == post)
		printf("The original post is the highest rated\n");
	else
		printf("Post %lu got ratio'd by repost %lu\n", post->id, ratio->id);
}
