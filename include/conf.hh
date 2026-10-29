extern "C" {
    #include "./types.h"
    #include "./common/mem.h"
};

#ifndef KRACK_CONFIG_HH
#define KRACK_CONFIG_HH
#define DO_DEBUG

#ifdef DO_DEBUG
#define DBG_FMT(s, ...) printf(const_cast<char *>((s)), __VA_ARGS__)
#define DBG_F(s, ...)   printf(const_cast<char *>((s)))
#define DBGS(s)         puts(const_cast<char *>((s)))
#else
#define DBG_FMT(s, ...)
#define DBG_F(s, ...)
#define DBGS(s)
#endif

#define HOOK_PR_INSN   /* Print insn in Dex class */
#define KRACK_UC_NOERR /* assert that there are no errors */
#define DEBUG_PORT 8473

#define UNIX_STACK_SIZE     0x800000
#define UNIX_STACK_TOP      0x7ffffff00000ULL
#define UNIX_STACK_BASE     (UNIX_STACK_TOP - UNIX_STACK_SIZE)

enum class KrackConf : __u64 {
    StackBase = 0x7ff0d000ULL,

    GS_ADDR = 0x6000000ULL,
    GS_SIZE = (20ULL << 20),

    FS_ADDR = 0x6000ULL,
    FS_SIZE = 0x6000ULL,
};

constexpr __u16 WIN10_BUILD_NUMBER = 19041;

constexpr __u64 HEAP_ADDR = 0xdead8000ULL;
constexpr __u64 STACK_SIZE = PAGE_ALIGN_UP(1000000);
constexpr __u64 stack_base = static_cast<__u64>(KrackConf::StackBase);
constexpr __u64 VIRT_SHADOW_BASE = 0xCC000ULL;
constexpr __u64 KUSER_SHARED_DATA_ADDR = 0x7FFE0000ULL;

#define TEB_ADDR ((__u64)KrackConf::GS_ADDR)
#define PEB_ADDR PAGE_ALIGN_UP(TEB_ADDR + sizeof(_TEB_) + 0x10)
#define LDR_ADDR PAGE_ALIGN_UP(PEB_ADDR + sizeof(_PEB_) + 0x10)

constexpr char *XTC_DEFAULT_OUTFILE = "/home/rax/INT3vms/dat/xtc_table/xtc.dat";
constexpr char *PE_SYMTAB_DIR       = "/home/rax/INT3vms/dat/symbols/windows/";
constexpr char *ELF_SYMTAB_DIR      = "/home/rax/INT3vms/dat/symbols/linux/";

constexpr char *WIN_SYSTEM32_PATH = "/home/rax/INT3vms/dat/win_dll/";
constexpr char *NTOSKRNL_EXE_PATH = "/home/rax/INT3vms/dat/win_dll/ntoskrnl.exe";
#define LOAD_DLL_BASE 0x1042000
#define MAIN_ELF_BASE 0x555555554000
#define RTLD_ELF_BASE 0x7f0000000000
// #define MAIN_UC_ADDR		0x10000
#endif
