#ifndef KRACK_ELF_HH
#define KRACK_ELF_HH

#include "../1d-lsd/binary.hh"

extern "C" {
	#include "./elf.h"
};

namespace SlitYaX86 {
	class ElfObj : public elfobj_t, public Binary_t {
		elfobj_t		*self{dynamic_cast<elfobj_t*>(this)};
		Binary_t		*bin{dynamic_cast<Binary_t*>(this)};
	public:
		__s8		ok{0};

		ElfObj() = delete;
		ElfObj(char *file, char *symtab_dir);
		constexpr bool IsX64();
		constexpr bool IsX86();
		__u64 virtBase();
		bool is_overflow(__u64 off);
	private:
		__s8 load_elf();
		__s8 parse_all();
		void parse_dynamic();
	};
};
#endif