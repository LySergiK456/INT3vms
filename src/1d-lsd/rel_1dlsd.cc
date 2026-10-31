#include "../../include/1d-lsd/1dlsd.hh"
#include "../../include/conf.hh"
#include "../../include/common/utils.hh"
extern "C" {
    #include "../../include/common/llist.h"
};
// extern "C" {
//     #include "../../include/1d-lsd/yayo.h"
// };

namespace SlitYaX86
{
bin_hdr_t *&Lyss::operator<<(bin_hdr_t&& v) {
    auto v_ptr = new bin_hdr_t;
    *v_ptr = std::move(v);

    tofree.push_back(reinterpret_cast<void*>(v_ptr));
    return reinterpret_cast<bin_hdr_t*&>(tofree.back());
}
bin_hdr_t *new_lyss(void *ptr, __u64 off, __u64 sz) {
    auto e = reinterpret_cast<bin_hdr_t*>(xmalloc(sizeof(bin_hdr_t)));
    e->mm = {
        (void*)((char*)ptr + off), MM_NONE, sz
    };
    e->off = off;
    return e;
}

std::map<__u64, struct _1Dlsd_entry> &Lyss::operator()() {
    return map;
}
};

namespace SlitYaX86 {
    using BinT = Binary_t;
    using MT = MemConvType;

    LSD_API __u64 Lyss::orig_vtov(BinT *bin, __u64 v) { return MemBase(bin, MT::Virt,  v, MT::Virt);}
    LSD_API __u64 Lyss::orig_ftof(BinT *bin, __u64 v) { return MemBase(bin, MT::Off,   v, MT::Off); }
    LSD_API __u64 Lyss::orig_vtof(BinT *bin, __u64 v) { return MemBase(bin, MT::Virt,  v, MT::Off); }
    LSD_API __u64 Lyss::orig_ftov(BinT *bin, __u64 v) { return MemBase(bin, MT::Off,   v, MT::Virt);}

    LSD_API __u64 Lyss::mconv_vtov(BinT *bin, __u64 v) { return MemConv(bin, MT::Virt, v, MT::Virt);}
    LSD_API __u64 Lyss::mconv_ftof(BinT *bin, __u64 v) { return MemConv(bin, MT::Off,  v, MT::Off); }
    LSD_API __u64 Lyss::mconv_vtof(BinT *bin, __u64 v) { return MemConv(bin, MT::Virt, v, MT::Off); }
    LSD_API __u64 Lyss::mconv_ftov(BinT *bin, __u64 v) { return MemConv(bin, MT::Off,  v, MT::Virt);}
};

namespace SlitYaX86 {
using MT = MemConvType;

__u64 Lyss::MemBase(Binary_t *bin, MT src_type, __u64 value, MT dst_type) {
    __u64 offset{};
    switch (src_type) {
        case MT::Virt:  offset = bin->vtof(value);  break;
        case MT::Off:   offset = value;             break;
        default: assert(0);
    }
    
    for (auto& [lsd_off, emm] : map) {
        if (    emm.src.img == bin
            &&  !!_contain_(emm.src.off, emm.mm->sz, offset))
        {
            __u64 ret = lsd_off + (offset - emm.src.off);
            switch (dst_type) {
                case MT::Virt:  return mconv_ftov(bin, ret);break;
                case MT::Off:   return ret;                 break;
                default: assert(0);
            }
        }
    }

    for (auto& [lsd_off, emm] : map) {
        DBG_FMT("%lx - - - %lx %lx %lx off=%lx; ok=%lx\n", emm.t, emm.src.off, emm.mm->sz, value, offset, !!_contain_(emm.src.off, emm.mm->sz, offset));
    }

    return -1;
}

struct _1Dlsd_entry *
Lyss::make_entry(
    Binary_t            *img,
    enum _1DLSD_TYPE    t,
    bin_hdr_t&          v,
    bool                new_mm=false)
{
    struct _1Dlsd_entry *ent = new struct _1Dlsd_entry;
    tofree.push_back(ent);

    *ent = (struct _1Dlsd_entry){
        .t=t, .src={img, v.off, v.mm.sz}, .dst={v.off, v.mm.sz}
    };
    ent->mm = (!new_mm) ? &v.mm : new mm_t(v.mm);
    if (!!new_mm) tofree.push_back(ent->mm);
    return ent;
}

struct _1Dlsd_entry *
Lyss::make_entry_extra(
    Binary_t            *img,
    enum _1DLSD_TYPE    t,
    bin_hdr_t&          v,
    __u64               off,
    __u64               size)
{
    struct _1Dlsd_entry *ent = new struct _1Dlsd_entry;
    tofree.push_back(ent);
    
    *ent = (struct _1Dlsd_entry){
        .t=t, .mm=new mm_t(v.mm), .src={img, off, size}, .dst={off, size}
    };
    tofree.push_back(ent->mm);
    return ent;
}

LSD_API void Lyss::push(Binary_t *img, enum _1DLSD_TYPE t, bin_hdr_t& v) {
    if (!v.mm.sz) return;

    for (auto& [off, entry] : map) {
        if (!!_crossed_(entry.dst.off, entry.dst.sz, v.off, v.mm.sz))
        {
            __u64 min = std::min(entry.dst.off, v.off);
            __u64 lvl = std::max(entry.dst.off, v.off);
            __u64 left= lvl + (lvl == v.off ? v.mm.sz : entry.mm->sz);
            std::vector<__u64> vec{min, lvl, left};

            for (auto& _off_ : vec)
            {
                __u64 next{std::max(entry.dst.off + entry.mm->sz, v.off + v.mm.sz)};
                for (auto& i_off : vec)
                {
                    if (i_off > _off_ && i_off < next)
                        next = i_off;
                }
                __u64 size = next - _off_;
                if (    (_off_ == v.off && v.off < entry.dst.off)
                    ||  (_off_ >= entry.dst.off + entry.dst.sz
                        &&  _off_ < v.off + v.mm.sz))
                {
                    map[_off_] = *make_entry_extra(img, t, v, _off_, size);
                }
            }
            return;
        }
    }

    map[v.off] = *make_entry(img, t, v);
}
LSD_API void Lyss::push_ptr(Binary_t *img, enum _1DLSD_TYPE t, bin_hdr_t *&&v) {
    tofree.push_back((void*)v);
    push(img, t, *v);
}
};

