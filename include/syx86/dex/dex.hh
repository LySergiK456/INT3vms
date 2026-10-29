
#ifndef KRACK_DEX_HH
#define KRACK_DEX_HH
// #ifdef KRAck_LDR_GUARD
// #define KRAck_LDR_GUARD
#include "../loader/windows/win_ldr.hh"
// #endif

namespace SlitYaX86
{
    class WinDex;
};

namespace SlitYaX86
{
class Dex {
public:
    Disass                  d;
    Razor<WinDex, WinLdr>   *r;
    dex_ctx_t               *ctx;

    Dex() = default;
    Dex(Dex &&x) = default;
    Dex(Razor<WinDex, WinLdr> *_r) : r{_r} {}
};
};

namespace SlitYaX86::DexVerse {
    dex_ctx_t *newDexCtx();
    void addDexSource(dex_ctx_t *dex, __u64 addr, __u64 size);
    int dex_callback(dex_ctx_t *dex, uc_engine *uc, __u64 addr);
};

namespace SlitYaX86
{
struct WinDex : public Dex {
    WinDex(Razor<WinDex, WinLdr> *razor)
        : Dex(razor)
    {
        
        ctx = DexVerse::newDexCtx();
        DexVerse::addDexSource(ctx, razor->ldr.proc.peb_addr, sizeof(_PEB_));
    }

    static void syscall_hook(SyscallHook t, const char *name, void *fn);
    static void hook_code(uc_engine *uc, __u64 addr, __u64 i, void *raw_ctx);
    static __u64 dispatch_syscall(uc_engine *uc, __u64 addr, __u64 i, void *raw_ctx);
    static inline __u64 __syscall(uc_engine *uc, __NtArgs &args, void *raw_ctx);
    static inline __u64 _syscall_(__NtArgs *args);
    static bool IsSyscall(void *data);  // Is Legacy or regular Syscall opcode
    void Print(insn_t &insn, void *addr);
};
};
#endif
