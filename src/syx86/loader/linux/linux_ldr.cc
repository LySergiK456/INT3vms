#include <elf.h>
#include <memory>
#include "../../../../include/conf.hh"
#include "../../../../include/syx86/syx86.hh"
#include "../../../../include/syx86/loader/linux/linux_ldr.hh"
extern "C" {
	#include "../../../../include/common/linux/proc_self_maps.h"
	#include "../../../../include/common/mem.h"
	#include "../../../../include/common/utils.h"
};

namespace SlitYaX86 {
	UnixLdr::UnixLdr(int argc, char **argv)
		:	Ldr(argc, argv),
			sm(this, stack.top-stack.size)
	{
		__u64 off{0};
		uc_err err{UC_ERR_OK};
		if (!_uc_okay(_uc_init_x86_64_(&uc), ok)) {
			DBGS("[!] Failed to init Unicorn-Engine");
			return;
		}
		err = init_proc_env();
		if (!_uc_okay(err, ok)) {
			DBGS("[!] Failed to initialize process environment");
			return;
		}

		DBG_FMT("[+] Loading \"%s\"", argv[0]);
		main_elf = elfs_push(argv[0], ELF_SYMTAB_DIR);

		const char *interp_str = elf_interp(main_elf);
		if (!!interp_str) {
			interp.name	= const_cast<char*>(interp_str);
			interp.elf	= elfs_push(const_cast<char*>(interp_str), ELF_SYMTAB_DIR);
		} else {
			DBGS("[!] The program doesn't require a RT linker\n");
		}

		DBG_FMT("[+] Loading %s\n", main_elf->f.name);
		err = load_new_elf(main_elf, MAIN_ELF_BASE);
		if (!_uc_okay(err, ok)) {
			DBG_FMT("[-] Failed to load main Binary_t [%s]\n", main_elf->f.name);
			return;
		}

		DBG_FMT("[+] Loading %s\n", interp.name);
		err = load_new_elf(interp.elf, RTLD_ELF_BASE);
		if (!_uc_okay(err, ok)) {
			DBG_FMT("[-] Failed to load RTLD [%s]\n", interp.name);
			return;
		}

		// MKStack(&sm, args.num, args.v, nullptr, nullptr, 0);
	}
};


namespace SlitYaX86 {
	Util::StrTab *MKStack::sm_strtab() {
		auto str = new Util::StrTab(sm->rsp, 8);
		str->push("HOME=/home/rax");
		str->push("PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin");
		str->push("USER=rax");
		str->push("LOGNAME=rax");
		str->push("PWD=/home/rax/INT3vms");
		str->push("SHELL=/bin/bash");
		str->push("LANG=en_US.UTF-8");
		str->push("TERM=xterm-256color");
		return str;
	}
};

namespace SlitYaX86
{
const char* UnixLdr::elf_interp(std::shared_ptr<ElfObj> elf) {
	elfx64_each_ph(dynamic_cast<elfobj_t*>(elf.get()), p)
		if (p->p_type == PT_INTERP)
			return (const char*)&main_elf->mem[p->p_offset];

	return nullptr;
}

std::shared_ptr<ElfObj>& UnixLdr::elfs_push(char *name, char *sym_dir) {
	return elfs.emplace_back(std::shared_ptr<ElfObj>{new ElfObj(name, sym_dir)});
}
};

namespace SlitYaX86
{
	uc_err UnixLdr::init_stack(__u64 addr, __u64 sz) {
		return mmap(addr-sz, sz, UC_PROT_RW, "[stack]");
	}

	uc_err UnixLdr::init_proc_env() {
		uc_err err{UC_ERR_OK};
		err = init_stack(stack.top - stack.size, stack.size);
		if (!_uc_okay(err, ok))
			return err;

		return UC_ERR_OK;
	}
};


