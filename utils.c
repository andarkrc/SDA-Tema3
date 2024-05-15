#include "utils.h"

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
    unsigned char *key_string = (unsigned char *) key;
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

int sizetcmp(void *uint1, void *uint2)
{
    size_t ui1 = *(size_t *)uint1;
    size_t ui2 = *(size_t *)uint2;
    if (ui1 > ui2) {
        return 1;
    }
    if (ui1 < ui2) {
        return -1;
    }
    return 0;
}