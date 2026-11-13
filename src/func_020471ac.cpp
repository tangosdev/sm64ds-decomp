//cpp
#include "types.h"
#include "msl/string.h"
#include "BMD_File.h"
#include "nitro/os/system.h"

/* The index of the BMD texture called name: the walk over numTextures and
   the 0x14-byte textures records. */
int func_020471ac(const BMD_File *tbl, const char *name) {
    u32 count = tbl->numTextures;
    u32 i = 0;
    if (i < count) {
        BMD_Texture *entry = tbl->textures;
        do {
            if (strcmp(entry->name, name) == 0)
                return (int)i;
            i++;
            entry++;
        } while (i < tbl->numTextures);
    }
    OS_Terminate();
    return -1;
}
