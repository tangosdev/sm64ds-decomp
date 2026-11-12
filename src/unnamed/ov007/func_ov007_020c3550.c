#include "Ov007Display.h"
#include "Ov007MenuMem.h"

/* Allocates a character record (the menu's alloc hook) and fills it. */
Ov007CharRecord *func_ov007_020c3550(const void *tiles, int width, int height, int is8bpp) {
    Ov007CharRecord *rec = func_ov007_020c3df4(0, sizeof(Ov007CharRecord));
    rec->tiles = tiles;
    rec->width = width;
    rec->height = height;
    rec->is8bpp = is8bpp;
    rec->reserved = 0;
    return rec;
}
