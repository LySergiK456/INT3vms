#ifndef YAYO_HH
#define YAYO_HH
#include "./binary.hh"
extern "C" {
	#include "../types.h"
	#include "./yayo.h"
	#include "../xtc/xtc.h"
	#include "../../x86disass/disass.h"
	#include "../common/llist.h"
};

namespace SlitYaX86
{
	struct YayoPx : public yayo_px {
		YayoPx *_rel_(enum yayo_px_t t, void *dat, __u64 off, __u64 size) {
			struct yayo_px px = {
				.t=t, .data=dat, orig_off=orig_off, .orig_size=orig_size
			};
			return reinterpret_cast<YayoPx*>(llist_push((_LL_TYPE_*)&ll_yayo_px, &px));
		}
	};

	class Yayo {
		rel_yayo	rel{};
		struct {
			Binary_t	*bin;
			__u64	offset;
			__u64	size;
		} src;
	public:
		Yayo(Binary_t *_bin, __u64 off, __u64 size)
			: src{_bin, off, size}
		{
			rel.bin = (decltype(rel.bin)){
				dynamic_cast<bin_t*>(src.bin), src.offset, src.size
			};
		    llist_init(&rel.ll_yayo_px,	sizeof(struct yayo_px));
		    llist_init(&rel.ll_xtc_px,	sizeof(struct xtc_px));
		}

		YayoPx *_rel_(enum yayo_px_t t, void *dat, __u64 orig_off, __u64 orig_size) {
			struct yayo_px px = {
				.t=t, .data=dat, orig_off=orig_off, .orig_size=orig_size
			};
			return reinterpret_cast<YayoPx*>(llist_push((_LL_TYPE_*)&rel.ll_yayo_px, &px));
		}
	};
};
#endif
