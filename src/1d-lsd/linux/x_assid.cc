#if defined(__linux__)
#include <elf.h>
#endif
#include "../../../include/1d-lsd/linux/assid.hh"
#include "../../../include/1d-lsd/1dlsd.hh"
#include "../../../include/conf.hh"
#include "../../../include/common/utils.hh"

namespace SlitYaX86
{
    XLyss::XLyss(x1Dlsd *xlsd, std::shared_ptr<ElfObj> elf)
        :   Lyss(xlsd, *dynamic_cast<Binary_t*>(elf.get())),
            lsd{xlsd},base_elf{elf}
    {
        Binary_t *bin = dynamic_cast<Binary_t*>(elf.get());
        void *pMem = elf->mem;
        /**
         *  Push Elf-Header */
        lyss.push_ptr(
            bin,
            ELF_Ehdr,
            new_lyss(
                elf->mem,
                0,
                sizeof(Elf64_Ehdr)));

        __u64 ph_ndx{};
        elfx64_each_ph(elf, ph)
        {   /** Push Each Elf_Phdr and it's content's **/
            lyss.push_ptr(
                bin,
                ELF_Phdr,
                new_lyss(
                    elf->mem,
                    elf->ehdr64->e_phoff + (ph_ndx++*sizeof(Elf64_Phdr)),
                    sizeof(Elf64_Phdr)));
            /**
             *  We got protections
            **/
            __u32 perms{};
            if (ph->p_flags&PF_R) perms |= LSD_R;
            if (ph->p_flags&PF_W) perms |= LSD_W;
            if (ph->p_flags&PF_X) perms |= LSD_X;

            lyss.push_ptr(
                bin,
                static_cast<enum _1DLSD_TYPE>(perms),
                new_lyss(pMem, ph->p_offset, ph->p_filesz));
        }

        __u64 sh_ndx{};
        elfx64_each_sh(elf, sec)
        {   /** Same for Sections. Header's and it's contents **/
            lyss.push_ptr(
                bin,
                ELF_Shdr,
                new_lyss(
                    pMem,
                    elf->ehdr64->e_shoff + (sh_ndx++*sizeof(Elf64_Shdr)),
                    sizeof(Elf64_Shdr)));

            lyss.push_ptr(
                bin,
                LSD_NONE,
                new_lyss(
                    pMem,
                    sec->sh_offset,
                    ALIGN_UP(sec->sh_size, sec->sh_addralign)));
        }
    }
};

namespace SlitYaX86 {
__u64 XLyss::_get_size(enum _1DLSD_TYPE t) {
    auto s = base_elf.get();
    __u64 v = 0;
    switch (t) {
        case ELF_Ehdr:
            v = (s->IsX86()) ? sizeof(Elf32_Ehdr)
            :   (s->IsX64()) ? sizeof(Elf64_Ehdr) : -1;
            break;
        case ELF_Phdr:
            v = (s->IsX86()) ? sizeof(Elf32_Phdr)
            :   (s->IsX64()) ? sizeof(Elf64_Phdr) : -1;
            break;
        case ELF_Shdr:
            v = (s->IsX86()) ? sizeof(Elf32_Shdr)
            :   (s->IsX64()) ? sizeof(Elf64_Shdr) : -1;
            break;
        case ELF_Rela:
            v = (s->IsX86()) ? sizeof(Elf32_Rela)
            :   (s->IsX64()) ? sizeof(Elf64_Rela) : -1;
            break;
        case ELF_Rel:
            v = (s->IsX86()) ? sizeof(Elf32_Rel)
            :   (s->IsX64()) ? sizeof(Elf64_Rel) : -1;
            break;
        case ELF_Sym:
            v = (s->IsX86()) ? sizeof(Elf32_Sym)
            :   (s->IsX64()) ? sizeof(Elf64_Sym) : -1;
            break;
        case ELF_Dyn:
            v = (s->IsX86()) ? sizeof(Elf32_Dyn)
            :   (s->IsX64()) ? sizeof(Elf64_Dyn) : -1;
            break;
    }
    return v;
}

void XLyss::align_bin(__u64 off, __u64 sz) {
    auto e_ehdr = lyss.get_by_type(ELF_Ehdr, 0);
    assert(!!e_ehdr);

    auto ehdr = (Elf64_Ehdr*)e_ehdr->mm->mem;
    if (ehdr->e_shoff > off) ehdr->e_shoff += sz;
    if (ehdr->e_phoff > off) ehdr->e_phoff += sz;

    lyss.feach(ELF_Phdr, [&](Elf64_Phdr *_p, struct _1Dlsd_entry *e) {
        if (_contain_(_p->p_offset, _p->p_filesz, off)) {
            _p->p_memsz     += sz;
            _p->p_filesz    += sz;
        } else if (_p->p_offset >= off) {
            _p->p_offset    += sz;
            _p->p_paddr     += sz;
            _p->p_vaddr     += sz;
        }
    });
    lyss.feach(ELF_Shdr, [&](Elf64_Shdr *sec, struct _1Dlsd_entry *e) {
        if (_contain_(sec->sh_offset, sec->sh_size, off)) {
            sec->sh_size    += sz;
        } else if (sec->sh_offset >= off) {
            sec->sh_addr    += sz;
            sec->sh_offset  += sz;
        }
    });
}

};
