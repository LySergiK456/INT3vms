#ifndef KRACK_LLIST_H
#define KRACK_LLIST_H
#include "../types.h"

#define __CONCAT1__(a, T, counter) a##T##counter
#define __CONCAT__(a, T, counter) __CONCAT1__(a, T, counter)

#define LLIST_OF(T) LLIST_OF_COUNTER(T, __COUNTER__)

#define LLIST_OF_COUNTER(T, cntr)                                                  \
    struct __CONCAT__(_llist_, at_, cntr) {                                       \
        struct __CONCAT__(_llist_, at_, cntr) * next, *prev;                      \
        T *v;                                                                  \
        __u64 sz;                                                              \
        __u8 used;                                                             \
    }

#define LLIST_SZ(size)  LLIST_SZ_COUNTER(size, __COUNTER__)
#define LLIST_SZ_COUNTER(size, cntr)                                              \
    struct __CONCAT__(_llist_, sizeof_, cntr) {                                   \
        struct __CONCAT__(_llist_, sizeof_, cntr) * next, *prev;                  \
        union {                                                                 \
            __u8    *array;                                                     \
            void    *v;                                                         \
        };                                                                      \
        __u64 sz;                                                               \
        __u8 used;                                                              \
    }

typedef LLIST_SZ(8)     _LL_TYPE_;
#define _LL_            (_LL_TYPE_*)

#define ll_each(ll, e)                                                         \
    for (typeof((ll)) e = (ll)->next; !!e && e != (ll); e = e->next)

#define ll_each_value(ll, e)                \
    ll_each(_LL_ (ll), _##e##_)             \
        for (typeof((ll)->v) e=(typeof((ll)->v))(_##e##_)->v; !!e; e=NULL)

#define LLIST_TWINE(last, new)                                                 \
    ({                                                                         \
        (new)->next = last->next;                                              \
        (new)->prev = last;                                                    \
        last->next = (new);                                                    \
    })

#define llist_empty(ll) ((ll)->next == (ll))

void llist_init(void *ll_ptr, __u64 size);
void *llist_push(_LL_TYPE_ *ll, void *dat);

#endif
