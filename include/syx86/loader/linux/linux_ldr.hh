#include <fcntl.h>
#include <linux/fs.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <memory>
#include <unicorn/unicorn.h>
#ifndef SYX86_LINUX_LDR_HH
#define SYX86_LINUX_LDR_HH
#include "../../syx86.hh"
#include "../../uc.hh"
#include "../../../elf/elf.hh"
#include "../../../common/utils.hh"
extern "C" {
    #include "../../../common/utils.h"
};

namespace SlitYaX86::SelfMaps {
	static int fdProcSelfMap{-1};
	__attribute__((constructor)) void init_self_maps();
};

namespace SlitYaX86 {
	class UnixLdr;
};

namespace SlitYaX86 {
class Stack {
	UnixLdr		*ldr;
public:
	__u64 rsp, rbp;
	Stack() = default;
	Stack(UnixLdr *_ldr, __u64 _rsp) : ldr{_ldr} { rbp=rsp=_rsp; }
	__u64 ptr() { return rsp; }
	template<typename T> void push(T  b) {
		rsp -= sizeof(b);
		ldr->mmemcpy(rsp, (void*)&b, sizeof(b));
	}
	void push_str(std::string& str) {
		push_str(str.c_str());
	}
	template<typename T> void push_str(T *str) {
		for (__u64 i = strlen(str)+1; !!(i+1); i--)
			push(((char*)str)[i]);
	}
};
};

namespace SlitYaX86 {
class MKStack {
	Stack									*sm;
	std::vector<std::pair<__u64, char*>>	m_env, m_arg;
public:
	MKStack(Stack*, __u32, char**, char**, Elf64_auxv_t*, __u16);

private:
	Util::StrTab *sm_strtab();
	void init_str(char **argv, char **envp, Elf64_auxv_t *auxv);
	void Build(__u32, char**, char**, Elf64_auxv_t*, __u16);
	std::pair<__u64, char*> &find(char *x, auto& vec);
};
};

namespace SlitYaX86
{
	class UnixLdr : public Ldr {
		struct
		{	/* Interpreter shit */
			char						*name;
			std::shared_ptr<ElfObj>		elf;
		} interp;
		struct {
			__u64	top{0x7ffffff00000ULL};
			__u64	size{0x800000};
		} stack;
		Stack		sm;	// Stack Machine
		std::shared_ptr<ElfObj>				main_elf;
		std::list<std::shared_ptr<ElfObj>>	elfs;
	public:
		UnixLdr() = default;
		UnixLdr(int argc, char **argv);

	private:
		const char* elf_interp(std::shared_ptr<ElfObj> elf);
		std::shared_ptr<ElfObj>& elfs_push(char *name, char *sym_dir);
		uc_err init_stack(__u64 addr, __u64 sz);
		void init_base_stack(__u32, char**, char**, Elf64_auxv_t*, __u16);
		uc_err init_proc_env();
		uc_err load_new_elf(std::shared_ptr<ElfObj> elf, __u64 base);
	};

	static bool ElfIsSupported(ElfObj& elf);
};
#endif
