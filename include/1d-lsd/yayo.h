#include "../../x86disass/disass.h"
#include "../xtc/xtc.h"
#include "../common/llist.h"
#include "../types.h"
#include "./binary.h"
#include "./1dlsd.h"
#include <unicorn/unicorn.h>

extern void YAYO_RELOC(int a);

#ifndef KRACK_YAYO_H
#define KRACK_YAYO_H

#define YAYO_API
#define YAYO_INTERNAL

enum yayo_ref_t {
    YAYO_BIN = 1,
    YAYO_SHELLCODE = 2,
    YAYO_UC_VMMAP = 4,
    YAYO_VMMAP = 8,
};
enum yayo_px_t {
    YAYO_REL        = 1ULL,
    YAYO_REPLACE    = 2ULL,
    YAYO_ADD    = 4ULL,
    YAYO_REMOVE = 8LL,
};
enum yayo_hook_t {
    YAYO_HOOK_ORIG = 1,
    YAYO_HOOK_DST = 2,
};

struct rel_evmmap {
    __u64 VirtualAddress;
    __u64 VirtualSize;
    __u32 Perm;
    __u64 Characteristics;
};

struct rel_uc_evmmap {
    __u64 virt;
    __u64 size;
    __u32 perm;
};

struct yayo_hook {
    enum yayo_hook_t t;
    __u64 src;
    __u64 v;
};

typedef struct yayo_px {
    enum yayo_px_t  t;
    void            *data;
    struct {
        __u64 orig_off, dst_off;
        __u64 orig_size, dst_size;
    };
    LLIST_OF(struct yayo_px) ll_yayo_px;
    LLIST_OF(struct yayo_hook) ll_hooks;
} yayo_px;

typedef __u64 (*yayo_imm_hook_t)(struct yayo_px *, __u64 i, instr_dat_t *in, __u64 imm);

struct rel_yayo {
    _1Dlsd_pack_t       *lsd;
    enum yayo_ref_t     t;
    yayo_imm_hook_t     hook_fn;
    union {
        /* PE64/ELF*/
        struct {
            bin_t    *bin;
            __u64       offset;
            __u64       size;
        } bin;
        /* Shellcode */
        struct {
            void *ptr;
            __u64 size;
        } mm;
        /* Virtual Mapping's */
        struct {
            LLIST_OF(struct rel_evmmap) ll_entries;
        } vmmap;
        /* Unicorn VM */
        struct {
            uc_engine *uc;
            LLIST_OF(struct rel_uc_evmmap) ll_entries;
        } uc;
    };
    struct {
        __u64       off;
        __u64       align;
    } out;
    LLIST_OF(struct xtc_px)     ll_xtc_px;
    LLIST_OF(struct yayo_px)    ll_yayo_px;
};

__s8 new_rel_yayo(struct rel_yayo *r);
#endif
