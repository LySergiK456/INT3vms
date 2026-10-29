#include "../../../../include/syx86/loader/linux/linux_ldr.hh"
extern "C" {
	#include "../../../../include/common/linux/proc_self_maps.h"
};

using namespace SlitYaX86;
namespace SlitYaX86
{
__attribute__((constructor)) void init_self_maps() {
	SelfMaps::fdProcSelfMap = open("/proc/self/maps", O_RDONLY|O_CLOEXEC);
	if (SelfMaps::fdProcSelfMap == -1) {
		DBGS("[!] failed to open /proc/self/maps");
		exit(-1);
	}
}
};

__u32 UnixMemQuery(void *addr) {
    struct procmap_query_compat q = {0};
    q.query_addr= reinterpret_cast<__u64>(addr);
    q.size      = sizeof(q);

    if (ioctl(SelfMaps::fdProcSelfMap, PROCMAP_QUERY_COMPAT, &q) == -1) {
        close(SelfMaps::fdProcSelfMap);
        return (SelfMaps::fdProcSelfMap = -1);
    }
    __u32 perm{0};
    if (!!(q.vma_flags & PROCMAP_QUERY_VMA_READABLE))  perm|=PROT_READ;
    if (!!(q.vma_flags & PROCMAP_QUERY_VMA_WRITABLE))  perm|=PROT_WRITE;
    if (!!(q.vma_flags & PROCMAP_QUERY_VMA_EXECUTABLE))perm|=PROT_EXEC;
    return perm;
}
