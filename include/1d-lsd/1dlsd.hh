#include <map>
#include <memory>
#include <vector>
extern "C" {
    #include "../types.h"
    #include "./binary.h"
    #include "./1dlsd.h"
};
#include "./linux/assid.hh"
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
    Lyss() = default;
    Lyss(_1Dlsd *dlsd, Binary_t& _bin) : ____lsd{dlsd}, bin{_bin} {}
    Lyss(x1Dlsd *dlsd, Binary_t& _bin) : nix_lsd{dlsd}, bin{_bin} {}
    virtual ~Lyss() {
        for (auto& e : tofree) delete e;
    }

    LSD_API void push_ptr(Binary_t *img, enum _1DLSD_TYPE t, bin_hdr_t *&&v) {
        tofree.push_back((void*)v);
        push(img, t, *v);
    }
    LSD_API void push(Binary_t *img, enum _1DLSD_TYPE t, bin_hdr_t& v) {
        if (!v.mm.sz) return;
        for (auto& [off, entry] : map)
        {
            if (entry.t != t) continue;

            if (!!_contains_(entry.dst.off, entry.dst.sz, v.off, v.mm.sz))
                return;
        }
        map[v.off] = (struct _1Dlsd_entry){
            .t=t, .mm=&v.mm, .src={img, v.off, v.mm.sz}, .dst={v.off, v.mm.sz}
        };
    }
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
                .sz = (!!e) ? sizeof(EntryT) : size
            },
            .raw_mem = mem,
        };

        llist_push((_LL_TYPE_*)&tab->px, &px);
        align_map(tab->dst.off+px.dst.off, px.dst.sz);
        align_bin(tab->dst.off+px.dst.off, px.dst.sz);
    }

    bin_hdr_t *&operator<<(bin_hdr_t&& v);
    std::map<__u64, struct _1Dlsd_entry> &operator()();
    struct _1Dlsd_entry *get_by_type(enum _1DLSD_TYPE t, __u64 off);

protected:
    virtual __u64 get_size(enum _1DLSD_TYPE t) {
        static_assert("You shall've implemented this!");
    };
    void align_map(__u64 off, __u64 sz);
    virtual void align_bin(__u64 off, __u64 sz) {
        static_assert("You Should've implement this!");
    }

    friend class _1Dlsd;
    friend class XLyss;
};
};

namespace SlitYaX86 {
    struct acid_pass {
        Util::MMap_t<lsd_oper_calback> op_callbacks;
      protected:
        _1Dlsd *lsd{nullptr};

      public:
        acid_pass() = default;
        virtual LSD_STATUS _init_(_1Dlsd *lsd) { return LSD_OK; }
        friend class _1Dlsd;
    };
};

namespace SlitYaX86 {
class _1Dlsd : public _1Dlsd_pack_t {
    Disass          d;
public: // sorry.. - NO, this is !FUCKED!
    acid_pass&      pass;
    Lyss            *lyss;
    _1Dlsd(Lyss *_lyss, acid_pass& _pass)
        : lyss{_lyss}, pass{_pass}, _1Dlsd_pack_t{0}
    {
        llist_init(&ll_px,    sizeof(struct _1Dlsd_entry));
        llist_init(&ll_rel,   sizeof(struct rel_yayo));
        llist_init(&ll_cb,    sizeof(struct lsd_oper_cb));
    };
    virtual ~_1Dlsd() = default;

    template<enum _1DLSD_TYPE T, typename Fn>
    void parse(Fn fn) {
        for (auto& [off, e] : lyss->map)
            if (e.t == T) fn(off, e);
    }
    void run() {
        static_assert("This is just wrong, Or not");
    }

    void make(char *f);
};
};

LSD_API __u64 lsd_align(struct rel_yayo *ctx, __u64 offset);
#endif

