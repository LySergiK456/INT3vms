#include <elf.h>
#ifndef KRACK_ELF_H
#define KRACK_ELF_H
#include "../types.h"
#include "../common/llist.h"

#define ELF_STATUS		int
#define ELF_OK			0
#define ELF_BAD_PHDR	22

enum ElfArchType
{
	ELF_X86=1, ELF_X86_64,
};

enum ElfExecType
{
	TypeNone, EXEC, REL, PIE, DeviceDriver, CoreDump,
};

#define ElfUn(t, e)					\
		union {						\
			Elf64_##t	*e##64;		\
			Elf32_##t	*e##32;		\
			void		*e;			\
		}

typedef ElfUn(Nhdr*, nhdr)	ElfU_Nhdr;
typedef ElfUn(Shdr, shdr)	ElfU_Shdr;
typedef ElfUn(Rela, rela)	ElfU_Rela;
typedef ElfUn(Rel, rel)	ElfU_Rel;
typedef ElfUn(Sym, sym)	ElfU_Sym;
typedef ElfUn(Dyn, dyn)	ElfU_Dyn;

typedef struct _elfobj_t {
	struct {
		char		*name;
		__u64		size;
	} f;
	char			*runpath;

	ElfArchType		arch;
	ElfExecType		type;
	union {
		void 		*map;
		__u8		*mem;
		ElfUn(Ehdr,	ehdr);
	};
	__u64			base_virt;

	ElfUn(Phdr, phdr);
	ElfUn(Shdr, shdr);
	LLIST_OF(ElfU_Rela)		ll_rela;
	LLIST_OF(ElfU_Rel)		ll_rel;
	LLIST_OF(ElfU_Sym)		ll_sym;
	LLIST_OF(ElfU_Sym)		ll_dynsym;
	LLIST_OF(ElfU_Nhdr)		ll_note;
	LLIST_OF(char*)			ll_libs;

	struct {
		char			*interp;
		ElfU_Shdr		*plt, *got, *got_plt;
		ElfU_Sym 		*dynsym, *symtab;
		ElfU_Dyn		*dynamic;
		
		ElfUn(Shdr,	str_shdr);
		char	*strtab;
	} e_usr;

	// union
	// {	/** C++ only **/
	// 	std::map<__u8 *, ElfN_Shdr>		sec_map;
	// 	void *_padding;
	// };
// public:
// 	Elf(char *file);

// 	bool hasPhdr() { return !!ehdr->e_phoff; }
// 	bool hasShdr() { return !!ehdr->e_shoff; }

// private:
// 	ELF_STATUS initArch();
// 	ELF_STATUS load(char *file);
// 	ELF_STATUS init();
// 	ELF_STATUS init_segments();
// 	ELF_STATUS init_sections();
} elfobj_t;

#define ELF_PHDR_ALIGN(ph)			((ph)->p_vaddr + ((ph)->p_align-1) & ~((ph)->p_align-1))
#define ELF_CASE(elf, a, b) ({					\
		__u64 _r3tn_ = 0;						\
		if ((elf)->IsX86()) _r3tn_=(elf)->##a;	\
		if ((elf)->IsX64()) _r3tn_=(elf)->##b;	\
		(_r3tn_);								\
	});

#define ELF_SZOF(elf, x) ({								\
		__u64 __r3t__=0;								\
		if ((elf)->IsX86()) __r3t__=sizeof(Elf32_##x);	\
    	if ((elf)->IsX64()) __r3t__=sizeof(Elf64_##x);	\
    	(__r3t__);										\
    });

#define AUXV_EACH(auxv, a, num)									\
		for (Elf64_auxv_t *a=(auxv); (void*)a < (void*)&(auxv)[(num)]; a++)

#define elfx64_each_ph(elf, p)									\
		for (Elf64_Phdr *p=(elf)->phdr64;						\
			(void*)p < (void*)(&(elf)->phdr64[(elf)->ehdr64->e_phnum]);	\
			p++)

#define elfx86_each_ph(elf, p)									\
		for (Elf32_Phdr *p=(elf)->phdr32;						\
			(void*)p < (void*)(&(elf)->phdr32[(elf)->ehdr32->e_phnum]);	\
			p++)

#define elfx64_each_sh(elf, s)									\
		for (Elf64_Shdr *s=(elf)->shdr64;						\
			(void*)s < (void*)(&(elf)->shdr64[(elf)->ehdr64->e_shnum]);	\
			s++)


#define elfx86_each_sh(elf, s)									\
		for (Elf32_Shdr *s=(elf)->shdr32;						\
			(void*)s < (void*)(&(elf)->shdr32[(elf)->ehdr32->e_shnum]);	\
			s++)

#define elfx64_each_dynamic(elf, dyn)			\
        for (Elf64_Dyn *dyn = (elf)->e_usr.dynamic->dyn64;	\
            !!dyn->d_tag && !!dyn->d_un.d_val;	\
            dyn++)

#define elfx86_each_dynamic(elf, dyn)			\
        for (Elf32_Dyn *dyn = (elf)->dynamic;	\
            !!dyn->d_tag && !!dyn->d_un.d_val;	\
            dyn++)


#endif
