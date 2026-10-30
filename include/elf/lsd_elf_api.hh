#include "../1d-lsd/1dlsd.hh"
#include "../1d-lsd/linux/assid.hh"

using T = bin_hdr_t;

struct _elf_object : public SlitYaX86::ElfObj {
	T  ehdr;
	T  phdr;
	T  shdr;
	T  dynamic;
    T  interp;
	LLIST_OF(T)    ll_symtab;
	LLIST_OF(T)    ll_dyntab;
	LLIST_OF(T)    ll_rela;
	LLIST_OF(T)    ll_rel;
	LLIST_OF(T)    ll_unknown;
	__u8           *&pMem{elf.mem};

	_elf_object(char *file, char *symtab_dir)
        : ElfObj(file, symtab_dir)
    {
        llist_init(&ll_symtab,  sizeof(bin_hdr_t));
        llist_init(&ll_dyntab,  sizeof(bin_hdr_t));
        llist_init(&ll_rela,    sizeof(bin_hdr_t));
        llist_init(&ll_rel,     sizeof(bin_hdr_t));
        llist_init(&ll_unknown, sizeof(bin_hdr_t));
    	auto e = dynamic_cast<SlitYaX86::ElfObj*>(this);

        ehdr = dynamic_cast<T>(BinHdr(e->ehdr, 0 sizeof(Elf64_Ehdr)));
        phdr = dynamic_cast<T>(BinHdr(
                e->phdr,
                e->ehdr64->e_phoff,
                e->ehdr64->e_phnum*sizeof(Elf64_Phdr)));
        shdr = dynamic_cast<T>(BinHdr(
                e->shdr,                                    // void *ptr
                e->ehdr64->e_shoff,                         // __u64 offset
                e->ehdr64->e_shnum*sizeof(Elf64_Shdr)));    // Size

        elfx64_each_ph(this, ph)
        {
            __u64 off{ph->p_offset}, size{size};
            auto hdr = BinHdr(&e->mem[off], off, size);
            switch (ph->p_type)
            {
                case PT_PHDR:
                {   /** Just check wheather PHDR in PT_PHDR is
                     *  the same as the one pointed to by ElfHdr
                    **/
                    if (e->IsX86()) {
                        assert(off == e->ehdr32->e_phoff
                            && size == (e->ehdr32->e_phnum * sizeof(Elf32_Phdr)));
                    }
                    if (e->IsX64()) {
                        assert(off == e->ehdr64->e_phoff
                            && size == (e->ehdr64->e_phnum * sizeof(Elf64_Phdr)));
                    }
                };
                break;
                case PT_LOAD:        break;
                case PT_DYNAMIC:
                    dynamic = dynamic_cast<T>(bin_hdr);
                    break;
                case PT_INTERP:
                    interp = dynamic_cast<T>(bin_hdr);
                    break;
                case PT_NOTE:        break;
                case PT_GNU_STACK:   break;

                case PT_NULL:
                case PT_LOPROC:
                case PT_HIPROC:
                case PT_SHLIB:
                {   /** Reserved **/
                break;
                };
            }
        }
    }
}
