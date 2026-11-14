//cpp
#include "nitro/gx/gx_load.h"
#include "Ov007Display.h"


/* Copies a character record's tiles to `offset` in one VRAM target: 0..4
 * the main engine's OBJ, BG0, BG1, BG2 and BG3 characters, 5..9 the sub
 * engine's. */
void func_ov007_020c33d0(Ov007CharRecord *rec, int target, unsigned int offset) {
    unsigned int tileSize = rec->is8bpp == 0 ? 0x20 : 0x40;
    unsigned int size = (unsigned int)rec->width * rec->height * tileSize;
    switch (target) {
    case 0:
        GX_LoadOBJ(rec->tiles, offset, size);
        break;
    case 1:
        GX_LoadBG0Char(rec->tiles, offset, size);
        break;
    case 2:
        GX_LoadBG1Char(rec->tiles, offset, size);
        break;
    case 3:
        GX_LoadBG2Char(rec->tiles, offset, size);
        break;
    case 4:
        GX_LoadBG3Char(rec->tiles, offset, size);
        break;
    case 5:
        GXS_LoadOBJ(rec->tiles, offset, size);
        break;
    case 6:
        GXS_LoadBG0Char(rec->tiles, offset, size);
        break;
    case 7:
        GXS_LoadBG1Char(rec->tiles, offset, size);
        break;
    case 8:
        GXS_LoadBG2Char(rec->tiles, offset, size);
        break;
    case 9:
        GXS_LoadBG3Char(rec->tiles, offset, size);
        break;
    }
}
