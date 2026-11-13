//cpp
#include "nitro/gx/gx_load.h"
#include "Ov007Display.h"



/* Loads a palette record's colours into palette RAM at 0x20-byte row
 * `row`: target 0/1 the main engine's OBJ/BG palette, 2/3 the sub
 * engine's. `skip` moves the source on by bytes but shortens the copy by
 * colours; every ov007 caller passes 0. */
void func_ov007_020c32e8(Ov007Palette *pal, int target, unsigned int row, int skip) {
    unsigned int offset = row << 5;
    unsigned int size = (unsigned int)((pal->colorCount - skip) << 1);
    switch (target) {
    case 0:
        GX_LoadOBJPltt((const char *)pal->colors + skip, offset, size);
        break;
    case 1:
        GX_LoadBGPltt((const char *)pal->colors + skip, offset, size);
        break;
    case 2:
        GXS_LoadOBJPltt((const char *)pal->colors + skip, offset, size);
        break;
    case 3:
        GXS_LoadBGPltt((const char *)pal->colors + skip, offset, size);
        break;
    }
}

