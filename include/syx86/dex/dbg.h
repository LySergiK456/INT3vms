#include "../../common/mem.h"

typedef struct {
	char		filename[0x100];
	x86_regs	regs;
	__u32		num_of_maps;
} dbg_header;

typedef struct {
	int		command;
	
} dbg_command;

typedef struct {
	char	name[0x100];
	__u64	begin;
	__u64	end;
	__u32	perm;
} dbg_memmap;