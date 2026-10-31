#include <elf.h>
#include <memory>
#include "../../../../include/conf.hh"
#include "../../../../include/syx86/syx86.hh"
#include "../../../../include/syx86/loader/linux/linux_ldr.hh"

namespace SlitYaX86
{
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