#ifndef BINANAL_HH
#define BINANAL_HH
#include "../syx86/syx86.hh"
extern "C" {
    #include "../types.h"
    #include "./binary.h"
};

namespace SlitYaX86 {
struct Binary_t : public bin_t {
    __s8 ok{0};

    Binary_t() : bin_t{} {};
    Binary_t(int type, void *map, __u64 sz);
    virtual ~Binary_t() = default;

    virtual __u64 vtof(__u64 v) { assert(0); };
    virtual __u64 ftov(__u64 v) { assert(0); };
};
};

struct BinHdr : public bin_hdr_t {
    BinHdr() = delete;
    BinHdr(void *ptr, __u64 off, __u64 size)
        : bin_hdr_t{.mm={.mem=ptr, .t=MM_NONE, .sz=size}, .off=off} {}
};
#endif
