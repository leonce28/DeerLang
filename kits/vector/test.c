#include <stdio.h>
#include "vector.h"

int main() {
    Vector *vec = vector_create(sizeof(int));
    if (!vec) {
        printf("Failed to create vector\n");
        return 1;
    }

    int x = 10;
    vector_push_back(vec, &x);
    vector_push_back(vec, &x);

    int *p1 = (int*)vector_at(vec, 0);
    int *p2 = (int*)vector_at(vec, 1);
    if (!p1 || !p2) {
        printf("Failed to get element from vector\n");
        vector_destroy(vec);
        return 1;
    }

    printf("Elements in vector: %d, %d\n", *p1, *p2);

    vector_destroy(vec);
    return 0;
}