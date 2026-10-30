#if defined(__linux__)
#include <elf.h>
#endif
#include <map>
#include <memory>
#include <vector>
extern "C" {
    #include "../types.h"
    #include "./binary.h"
    #include "./1dlsd.h"
};
#include "./binary.hh"
#include "../common/mem.hh"
#include "../syx86/syx86.hh"
#include <unicorn/unicorn.h>

#ifndef _1DLSD_HH
#define _1DLSD_HH
#include "../../x86disass/disass.hpp"
#include "../common/utils.hh"

extern "C" {
    #include "./yayo.h"
    #include "./1dlsd.h"
};

namespace SlitYaX86 {
    class _1Dlsd;
    class x1Dlsd;   // Linux packer
    #if defined(__linux__)
        class ElfObj;
        class XLyss;
    #endif
};

namespace SlitYaX86 {
    enum class MemConvType : __u8
    {
        Virt=1,
        Off =2,
    };
};

namespace SlitYaX86 {
struct Lyss {
    struct {
        x1Dlsd  *nix_lsd;
        _1Dlsd  *____lsd;
    };
    Binary_t&               bin;
private:
    std::vector<void*>      tofree;
public:
    std::map<struct _1Dlsd_entry, struct rel_yayo>  m_yayo_entry;
    std::map<__u64, struct _1Dlsd_entry>            map;

    /**
     * Linux    */
    Lyss() = delete;
    Lyss(_1Dlsd *dlsd, Binary_t& _bin) : ____lsd{dlsd}, bin{_bin} {}
    Lyss(x1Dlsd *dlsd, Binary_t& _bin) : nix_lsd{dlsd}, bin{_bin} {}
    virtual ~Lyss() {
        for (auto& e : tofree) delete e;
    }

    LSD_API void push_ptr(Binary_t *img, enum _1DLSD_TYPE t, bin_hdr_t *&&v);
    LSD_API void push(Binary_t *img, enum _1DLSD_TYPE t, bin_hdr_t& v);
    Generic(Fn) int feach(enum _1DLSD_TYPE t, Fn fn) {
        return feach(t, nullptr, fn);
    }
    Generic(Fn) int feach(enum _1DLSD_TYPE t, struct _1Dlsd_entry *v, Fn fn) {
        __u64 i{};
        try {
            for (auto& entry : map) {
                auto& e = std::get<struct _1Dlsd_entry>(entry);

                if (e.t == t && ++i && !!e.mm) {
                    if (!!v) *v = e;
                    for (__u64 off = 0, sz = get_size(t); !!sz && off < e.mm->sz; off += sz) {
                        fn(static_cast<lambda_arg<decltype(fn)>>(e.mm->mem+off), &e);
                    }
                }
            }
        } catch (...) {}
        return i;
    }

    template<typename TablT, typename EntryT>
    LSD_API void insert(
        enum _1DLSD_TYPE    t,
        void                *mem,   // Shellcode, whatever
        __u64               size,   // size of mem/data
        TablT               *tab,   // in case of Phdr/Ehdr/Dynamic
        __u32               nth,    // tab[nth]
        EntryT*             e)      // the fucking entry
    {
        if (llist_empty(&tab->px)) {
            llist_init(&tab->px, sizeof(struct _1Dlsd_entry));
        }

        struct _1Dlsd_entry px = {
            .dst = {
                .off= nth * sizeof(EntryT),
                .sz = !!e ? sizeof(EntryT) : size
            },
            .raw_mem = mem,
        };

        llist_push((_LL_TYPE_*)&tab->px, &px);
        align_map(tab->dst.off + px.dst.off, px.dst.sz);
        align_bin(tab->dst.off + px.dst.off, px.dst.sz);
    }

    virtual
    __u64 MemConv(Binary_t*, MemConvType, __u64, MemConvType) = 0;
    __u64 MemBase(Binary_t*, MemConvType, __u64, MemConvType);

    LSD_API __u64 orig_vtov(Binary_t*, __u64);
    LSD_API __u64 orig_ftof(Binary_t*, __u64);
    LSD_API __u64 orig_vtof(Binary_t*, __u64);
    LSD_API __u64 orig_ftov(Binary_t*, __u64);

    LSD_API __u64 mconv_vtov(Binary_t*, __u64);
    LSD_API __u64 mconv_ftof(Binary_t*, __u64);
    LSD_API __u64 mconv_vtof(Binary_t*, __u64);
    LSD_API __u64 mconv_ftov(Binary_t*, __u64);

    bin_hdr_t *&operator<<(bin_hdr_t&& v);
    std::map<__u64, struct _1Dlsd_entry> &operator()();
    struct _1Dlsd_entry *get_by_type(enum _1DLSD_TYPE t, __u64 off);

protected:
    virtual __u64 get_size(enum _1DLSD_TYPE t) {
        assert(0);//, "You shall've implemented this!"
    };
    void align_map(__u64 off, __u64 sz);
    virtual void align_bin(__u64 off, __u64 sz) {
        assert(0);//, "You Should've implement this!"
    }
private:
    struct _1Dlsd_entry *make_entry(Binary_t*, enum _1DLSD_TYPE, bin_hdr_t&, bool);
    struct _1Dlsd_entry *make_entry_extra(Binary_t*, enum _1DLSD_TYPE, bin_hdr_t&, __u64, __u64);

    friend class _1Dlsd;
    friend class XLyss;
};
};

namespace SlitYaX86 {
    struct acid_pass {
        Util::MMap_t<lsd_oper_calback> op_callbacks;
      protected:
        _1Dlsd *lsd{nullptr};
        x1Dlsd *xlsd{nullptr};

      public:
        acid_pass() = default;
        virtual LSD_STATUS _init_(_1Dlsd *lsd) { return LSD_OK; }
        virtual LSD_STATUS _init_(x1Dlsd *lsd) { return LSD_OK; }
        
        friend class _1Dlsd;
        friend class x1Dlsd;
    };
};

namespace SlitYaX86 {
class _1Dlsd : public _1Dlsd_pack_t {
    Disass          d;
public: // sorry.. - NO, this is !FUCKED!
    acid_pass       *pass;
    Lyss            *lyss;
    _1Dlsd(acid_pass *_pass, std::shared_ptr<ElfObj> elfobj)
        : pass{_pass}, _1Dlsd_pack_t{0}
    {
        llist_init(&ll_px,    sizeof(struct _1Dlsd_entry));
        llist_init(&ll_rel,   sizeof(struct rel_yayo));
        llist_init(&ll_cb,    sizeof(struct lsd_oper_cb));
    };
    virtual ~_1Dlsd() = default;

    void set_lyss(Lyss *l) { lyss=l; }
    template<enum _1DLSD_TYPE T, typename Fn>
    void parse(Fn fn) {
        for (auto& [off, e] : lyss->map)
            if (e.t == T) fn(off, e);
    }
    virtual void run() = 0;
    void make(char *f);
};
};

LSD_API __u64 lsd_align(struct rel_yayo *ctx, __u64 offset);
#endif

