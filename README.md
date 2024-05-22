#### Echipa Tema 3 SDA:
- andrei.popa0810
- marius.baican

Because there was no need for change, the implementation of the linked list and hashmap are the same
as the one I (Andrei) implemented for Tema 2. The documentation is also the same. I also am the main contributor to the README.

## Fast Access
- [Task 1](#task1)
- [Task 2](#task2)
- [Task 3](#task3)
- [Graphs](#graph)
- [Graph Example](#graph-example)
- [Hashmaps](#hmap)
- [Linked Lists](#llist)

# Social Media: Tema 3

1. ## General Overview

	To implement the social media app, we decided to go with 2 graphs (1 for users and 1 for posts).
	We also needed a way to integrate the ID system used by the checker with the ID system used by the graphs, so
	we just used another 2 hashmaps. there was also a need for an ID counter for the posts, as their ID's shouldn't be
	reused. The final struct looks like this:
	```C
	struct app_wrapper_t {
		// Users
		struct graph_t *users;
		struct map_t *user_ids;
		
		// Posts
		struct graph_t *posts;
		struct map_t *post_ids;
		size_t post_id_counter;
	}
	```
	The implementation will be further explained separately.

1. ## Task 1: Friends Network <a name="task1"></a>

	**WORK IN PROGRESS**

2. ## Task 2: Posts and Reposts <a name="task2"></a>

	1. ### Overview

		A post should:
		
		- Be created with an unique ID;
		- Have a title;
		- Know what user created it;
		- Know how many likes it has;
		- Know what users liked it;
		- Know what is the original post (if it is a repost).

		With all this in mind, this is the final struct for the posts:
		
		```C
		struct post_t {
			size_t id;
			size_t post_level;
			char *title;
			size_t user_id;
			uint like_count;
			struct map_t *likes;
			struct graph_node_t gnode;
		};
		```

		As is mentioned previously, all the posts live inside a big graph. Every connex component of the graph
		is a tree of reposts, whose root is the original post. Between every post and repost (or repost of a repost)
		there is a 'one way link' from post to repost. Also, every time a post / repost is created, it's level on the tree
		is saved, so it is easier to find the common repost of 2 reposts.

	1. ### Create and Repost
		Whenever a post is created, it is added as a new node in the `app->posts` graph. Every time it gets
		a repost, the repost is created as a new 'post', and a link is formed from the original post to the new one.
		The new post will have as `post_level`, the original's `post_level` + 1.

	2. ### Common Repost
		To get the common repost of 2 reposts we need to:

		- Look at both reposts' `post_level`.
		- If they are not equal, we call these 'offseted posts', and we need to 'climb' the one who has
		a bigger `post_level` until we get them to equal 'levels'.
		- If they end up being the same post, it means that post is the 'common ancestor'.
		- Once we get both posts at the same level, we 'climb' both of them, until they both have the same parent.
		- That parent should be the 'common ancestor' of both reposts.

	3. ### Like & Get Likes

		Whenever a user likes a post, we verify if they liked it previously (if the `likes` hashmap already has an
		entry for that user). If they **didn't** like it previously, then we **add** 1 to the post's `like_count`. If they **did** actually
		liked it, then that is an 'unlike', and we need to **subtract** 1 from the post's `like_count`.<br>
		To display the number of likes a post has, we just display its `like_count`.

	4. ### Delete & Get Reposts.

		To delete a post, a DFS algorithm (depth first search) is done on the post. We continously go to the next 'child' of the post
		until there are no more children left to check. After that, the algo' returns and deletes every post / repost. <br>

		To get the reposts of a post, we apply the same DFS algo', but instead of deleting the post it is displayed. Also, the function
		is called before the algorithm moves on to the next child.

	5. ### Ratio

		To find a repost that 'ratios' the given post, we just check every 'direct' repost and see if it has a bigger like count than the original.<br>
		There is no need to check **all** reposts, as that is entering 'ratio of a ratio' territory and it technically isn't a ratio to the original
		post anymore.


3. ## Task 3: Social Media <a name="task3"></a>
	
	**WORK IN PROGRESS**

4. ## Linked List Implementation <a name="llist"></a>

	The linked list I implement is similar to the one used in the Linux Kernel: the list's nodes are
	part of the data, not the other way around:<br>
	```C
	struct list_node_t {
	    struct list_node_t *prev;
	    struct list_node_t *next;
	};
	```
	As can be seen from the node, the list is doubly linked. It is NOT circular though.<br>
	```C
	struct data_struct_t {
	    // The fields of the struct
	    // ..
	    // ..
	    struct list_node_t node; // Static nodes don't need any clean-up
	};
	```
	This way, it's really easy to implement any type of list without descending into *pointer madness*.
	The next thing you might ask is: "How do you retrieve the data if you only have the node?". The answer
	is simple, by using black pointer magic:<br> 
	> STRUCT_FROM_MEMBER(struct_name, member_address, member_name)

	This macro will get the address of a struct from which you know the address of a member and its name.
	It's exactly what it's needed to make this type of list work:<br> 
	```C
	struct data_struct_t *data = STRUCT_FROM_MEMBER(data_struct_t, node_ptr, node);
	```

5. ## Hashmap Implementations <a name="hmap"></a>

	The hash map that I implemented is simply an array of linked lists of map_entry_t:<br>
	```C
	struct map_entry_t  {
	    void *key;
	    void *value;
	    struct list_node_t node;
	};
	```
	I already made simple functions to go with the hashmap, namely:
	```C
	int strkeycmp(void *key1, void *key2); // Used for string keys.
	void simple_entry_destroy(list_node_t *node); // Used as a destructor for the list's elements
	```
	And also this one as I needed it:
	```C
	int sizetcmp(void *key1, void *key2); // Used for keys of type size_t.
	```

	The destructor that I implemented: `simple_entry_destroy` should be used for any hash map that
	uses simple data types for the key and value. By simple data types I mean: any primitive type (int,
	float, char, double etc), strings (in this case `void *value` can be casted to `(char *)value`,
	and that's the string). It can also be used for any 'static' structs (structs that do not include 
	any dynamic allocated pointers).<br>
	Maybe, one day, I will implement some sort of dynamic hashmap, that doesn't have the BUCKET values
	hard-coded as macros. ;)

6. ## Graph Implementation <a name="graph"></a>

	Buckle up, this is going to be a rather lengthy explanation.<br><br>

	I spent around 2 days designing and coding this implementation and I think that I cooked
	something rather nice.<br>
	For starters, the graph is nothing more than a list of 'graph nodes' ('gnodes') with some
	extra data on the side to create a pretty handy graph.
	```C
	struct graph_t {
		struct linked_list_t *nodes; // Honestly, I should have named this member 'gnodes'.
		struct map_t *highway;
		struct linked_list_t *old_ids;
	}
	```
	As you can see, there are these extra members: `highway` and `old_ids`. These are part of the graph's ID system.
	I'll explain this a little later.<br>
	An element of the graph (a gnode) is just 2 lists of links:
	```C
	struct graph_node_t {
		size_t id;
		struct linked_list_t *out_links;
		struct linked_list_t *in_links;
		struct list_node_t node;
	}
	```
	The `out_links` list contains links that start from this node to other nodes. <br>
	Intuitively, `in_links` contains links that end in this node and start in other nodes.<br>
	The link just contains a pointer the other graph node:
	```C
	struct graph_link_t {
		struct graph_node_t *link; // I think gnode could be another way to name this member. 
		struct list_node_t node;
	}
	```
	I think that you already guessed this by now, but yes, the graph node uses the same concept
	as a linked list node: it is also part of the data. The only difference is that the graph node
	**NEEDS** to be initialized and cleaned-up (because of the lists of links). Check the [example](#graph-example) to understand how
	this is done.<br>
	```C
	graph_node_t graph_node_create(void);
	void graph_node_destroy(graph_node_t gnode);
	```
	These 2 functions will to just that. They work with graph nodes declared on the stack.
	I don't recommend using graph nodes allocated on the heap, as that's just one extra (and useless) step, as the 
	memory used by the `gnode` member will already be on the heap (when you allocate a new data struct).

7. ## Graph's ID Sytem

	For the sake of ease-of-use I implemented a basic ID system for the graph.<br>
	You SHOULD **NOT** really interact with it externally.<br>
	This is why these functions exist and do what they do:
	```C
	graph_add_node() // -> This function adds a new node to the graph and returns the node's ID.
	graph_get_node() // -> This function returns a pointer to the graph node that has the specified ID.
	```
	The `highway` hashmap contains:

	- Key:   ID of the node.
	- Value: Pointer to the node.

	It is used for constant access time to a specific node.<br>

	The `old_ids` linked list is used to keep track of what IDs have been removed from the graph.
	For example: if you have the nodes 0, 1, 2, 3 and then remove node 1, there should be a way to know
	that ID 1 is free to use.<br>
	The way this tracking it's done is:

	- Firstly, we only keep track of free id 'blocks' (instead of 0, 1, 2 we keep track of 0).
	- Secondly, we only track the beginning of a block (we can verify in O(1) time if ID `block_start + 1` is free or not).
	- Thirdly, when adding a node, we check if ID `graph->nodes->size` is free to use (if we add the n'th node, we assign ID `n-1`).
	- Fourthly, if we can't, we assign it the first ID found in `old_ids`.
	- Fifthly?, when a node it's removed, also remove all nodes in `old_ids` that are bigger or equal to `graph->node->size` (
	we don't want to accidentaly assign the same ID to multiple nodes).
	- Lastly, blocks are added to the `old_ids` list only if `removed_id - 1` is not available to use.

	The way the system is implemented, blocks of ids are merged only if the IDs were freed in ascending order:

	- Freeing 0, 1 and 2 will result in one block: 0
	- Freeing 2, 1, 0 will result in three blocks: 2, 1, 0

	I think that this system is working pretty good right now. Also, I have no need, nor desire, to optimize it.<br>
	If you want to use another ID system, feel free to use an external hashmap or anything else to keep track of the IDs.<br>
	Rember that `graph_add_node()` returns the ID of the node in the graph. (:

8. ## How do I use the graph? <a name="graph-example"></a>

	Here I provided a short example of how the graph should be
	initialized and how the destructor should look like.

	```C
	typedef struct example_t {
		int some_data;
		char *some_string;
		...

		graph_node_t gnode; // The graph node is a staticly declared member.
	} example_t;

	static void example_destructor(list_node_t *node)
	{
		// Also check the declaration of STRUCT_FROM_MEMBER to see how it works.
		// Your data struct should have a graph node encapsulated in it.
		// (and the graph node has a linked list node encapsulated in it).
		// The only real thing you should worry about is a way to keep track
		// of what data is placed around what graph node.

		graph_node_t *gnode = STRUCT_FROM_MEMBER(graph_node_t, node, node);
		example_t *example = STRUCT_FROM_MEMBER(example_t, gnode, gnode);
		
		// Free the allocated memory in your struct.
		free(example->some_string);
		...
		

		graph_node_destroy(example->gnode); // Destroy the graph node.

		free(example); // Free the memory allocated for your struct.
	}

	graph_t *my_graph = graph_create(example_destructor);

	// The graph nodes provide a basic ID system: the highway.
	// If you want to use another one, feel free.
	// Just don't overwrite the graph's system.

	// Allocate your struct.
	example_t *new_example = malloc(sizeof(example_t)); // Implicit cast ik.
	new_example->some_data = 56;
	new_example->some_string = another_string;
	...

	// Now, add the new node to the graph.
	new_example->gnode = graph_node_create();
	size_t new_id = graph_add_node(my_graph, &new_example->gnode);

	// You can also keep track of where you data is in the graph.
	// But you have to do this on your own ;).
	keep_track_of_the_data(data_tracker, new_example, new_id);

	...
	// Free the graph when you don't need it anymore.
	graph_destroy(my_graph);
	```