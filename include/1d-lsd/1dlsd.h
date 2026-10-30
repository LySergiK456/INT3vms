#include "../types.h"
#include "../common/llist.h"
#include "./binary.h"
#include "../../x86disass/include/disass.h"

#ifndef LSD_API_H
#define LSD_API_H
#define LSD_API
typedef unsigned long long LSD_STATUS;

enum LSD_STATUS_COdE {
    LSD_OK,
    LSD_NULLPTR,
};

enum _1DLSD_bin_tYPE {
    LSD_None    = 0,
    LSD_ELF32   = 1,
    LSD_ELF64   = 2,
    LSD_PE32    = 3
};
enum _1DLSD_TYPE {
    LSD_R   = 0x100,
    LSD_W  = 0x200,
    LSD_X  = 0x400,
    LSD_NONE= 0,
    ELF_Ehdr=1,
    ELF_Phdr,
    ELF_Shdr,
    ELF_Rela,
    ELF_Rel,
    ELF_Sym,
    ELF_Dyn,
    ELF_GNU_Verneed,
    ELF_GNU_Versym,
};

typedef int (*lsd_oper_calback)(instr_dat_t *insn, __u64 i, __u64 imm, instr_dat_t *out);

struct lsd_oper_cb {
    bin_t			*lsd_img;
    lsd_oper_calback    lsd_cb;
    operand             lsd_oper;
};

struct _1Dlsd_entry {
    enum _1DLSD_TYPE    t;
    mm_t                *mm;
    struct {
        bin_t       *img;
        __u64       off, sz;
    } src;
    struct {
        __u64      off, sz;
    } dst;
    void                *raw_mem;
    LLIST_OF(struct _1Dlsd_entry) px;
};

typedef struct {
    LLIST_OF(struct _1Dlsd_entry)       ll_px;
    LLIST_OF(struct rel_yayo)           ll_rel;
    LLIST_OF(struct lsd_oper_cb)  ll_cb;
} _1Dlsd_pack_t;

LSD_API __u64 lsd_align(struct rel_yayo *ctx, __u64 offset);
#endif
