#ifndef SOCIAL_MEDIA_H
#define SOCIAL_MEDIA_H

#include "graph.h"

#define USERS_BUCKETS 50
#define POSTS_BUCKETS 50

#define USERNAME_LENGTH 100
#define POST_TITLE_LENGTH 281

typedef struct app_wrapper_t {
	graph_t *users;
	map_t *user_ids;
	graph_t *posts;
	map_t *post_ids;
	size_t post_id_counter;
} app_wrapper_t;

typedef struct user_t {
	char *username;
	graph_node_t gnode;
} user_t;

typedef struct post_t {
	size_t id;
	size_t post_level;
	char *title;
	size_t user_id;
	uint like_count;
	map_t *likes; // Seems kinda overkill for a job fit for a bitmap.
	graph_node_t gnode; // This is the 'events' member.
} post_t;

/**
 * app_create() - Creates an app wrapper (for the social media app).
 * @return - Pointer to the new app.
 */
app_wrapper_t *app_create(void);

/**
 * app_destroy() - Destroys an app wrapper.
 * @param app: Pointer to the app to be destroyed.
 */
void app_destroy(app_wrapper_t *app);

/**
 * app_add_user() - Adds a new user to the app.
 * @param app: Pointer to the app.
 * @param username: Name of the new user.
 */
void app_add_user(app_wrapper_t *app, char *username);

/**
 * app_create_post() - Creates a new post.
 * @param app: Pointer to the app where the post will be created.
 * @param title: Title of the post.
 * @param user_id: ID of the user who made the post / repost.
 * 
 * @return - ID of the new post (NOT the id of the graph node).
 */
size_t app_create_post(app_wrapper_t *app, char *title, size_t user_id);

/**
 * app_get_user_graph_id() - Get the id of the user that has 'username'.
 * 							 This id is of the graph node that stores the user.
 * @param app: App where the user is registered in.
 * @param username: Username of the user.
 * 
 * @return - ID of the user if it exists.
 * 			 Biggest number the CPU can handle if it doesn't exist.
 * 			 (64 bit for normal computers).
 */
size_t app_get_user_graph_id(app_wrapper_t *app, char *username);

/**
 * app_get_post_graph_id() - Get the id of the provided post.
 * 							 This id is of the graph node that stores the post.
 * @param app: App where the post was created.
 * @param username: Username of the user.
 * 
 * @return - ID of the post if it exists.
 * 			 Biggest number the CPU can handle if it doesn't exist.
 * 			 (64 bit for normal computers).
 */
size_t app_get_post_graph_id(app_wrapper_t *app, size_t id);

#endif