/**
 * The entrypoint of the homework. Every initialization must be done here
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "social_media.h"
#include "posts.h"
#include "feed.h"
#include "friends.h"
#include "users.h"


/**
 * Entrypoint of the program, compiled with different defines for each task
*/
int main(void)
{
	// I don't even think I need this.
	// I just went off and created something else on my own.
	init_users();

	app_wrapper_t *app = app_create();

	char *input = (char *)malloc(MAX_COMMAND_LEN);
	while (1) {
		char *got_input = fgets(input, MAX_COMMAND_LEN, stdin);

		// If fgets returns null, we reached EOF
		if (!got_input)
			break;

		#ifdef TASK_1
		handle_input_friends(input, app);
		#endif

		#ifdef TASK_2
		handle_input_posts(input, app);
		#endif

		#ifdef TASK_3
		handle_input_feed(input, app);
		#endif
	}

	free_users();
	app_destroy(app);
	free(input);

	return 0;
}

// Region for app wrapper

static void user_destructor(list_node_t *node)
{
	graph_node_t *gnode = STRUCT_FROM_MEMBER(graph_node_t, node, node);
	user_t *user = STRUCT_FROM_MEMBER(user_t, gnode, gnode);
	free(user->username);

	graph_node_destroy(user->gnode);
	free(user);
}

static void post_destructor(list_node_t *node)
{
	graph_node_t *gnode = STRUCT_FROM_MEMBER(graph_node_t, node, node);
	post_t *post = STRUCT_FROM_MEMBER(post_t, gnode, gnode);

	if (post->title != NULL) {
		free(post->title);
	}

	graph_node_destroy(post->gnode);
	map_destroy(post->likes);
	free(post);
}

app_wrapper_t *app_create(void)
{
	// Needs proper implementation
	app_wrapper_t *app = malloc(sizeof(*app));
	DIE(!app, "Malloc failed!\n");
	app->users = graph_create(user_destructor);
	app->posts = graph_create(post_destructor);
	app->user_ids = map_create(USERS_BUCKETS, USERNAME_LENGTH, sizeof(size_t), 
							   simple_entry_destroy, hash_string, strkeycmp);
	app->post_ids = map_create(POSTS_BUCKETS, sizeof(size_t), sizeof(size_t),
							   simple_entry_destroy, hash_size_t, sizetcmp);
	app->post_id_counter = 1;
	return app;
}

void app_destroy(app_wrapper_t *app)
{
	graph_destroy(app->users);
	graph_destroy(app->posts);
	map_destroy(app->user_ids);
	map_destroy(app->post_ids);
	free(app);
}

void app_add_user(app_wrapper_t *app, char *username)
{
	if (map_has_key(app->user_ids, username)) {
		return;
	}
	user_t *user = malloc(sizeof(*user));
	DIE(!user, "Malloc failed!\n");
	user->username = malloc(USERNAME_LENGTH);
	DIE(!user->username, "Malloc failed!\n");
	strcpy(user->username, username);
	user->gnode = graph_node_create();
	size_t new_id = graph_add_node(app->users, &user->gnode);
	map_add(app->user_ids, user->username, &new_id);
}

size_t app_get_user_graph_id(app_wrapper_t *app, char *username)
{
	void *graph_id = map_get_value(app->user_ids, username);
	return (graph_id == NULL) ? (size_t)(-1) : *(size_t *)graph_id;
}

size_t app_create_post(app_wrapper_t *app, char *title, size_t user_id)
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

size_t app_get_post_graph_id(app_wrapper_t *app, size_t id)
{
	void *graph_id = map_get_value(app->post_ids, &id);
	return (graph_id == NULL) ? (size_t)(-1) : *(size_t *)graph_id;
}

// End of region