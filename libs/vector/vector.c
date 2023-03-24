#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Vector {
    size_t size;            // 向量大小
    size_t capacity;        // 向量容量
    size_t elem_size;       // 元素大小
    char* data;             // 元素数组
} Vector;

Vector* create_vector(size_t elem_size) {
    Vector* vec = (Vector*) malloc(sizeof(Vector));
    if (!vec) {
        return NULL;
    }

    vec->size = 0;
    vec->capacity = 16;
    vec->elem_size = elem_size;
    vec->data = (char*) malloc(vec->capacity * elem_size);
    if (!vec->data) {
        free(vec);
        return NULL;
    }

    return vec;
}

void destroy_vector(Vector* vec) {
    free(vec->data);
    free(vec);
}

void* vector_at(Vector* vec, size_t index) {
    if (index >= vec->size) {
        return NULL;
    }
    return vec->data + index * vec->elem_size;
}

void vector_push_back(Vector* vec, const void* elem) {
    if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        vec->data = (char*) realloc(vec->data, vec->capacity * vec->elem_size);
        if (!vec->data) {
            return;
        }
    }
    memcpy(vec->data + vec->size * vec->elem_size, elem, vec->elem_size);
    vec->size++;
}

void vector_pop_back(Vector* vec) {
    if (vec->size == 0) {
        return;
    }
    vec->size--;
}

size_t vector_size(Vector* vec) {
    return vec->size;
}

int main() {
    Vector* vec = create_vector(sizeof(int));
    if (!vec) {
        printf("Failed to create vector\n");
        return 1;
    }

    int x = 10;
    vector_push_back(vec, &x);
    vector_push_back(vec, &x);

    int* p1 = (int*) vector_at(vec, 0);
    int* p2 = (int*) vector_at(vec, 1);
    if (!p1 || !p2) {
        printf("Failed to get element from vector\n");
        destroy_vector(vec);
        return 1;
    }

    printf("Elements in vector: %d, %d\n", *p1, *p2);

    destroy_vector(vec);
    return 0;
}
