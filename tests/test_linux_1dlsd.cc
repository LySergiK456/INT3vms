#include <elf.h>
#include <memory>
#include "../include/1d-lsd/linux/assid.hh"
#include "../include/1d-lsd/binary.h"
#include "../include/1d-lsd/yayo.hh"
#include "../include/syx86/loader/linux/linux_ldr.hh"
#include "../include/elf/elf.hh"

namespace SYX = SlitYaX86;
using XLSD = SlitYaX86::x1Dlsd;

#define LSD_X64(x)	Elf64_##x
#define LSD_X86(x)	Elf32_##x

struct ObfuscA
	: SYX::acid_pass
{
    ObfuscA() = default;
    LSD_STATUS _init_(XLSD *lsd);
};

static char *SC =
	"\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc"
	"\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc"
	"\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc"
	"\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc"
	"\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc"
	"\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc"
	"\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc"
	"\xcc\xcc";

LSD_STATUS ObfuscA::_init_(XLSD *lsd) {
	auto phdr_st = lsd->lyss->get_by_type(ELF_Phdr, 0);
	assert(!!phdr_st);
	lsd->lyss->insert(
		ELF_Phdr,	// Type
		(void*)SC,	// The fucking shellcode which shall reide as a Phdr Entry
		56,			// SizeOf Shellcode
		phdr_st,	// struct _1Dlsd_entry *
		0,			// very first of all PHDR's
		reinterpret_cast<Elf64_Phdr*>(phdr_st->mm->mem)	// Needs the fucking typecasting
	);
	// lsd->parse<ELF_Phdr>([&](__u64 off, struct _1Dlsd_entry& e) {
	// 	auto p = reinterpret_cast<Elf64_Phdr*>(e.mm->mem);
	// 	switch (p->p_type) {
	// 	case PT_LOAD:
	// 	case PT_PHDR:
	// 	case PT_DYNAMIC:
	// 	case PT_INTERP:
	// 	case PT_GNU_PROPERTY:
	// 	case PT_GNU_EH_FRAME:
	// 	case PT_GNU_STACK:
	// 	case PT_GNU_RELRO:		break;
	// 	default:
	// 		__u32 perm{};
	// 		__wr_init(
	// 			reinterpret_cast<void*>(PAGE_ALIGN_DOWN(p)),
	// 			PAGE_SIZE,
	// 			&perm);

	// 		p->p_offset	= 0xccccccccdeadbeef;
	// 		p->p_filesz	= 0xccccccccbeefcafe;
	// 		p->p_vaddr	= 0xcccccc0ffeadd1c7;
	// 		p->p_memsz	= 0xacab4deadccccccc;

	// 		__wr_exit(
	// 			reinterpret_cast<void*>(PAGE_ALIGN_DOWN(p)),
	// 			PAGE_SIZE,
	// 			perm);
	// 	break;
	// 	}
	// });
    return LSD_OK;
}

int main(int argc, char **argv) {
	auto elf = std::make_shared<SYX::ElfObj>(const_cast<char*>("./specimen/a.out"), ELF_SYMTAB_DIR);
	auto pass = ObfuscA();
	auto lsd = SYX::x1Dlsd(&pass, elf);
	lsd.run();
	lsd.make("/tmp/hello.out");
	// lsd.run();
	// auto ldr = SYX::UnixLdr(argc-1, &argv[1]);
	// xx(ldr.ok);
	
	// Elf64_Shdr *x_sec{nullptr};
	// elfx64_each_sh(dynamic_cast<elfobj_t*>(elf.get()), sec) {
	// 	if (sec->sh_offset > 0  && !!(sec->sh_flags & SHF_EXECINSTR)) {
	// 		x_sec = sec;
	// 		break;
	// 	}
	// }
	// auto y = SYX::Yayo(dynamic_cast<SYX::Binary_t*>(elf.get()), x_sec->sh_offset, x_sec->sh_size);
	// y._rel_(
	// 	YAYO_REPLACE,
	// 	(void*)"\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc"
	// 	"\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc",
	// 	x_sec->sh_offset+0x10,
	// 	0x10);
	// y._rel_();
	return 0;
}
