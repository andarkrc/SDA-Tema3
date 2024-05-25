#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "friends.h"
#include "users.h"

static void _add_friend(app_wrapper_t *app);

static void _remove_friend(app_wrapper_t *app);

static void _friend_suggestions(app_wrapper_t *app);

static void _user_distance(app_wrapper_t *app);

static void _common_friends(app_wrapper_t *app);

static void _friend_count(app_wrapper_t *app);

static void _most_popular_friend(app_wrapper_t *app);

static void _bubble_sort(uint16_t *array, int array_size);

static void _swap_uint16(uint16_t *a, uint16_t *b);

static void _print_friends(uint16_t *friend_ids, int friend_count);

void handle_input_friends(char *input, app_wrapper_t *app)
{
	app = app;
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd) {
		free(commands);
		return;
	}

	if (!strcmp(cmd, "add"))
		_add_friend(app);
	else if (!strcmp(cmd, "remove"))
		_remove_friend(app);
	else if (!strcmp(cmd, "suggestions"))
		_friend_suggestions(app);
	else if (!strcmp(cmd, "distance"))
		_user_distance(app);
	else if (!strcmp(cmd, "common"))
		_common_friends(app);
	else if (!strcmp(cmd, "friends"))
		_friend_count(app);
	else if (!strcmp(cmd, "popular"))
		_most_popular_friend(app);

	free(commands);
}

struct next_usr_t {
	size_t id;
	list_node_t node;
};

static void next_usr_dest(list_node_t *node)
{
	struct next_usr_t *nu;
	nu = STRUCT_FROM_MEMBER(struct next_usr_t, node, node);
	free(nu);
}

static void _user_distance(app_wrapper_t *app)
{
	char *username1 = strtok(NULL, "\n ");
	char *username2 = strtok(NULL, "\n");

	app_add_user(app, username1);
	app_add_user(app, username2);

	size_t graph_id1 = app_get_user_graph_id(app, username1);
	size_t graph_id2 = app_get_user_graph_id(app, username2);

	map_t *distance = map_create(USERS_BUCKETS, sizeof(size_t), sizeof(size_t),
								 simple_entry_destroy, hash_size_t, sizetcmp);
	size_t dist0 = 0;
	map_add(distance, &graph_id1, &dist0);

	linked_list_t *q = list_create(next_usr_dest);
	struct next_usr_t *first = malloc(sizeof(*first));
	DIE(!first, "Malloc failed\n");
	first->id = graph_id1;
	list_push(q, &first->node);

	list_node_t *q_head;
	struct next_usr_t *next_usr;
	while (!list_empty(q)) {
		if (map_has_key(distance, &graph_id2))
			break;

		q_head = list_remove(q, 0);

		// Go through all nodes of this child
		next_usr = STRUCT_FROM_MEMBER(struct next_usr_t, q_head, node);

		graph_node_t *gnode;
		gnode = graph_get_node(app->users, next_usr->id);
		list_node_t *current = gnode->out_links->head;
		while (current) {
			graph_link_t *link;
			link = STRUCT_FROM_MEMBER(graph_link_t, current, node);
			if (!map_has_key(distance, &link->link->id)) {
				struct next_usr_t *new = malloc(sizeof(*new));
				DIE(!new, "Malloc failed\n");
				new->id = link->link->id;
				size_t new_dist;
				new_dist = *(size_t *)map_get_value(distance, &gnode->id) + 1;
				list_push(q, &new->node);
				map_add(distance, &link->link->id, &new_dist);
				if (new->id == graph_id2)
					break;
			}
			current = current->next;
		}

		q->destructor(q_head);
	}

	if (map_has_key(distance, &graph_id2)) {
		size_t dist = *(size_t *)map_get_value(distance, &graph_id2);
		printf("The distance between %s - %s", username1, username2);
		printf(" is %lu\n", dist);
	} else {
		printf("There is no way to get from %s to %s\n", username1, username2);
	}

	list_destroy(q);
	map_destroy(distance);
}

static void _add_friend(app_wrapper_t *app)
{
	char *user1 = strtok(NULL, "\n ");
	char *user2 = strtok(NULL, "\n");

	app_add_user(app, user1);
	app_add_user(app, user2);

	size_t user1_id = app_get_user_graph_id(app, user1);
	size_t user2_id = app_get_user_graph_id(app, user2);

	graph_blink_by_id(app->users, user1_id, user2_id);

	printf("Added connection %s - %s\n", user1, user2);
}

static void _remove_friend(app_wrapper_t *app)
{
	char *user1 = strtok(NULL, "\n ");
	char *user2 = strtok(NULL, "\n");

	app_add_user(app, user1);
	app_add_user(app, user2);

	size_t user1_id = app_get_user_graph_id(app, user1);
	size_t user2_id = app_get_user_graph_id(app, user2);

	graph_unblink_by_id(app->users, user1_id, user2_id);

	printf("Removed connection %s - %s\n", user1, user2);
}

static void _friend_count(app_wrapper_t *app)
{
	char *username = strtok(NULL, "\n");

	app_add_user(app, username);

	size_t id = app_get_user_graph_id(app, username);

	graph_node_t *gnode = graph_get_node(app->users, id);

	printf("%s has %lu friends\n", username, gnode->out_links->size);
}

