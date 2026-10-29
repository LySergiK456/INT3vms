#include "../types.h"
#include "../common/mem.h"

#ifndef KRACK_PE64_H
#define KRACK_PE64_H
typedef struct _pe64_image pe64_image;
pe64_image *new_pe64(char *file);
#endif