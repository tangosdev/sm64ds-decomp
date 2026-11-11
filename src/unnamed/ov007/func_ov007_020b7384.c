#include "Ov007Display.h"

/* src/engine/gx/GXS.cpp's OAM copies: 0x07000000 (main) and 0x07000400
 * (sub) plus offset. */
extern void GX_LoadOAM(const void *src, unsigned int offset, unsigned int count);
extern void GXS_LoadOAM(const void *src, unsigned int offset, unsigned int count);

/* Copies the two 0x400-byte OAM buffers into the main and sub OAM. */
void func_ov007_020b7384(Ov007Display *d) {
    GX_LoadOAM(d->oamMain, 0, 0x400);
    GXS_LoadOAM(d->oamSub, 0, 0x400);
}
