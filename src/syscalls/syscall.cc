#include "../../include/syx86/loader/win_ldr.hh"
#include <assert.h>
#include <windows.h>
extern "C" {
	#include "../../include/syx86/syscall.h"
};

__attribute__((constructor)) void SlitYerX86::init_wine_syscalls() {
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    assert(HANDLE_OK(ntdll));

    WINE_EACH_SYSCALL(e) {
        assert(!!e && !!e->name); // "Nullptr in Wine-SyscallTable")
        e->fn = (void *)GetProcAddress(ntdll, e->name);

        if (!e->fn) DBG_FMT("[!] Failed to GetProcAddress of \"%s\"\n", e->name);
    }
}
