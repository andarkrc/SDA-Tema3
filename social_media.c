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


{ // Region for app wrapper

app_wrapper_t *app_create(void)
{
	// Needs proper implementation
	app_wrapper_t *app = malloc(sizeof(*app));
	app->users = graph_create(user_destructor);
	app->posts = graph_create(post_destructor);
	app->user_ids = map_create(100, USERNAME_LENGTH, sizeof(size_t), 
							   simple_entry_destroy, hash_string, strkeycmp);
	return app;
}

void user_destructor(list_node_t *node)
{
	graph_node_t *gnode = STRUCT_FROM_MEMBER(graph_node_t, node, node);
	user_t *user = STRUCT_FROM_MEMBER(user_t, gnode, gnode);
	free(user->username);
	free(user);
}

void post_destructor(list_node_t *node)
{
	graph_node_t *gnode = STRUCT_FROM_MEMBER(graph_node_t, node, node);
	post_t *post = STRUCT_FROM_MEMBER(post_t, gnode, gnode);

	// Free post data here.

	free(post);
}

void app_destroy(app_wrapper_t *app)
{
	graph_destroy(app->users);
	graph_destroy(app->posts);
	map_destroy(app->user_ids);
	free(app);
}

}

/**
 * Initializez every task based on which task we are running
*/
void init_tasks(void)
{
	#ifdef TASK_1

	#endif

	#ifdef TASK_2

	#endif

	#ifdef TASK_3

	#endif
}

/**
 * Entrypoint of the program, compiled with different defines for each task
*/
int main(void)
{
	init_users();

	init_tasks();

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
