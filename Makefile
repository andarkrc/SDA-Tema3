CC=gcc
CFLAGS=-Wall -Wextra -Werror -g

.PHONY: build clean

all: build

build: friends posts feed

# Any other object file that is needed for the other files
# add it here, separated by space
UTILS = users.o

friends: $(UTILS) friends.o social_media.c linked_list.c hash_map.c graph.c utils.c set.c
	$(CC) $(CFLAGS) -D TASK_1 -o $@ $^

posts: $(UTILS) posts.o social_media.c linked_list.c hash_map.c graph.c utils.c set.c
	$(CC) $(CFLAGS) -D TASK_2 -o $@ $^
	
feed: $(UTILS) posts.o friends.o feed.o social_media.c linked_list.c hash_map.c graph.c utils.c set.c
	$(CC) $(CFLAGS) -D TASK_3 -o $@ $^

social_media_friends.o:
	$(CC) $(CFLAGS) -c -D TASK_1 -o $@ social_media.c linked_list.c hash_map.c graph.c utils.c 

social_media_posts.o:
	$(CC) $(CFLAGS) -c -D TASK_2 -o $@ social_media.c linked_list.c hash_map.c graph.c utils.c 

social_media_feed.o:
	$(CC) $(CFLAGS) -c -D TASK_1 -D TASK_2 -D TASK_3 -o $@ social_media.c linked_list.c hash_map.c graph.c utils.c 

clean:
	rm -rf *.o friends posts feed