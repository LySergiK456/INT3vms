#include "../../include/common/utils.h"
#include "../../include/common/mem.h"
#include <assert.h>
#if defined(_WIN32)
# include <windows.h>
#endif
#if defined(__linux__)
# include <sys/mman.h>
# include "../../include/common/linux/proc_self_maps.h"
#endif

const char *strerr(perr_t err) {
    switch (err) {
    case ErrOK:
        return "Success";

    case ErrNoDir:
        return "No such directory";

    case ErrNoFile:
        return "No such file";

    default:
        return "Unknown Error";
    }
}

void *xmalloc(__u64 size) {
    void *ptr = malloc(size);
    assert(!!ptr);
    memset(ptr, 0, size);
    return ptr;
}

void hexdump(void *addr, __u64 sz) {
    __u8 *ptr = (__u8 *)addr;

    for (__u64 i = 0; i <= sz; i++) {
        __u8 ch = ptr[i];

        if ((!(i % 0x10) && i) || i >= sz) {
            __u8 n = 0x10;

            if (i >= sz) {
                n = !(sz % 0x10) ? 0x10 : (sz % 0x10);
                for (int x = n; x < 0x10; x++) {
                    if (!(i % 4))
                        prf("  ");
                    prf("   ");
                }
            }

            prf("\t|\t");

            for (__u8 l = 0; l < n; l++) {
                __u8 c = ptr[(i - n) + l];
                prf("%c", isprint(c) ? c : '.');
            }

            pr("");
            if (i >= sz)
                return;
        }

        if (!i || !(i % 0x10))
            prf("0x%06x :\t", i);

        if (!(i % 4) && i && (i % 0x10))
            prf("| ");

        prf("%02x ", ch);
    }
}

__u64 ptrArrSize(char **x) {
    __u64 r=0;
    while(!!*x) { x++; r++; };
    return r;
}

#if defined(_WIN32)
__u32 FileExists(char *file) {
    __u32 dwAttr = GetFileAttributes(file);
    return (dwAttr != INVALID_FILE_ATTRIBUTES &&
            !(dwAttr & FILE_ATTRIBUTE_DIRECTORY));
}

__u32 DirExists(char *dir) {
    __u32 dwAttr = GetFileAttributes(dir);
    return (dwAttr != INVALID_FILE_ATTRIBUTES &&
            !!(dwAttr & FILE_ATTRIBUTE_DIRECTORY));
}
#endif
/**/


int __wr_init(void *addr, __u64 size, __u32 *perm) {
#if defined(_WIN32)
    return VirtualProtect(PAGE_ALIGN_DOWN(addr), PAGE_ALIGN_UP(size),
                              PAGE_READWRITE, perm);
#endif
#if defined(__linux__)
    *perm = UnixMemQuery(PAGE_ALIGN_DOWN(addr));
    return mprotect(addr, PAGE_ALIGN_UP(size), PROT_READ|PROT_WRITE);
#endif
}

int __wr_exit(void *addr, __u64 size, __u32 perm) {
#if defined(_WIN32)
    return VirtualProtect(PAGE_ALIGN_DOWN(addr), PAGE_ALIGN_UP(size), perm,
                          NULL);
#endif
#if defined(__linux__)
    return mprotect(addr, PAGE_ALIGN_UP(size), perm);
#endif
}

int _write_mem(void *dst, void *src, __u64 size) {
    __u32 dw;
    if (!dst || !src)
        return -1;

    if (!__wr_init(dst, size, &dw))
        return -1;
    memcpy(dst, src, size);
    if (!__wr_exit(dst, size, dw))
        return -1;

    return 0;
}

int _zero_memory(void *dst, __u64 size) {
    return _write_memset(dst, 0, size);
}

int _write_memset(void *dst, int ch, __u64 size) {
    __u32 dw;
    if (!dst)
        return -1;

    if (!__wr_init(dst, size, &dw))
        return -1;
    memset(dst, ch, size);
    if (!__wr_exit(dst, size, dw))
        return -1;

    return 0;
}
__s8 _memcmp(void *a, void *b, __u64 size) {
    for (__u64 i = 0; i < size; i++)
        if (((char *)a)[i] != ((char *)b)[i])
            return -1;

    return 0;
}
