#include "../../include/pe64/lsd_pe64_api.hh"

extern "C" {
pe64_image *new_pe64(char *file) {
    return new pe64_image((char *)file, (char *)PE_SYMTAB_DIR);
}

void free_pe64(pe64_image *pe) { delete pe; }
};
