#ifndef UTILS_H
#define UTILS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint;

#define MEMBER_OFFSET(struct, member) ((size_t) (&((struct *)0)->member))

#define STRUCT_FROM_MEMBER(struct, member_address, member_name) (struct *)(   \
	(char *)member_address - MEMBER_OFFSET(struct, member_name))

#define DIE(assertion, call_description)                                      \
    do {                                                                      \
        if (assertion) {                                                      \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                \
            perror(call_description);                                         \
            exit(errno);                                                      \
        }                                                                     \
    } while (0)

uint hash_uint(void *key);
uint hash_string(void *key);
uint hash_size_t(void *key);

int sizetcmp(void *uint1, void *uint2);

#endif