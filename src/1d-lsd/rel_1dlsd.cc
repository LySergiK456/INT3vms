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

/** 4 Yayo **/
// LSD_API __u64 lsd_align(struct rel_yayo *ctx, __u64 offset) {
//     return offset;
// }
namespace SlitYaX86
{
void Lyss::align_map(__u64 off, __u64 sz) {
    for (auto& [k, emm] : map) {
        if (_contain_(k, emm.mm->sz, off)) {
            map[k].mm->sz += sz;
        } else if (k >= off) {
            map.erase(k);
            map[k+sz] = emm;
        }
    }
}

/**
 * Build the binary
**/
void _1Dlsd::make(char *f) {
    auto out = Util::outfile_t(f);

    DBG_FMT("-[_1Dlsd]-\t~[+]~ Dumping to [%s]\n", f);
    for (auto& [off, e] : lyss->map) {
         if (!llist_empty(&e.px))
        {
            ll_each_value(&e.px, patch)
            {

            }
        }
        // DBG_FMT("        -> @0x%llx - size 0x%lx\n", off, e.mm->sz);
        out.pwrite(e.mm->mem, e.mm->sz, off);
    }
}
};


namespace SlitYaX86 {
struct _1Dlsd_entry *Lyss::get_by_type(enum _1DLSD_TYPE t, __u64 off) {
    struct _1Dlsd_entry *retn=nullptr;
    feach(t, [&](Elf64_Phdr *ph, struct _1Dlsd_entry *e) {
        if (e->src.off >= off) { retn=e; return; };
    });
    return retn;
}
};