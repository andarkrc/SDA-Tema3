#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "posts.h"
#include "users.h"

void handle_input_posts(char *input, app_wrapper_t *app)
{
	app = app;
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "create"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "repost"))
		(void)cmd;
		// TODO: Add function
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
	else if (!strcmp(cmd, "get-likes"))
		(void)cmd;
		// TODO: Add function
}