namespace SlitYaX86
{
	uc_err UnixLdr::load_new_elf(std::shared_ptr<ElfObj> elf, __u64 base) {
		if (!ElfIsSupported(elf))
			return UC_ERR_RESOURCE;

		elfx64_each_ph(dynamic_cast<elfobj_t*>(main_elf.get()), ph) {
			if (ph->p_type == PT_LOAD)
			{	/** Must be PIC **/
				__u64 base_addr = base + ph->p_vaddr;
			
				 __u32 perms = 0;
		        if (ph->p_flags&PF_R) perms |= UC_PROT_READ;
		        if (ph->p_flags&PF_W) perms |= UC_PROT_WRITE;
		        if (ph->p_flags&PF_X) perms |= UC_PROT_EXEC;

		        if (!!(base_addr & PAGE_MASK))
		        {	/** Interleaving PT_LOAD's **/
		        	base_addr = PAGE_ALIGN_DOWN(base_addr);
		        }

				mmap(base_addr, PAGE_ALIGN_UP(ph->p_memsz), perms, elf->f.name);
				mmemcpy(base + ph->p_vaddr, (void*)&elf->mem[ph->p_offset], ph->p_filesz);
			}
		}
		return UC_ERR_OK;
	}
};

namespace SlitYaX86 {
static bool ElfIsSupported(std::shared_ptr<ElfObj> elf) {
	if (!elf->IsPIC) {
		DBG_FMT("[-] ELF is not Position-Independent \"%s\"",
			elf->f.name);
		return false;
	}
	if (!elf->IsX64()) {
		DBG_FMT("[-] ELF is not x86_64 - \"%s\"\n",
			elf->f.name);
		return false;
	}
	return true;
}
};

namespace SlitYaX86
{
MKStack::MKStack(
	Stack *_sm,
	__u32 argc,
	char **argv,
	char **envp,
	Elf64_auxv_t *auxv,
	__u16 auxv_count)
	: sm{_sm}
{
	// auto env_strtab = SM_envp();
	// init_str(argv, &env_strtab->raw, auxv);
	Build(argc, argv, envp, auxv, auxv_count);
}

void MKStack::Build(
	__u32 argc,
	char **argv,
	char **envp,
	Elf64_auxv_t *auxv,
	__u16 auxv_count)
{
	__u64 t{};
	t += ALIGN_UP(sizeof(int), 0x8);
	t += ptrArrSize(argv) * sizeof(char*);
	t += sizeof(NULL);
	t += ptrArrSize(envp) * sizeof(char*);
	t += sizeof(NULL);
	t += auxv_count * sizeof(Elf64_auxv_t);
	t += sizeof(NULL);

	/**	ArgCount[argc] **/
	sm->push(static_cast<__u64>(argc));

	/** Arguments[argv] **/
	for (char **x=argv; !!*x; x++) {
		sm->push(reinterpret_cast<__u64>(sm->rsp+t+std::get<__u64>(find(*x, m_arg))));
	}
	sm->push(0LL);
	/**	Envirnment vars[envp] **/
	for (char **x=envp; !!*x; x++) {
		sm->push(reinterpret_cast<__u64>(sm->rsp+t+std::get<__u64>(find(*x, m_env))));
	}
	sm->push(0LL);

	/** Auxv **/
	AUXV_EACH(auxv, a, auxv_count)
	{	/**	Little Endian **/
		sm->push(a->a_un.a_val);
		sm->push(a->a_type);
	}
	sm->push(0LL);

	/** Set up strings **/
	for (char **x=argv; !!*x; x++) sm->push_str(*x);
	for (char **x=envp; !!*x; x++) sm->push_str(*x);
}

void MKStack::init_str(char **argv, char **envp, Elf64_auxv_t *auxv) {
	__u64 off{};
	for (char **x=argv; !!*x; x++) {
		m_arg.push_back(std::make_pair(off, *x));
		off += (!!*x) ? strlen(*x)+1 : sizeof(0LL);
	}

	for (char **x=envp; !!*x; x++) {
		m_env.push_back(std::make_pair(off, *x));
		off += (!!*x) ? strlen(*x)+1 : sizeof(0LL);
	}
}

std::pair<__u64, char*> &MKStack::find(char *x, auto& vec) {
	auto r = std::find_if(vec.begin(), vec.end(),
		[&](const auto& e){
			return std::get<char*>(e) == x;
		});
	assert(r != vec.end());
	return *r;
}
};
