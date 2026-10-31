#include <algorithm>
#include "../../include/conf.hh"
#include "../../include/syx86/syx86.hh"
#include "../../include/syx86/uc.hh"
#include <assert.h>
#if defined(_WIN32)
# include <windows.h>
#endif
#if defined(__linux__)
# include <sys/mman.h>
# include <unistd.h>
#endif
extern "C" {
    #include "../../include/common/utils.h"
    #include "../../include/common/linux/proc_self_maps.h"
};

static __u64 heap_off = 0;
namespace SlitYaX86
{
__u64 Ldr::uc_malloc(__u64 size) {
    if (!size) return 0;

    __u64 virt = HEAP_ADDR + heap_off;
    const __u64 end = virt + size;
    const __u64 next_off = heap_off + size;

    if (end < virt || next_off < heap_off)
        return 0;

    const __u64 map_begin = PAGE_ALIGN_UP(virt);
    const __u64 map_end = PAGE_ALIGN_UP(end);
    if (map_begin < virt || map_end < end || map_end < map_begin)
        return 0;

    if (map_end > map_begin) {
        uc_err err = mmap(map_begin, map_end - map_begin, UC_PROT_RW);
        if (!_uc_ok(err))
            return 0;
    }

    heap_off = next_off;
 
    return virt;
}
};

namespace SlitYaX86
{
    void Ldr::virt_map_fxd(__u64 addr, __u64 size, __u32 prot) {
#if defined(_WIN32)
        constexpr __u64 GRAN = 0x10000;
        const __u64 end = addr + size;
        if (!size || end < addr)
            return;

        for (__u64 cur = addr; cur < end;) {
            MEMORY_BASIC_INFORMATION mbi{};

            if (VirtualQuery(
                    reinterpret_cast<void *>(cur),
                    &mbi,
                    sizeof(mbi)) != sizeof(mbi))
                return;

            const __u64 region_end =
                reinterpret_cast<__u64>(mbi.BaseAddress) + mbi.RegionSize;

            const __u64 stop = std::min(reinterpret_cast<__u64>(end), region_end);
            if (stop <= cur)
                return;

            if (mbi.State == MEM_FREE) {
                const __u64 base = cur & ~(GRAN - 1);
                const __u64 alloc_end = (stop + GRAN - 1) & ~(GRAN - 1);

                if (!VirtualAlloc(
                        reinterpret_cast<void *>(base),
                        alloc_end - base,
                        MEM_RESERVE | MEM_COMMIT,
                        prot))
                    return;
            } else if (mbi.State == MEM_RESERVE) {
                if (!VirtualAlloc(
                        reinterpret_cast<void *>(cur),
                        stop - cur,
                        MEM_COMMIT,
                        prot))
                    return;
            } else if (mbi.State == MEM_COMMIT) {
                if (!VirtualProtect(
                        reinterpret_cast<void *>(cur),
                        stop - cur,
                        prot,
                        NULL))
                    return;
            }

            cur = stop;
        }
#endif
#if defined(__linux__)
        ::mmap(
            (void*)addr,
            size,
            prot,
            MAP_FIXED|MAP_ANON|MAP_PRIVATE,
            -1, 0);
#endif
    }

    uc_err Ldr::uc_map_fxd(__u64 addr, __u64 size, __u32 prot) {
        const __u64 end = addr + size;
        if (!size || (addr & 0xfff) || (size & 0xfff) || end < addr)
            return UC_ERR_ARG;

        for (__u64 cur = addr; cur < end;) {

            auto mapped = Uc::get_map(uc, cur);

            if (!!mapped.begin) {
                __u64 stop = std::min<__u64>(end, mapped.end + 1);
                if (stop <= cur)
                    return UC_ERR_ARG;

                uc_err err = uc_mem_protect(uc, cur, stop - cur, prot);
                if (!_uc_ok(err))
                    return err;

                cur = stop;
                continue;
            }

            auto next = Uc::get_map_next(uc, cur);
            __u64 stop = (!!next.begin) ? std::min<__u64>(end, next.begin) : end;

            if (stop <= cur)
                return UC_ERR_ARG;

            uc_err err = uc_mem_map(uc, cur, stop - cur, prot);
            if (!_uc_ok(err))
                return err;
            cur = stop;
        }
        return UC_ERR_OK;
    }
};


namespace SlitYaX86 {
uc_err Ldr::mmap(__u64 virt, __u64 size, __u32 p, char *name) {
    __u64 base_virt = VIRT_SHADOW_BASE + virt;
    assert(!(virt & 0xfff) && !!size);
    uc_err err = uc_map_fxd(virt, size, p);
    if (!_uc_okay(err, ok))
        return err;

    auto sh_mem = new shadow_mem_t{
        (void *)(base_virt),
        size,
        uc_perm(p),
        new mem_map_t(name, virt, size, p)
    };
    if (!sh_mem) return UC_ERR_NOMEM;

    do {
        virt_map_fxd(reinterpret_cast<__u64>(sh_mem->addr), size, sh_mem->perm);
        // _zero_memory(sh_mem->addr, sh_mem->size);
    } while (0);
    pmem_maps.emplace_back(sh_mem->uc_mem);
    shadow.push_back(sh_mem);

    return UC_ERR_OK;
}

uc_err Ldr::mmap(__u64 virt, __u64 size, __u32 perm) {
    return mmap(virt, size, perm, (char *)NULL);
}
};

/**
 *  Windows
 **/
#if defined(_WIN32)
namespace SlitYaX86 {
__u64 Ldr::AllocUcStr(char *str) {
    __u64 s_len = strlen(str) + 1;
    wchar_t wstr[0x200]{};

    for (__u64 i = 0; i < s_len; i++)
        wstr[i] = static_cast<__u16>(str[i]);

    return AllocUcStr(wstr);
}

__u64 Ldr::AllocUcStr(wchar_t *wstr) {
    __u64 len = wcslen(wstr) * sizeof(wchar_t);
    auto uni = (_UNICODE_STRING_ *)uc_malloc(sizeof(_UNICODE_STRING_));
    __u64 virt_buf = uc_malloc(len + 2);

    mwrite(&uni->Length, (__u16)(len));
    mwrite(&uni->MaximumLength, (__u16)(len + sizeof(wchar_t)));
    mwrite(&uni->Buffer, (void *)(virt_buf));
    mmemcpy(virt_buf, wstr, len);

    return reinterpret_cast<__u64>(uni);
}
};
#endif