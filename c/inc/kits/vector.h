#pragma once

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  

#define MIN_LEN       256
#define VCEFAILED      -1
#define VCESUCCESS      0
#define VCEPUSHBACK     1
#define VCEPOPBACK      2
#define VCEINSERT       3
#define VCERM           4
#define EXPANED_VAL     1
#define REDUSED_VAL     2

typedef void *iterator;
typedef struct _vector *vector;

void      vector_destroy(const vector cv);
int       vector_pushback(const vector cv, void *memb);
int       vector_popback(const vector cv, void *memb);
int       vector_iter_val(const vector cv, iterator iter, void *memb);
int       vector_val_at(const vector cv, size_t index, void *memb);
int       vector_insert(const vector cv, iterator iter, void *memb);
int       vector_insert_at(const vector cv, size_t index, void *memb);
int       vector_rm(const vector cv, iterator iter);
int       vector_rm_at(const vector cv, size_t index);
size_t    vector_length(const vector cv);
size_t    vector_iter_at(const vector cv, iterator iter);
vector    vector_create(const size_t size);
iterator  vector_begin(const vector cv);
iterator  vector_end(const vector cv);
iterator  vector_next(const vector cv, iterator iter);

/* for test  */
void      vector_info(const vector cv);
void      vector_print(const vector cv);

