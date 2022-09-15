#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kits/vector.h"

#ifndef __gnu_linux__
#define __func__ "unknown"
#define inline __forceinline
#endif

#define CWARNING_ITER(vc, iter, file, func, line) \
    do { \
        if ((vector_begin(vc) > iter) || (vector_end(vc) <= iter)) { \
            fprintf(stderr, "var(" #iter ") warng out of range, " \
            "at file:%s func:%s line:%d!!\n", file, func, line); \
            return VCEFAILED;\
        } \
    } while(0)

struct _vector {
    void *vc_pdata;
    size_t vc_len, vc_tot_len, vc_size;
};

vector vector_create(const size_t size)
{
    vector vc = (vector)malloc(sizeof(struct _vector));
    if (!vc) return NULL;

    vc->vc_pdata = malloc(MIN_LEN * size);
    if (!vc->vc_pdata) {
        free(vc);
        return NULL;
    }

    vc->vc_size = size;
    vc->vc_tot_len = MIN_LEN;
    vc->vc_len = 0;

    return vc;
}

void vector_destroy(const vector vc)
{
    free(vc->vc_pdata);
    free(vc);
    return;
}

size_t vector_length(const vector vc)
{
    return vc->vc_len;
}

int vector_pushback(const vector vc, void *memb)
{
    if (vc->vc_len >= vc->vc_tot_len) {
        void *pd_sav = vc->vc_pdata;
        vc->vc_tot_len <<= EXPANED_VAL;
        vc->vc_pdata = realloc(vc->vc_pdata, vc->vc_tot_len * vc->vc_size);

        if (!vc->vc_pdata) {
            vc->vc_pdata = pd_sav;
            vc->vc_tot_len >>= EXPANED_VAL;
            return VCEPUSHBACK;
        }
    }

    memcpy((char *)vc->vc_pdata + vc->vc_len * vc->vc_size, memb, vc->vc_size);
    vc->vc_len++;

    return VCESUCCESS;
}

int vector_popback(const vector vc, void *memb)
{
    if (vc->vc_len <= 0) return VCEPOPBACK;

    vc->vc_len--;
    memcpy(memb, (char *)vc->vc_pdata + vc->vc_len * vc->vc_size, vc->vc_size);

    if ((vc->vc_tot_len >= (MIN_LEN << REDUSED_VAL))
        && (vc->vc_len <= (vc->vc_tot_len >> REDUSED_VAL))) {
        void *pd_sav = vc->vc_pdata;
        vc->vc_tot_len >>= EXPANED_VAL;
        vc->vc_pdata = realloc(vc->vc_pdata, vc->vc_tot_len * vc->vc_size);

        if (!vc->vc_pdata) {
            vc->vc_tot_len <<= EXPANED_VAL;
            vc->vc_pdata = pd_sav;
            return VCEPOPBACK;
        }
    }

    return VCESUCCESS;
}

size_t vector_iter_at(const vector vc, iterator iter)
{
    CWARNING_ITER(vc, iter, __FILE__, __func__, __LINE__);
    return ((char *)iter - (char *)vc->vc_pdata) / vc->vc_size;
}

int vector_iter_val(const vector vc, iterator iter, void *memb)
{
    CWARNING_ITER(vc, iter, __FILE__, __func__, __LINE__);
    memcpy(memb, iter, vc->vc_size);
    return 0;
}

iterator vector_begin(const vector vc)
{
    return vc->vc_pdata;
}

iterator vector_end(const vector vc)
{
    return (char *)vc->vc_pdata + (vc->vc_size * vc->vc_len);
}

static inline void vcmemove_foreward(const vector vc, void *from, void *to)
{
    size_t size = vc->vc_size;
    char *p;
    for (p = (char *)to; p >= (char *)from; p -= size) memcpy(p + size, p, size);
    return;
}

static inline void vcmemove_backward(const vector vc, void *from, void *to)
{
    memcpy(from, (char *)from + vc->vc_size, (char *)to - (char *)from);
    return;
}

