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
#include "../../elf/elf.hh"
extern "C" {
    #include "../../elf/elf.h"
    #include "../../types.h"
    #include "../yayo.h"
    #include "../binary.h"
    #include "../1dlsd.h"
};
#include "../1dlsd.hh"

namespace SlitYaX86
{
/** Feed this to Lyss::push() **/
template<typename T>
static bin_hdr_t *new_Lyss_entry(T *ptr, __u64 off, __u64 sz) {
    auto e = new bin_hdr_t();
    e->mm = std::move(mm_t{(void*)ptr, MM_HEAP, sz});
    e->off  = off;
    return e;
}
};

namespace SlitYaX86 {
class x1Dlsd;
struct XLyss : public Lyss {
    x1Dlsd                  *lsd;
    std::shared_ptr<ElfObj> base_elf;
    Lyss&                   lyss{*dynamic_cast<Lyss*>(this)};

    XLyss(x1Dlsd *xlsd, std::shared_ptr<ElfObj> elf);

    __u64 get_size(enum _1DLSD_TYPE &t) override {
        return _get_size(t);
    }
private:
    __u64 _get_size(enum _1DLSD_TYPE &t);
};

class x1Dlsd : public _1Dlsd {
    XLyss     lyss;
public:
    x1Dlsd(acid_pass& _pass, std::shared_ptr<ElfObj> elfptr)
        :   lyss(this, elfptr),
            _1Dlsd(dynamic_cast<Lyss*>(&lyss), _pass)
    {

    }
};
};
#endif