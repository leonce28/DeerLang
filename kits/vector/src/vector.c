#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

struct Vector {
    int size;
    int capacity;
    int esize;
    void *elements;
};

Vector *vector_create(int esize) {
    Vector *vec = (Vector *)malloc(sizeof(Vector));
    if (!vec) {
        return NULL;
    }

    vec->size = 0;
    vec->capacity = 16;
    vec->esize = esize;
    vec->elements = (void *)malloc(vec->capacity * esize);
    if (!vec->elements) {
        free(vec);
        return NULL;
    }

    return vec;
}

void vector_destroy(Vector *vec) {
    free(vec->elements);
    free(vec);
}

void *vector_at(Vector *vec, int index) {
    if (index >= vec->size) {
        return NULL;
    }
    return vec->elements + index * vec->esize;
}

void vector_push_back(Vector *vec, const void *elem) {
    if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        vec->elements = (void *)realloc(vec->elements, vec->capacity * vec->esize);
        if (!vec->elements) {
            return;
        }
    }
    memcpy(vec->elements + vec->size * vec->esize, elem, vec->esize);
    vec->size++;
}

void vector_pop_back(Vector *vec) {
    if (vec->size == 0) {
        return;
    }
    vec->size--;
}

int vector_size(Vector *vec) {
    return vec->size;
}


