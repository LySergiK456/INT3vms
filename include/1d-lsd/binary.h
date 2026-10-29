#include "../types.h"
#include "../common/llist.h"
#include "../common/mem.h"

#ifndef BINANAL_H
#define BINANAL_H
struct load_region {
    struct {
        __s8 r:1;
        __s8 w:2;
        __s8 x:3;
    } prot;
    __u64   off, virt;
    __u64   size;
    __s64   align;
    int     fillwith;
};

typedef struct bin_t {
	__u8    	t;         // Binary_t_ELF64 or Binary_t_PE32plus
    __s8    	IsPIC;     // Postion Independent Code
    union {
        void    *map;
        __u8    *mem;
    } src;
    __u64       size;
    LLIST_OF(struct load_region)    ll_regions;
    void ((*init_fn)());
    void ((*fini_fn)());
    __u64                           entry_va;
    __u64                           va_base;
    __s8                            lazy_binding;
} bin_t;

typedef struct bin_hdr_t {
    union {
        mm_t mm;
        void *ptr;
    };
    __u64 off;
} bin_hdr_t;
#endif
