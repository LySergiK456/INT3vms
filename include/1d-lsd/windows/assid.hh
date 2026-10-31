#include <map>
#include <memory>
#include <vector>
#include "../binary.hh"
#include "../../common/mem.hh"
#include "../../syx86/syx86.hh"
#include <unicorn/unicorn.h>

#ifndef _1DLSD_LINUX_HH
#define _1DLSD_LINUX_HH
#include "../../../x86disass/disass.hpp"
#include "../../common/utils.hh"
#include "../../pe64/pe64.hh"
extern "C" {
    #include "../../types.h"
    #include "../binary.h"
    #include "../1dlsd.h"
};
#include "../1dlsd.hh"

namespace SlitYaX86 {
class w1Dlsd;
struct WLyss : public Lyss {
    w1Dlsd                  *lsd;
    Lyss&                   lyss{*dynamic_cast<Lyss*>(this)};
    std::shared_ptr<PE64>   base_pe64;

    WLyss(w1Dlsd *xlsd, std::shared_ptr<PE64> elf);

    __u64 MemConv(Binary_t*, MemConvType, __u64, MemConvType) override;
    __u64 get_size(enum _1DLSD_TYPE t) override;

private:
    void align_bin(__u64 off, __u64 sz) override;
};

class w1Dlsd : public _1Dlsd {
public:
    WLyss     wlyss;
    w1Dlsd(acid_pass *_pass, std::shared_ptr<PE64> pbin)
        :   wlyss(this, pbin), _1Dlsd(_pass)
    {
        set_lyss(dynamic_cast<Lyss*>(&wlyss));
    }

    void run() override;
};
};
#endif