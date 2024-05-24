#ifndef UTILS_H
#define UTILS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint;

#define MEMBER_OFFSET(struct, member) ((size_t)(&((struct *)0)->member))

#define STRUCT_FROM_MEMBER(struct, member_address, member_name) (struct *)(   \
	(char *)member_address - MEMBER_OFFSET(struct, member_name))

#define DIE(assertion, call_description)									  \
	do {																	  \
		if (assertion) {													  \
			fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);				\
			perror(call_description);										 \
			exit(errno);													  \
		}																	 \
	} while (0)

/**
 * hash_uint() - Hash the unsigned int at the address 'key'.
 * @param key: -> *(uint *)key to get the uint.
 *
 * @return - Hash
 */
uint hash_uint(void *key);

/**
 * hash_string() - Hash the string at the address 'key'.
 * @param key: -> (char *)key to get the string.
 *
 * @return - Hash
 */
uint hash_string(void *key);

/**
 * hash_size_t() - Hash the size_t at the address 'key'.
 * @param key: -> *(size_t *)key to get the size_t.
 *
 * @return - Hash
 */
uint hash_size_t(void *key);

/**
 * sizetcmp() - Compare the 2 uints at the addresses given as arguments.
 * @param uint1: -> *(uint *)uint1
 * @param uint2: -> *(uint *)uint2
 *
 * @return - 1 if uint1 > uint2
 *		  -1 if uint1 < uint2
 *		   0 if uint1 == uint2
 */
int sizetcmp(void *uint1, void *uint2);

/**
 * @ptrcmp() - Compare the 2 pointers at the addressses given as arguments.
 * @param ptr1: -> *(void **)ptr1
 * @param ptr2: -> *(void **)ptr2
 *
 * @return - 1 if ptr1 > ptr2
 *		  -1 if ptr1 < ptr2
 *		   0 if ptr1 == ptr2
 */
int ptrcmp(void *ptr1, void *ptr2);

/**
 * strtosizet() - Convert a string to a size_t.
 * @param str: The string to convert.
 *
 * @return - The string as a number.
 */
size_t strtosizet(char *str);

#endif
