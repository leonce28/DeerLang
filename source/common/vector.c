#include "common/vector.h"

#ifndef nullptr
#define nullptr     NULL
#endif

DeerVector *dvector_create(size_t size)
{
    assert(size > 0);

    DeerVector *vector = malloc(sizeof(DeerVector));
    assert(vector);

    memset(vector, 0, sizeof(DeerVector));

    vector->size = 0;
    vector->esize = size;
    vector->capacity = 16;
    vector->elements = malloc(vector->capacity * vector->esize);
    assert(vector->elements);

    return vector;
}

void dvector_destroy(DeerVector *vector) 
{
    free(vector->elements);
    free(vector);
}

void dvector_expand(DeerVector *vector)
{
    assert(vector);

    void *old = vector->elements;
    int size = vector->capacity * vector->esize * 2;

    vector->elements = malloc(size);
    assert(vector->elements);

    memset(vector->elements, 0, size);
    memcpy(vector->elements, old, vector->capacity * vector->esize);

    vector->capacity *= 2;

    free(old);
}

void dvector_push(DeerVector *vector, void *data)
{
    assert(vector && data);

    if (vector->size == vector->capacity) { 
        dvector_expand(vector);
    }

    memcpy(vector->elements + vector->size * vector->esize, data, vector->esize);

    vector->size++;
}

void dvector_pop(DeerVector *vector)
{
    assert(vector);

    if (vector->size == 0) {
        return ;
    }

    vector->size--;
    memset(vector->elements + vector->size * vector->esize, 0, vector->esize);
}

void *dvector_fetch(DeerVector *vector, int index) 
{
    assert(vector && index > 0);

    if (index >= vector->size) {
        return nullptr;
    }

    return vector->elements + index * vector->esize;
}

void *dvector_fetch_back(DeerVector *vector)
{
    assert(vector);

    if (vector->size == vector->capacity) { 
        dvector_expand(vector);
    }

    vector->size++;

    return vector->elements + (vector->size - 1) * vector->esize;
}

void *dvector_back(DeerVector *vector) 
{
    assert(vector);

    if (vector->size == 0) {
        return nullptr;
    }

    return vector->elements + (vector->size - 1) * vector->esize;
}

void dvector_place(DeerVector *vector, int index, void *data, size_t size)
{
    assert(vector && data && index > 0 && size > 0);

    void *place = dvector_fetch(vector, index);
    assert(place); 

    memcpy(place, data, size);
}

void dvector_place2(DeerVector *vector, int index, void *data, size_t size)
{
    dvector_place(vector, index, data, size);

    if (data) {
        free(data);
    }
}
