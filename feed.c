#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "feed.h"
#include "users.h"
#include "friends.h"

static void _feed(app_wrapper_t *app);

static void _view_profile(app_wrapper_t *app);

static void _friends_repost(app_wrapper_t *app);

static void _clique(app_wrapper_t *app);

static void _swap_uint16(uint16_t *a, uint16_t *b);

static void _bubble_sort(uint16_t *array, int array_size);

static void _print_friends(uint16_t *friend_ids, int friend_count);

void handle_input_feed(char *input, app_wrapper_t *app)
{
	app = app;
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd){
		free(commands);
		return;
	}

	if (!strcmp(cmd, "feed")) {
		_feed(app);
	}
	else if (!strcmp(cmd, "view-profile")) {
		_view_profile(app);
	}
	else if (!strcmp(cmd, "friends-repost")) {
		_friends_repost(app);
	}
	else if (!strcmp(cmd, "common-group")) {
		_clique(app);
	}

	free(commands);
}

// Graph out_links to set
static set_t *golinks_to_set(graph_node_t *gnode)
{
	set_t *set = set_create(simple_set_destructor, ptrcmp, sizeof(void *));
	list_node_t *current = gnode->out_links->head;
	while (current != NULL) {
		graph_link_t *link = STRUCT_FROM_MEMBER(graph_link_t, current, node);
		set_add(set, &link->link);
		current = current->next;
	}
	return set;
}

static void _clique_rec(set_t *r, set_t *p, set_t *x, set_t **mx)
{
	if (set_empty(p) && set_empty(x)) {
		set_destroy(p);
		set_destroy(x);
		if (r->list->size > (*mx)->list->size) {
			set_destroy(*mx);
			*mx = r;
		} else {
			set_destroy(r);
		}
		return;
	}

	list_node_t *current = p->list->head;
	while (current != NULL) {
		list_node_t *backup = current->next;
		set_element_t *elem;
		elem = STRUCT_FROM_MEMBER(set_element_t, current, node);
		set_t *nv = golinks_to_set(*(graph_node_t**)elem->data);
		set_t *temp = set_create(simple_set_destructor, ptrcmp, sizeof(void *));
		set_add(temp, elem->data);
		_clique_rec(set_union(r, temp), set_intersect(p, nv), set_intersect(x, nv), mx);

		set_destroy(nv);
		set_destroy(temp);
		set_add(x, elem->data);
		set_remove(p, elem->data);
		current = backup;
	}
	set_destroy(p);
	set_destroy(x);
	set_destroy(r);
}

static void _clique(app_wrapper_t *app)
{
	char *username = strtok(NULL, "\n");
	app_add_user(app, username);
	size_t uid = app_get_user_graph_id(app, username);
	graph_node_t *gnode = graph_get_node(app->users, uid);
	set_t *r = set_create(simple_set_destructor, ptrcmp, sizeof(void *));
	
	set_t *p = golinks_to_set(gnode);
	set_add(p, &gnode);
	set_t *x = set_create(simple_set_destructor, ptrcmp, sizeof(void *));

	set_t *mx = set_create(simple_set_destructor, ptrcmp, sizeof(void *));
	_clique_rec(r, p, x, &mx);

	uint16_t *friend_ids = malloc(sizeof(*friend_ids) * MAX_PEOPLE);
	DIE(!friend_ids, "Malloc failed\n");
	int idx = 0;

	list_node_t *current = mx->list->head;
	while (current != NULL) {
		set_element_t *elem;
		elem = STRUCT_FROM_MEMBER(set_element_t, current, node);
		gnode = *(graph_node_t**)elem->data;
		user_t *user = STRUCT_FROM_MEMBER(user_t, gnode, gnode);
		friend_ids[idx++] = get_user_id(user->username);
		current = current->next;
	}
	set_destroy(mx);
	_bubble_sort(friend_ids, idx);
	printf("The closest friend group of %s is:\n", username);
	_print_friends(friend_ids, idx);
	free(friend_ids);
}

static void _friends_repost_rec(graph_node_t *gnode, app_wrapper_t *app, size_t uid)
{
	linked_list_t *children = gnode->out_links;
	list_node_t *current = children->head;

	post_t *post = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
	graph_node_t *user_gnode;
	user_gnode = graph_get_node(app->users, post->user_id);
	user_t *user = STRUCT_FROM_MEMBER(user_t, user_gnode, gnode);
	if (graph_has_link_by_id(app->users, uid, post->user_id)) {
		if (post->post_level != 0) {
			printf("%s\n", user->username);
		}
	}

	while (current != NULL) {
		graph_link_t *child = STRUCT_FROM_MEMBER(graph_link_t, current, node);
		_friends_repost_rec(child->link, app, uid);
		current = current->next;
	}
}

static void _friends_repost(app_wrapper_t *app)
{
	char *username = strtok(NULL, "\n ");
	size_t uid = app_get_user_graph_id(app, username);
	char *post_id = strtok(NULL, "\n");
	size_t id = strtosizet(post_id);
	size_t graph_post_id = app_get_post_graph_id(app, id);

	graph_node_t *gnode = graph_get_node(app->posts, graph_post_id);
	_friends_repost_rec(gnode, app, uid);
}

static void _view_profile(app_wrapper_t *app)
{
	char *username = strtok(NULL, "\n");
	size_t uid = app_get_user_graph_id(app, username);
	list_node_t *current = app->posts->nodes->head;
	while (current != NULL) {
		graph_node_t *gnode = STRUCT_FROM_MEMBER(graph_node_t, current, node);
		post_t *post = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
		if (post->post_level == 0 && post->user_id == uid) {
			printf("Posted: %s\n", post->title);
		}
		current = current->next;
	}
	current = app->posts->nodes->head;
	while (current != NULL) {
		graph_node_t *gnode = STRUCT_FROM_MEMBER(graph_node_t, current, node);
		post_t *post = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
		if (post->post_level != 0 && post->user_id == uid) {
			printf("Reposted: %s\n", post->title);
		}
		current = current->next;
	}
}

static void _feed(app_wrapper_t *app)
{
	char *username = strtok(NULL, "\n ");
	char *feed_size = strtok(NULL, "\n");
	size_t post_num = strtosizet(feed_size);
	size_t uid = app_get_user_graph_id(app, username);
	size_t post_found = 0;
	list_node_t *current = app->posts->nodes->tail;
	while (current && post_found != post_num) {
		graph_node_t *gnode = STRUCT_FROM_MEMBER(graph_node_t, current, node);
		post_t *post = STRUCT_FROM_MEMBER(post_t, gnode, gnode);
		if (post->post_level == 0) {
			gnode = graph_get_node(app->users, post->user_id);
			user_t *user = STRUCT_FROM_MEMBER(user_t, gnode, gnode);
			if (graph_has_link_by_id(app->users, uid, gnode->id) || gnode->id == uid) {
				post_found++;
				printf("%s: %s\n", user->username, post->title);
			}
		}
		current = current->prev;
	}
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
		for (int i = 0; i < array_size - 1; i++){
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
	if (friend_count != 0) {
		printf("%s\n", get_user_name(friend_ids[0]));
	}
	for (int i = 1; i < friend_count; i++) {
		if (friend_ids[i - 1] != friend_ids[i]) {
			printf("%s\n", get_user_name(friend_ids[i]));
		}
	}
}