int vector_insert(const vector vc, iterator iter, void *memb)
{
    CWARNING_ITER(vc, iter, __FILE__, __func__, __LINE__);

    if (vc->vc_len >= vc->vc_tot_len) {
        void *pd_sav = vc->vc_pdata;
        vc->vc_tot_len <<= EXPANED_VAL;
        vc->vc_pdata = realloc(vc->vc_pdata, vc->vc_tot_len * vc->vc_size);

        if (!vc->vc_pdata) {
            vc->vc_pdata = pd_sav;
            vc->vc_tot_len >>= EXPANED_VAL;
            return VCEINSERT;
        }
    }

    vcmemove_foreward(vc, iter, (char *)vc->vc_pdata + vc->vc_len * vc->vc_size);
    memcpy(iter, memb, vc->vc_size);
    vc->vc_len++;

    return VCESUCCESS;
}

int vector_insert_at(const vector vc, size_t index, void *memb)
{
    iterator iter;

    if (index >= vc->vc_tot_len) {
        vc->vc_len = index + 1;
        while (vc->vc_len >= vc->vc_tot_len) vc->vc_tot_len <<= EXPANED_VAL;
        vc->vc_pdata = realloc(vc->vc_pdata, vc->vc_tot_len * vc->vc_size);
        iter = (char *)vc->vc_pdata + vc->vc_size * index;
        memcpy(iter, memb, vc->vc_size);
    } else {
        iter = (char *)vc->vc_pdata + vc->vc_size * index;
        vector_insert(vc, iter, memb);
    }

    return 0;
}

iterator vector_next(const vector vc, iterator iter)
{
    return (char *)iter + vc->vc_size;
}

int vector_val(const vector vc, iterator iter, void *memb)
{
    memcpy(memb, iter, vc->vc_size);
    return 0;
}

int vector_val_at(const vector vc, size_t index, void *memb)
{
    memcpy(memb, (char *)vc->vc_pdata + index * vc->vc_size, vc->vc_size);
    return 0;
}

int vector_rm(const vector vc, iterator iter)
{
    iterator from;
    iterator end;
    CWARNING_ITER(vc, iter, __FILE__, __func__, __LINE__);
    from = iter;
    end = vector_end(vc);
    memcpy(from, (char *)from + vc->vc_size, (char *)end - (char *)from);
    vc->vc_len--;

    if ((vc->vc_tot_len >= (MIN_LEN << REDUSED_VAL))
        && (vc->vc_len <= (vc->vc_tot_len >> REDUSED_VAL))) {
        void *pd_sav = vc->vc_pdata;
        vc->vc_tot_len >>= EXPANED_VAL;
        vc->vc_pdata = realloc(vc->vc_pdata, vc->vc_tot_len * vc->vc_size);

        if (!vc->vc_pdata) {
            vc->vc_tot_len <<= EXPANED_VAL;
            vc->vc_pdata = pd_sav;
            return VCERM;
        }
    }

    return VCESUCCESS;
}

int vector_rm_at(const vector vc, size_t index)
{
    iterator iter;
    iter = (char *)vc->vc_pdata + vc->vc_size * index;
    CWARNING_ITER(vc, iter, __FILE__, __func__, __LINE__);
    return vector_rm(vc, iter);
}

void vector_info(const vector vc)
{
    printf("\n\ntot :%s : %ld\n", __func__, vc->vc_tot_len);
    printf("len :%s : %ld\n", __func__, vc->vc_len);
    printf("size:%s : %ld\n\n", __func__, vc->vc_size);
    return;
}

void vector_print(const vector vc)
{
    int num;
    iterator iter;

    if (vector_length(vc) == 0)
        fprintf(stderr, "file:%s func:%s line:%d error, null length vector!!\n", __FILE__, __func__, __LINE__);

    for (iter = vector_begin(vc);
        iter != vector_end(vc);
        iter = vector_next(vc, iter)) {
        vector_iter_val(vc, iter, &num);
        printf("var:%d at:%ld\n", num, vector_iter_at(vc, iter));
    }

    return;
}