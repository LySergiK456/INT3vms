#include "../../include/1d-lsd/1dlsd.hh"
#include "../../include/conf.hh"
#include "../../include/common/utils.hh"
// extern "C" {
//     #include "../../include/1d-lsd/yayo.h"
// };

namespace SlitYaX86
{
bin_hdr_t *&Lyss::operator<<(bin_hdr_t&& v) {
    auto v_ptr = new bin_hdr_t;
    *v_ptr = std::move(v);

    tofree.push_back(reinterpret_cast<void*>(v_ptr));
    return reinterpret_cast<bin_hdr_t*&>(tofree.back());
}
bin_hdr_t *new_lyss(void *ptr, __u64 off, __u64 sz) {
    auto e = reinterpret_cast<bin_hdr_t*>(xmalloc(sizeof(bin_hdr_t)));
    e->mm = {
        (void*)((char*)ptr + off), MM_NONE, sz
    };
    e->off = off;
    return e;
}

std::map<__u64, struct _1Dlsd_entry> &Lyss::operator()() {
    return map;
}
};

namespace SlitYaX86 {
    using BinT = Binary_t;
    using MT = MemConvType;

    LSD_API __u64 Lyss::orig_vtov(BinT *bin, __u64 v) { return MemBase(bin, MT::Virt,  v, MT::Virt);}
    LSD_API __u64 Lyss::orig_ftof(BinT *bin, __u64 v) { return MemBase(bin, MT::Off,   v, MT::Off); }
    LSD_API __u64 Lyss::orig_vtof(BinT *bin, __u64 v) { return MemBase(bin, MT::Virt,  v, MT::Off); }
    LSD_API __u64 Lyss::orig_ftov(BinT *bin, __u64 v) { return MemBase(bin, MT::Off,   v, MT::Virt);}

    LSD_API __u64 Lyss::mconv_vtov(BinT *bin, __u64 v) { return MemConv(bin, MT::Virt, v, MT::Virt);}
    LSD_API __u64 Lyss::mconv_ftof(BinT *bin, __u64 v) { return MemConv(bin, MT::Off,  v, MT::Off); }
    LSD_API __u64 Lyss::mconv_vtof(BinT *bin, __u64 v) { return MemConv(bin, MT::Virt, v, MT::Off); }
    LSD_API __u64 Lyss::mconv_ftov(BinT *bin, __u64 v) { return MemConv(bin, MT::Off,  v, MT::Virt);}
};

namespace SlitYaX86 {
using MT = MemConvType;

__u64 Lyss::MemBase(Binary_t *bin, MT src_type, __u64 value, MT dst_type) {
    __u64 offset{};
    switch (src_type) {
        case MT::Virt:  offset = bin->vtof(value);
        case MT::Off:   offset = value;
        default: assert(0);
    }
    
    for (auto& [lsd_off, emm] : map) {
        if (    emm.src.img == bin
            &&  !!_contain_(emm.src.off, emm.src.sz, offset))
        {
            __u64 ret = lsd_off + (offset - emm.src.off);
            switch (dst_type) {
                case MT::Virt:  return mconv_ftov(bin, ret);
                case MT::Off:   return ret;
                default: assert(0);
            }
        }
    }
    return -1;
}
};

/** 4 Yayo **/
// LSD_API __u64 lsd_align(struct rel_yayo *ctx, __u64 offset) {
//     return offset;
// }
namespace SlitYaX86
{
// void Lyss::align_map(__u64 off, __u64 sz) {
//     for (auto& [k, emm] : map) {
//         if (_contain_(k, emm.mm->sz, off)) {
//             map[k].mm->sz += sz;
//         } else if (k >= off) {
//             struct _1Dlsd_entry e{std::move(emm)};
//             __u64 key{k+sz};

//             map.erase(k);
//             map[key] = std::move(e);
//         }
//         // DBG_FMT("/0x%lx\n", k);
//     }
// }
void Lyss::align_map(__u64 off, __u64 sz) {
    std::map<__u64, struct _1Dlsd_entry> out;
    for (auto& [k, emm] : map) out[k] = map[k];

    for (auto& [k, emm] : map) {
        if (_contain_(k, emm.mm->sz, off)) {
            out[k].mm->sz += sz;
        } else if (k >= off) {
            out.erase(k);
            out[k+sz] = emm;
        }
    }
    map.erase(map.begin(), map.end());
    map = std::move(out);
}

/**
 * Build the binary
**/
void _1Dlsd::make(char *f) {
    auto out = Util::outfile_t(f);

    DBG_FMT("-[_1Dlsd]-\t~[+]~ Dumping to [%s]\n", f);
    for (auto& [off, e] : lyss->map) {
        // if (!llist_empty(&e.px))
        // {
        //     ll_each_value(&e.px, patch)
        //     {

        //     }
        // }
        if (e.t == ELF_Dyn)
            DBG_FMT("        -> @0x%llx - size 0x%lx\n", off, e.mm->sz);
        out.pwrite(e.mm->mem, e.mm->sz, off);
    }
}
};


namespace SlitYaX86 {
struct _1Dlsd_entry *Lyss::get_by_type(enum _1DLSD_TYPE t, __u64 off) {
    struct _1Dlsd_entry *retn=nullptr;
    feach(t, [&](void *_, struct _1Dlsd_entry *e) {
        if (e->src.off >= off) { retn=e; return; };
    });
    return retn;
}
};