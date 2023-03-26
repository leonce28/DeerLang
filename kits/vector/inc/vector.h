#pragma once

typedef struct Vector Vector;

extern Vector *vector_create(int elem_size);
extern void vector_destroy(Vector *vec);
extern void *vector_at(Vector *vec, int index);
extern void vector_push_back(Vector *vec, const void *elem);
extern void vector_pop_back(Vector *vec);
extern int vector_size(Vector *vec);


