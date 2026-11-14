//cpp
#include "types.h"
#include "msl/string.h"
#include "BMD_File.h"
#include "nitro/os/system.h"

/* The index of the BMD palette named name: a search of the model's palette
   table (BMD_File::palettes). An unknown name crashes. dExtAnmTexPat_c's
   prepare step (func_02046d50) uses it. */
int func_02047144(const BMD_File *tbl, const char *name) {
    u32 count = tbl->numPalettes;
    u32 i = 0;
    if (i < count) {
        BMD_Palette *entries = tbl->palettes;
        do {
            if (strcmp(entries[i].name, name) == 0)
                return i;
            i++;
        } while (i < tbl->numPalettes);
    }
    OS_Terminate();
    return -1;
}
