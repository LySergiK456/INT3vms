#include "../../include/syx86/syx86.hh"
#include <assert.h>
#include <windows.h>
extern "C" {
    #include "../../include/syx86/syscall/windows/win_syscall.h"
};

namespace SlitYaX86 {
    shadow_mem_t *Ldr::VAtoShadow(__u64 virt) {
        for (auto sh_mem : shadow) {
            if (!!_contain_(sh_mem->uc_mem->addr, sh_mem->uc_mem->size, virt)) {
                return sh_mem;
            }
        }
        return nullptr;
    }
};

__s8 SlitYaX86::wine_name(char *ret, __u64 size, char *dll_name) {
#if defined (_WIN32)
    if (!ret)
        return -1;

    memset(ret, 0, size);
    if (strlen(WIN_SYSTEM32_PATH) + strlen(dll_name) + 1 > size)
        return -1;

    memcpy(ret, WIN_SYSTEM32_PATH, strlen(WIN_SYSTEM32_PATH));
    strcat(ret, dll_name);
    for (__u64 i = 0; i < strlen(ret); i++) {
        ret[i] = tolower(ret[i]);
    }

    assert(strlen(ret) + 1 <= size);

    return 0;
#endif
}
