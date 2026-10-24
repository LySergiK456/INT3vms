#ifndef KRACK_ELF_H
#define KRACK_ELF_H

#define ELF_STATUS		int
#define ELF_OK			0
#define ELF_BAD_PHDR	22

enum ElfArchType
{
	X86=1, X86_64,
};

enum ElfExecType
{
	None, EXEC, REL, PIE, DeviceDriver, CoreDump,
};

#define ElfUn(t, e)					\
		union {						\
			Elf64_##t	*e##64;		\
			Elf32_##t	*e##32;		\
			void		*e;			\
		}

typedef ElfUn(Nhdr*, nhdr)	ElfU_Nhdr;
typedef ElfUn(Shdr*, shdr)	ElfU_Shdr;
typedef ElfUn(Rela*, rela)	ElfU_Rela;
typedef ElfUn(Rel*, rel)	ElfU_Rel;
typedef ElfUn(Sym*, sym)	ElfU_Sym;
typedef ElfUn(Dyn*, dyn)	ElfU_Dyn;

class Elf {
	struct {
		char		*name;
		__u64		size;
	} f;

	ElfArchType		arch;
	ElfExecType		type;
	union {
		void 		*map;
		__u8		*mem;
		ElfUn(Ehdr*,	ehdr);
	};
	Elf				*self{this;}
	__s8			ok{};

	ElfUn(Phdr*, phdr);
	ElfUn(Shdr*, shdr);
	LLIST_OF(ElfU_Rela)		ll_rela;
	LLIST_OF(ElfU_Rel)		ll_rel;
	LLIST_OF(ElfU_Sym)		ll_sym;
	LLIST_OF(ElfU_Sym)		ll_dynsym;
	LLIST_OF(ElfU_Nhdr)		ll_note;

	struct {
		char			*interp;
		ElfX_Shdr		*plt, *got, *got_plt;
		ElfX_Sym 		*dynsym, *symtab;
		ElfX_Dyn		*dynamic;

		ElfU(Shdr,	shdr);
		ElfU(Shdr,	str_shdr);
		__u8	*strtab;
	} e_usr;

	// union
	// {	/** C++ only **/
	// 	std::map<__u8 *, ElfN_Shdr>		sec_map;
	// 	void *_padding;
	// };
public:
	Elf(char *file);

	bool hasPhdr() { return !!ehdr->e_phoff; }
	bool hasShdr() { return !!ehdr->e_shoff; }

private:
	ELF_STATUS initArch();
	ELF_STATUS load(char *file);
	ELF_STATUS init();
	ELF_STATUS init_segments();
	ELF_STATUS init_sections();
}

#define elfx64_each_ph(elf, p)									\
		for (Elf64_Phdr *p=(elf)->phdr64;						\
			(void*)p < (void*)(&(elf)->phdr64[(elf)->e_phnum]);	\
			p++)

#define elfx86_each_ph(elf, p)									\
		for (Elf32_Phdr *p=(elf)->phdr32;						\
			(void*)p < (void*)(&(elf)->phdr32[(elf)->e_phnum]);	\
			p++)

#define elfx64_each_sh(elf, s)									\
		for (Elf64_Shdr *s=(elf)->shdr64;						\
			(void*)s < (void*)(&(elf)->shdr64[(elf)->e_shnum]);	\
			s++)


#define elfx86_each_sh(elf, s)									\
		for (Elf32_Shdr *s=(elf)->shdr32;						\
			(void*)s < (void*)(&(elf)->shdr32[(elf)->e_shnum]);	\
			s++)

#endif
