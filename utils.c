#include "utils.h"
#include <string.h>

uint hash_uint(void *key)
{
	uint uint_key = *((uint *)key);

	uint_key = ((uint_key >> 16u) ^ uint_key) * 0x45d9f3b;
	uint_key = ((uint_key >> 16u) ^ uint_key) * 0x45d9f3b;
	uint_key = (uint_key >> 16u) ^ uint_key;

	return uint_key;
}

uint hash_string(void *key)
{
	unsigned char *key_string = (unsigned char *)key;
	uint hash = 5381;
	int c;

	while ((c = *key_string++))
		hash = ((hash << 5u) + hash) + c;

	return hash;
}

uint hash_size_t(void *key)
{
	uint uint_key = (uint)(*((size_t *)key));

	uint_key = ((uint_key >> 16u) ^ uint_key) * 0x45d9f3b;
	uint_key = ((uint_key >> 16u) ^ uint_key) * 0x45d9f3b;
	uint_key = (uint_key >> 16u) ^ uint_key;

	return uint_key;
}

size_t strtosizet(char *str)
{
	size_t len = strlen(str);
	if (len == 0)
		return 0;

	size_t result = 0;
	for (; len != 0; len--) {
		if (*str < '0' || *str > '9') {
			str++;
			continue;
		}

		result = result * 10 + ((*str) - '0');
		str++;
	}
	return result;
}

int sizetcmp(void *uint1, void *uint2)
{
	size_t ui1 = *(size_t *)uint1;
	size_t ui2 = *(size_t *)uint2;
	if (ui1 > ui2)
		return 1;

	if (ui1 < ui2)
		return -1;

	return 0;
}

int ptrcmp(void *ptr1, void *ptr2)
{
	void *p1 = *(void **)ptr1;
	void *p2 = *(void **)ptr2;
	if (p1 > p2)
		return 1;

	if (p1 < p2)
		return -1;

	return 0;
}
