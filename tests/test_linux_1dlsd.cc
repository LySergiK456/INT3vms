#include <memory>
#include "../include/1d-lsd/linux/assid.hh"
#include "../include/1d-lsd/binary.h"
#include "../include/1d-lsd/yayo.hh"
#include "../include/syx86/loader/linux/linux_ldr.hh"
#include "../include/elf/elf.hh"

namespace SYX = SlitYaX86;
using LSD = SlitYaX86::_1Dlsd;

#define LSD_X64(x)	Elf64_##x
#define LSD_X86(x)	Elf32_##x

struct ObfuscA
	: SYX::acid_pass
{
    ObfuscA() = default;
    LSD_STATUS _init_(LSD *lsd);
};

LSD_STATUS ObfuscA::_init_(LSD *lsd) {
	// lsd->insert();
	lsd->parse<Phdr>([&](__u64 off, struct _1Dlsd_entry& e) {
		printf("OI!\n");
	});
    return LSD_OK;
}

int main(int argc, char **argv) {
	auto elf = std::make_shared<SYX::ElfObj>(const_cast<char*>("./specimen/a.out"), ELF_SYMTAB_DIR);
	auto pass = ObfuscA();
	auto lsd = SYX::x1Dlsd(pass, elf);
	lsd.run();
	auto ldr = SYX::UnixLdr(argc-1, &argv[1]);
	xx(ldr.ok);
	
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
