#include "../1d-lsd/1dlsd.hh"
#include "../conf.hh"
#include "./pe64.hh"
#include "../syx86/syx86.hh"
#include "../1d-lsd/1dlsd.hh"
extern "C" {
    #include "../common/mem.h"
    #include "../1d-lsd/binary.h"
    #include "./pe64.h"
};
#include "../1d-lsd/binary.h"

#ifndef KRACK_PE64_C_API_HH
#define KRACK_PE64_C_API_HH
using IMG_DOS   = IMAGE_DOS_HEADER;
using IMG_NT    = IMAGE_NT_HEADERS;
using IMG_SEC   = IMAGE_SECTION_HEADER;

typedef bin_hdr_t pe64_hdr_t;
using Binary_t = SlitYaX86::Binary_t;

struct _pe64_image : SlitYaX86::Binary_t {
    PE64 bin;
    __u8                    *&pMem{bin.pMem};
    pe64_hdr_t              dos;
    pe64_hdr_t              dos_stub;
    pe64_hdr_t              nt;
    std::vector<pe64_hdr_t> sechdr;
    std::vector<pe64_hdr_t> sections;

    _pe64_image(char *file, char *symtab_dir)
        : bin{std::move(PE64(file, symtab_dir))} //, Binary_t(Binary_t_PE32CC, bin.pMap, bin.f.size)
    {
        if (bin.ok != -1 && (ok=-1)) return;
        pe64_hdr_t pe64_headers[3] =
        {   {   /* DOS - Header */
                { (void *)bin.dos, MM_NONE, sizeof(IMG_DOS) },
                0
            },  ////////////////
            {   /* DOS - stub*/
                { (void*)(&bin.pMem[sizeof(IMG_DOS)]), MM_NONE, bin.dos->e_lfanew - sizeof(IMG_DOS) },
                sizeof(IMG_DOS)
            },  ///////////////////
            {   /* NT - Headers */
                { (void *)bin.nt, MM_NONE, sizeof(IMG_NT) },
                bin.dos->e_lfanew
        }   };

        do {
            dos     = pe64_headers[0];
            dos_stub= pe64_headers[1];
            nt      = pe64_headers[2];
        } while (0);

        __u32 i{0};
        PE_EACH_SEC(&bin, sec) {
            sechdr.push_back((pe64_hdr_t){
                { sec, MM_NONE, sizeof(IMG_SEC) },
                bin.dos->e_lfanew + sizeof(IMG_NT) + (i++) * sizeof(IMG_SEC)
            });

            sections.push_back((pe64_hdr_t){
                { (void*)&bin.pMem[sec->PointerToRawData], MM_NONE, sec->SizeOfRawData,
                }, sec->PointerToRawData
            });
        }
    }
};

#endif