static void _common_friends(app_wrapper_t *app)
{
	char *username1 = strtok(NULL, "\n ");
	char *username2 = strtok(NULL, "\n");

	app_add_user(app, username1);
	app_add_user(app, username2);

	size_t id1 = app_get_user_graph_id(app, username1);
	size_t id2 = app_get_user_graph_id(app, username2);

	graph_node_t *user1 = graph_get_node(app->users, id1);
	graph_node_t *user2 = graph_get_node(app->users, id2);

	uint16_t *friend_ids = malloc(sizeof(*friend_ids) * MAX_PEOPLE);
	DIE(!friend_ids, "Malloc failed\n");
	int idx = 0;

	list_node_t *temp_node = user1->out_links->head;
	while (temp_node) {
		graph_link_t *user1_friend;
		user1_friend = STRUCT_FROM_MEMBER(graph_link_t, temp_node, node);
		if (graph_has_link_nodes(app->users, user1_friend->link, user2)) {
			user_t *user;
			user = STRUCT_FROM_MEMBER(user_t, user1_friend->link, gnode);
			friend_ids[idx++] = get_user_id(user->username);
		}
		temp_node = temp_node->next;
	}
	_bubble_sort(friend_ids, idx);

	if (idx) {
		printf("The common friends between %s and %s", username1, username2);
		printf(" are:\n");
		_print_friends(friend_ids, idx);
	} else {
		printf("No common friends for %s and %s\n", username1, username2);
	}

	free(friend_ids);
}

static void _friend_suggestions(app_wrapper_t *app)
{
	char *username = strtok(NULL, "\n ");

	app_add_user(app, username);

	size_t id = app_get_user_graph_id(app, username);

	graph_node_t *gnode = graph_get_node(app->users, id);

	uint16_t *friend_ids = malloc(sizeof(*friend_ids) * MAX_PEOPLE);
	DIE(!friend_ids, "Malloc failed\n");
	int idx = 0;

	list_node_t *temp1 = gnode->out_links->head;
	while (temp1) {
		graph_link_t *friend = STRUCT_FROM_MEMBER(graph_link_t, temp1, node);
		list_node_t *temp2 = friend->link->out_links->head;
		while (temp2) {
			graph_link_t *friends_friend;
			friends_friend = STRUCT_FROM_MEMBER(graph_link_t, temp2, node);
			user_t *user;
			user = STRUCT_FROM_MEMBER(user_t, friends_friend->link, gnode);
			if (!graph_has_link_nodes(app->users, gnode, friends_friend->link))
				if (gnode != friends_friend->link)
					friend_ids[idx++] = get_user_id(user->username);

			temp2 = temp2->next;
		}
		temp1 = temp1->next;
	}

	_bubble_sort(friend_ids, idx);

	if (idx) {
		printf("Suggestions for %s:\n", username);
		_print_friends(friend_ids, idx);
	} else {
		printf("There are no suggestions for %s\n", username);
	}

	free(friend_ids);
}

static void _most_popular_friend(app_wrapper_t *app)
{
	char *username = strtok(NULL, "\n");

	app_add_user(app, username);

	size_t id = app_get_user_graph_id(app, username);
	graph_node_t *gnode = graph_get_node(app->users, id);
	list_node_t *temp_node = gnode->out_links->head;

	char *most_popular = username;
	size_t max_friends = gnode->out_links->size;

	while (temp_node) {
		graph_link_t *friend;
		friend = STRUCT_FROM_MEMBER(graph_link_t, temp_node, node);
		size_t friend_count = friend->link->out_links->size;
		if (friend_count >= max_friends) {
			if (friend_count != gnode->out_links->size) {
				user_t *user = STRUCT_FROM_MEMBER(user_t, friend->link, gnode);
				if (friend_count > max_friends) {
					most_popular = user->username;
				} else {
					uint16_t friend_id = get_user_id(user->username);
					uint16_t popular_id = get_user_id(most_popular);

					if (friend_id < popular_id)
						most_popular = user->username;
				}
			}
			max_friends = friend_count;
		}
		temp_node = temp_node->next;
	}
	if (most_popular == username)
		printf("%s is the most popular\n", username);
	else
		printf("%s is the most popular friend of %s\n", most_popular, username);
}

static void _swap_uint16(uint16_t *a, uint16_t *b)
{
	uint16_t temp = *a;
	*a = *b;
	*b = temp;
}

static void _bubble_sort(uint16_t *array, int array_size)
{
	char sorted = 0;

	while (!sorted) {
		sorted = 1;
		for (int i = 0; i < array_size - 1; i++) {
			if (array[i] > array[i + 1]) {
				_swap_uint16(&array[i], &array[i + 1]);
				sorted = 0;
			}
		}
		array_size--;
	}
}

static void _print_friends(uint16_t *friend_ids, int friend_count)
{
	if (friend_count != 0)
		printf("%s\n", get_user_name(friend_ids[0]));

	for (int i = 1; i < friend_count; i++)
		if (friend_ids[i - 1] != friend_ids[i])
			printf("%s\n", get_user_name(friend_ids[i]));
}
