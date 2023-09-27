#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct DeerVector {
    int size;
    int esize;
    int capacity;
    void *elements;
} DeerVector; 

extern DeerVector *dvector_create(size_t size);
extern void dvector_destroy(DeerVector *vector);

extern void *dvector_back(DeerVector *vector);
extern void *dvector_fetch_back(DeerVector *vector);
extern void *dvector_fetch(DeerVector *vector, int index);

extern void dvector_push(DeerVector *vector, void *data);
extern void dvector_pop(DeerVector *vector);

extern void dvector_place(DeerVector *vector, int index, void *data, size_t size); 
extern void dvector_place2(DeerVector *vector, int index, void *data, size_t size); // free data
