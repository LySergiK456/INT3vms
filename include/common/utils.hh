#ifndef KRACK_UTILS_HH
#define KRACK_UTILS_HH
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <fstream>
#include <functional>
#include <memory>
extern "C" {
    #include "../types.h"
    #include "./utils.h"
    #include "./mem.h"
};
#if defined(__linux__)
#include <sys/mman.h>
#endif

#define Generic(T) template <typename T>

Generic(T) struct lambda_traits;
template <typename C, typename Ret, typename Arg, typename... Args>
struct lambda_traits<Ret (C::*)(Arg, Args...) const> {
    using first = Arg;
};
Generic(Fn) using lambda_arg =
    typename lambda_traits<decltype(&Fn::operator())>::first;

template <typename Fn> auto defer(Fn fn) {
    return std::shared_ptr<void>(
        nullptr, std::bind([fn = std::move(fn)](void) mutable { fn(); }));
};

#define __cafe_cat1_(a, b) a##b
#define __cafe_cat__(a, b) __cafe_cat1_(a, b)

#define _defer(fn) auto __cafe_cat__(__defer_, __COUNTER__) = defer(fn)

namespace Util {
struct outfile_t {
    std::ofstream out;

    template <typename S>
    outfile_t(S *file)
        : out(reinterpret_cast<char *>(file),
              std::ios::out | std::ios::binary) {}
    ~outfile_t() {
        _defer([&] { out.close(); }); // It's neat
    }

    template <typename T> void pwrite(T *ptr, __u64 size, __u64 off) {
        out.seekp(off, std::ios::beg);
        out.write(reinterpret_cast<char *>(ptr), size);
    }

    __u64 pos() { return static_cast<__u64>(out.tellp()); }
    bool ok() { return (!!out && out.good()); }
};
}; // namespace Util

namespace Util {
template <typename E> struct MMap_t {
    union {
        void *mem;
        E *e;
    };
    __u32 count;
    __u64 sz;
    __s8 ok{};

  private:
    __u32 ndx{0};

  public:
    struct iterator {
        E *ptr;

        E &operator*() const { return *ptr; }
        iterator &operator++() {
            ptr++;
            return *this;
        }
        bool operator!=(const iterator &i) const { return ptr != i.ptr; };
    };
    MMap_t(__u32 n=0x4000)
        : count{n}, sz{PAGE_ALIGN_UP(count * sizeof(E))}
    {
        mem = alloc(sz);
        if (!mem && (ok = -1))
            return;
    }

    E &push(E &new_e) {
        if (ndx == count) {
            __u64 new_sz = PAGE_ALIGN_UP((count + 0x4000) * sizeof(E));
            void *new_mem = alloc(new_sz);
            memcpy(new_mem, mem, sz);

            # if defined(_WIN32)
            VirtualFree(mem, 0, MEM_RELEASE);
            # endif
            # if defined(__linux__)
            munmap(mem, size);
            # endif

            mem = new_mem;
            e = static_cast<E *>(mem);
            sz = new_sz;
            count += 0x4000;
        }
        return (e[ndx++] = std::move(new_e));
    }

    __u64 size() { return ndx + 1; }

    iterator begin() { return {e}; }
    iterator end() { return {&e[count]}; }

  private:
    void *alloc(__u64 size) {
        # if defined(_WIN32)
        void *dat = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        # endif
        # if defined(__linux__)
        void *dat = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
        # endif
        
        memset(dat, 0, size);
        return dat;
    }
};

__u64 hash(char *);
};


namespace Util
{ 
class StrTab {
    union {
        void *dat;
        char *mem;
    } tab;
    
    struct {
        char **st_ptr_vec;
        __u64 st_ptr_num;
        __u64 st_val_off;
        char *st_vals;
    } s_tab;

    __u64 size;
    __u64 ndx;
    __u64 val_off;
    __u64 uc_rsp_addr;
public:
    char *&raw{reinterpret_cast<char*&>(s_tab.st_ptr_vec)};
    
    StrTab(__u64 virt, __u64 n_str)
        : uc_rsp_addr{virt}, val_off{(n_str+1)*sizeof(char*)}, ndx{}, size{}
    {
        tab.dat         = (*(void**)&s_tab.st_ptr_vec) = _alloc(n_str, PAGE_SIZE);
        s_tab.st_vals   = &tab.mem[val_off];
        s_tab.st_val_off= (s_tab.st_vals - (char*)s_tab.st_ptr_vec);
        s_tab.st_ptr_num= n_str;
    }

    template<typename T=__u8>
    __u64 operator<<(T *str) { return push(str); }

    template<typename T=__u8>
    __u64 push(T *str) {   
        __u64 str_len{strlen((char*)str)};
        if (val_off + str_len+1 > size)
            tab.dat = _realloc(s_tab.st_ptr_num+0x100);

        s_tab.st_ptr_vec[ndx++] =
            reinterpret_cast<char*>(uc_rsp_addr
                +   (__u64)(s_tab.st_vals - (char*)s_tab.st_ptr_vec)
                +   val_off);

        memcpy(reinterpret_cast<void*>(&s_tab.st_vals[val_off]), (void*)str, str_len+1);
        val_off += str_len+1;

        return uc_rsp_addr + val_off;
    }

private:
    // sz accounted for NullByte's
    void *_realloc(__u64 n_str) {
        __u64 prev_size = size;
        char *prev_vals = s_tab.st_vals;
        if (n_str <= s_tab.st_ptr_num)
            return tab.dat;

        char **tmp = (char**)_alloc(n_str, size+PAGE_SIZE);
        for (__u64 i{}; i<s_tab.st_ptr_num; i++)
        {   /** Copy each (char*) ptr **/
            tmp[i] = s_tab.st_ptr_vec[i];
        }

        s_tab.st_vals   = &((char*)tmp)[(n_str+1)*sizeof(char*)];
        s_tab.st_val_off= size - sizeof(char*)*n_str;
        s_tab.st_ptr_num= n_str;

        memcpy((char*)tmp + s_tab.st_val_off,
                prev_vals,
                prev_size - (__u64)prev_vals);

        free(tab.mem);
        return tmp;
    }
    void *_alloc(__u64 n_str, __u64 sz) {
        return xmalloc((size = ((n_str+1)*sizeof(char*)) + sz));
    }
};
};

namespace Util {
#if defined(_WIN32)
void Red();
void Green();
void Blue();
void Purple();
void White();
void Reset();
#endif
}; // namespace Util
#endif
