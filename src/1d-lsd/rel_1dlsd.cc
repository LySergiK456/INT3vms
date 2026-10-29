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

std::map<__u64, struct _1Dlsd_entry> &Lyss::operator()() {
    return map;
}
};

LSD_API __u64 lsd_align(struct rel_yayo *ctx, __u64 offset) {
    return offset;
}

namespace SlitYaX86
{
bin_hdr_t *new_Lyss_entry(void *ptr, __u64 off, __u64 sz) {
    auto e = new bin_hdr_t();
    e->mm = std::move(mm_t{(void*)((char*)ptr + off), MM_HEAP, sz});
    e->off  = off;
    return e;
}
};

namespace SlitYaX86
{
void Lyss::SliceSegments() {
    using F = offsz_t;
    F *prev{};
    std::list<F> ll;
    
    for (auto& [off, e] : map) {
        ll.push_back({off, e.mm->sz});
    }
    ll.push_back({ sizeof(Elf64_Ehdr), 0 });
    ll.sort([](F& a, F& b) { return a.off < b.off; });

    for (auto& e : ll) {
        __u64 p = (!!prev ? prev->off + prev->sz : 0);
        if (p < e.off) {
            map[p].mm = new mm_t({
                (void*)&bin.src.mem[p],
                MM_NONE,
                e.off-p,
            });
            map[p].src.img  = &bin;
            map[p].src.off  = p;
            map[p].src.sz   = e.off-p;
        }
        prev = &e;
    }
}

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

struct _1Dlsd_entry *Lyss::get_by_type(enum _1DLSD_TYPE t, __u64 off) {
    struct _1Dlsd_entry *retn=nullptr;
    feach(t, [&](void *ptr, struct _1Dlsd_entry& e){
        if (e.src.off >= off) {retn=&e; return;}
    });
    return retn;
}
/**
 * Build the binary
**/
void _1Dlsd::make(char *f) {
    auto out = Util::outfile_t(f);

    DBG_FMT("-[_1Dlsd]-\t~[+]~ Dumping to [%s]\n", f);
    for (auto& [off, e] : lyss->map) {
        DBG_FMT("        -> @0x%llx - size 0x%lx\n", off, e.mm->sz);
        out.pwrite(e.mm->mem, e.mm->sz, off);
    }
}
};
