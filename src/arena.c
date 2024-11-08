#ifndef ARENA_C
#define ARENA_C

#include <stdlib.h>

typedef struct Region Region;

struct Region {
    Region *next;
    int count;
    int capacity;
    int data[];
};



#endif
