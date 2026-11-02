//cpp
#include "types.h"
#include "cstd.h"
#include "BMD_File.h"

extern "C" {

extern void Crash(void);

/* The index of the BMD texture called name: the walk over numTextures and
   the 0x14-byte textures records. */
int func_020471ac(const BMD_File *tbl, const char *name) {
    u32 count = tbl->numTextures;
    u32 i = 0;
    if (i < count) {
        BMD_Texture *entry = tbl->textures;
        do {
            if (cstd::strcmp(entry->name, name) == 0)
                return (int)i;
            i++;
            entry++;
        } while (i < tbl->numTextures);
    }
    Crash();
    return -1;
}
}
