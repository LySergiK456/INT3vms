#include "../../../../x86disass/disass.hpp"
#include "../../../conf.hh"
#include "../../../common/mem.hh"
#include "../../../pe64/pe64.hh"
#include "../../uc.hh"
#include <list>
#include <memory>
#include <type_traits>
#include <string.h>
#include <unicorn/unicorn.h>
#include "../../syx86.hh"
#include "../../dex/dex.hh"
extern "C" {
    #include "../../../types.h"
    #include "../../syscall/windows/win_syscall.h"
    #include "../../dex/dex.h"
};
#ifndef SYX86_WIN_LDR_HH
#define SYX86_WIN_LDR_HH

namespace SlitYaX86 {
    __attribute__((constructor)) void init_wine_syscalls();

    __s8 wine_name(char *ret, __u64 size, char *dll_name);
    class RtlHeap;
    class WinDex;
};

namespace SlitYaX86
{
class WinLdr : public Ldr {
public:
	struct {
        __u64 teb_addr{TEB_ADDR};
        __u64 peb_addr{PEB_ADDR};
        __u64 ldr_addr{LDR_ADDR};
        __u64 kusd_addr{KUSER_SHARED_DATA_ADDR};
    } proc;

    __u64 _gs{static_cast<__u64>(KrackConf::GS_ADDR)};
    __u64 _fs{static_cast<__u64>(KrackConf::FS_ADDR)};
private:
    std::list<std::shared_ptr<PE64_IMAGE>>  img;
    std::shared_ptr<PE64_IMAGE>             main_image;
public:

    WinLdr(int argc, char **argv);
    uc_err Run();

    std::shared_ptr<PE64_IMAGE> VAtoImage(__u64 va);

    void addLdrEntry(std::shared_ptr<PE64_IMAGE> dll_img);
    void addLdrEntry(__u64 virt, char *basename, char *fullname);
    void addLdrEntry(__u64 virt, wchar_t *wbasename, wchar_t *wfullname);

  private:
    /** Init environment -
     * - TEB, PEB, LDR, stack, gs:[]&fs:[]
     **/
    uc_err InitProcEnv();
    uc_err init_gs_fs(__u64 gs, __u64 fs);
    uc_err init_stack(__u64 addr, __u64 sz);
    void initTEB();
    void initPEB();
    void initLDR();
    void initKUSD();

    uc_err InitDLLs(std::shared_ptr<PE64_IMAGE> pe_img);

    /** Image - relocation, IAT-fixups, Mappings
     */
    uc_err MapImage(std::shared_ptr<PE64_IMAGE> pe_img);
    uc_err MapImage(std::shared_ptr<PE64_IMAGE> pe_img, __u64 img_base);
    uc_err MapDLLs(std::shared_ptr<PE64_IMAGE> pe_img, __u64 *off);
    uc_err FixIAT(std::shared_ptr<PE64_IMAGE> pe_img);
    uc_err RelocateImage(std::shared_ptr<PE64_IMAGE> pe_img);

public:
    std::shared_ptr<PE64_IMAGE> FindDLL(char *name);
    std::shared_ptr<PE64_IMAGE> img_push(char *file);
    std::shared_ptr<PE64_IMAGE> img_push(char *file, __u64 base);
};
};

/**
 * Loader(~SlitYaX86::Ldr) Utils
 **/
namespace SlitYaX86 {
namespace LdrUtils {
class RtlHeap {
    WinLdr      &ldr;
    std::shared_ptr<PE64_IMAGE> ntdll_img;
    PE64      *ntdll;
    __u64 retn_addr{0xdeadc000};

  public:
    __s8 ok{-1};

    RtlHeap(WinLdr &l);
    operator bool() const { return ok != -1; }

  private:
    void setup_regs();
    uc_err init(__u64 fn);
};
};
};

namespace SlitYaX86 {
    class WinRazor : public Razor<WinDex, WinLdr> {

    };
};
#endif
