#include <stdio.h>
#include "./include/common/mem.h"
#include "./include/syx86/syx86.hh"
#include "./include/syx86/uc.hh"
#include "./include/syx86/dex/dbgserver.hh"
#include "./include/syx86/dex/dex.hh"
#include "./include/syx86/loader/windows/win_ldr.hh"
#include "./include/common/shellcode.hh"
extern "C" {
    #include "./include/common/utils.h"
};

namespace SYX = SlitYaX86;

int main(int argc, char **argv) {
    // _TEB_ *teb = reinterpret_cast<_TEB_*>(__readgsqword(0x30));
    // _PEB_ *peb = reinterpret_cast<_PEB_*>(__readgsqword(0x60));
    // assert((void*)teb->PebAddress == (void*)peb);

    // _TEB_ orig_teb, virt_teb;
    // memcpy(reinterpret_cast<char*>(&orig_teb), teb, sizeof(_TEB_));
    // _uc_assert(uc_mem_read(r->ldr.uc, r->ldr.teb_addr, (void*)&virt_teb,
    // sizeof(_TEB_)));

    /* setup TEB properly inside ldr/ - */
    // memcpy(teb, &virt_teb, sizeof(_TEB_));
    // fucking shadow memory

    auto razor = SYX::Razor<SlitYaX86::WinDex, SlitYaX86::WinLdr>(argc - 1, &argv[1]);
    // auto razor = SYX::Razor(argc-1, &argv[1]);
    // printf("++%lx\n", razor->ldr.ok);
    uc_err err = razor.Run();
    if (!_uc_ok(err)) {
        puts(uc_strerror(err));
        return -1;
    }

    // auto dbgserver = DbgTcp(std::make_shared<SYX::Razor>(&razor), DEBUG_PORT);
    // dbgserver.Run();
    return 0;
}