/** 4 Yayo **/
// LSD_API __u64 lsd_align(struct rel_yayo *ctx, __u64 offset) {
//     return offset;
// }
namespace SlitYaX86
{
// void Lyss::align_map(__u64 off, __u64 sz) {
//     for (auto& [k, emm] : map) {
//         if (_contain_(k, emm.mm->sz, off)) {
//             map[k].mm->sz += sz;
//         } else if (k >= off) {
//             struct _1Dlsd_entry e{std::move(emm)};
//             __u64 key{k+sz};

//             map.erase(k);
//             map[key] = std::move(e);
//         }
//         // DBG_FMT("/0x%lx\n", k);
//     }
// }
void Lyss::align_map(__u64 off, __u64 sz) {
    std::map<__u64, struct _1Dlsd_entry> out;
    for (auto& [k, emm] : map) out[k] = map[k];

    for (auto& [k, emm] : map) {
        if (_contain_(k, emm.mm->sz, off)) {
            out[k].mm->sz += sz;
            out[k].dst.sz += sz;
        } else if (k >= off) {
            out.erase(k);
            emm.dst.off = k+sz;
            out[k+sz] = emm;
        }
    }
    map.erase(map.begin(), map.end());
    map = std::move(out);
}

LSD_API
void
Lyss::insert(
    enum _1DLSD_TYPE    t,
    void                *mem,   // Shellcode, whatever
    __u64               size,   // size of mem/data
    __u64               offset,    // offset
    void               *tab,    // in case of Phdr/Ehdr/Dynamic
    __u32               nth,    // tab[nth]
    void                *e)     // the fucking entry
{
    xx(offset);
    // tab=nullptr;
    // e=nullptr;
    // if (!!tab) {
    //     if (llist_empty(&tab->px))
    //         llist_init(&tab->px, sizeof(struct _1Dlsd_entry));
    // }

    struct _1Dlsd_entry px = {
        .dst = {
            .off= offset,//(!!e ? nth * sizeof(EntryT) : off),
            .sz = size//(!!e ? sizeof(EntryT) : size)
        },
        .raw_mem = mem,
    };

    if (!tab) {
        // llist_push((_LL_TYPE_*)&tab->px, px);
    } else {
        for (auto& [off, entry] : map)
            if (!!_contain_(off, entry.mm->sz, off)) {
                if (llist_empty(&entry.px))
                    llist_init(&entry.px, sizeof(struct _1Dlsd_entry));

                llist_push((_LL_TYPE_*)&entry.px, &px);
                break;
            }
    }
    __u64 out_off = offset;//((!!tab) ? tab->dst.off + px.dst.off : off);
    align_map(out_off, px.dst.sz);
    align_bin(out_off, px.dst.sz);
}

/**
 * Build the binary
**/
void _1Dlsd::make(char *f) {
    auto out = Util::outfile_t(f);

    DBG_FMT("-[_1Dlsd]-\t~[+]~ Dumping to [%s]\n", f);
    for (auto& [off, e] : lyss->map) {
        if (!llist_empty(&e.px)) {
            ll_each_value(&e.px, px) {
                // DBG_FMT("~> @0x%llx - size 0x%lx\n", px->dst.off, px->dst.sz);
                out.pwrite(px->raw_mem, px->dst.sz, px->dst.off);
            }
        }
        // DBG_FMT("-> @0x%llx - size 0x%lx\n", off, e.mm->sz);
        out.pwrite(e.mm->mem, e.mm->sz, off);
    }
}
};


namespace SlitYaX86 {
struct _1Dlsd_entry *Lyss::get_by_type(enum _1DLSD_TYPE t, __u64 off) {
    struct _1Dlsd_entry *retn=nullptr;
    feach(t, [&](void *_, struct _1Dlsd_entry *e) {
        if (e->src.off >= off) { retn=e; return; };
    });
    return retn;
}
};