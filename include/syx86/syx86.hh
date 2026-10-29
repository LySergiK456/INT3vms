#include "../common/mem.hh"
#include "../conf.hh"

#ifndef KRACK_SYX86_GUARD
#define KRACK_SYX86_GUARD
#endif
#include "./uc.hh"
#include <list>
#include <memory>
#include <type_traits>
#include <string.h>
#include <unicorn/unicorn.h>
#include "../../x86disass/disass.hpp"
extern "C" {
    #include "../types.h"
    #include "./syscall/windows/win_syscall.h"
    #include "./dex/dex.h"
};

#ifndef KRACK_SYPE_HH
#define KRACK_SYPE_HH

#define __ENTRY__(addr, entry, e) (__u64)(addr) + ((__u64)(entry) - (__u64)(e))

namespace SlitYaX86 {
    template<class TLdr, class TDex> class Razor;
};

namespace SlitYaX86 {
    enum class SyscallHook : __u8
    {
        PreHook = 1,
        PostHook= 2,
    };
};

namespace SlitYaX86 {
    class Ldr;
    
    namespace Memory {
        template<typename T>
        struct syx_retn_t {
            T       v;
            uc_err  err;
        };
    };
};
#define SYX_RETN(T) syx_retn_t<T>
#define SYX_MEM_API

namespace SlitYaX86 {
class Ldr {
public:
    struct {
        int num;
        char **v;
    } args;

    std::list<mem_map_t *>      pmem_maps;
    std::list<shadow_mem_t *>   shadow;
    
    uc_engine *uc;
    __s8 ok{0};

    Ldr() = default;
    Ldr(int argc, char **argv)
        : args{ .num=argc, .v=argv } {}

    virtual ~Ldr() = default;
    uc_err Run() {
        assert(0);
    }

    shadow_mem_t *VAtoShadow(__u64 virt);

private:
    void virt_map_fxd(__u64 addr, __u64 size, __u32 prot);
    uc_err uc_map_fxd(__u64 addr, __u64 size, __u32 prot);

public:
    __u64 AllocUcStr(char *str);
    __u64 AllocUcStr(wchar_t *wstr);
    /** uc~mappings 2 Sh4d0w mappings
     */
    uc_err mmap(__u64 virt, __u64 size, __u32 perm);
    uc_err mmap(__u64 virt, __u64 size, __u32 perm, char *name);
    uc_err mmemcpy(__u64 virt, void *ptr, __u64 size);
    template <typename X = __u64, typename V, typename E, typename ST>
    uc_err mwrite(X v, V value, E *entry, ST *e) {
        return mwrite(__ENTRY__(v, entry, e), value);
    }
    uc_err mwrite(__u64 virt, auto v);
    template <typename T, typename V> uc_err mwrite(T virt, V v) {
        return mwrite((__u64)virt, v);
    }

    template <typename X = __u64, typename V, typename E, typename ST>
    uc_err mcopy(X v, V value, E *entry, ST *e) {
        return mcopy(__ENTRY__(v, entry, e), value);
    }
    uc_err mcopy(__u64 virt, auto v);
    template <typename T, typename V> uc_err mcopy(T virt, V v) {
        return mwrite((__u64)virt, v);
    }

protected:
    __u64 uc_malloc(__u64 size);
};
};

namespace SlitYaX86 {
    template<typename TDex, typename TLdr>
    class Razor {
        Disass      d;
        uc_hook     uc_insn_hook;    
        TDex         *dex;

    public:
        TLdr        ldr;

        Razor() = default;
        Razor(Razor *r)
            : Razor(std::move(*r)) {}
        Razor(Razor &r) = default;
        Razor(Razor &&r) = default;
        Razor(int argc, char **argv);
        Razor &operator=(Razor &&r) = default;

        virtual uc_err Run() { return ldr.Run(); }
    };

    // template<typename TLdr, typename TDex>
    // class Dex {
    // public:
    //     Disass      d;
    //     Razor       *r;
    //     // DEX_CTX     *ctx;

    //     Dex(Dex &&x) = default;
    //     Dex(Razor *_r) : r{_r} {}
    // };
};
#endif
