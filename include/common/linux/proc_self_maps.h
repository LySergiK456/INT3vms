#if defined(__linux__)
# ifndef PROC_IOCTL_HH
# define PROC_IOCTL_HH
# include <sys/ioctl.h>
# include <stdint.h>
struct procmap_query_compat {
    uint64_t size, query_flags, query_addr;
    uint64_t vma_start, vma_end, vma_flags;
    uint64_t vma_page_size, vma_offset, inode;
    uint32_t dev_major, dev_minor;
    uint32_t vma_name_size, build_id_size;
    uint64_t vma_name_addr, build_id_addr;
};

# define PROCMAP_QUERY_COMPAT \
    _IOWR('f', 17, struct procmap_query_compat)

# define PROCMAP_QUERY_VMA_READABLE   0x01
# define PROCMAP_QUERY_VMA_WRITABLE   0x02
# define PROCMAP_QUERY_VMA_EXECUTABLE 0x04

__u32 UnixMemQuery(void *addr);

# endif
#endif