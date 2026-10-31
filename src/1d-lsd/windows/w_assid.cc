#include "../../../include/1d-lsd/1dlsd.hh"
#include "../../../include/1d-lsd/windows/assid.hh"
#include "../../../include/conf.hh"
#include "../../../include/common/utils.hh"

namespace SlitYaX86
{
	WLyss::WLyss(w1Dlsd *xlsd, std::shared_ptr<PE64> pe)
        :   Lyss(xlsd, *dynamic_cast<Binary_t*>(pe.get())),
            lsd{xlsd}, base_pe64{pe}
    {}
};

namespace SlitYaX86 {
__u64 WLyss::MemConv(Binary_t*, MemConvType, __u64, MemConvType) {
	return 0;
}
__u64 WLyss::get_size(enum _1DLSD_TYPE t) {
	return 0;
}
void WLyss::align_bin(__u64 off, __u64 sz) {
	return;
}
};