bits 64

section .note.GNU-stack noalloc noexec nowrite progbits

section .text
global YAYO_RELOC

struc yayo_px
    .t:				resq 1
	.dat:			resq 1
	.orig_off:		resq 1
	.dst_off:		resq 1
	.orig_size:		resq 1
	.dst_size:		resq 1
endstruc

YAYO_RELOC:
	int3