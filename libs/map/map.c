#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Entry {
    void* key;
    void* value;
    struct Entry* next;
} Entry;

typedef struct HashMap {
    size_t size;                // 哈希表大小
    size_t capacity;            // 哈希表容量
    size_t key_size;            // 键大小
    size_t value_size;          // 值大小
    size_t (*hash_func)(const void* key, size_t key_size);  // 哈希函数
    int (*key_eq_func)(const void* key1, const void* key2, size_t key_size);   // 键比较函数
    Entry** table;              // 哈希表数组
} HashMap;

typedef unsigned int uint32_t;

uint32_t utf8_hash(const char* str) {
    uint32_t hash = 5381;
    while (*str) {
        // 计算UTF-8编码的字符长度
        size_t len = 0;
        unsigned char c = (unsigned char)*str;
        if ((c & 0x80) == 0) {
            len = 1;
        } else if ((c & 0xE0) == 0xC0) {
            len = 2;
        } else if ((c & 0xF0) == 0xE0) {
            len = 3;
        } else if ((c & 0xF8) == 0xF0) {
            len = 4;
        }
        // 对每个字符进行哈希运算
        for (size_t i = 0; i < len; i++) {
            hash = ((hash << 5) + hash) + (uint32_t)*str;
            str++;
        }
    }
    return hash;
}

HashMap* create_hash_map(size_t key_size, size_t value_size, size_t (*hash_func)(const void* key, size_t key_size),
                         int (*key_eq_func)(const void* key1, const void* key2, size_t key_size)) {
    HashMap* map = (HashMap*) malloc(sizeof(HashMap));
    if (!map) {
        return NULL;
    }

    map->size = 0;
    map->capacity = 16;
    map->key_size = key_size;
    map->value_size = value_size;
    map->hash_func = hash_func;
    map->key_eq_func = key_eq_func;
    map->table = (Entry**) malloc(map->capacity * sizeof(Entry*));
    if (!map->table) {
        free(map);
        return NULL;
    }

    memset(map->table, 0, map->capacity * sizeof(Entry*));

    return map;
}

void destroy_hash_map(HashMap* map) {
    for (size_t i = 0; i < map->capacity; i++) {
        Entry* entry = map->table[i];
        while (entry) {
            Entry* next = entry->next;
            free(entry->key);
            free(entry->value);
            free(entry);
            entry = next;
        }
    }
    free(map->table);
    free(map);
}

void* hash_map_get(HashMap* map, const void* key) {
    size_t index = map->hash_func(key, map->key_size) % map->capacity;
    Entry* entry = map->table[index];
    while (entry) {
        if (map->key_eq_func(entry->key, key, map->key_size)) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

void hash_map_put(HashMap* map, const void* key, const void* value) {
    size_t index = map->hash_func(key, map->key_size) % map->capacity;
    Entry* entry = map->table[index];
    while (entry) {
        if (map->key_eq_func(entry->key, key, map->key_size)) {
            memcpy(entry->value, value, map->value_size);
            return;
        }
        entry = entry->next;
    }
    Entry* new_entry = (Entry*) malloc(sizeof(Entry));
    if (!new_entry) {
        return;
    }

    new_entry->key = malloc(map->key_size);
    if (!new_entry->key) {
        free(new_entry);
        return;
    }
    memcpy(new_entry->key, key, map->key_size);

    new_entry->value = malloc(map->value_size);
    if (!new_entry->value) {
        free(new_entry->key);
        free(new_entry);
        return;
    }
    memcpy(new_entry->value, value, map->value_size);

    new_entry->next = map->table[index];
    map->table[index] = new_entry;

    map->size++;

    if (map->size > map->capacity * 0.75) {
        HashMap* new_map = create_hash_map(map->key_size, map->value_size, map->hash_func, map->key_eq_func);
        if (!new_map) {
            return;
        }
        for (size_t i = 0; i < map->capacity; i++) {
            Entry* entry = map->table[i];
            while (entry) {
                hash_map_put(new_map, entry->key, entry->value);
                Entry* next = entry->next;
                free(entry->key);
                free(entry->value);
                free(entry);
                entry = next;
            }
        }
        free(map->table);
        map->table = new_map->table;
        map->capacity = new_map->capacity;
        free(new_map);
    }
}

void hash_map_remove(HashMap* map, const void* key) {
    size_t index = map->hash_func(key, map->key_size) % map->capacity;
    Entry* entry = map->table[index];
    Entry* prev = NULL;
    while (entry) {
        if (map->key_eq_func(entry->key, key, map->key_size)) {
            if (prev) {
                prev->next = entry->next;
            } else {
                map->table[index] = entry->next;
            }
            free(entry->key);
            free(entry->value);
            free(entry);
            map->size--;
            return;
        }
        prev = entry;
        entry = entry->next;
    }
}

void hash_map_clear(HashMap* map) {
    for (size_t i = 0; i < map->capacity; i++) {
        Entry* entry = map->table[i];
        while (entry) {
            Entry* next = entry->next;
            free(entry->key);
            free(entry->value);
            free(entry);
            entry = next;
        }
        map->table[i] = NULL;
    }
    map->size = 0;
}

size_t hash_string(const void* key, size_t key_size) {
    const char* str = (const char*) key;
    size_t hash = 0;
    for (size_t i = 0; i < key_size; i++) {
        hash = hash * 31 + str[i];
    }
    return hash;
}


int compare_string(const void* key1, const void* key2, size_t key_size) {
    return memcmp(key1, key2, key_size) == 0;
}

int main() {
    HashMap* map = create_hash_map(sizeof(char*), sizeof(int), hash_string, compare_string);
    if (!map) {
        return 1;
    }

    const char* keys[] = {"apple", "banana", "cherry", "durian", "elderberry"};
    int values[] = {

    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        hash_map_put(map, &keys[i], &values[i]);
    }

    int* value = (int*) hash_map_get(map, "banana");
    if (value) {
        printf("The value for 'banana' is %d\n", *value);
    } else {
        printf("'banana' not found\n");
    }

    hash_map_remove(map, "banana");

    value = (int*) hash_map_get(map, "banana");
    if (value) {
        printf("The value for 'banana' is %d\n", *value);
    } else {
        printf("'banana' not found\n");
    }

    hash_map_clear(map);

    destroy_hash_map(map);

    return 0;
}


