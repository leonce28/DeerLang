#include <stdio.h>
#include <stdlib.h>
#include "pool.h"

typedef struct MemoryPool {
    size_t size;             // 内存池大小
    size_t block_size;       // 块大小
    char* start;             // 内存池起始地址
    char* end;               // 内存池结束地址
    char* next;              // 下一个可用块的地址
} MemoryPool;

MemoryPool* create_memory_pool(size_t size, size_t block_size) {
    MemoryPool* pool = (MemoryPool*) malloc(sizeof(MemoryPool));
    if (!pool) {
        return NULL;
    }

    pool->size = size;
    pool->block_size = block_size;
    pool->start = (char*) malloc(size);
    if (!pool->start) {
        free(pool);
        return NULL;
    }

    pool->end = pool->start + size;
    pool->next = pool->start;
    return pool;
}

void* memory_pool_alloc(MemoryPool* pool, size_t size) {
    char* p = pool->next;
    pool->next += pool->block_size;
    if (pool->next > pool->end) {
        return NULL;
    }
    return p;
}

void destroy_memory_pool(MemoryPool* pool) {
    free(pool->start);
    free(pool);
}

int main() {
    MemoryPool* pool = create_memory_pool(1024, 16);
    if (!pool) {
        printf("Failed to create memory pool\n");
        return 1;
    }

    void* p1 = memory_pool_alloc(pool, 16);
    void* p2 = memory_pool_alloc(pool, 16);
    if (!p1 || !p2) {
        printf("Failed to allocate memory from pool\n");
        destroy_memory_pool(pool);
        return 1;
    }

    printf("Allocated memory from pool: %p, %p\n", p1, p2);

    destroy_memory_pool(pool);
    return 0;
}
