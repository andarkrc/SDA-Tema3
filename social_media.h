#ifndef SOCIAL_MEDIA_H
#define SOCIAL_MEDIA_H

#include "graph.h"

#define USERS_BUCKETS 100
#define POSTS_BUCKETS 200

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
	map_t *likes;
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

#endif