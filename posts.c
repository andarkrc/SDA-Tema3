#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "posts.h"
#include "users.h"

static size_t create_post(app_wrapper_t *app, char *title, size_t user_id)
{
	post_t *post = malloc(sizeof(*post));
	DIE(!post, "Malloc failed!\n");
	post->id = app->post_id_counter;
	post->post_level = 0;
	post->title = malloc(POST_TITLE_LENGTH);
	DIE(!post->title, "Malloc failed!\n");
	if (title != NULL) {
		strcpy(post->title, title);
	}
	post->user_id = user_id;
	post->gnode = graph_node_create();
	size_t new_id = graph_add_node(app->posts, &post->gnode);
	map_add(app->post_ids, &post->id, &new_id);
	post->like_count = 0;
	post->likes = map_create(USERS_BUCKETS, sizeof(size_t), sizeof(char),
							 simple_entry_destroy, hash_size_t, sizetcmp);
	app->post_id_counter++;
	return post->id;
}

static void _create(app_wrapper_t* app, char *username, char *title)
{
	user_add(app, username);
	size_t uid = *(size_t *)map_get_value(app->user_ids, username);
	create_post(app, title, uid);
	printf("Created %s for %s\n", title, username);
}

static void _repost(app_wrapper_t *app, char *username, size_t id)
{
	user_add(app, username);
	size_t uid = *(size_t *)map_get_value(app->user_ids, username);
	size_t post_id = *(size_t *)map_get_value(app->post_ids, &id);

	graph_node_t *gnode = graph_get_node(app->posts, post_id);
	post_t *post = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
	
	size_t new_id = create_post(app, post->title, uid);
	size_t repost_id = *(size_t *)map_get_value(app->post_ids, &new_id);

	gnode = graph_get_node(app->posts, repost_id);
	post_t *repost = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
	repost->post_level = post->post_level + 1;

	graph_link_by_id(app->posts, post_id, repost_id);
	printf("Created Repost #%lu for %s\n", new_id, username);
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
	else if (!strcmp(cmd, "repost")){
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
	else if (!strcmp(cmd, "common-repost"))
		(void)cmd;
		// TODO: Add function
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

