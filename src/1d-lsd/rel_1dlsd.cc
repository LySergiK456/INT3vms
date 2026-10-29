// #include <windows.h>
// #include "../../include/pe64/pe64.hh"
#if defined(__linux__)
#include <elf.h>
#endif
#include "../../include/1d-lsd/linux/assid.hh"
#include "../../include/1d-lsd/1dlsd.hh"
#include "../../include/conf.hh"
#include "../../include/common/utils.hh"
extern "C" {
    #include "../../include/1d-lsd/yayo.h"
};

namespace SlitYaX86
{
    XLyss::XLyss(x1Dlsd *xlsd, std::shared_ptr<ElfObj> elf)
        :   Lyss(xlsd, *dynamic_cast<Binary_t*>(elf.get())),
            lsd{xlsd},base_elf{elf}
    {
        lyss.push_ptr(
            dynamic_cast<Binary_t*>(elf.get()),
            Ehdr,
            new_Lyss_entry(elf->mem, 0, sizeof(Elf64_Ehdr)));

        __u64 i{};
        elfx64_each_ph(elf, ph) {
            __u64 ph_off = elf->ehdr64->e_phoff + (++i*sizeof(Elf64_Phdr));
            void *ph_ptr = reinterpret_cast<void*>(elf->mem[ph_off]);
            lyss.push_ptr(
                dynamic_cast<Binary_t*>(elf.get()),
                Phdr,
                new_Lyss_entry(ph_ptr, ph_off, sizeof(Elf64_Phdr)));
        }
    }
};

namespace SlitYaX86 {
__u64 XLyss::_get_size(enum _1DLSD_TYPE &t) {
    auto s = base_elf.get();
    __u64 v = 0;
    switch (t) {
        case Phdr:
            v = (s->IsX86()) ? sizeof(Elf32_Phdr)
            :   (s->IsX64()) ? sizeof(Elf64_Phdr) : -1;
            break;
        case Shdr:
            v = (s->IsX86()) ? sizeof(Elf32_Shdr)
            :   (s->IsX64()) ? sizeof(Elf64_Shdr) : -1;
            break;
        case Rela:
            v = (s->IsX86()) ? sizeof(Elf32_Rela)
            :   (s->IsX64()) ? sizeof(Elf64_Rela) : -1;
            break;
        case Rel:
            v = (s->IsX86()) ? sizeof(Elf32_Rel)
            :   (s->IsX64()) ? sizeof(Elf64_Rel) : -1;
            break;
        case Sym:
            v = (s->IsX86()) ? sizeof(Elf32_Sym)
            :   (s->IsX64()) ? sizeof(Elf64_Sym) : -1;
            break;
        case Dyn:
            v = (s->IsX86()) ? sizeof(Elf32_Dyn)
            :   (s->IsX64()) ? sizeof(Elf64_Dyn) : -1;
            break;
    }
    return v;
}
};

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
};


namespace SlitYaX86 {
__s8 _1Dlsd::make(char *f) {
    auto out = Util::outfile_t(f);

    DBG_FMT("-[_1Dlsd]-\t~[+]~ Dumping to [%s]\n", f);
    for (auto& [off, e] : lyss->map) {
    DBG_FMT("        -> @0x%llx - size 0x%lx\n", off, e.mm->sz);
        out.pwrite(e.mm->mem, e.mm->sz, off);
    }

    return 0;
}
};
