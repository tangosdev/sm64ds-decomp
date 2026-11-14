#include "Ov007Display.h"
#include "nitro/gx/gx_load.h"

/* Copies the two 0x400-byte OAM buffers into the main (0x07000000) and sub
 * (0x07000400) OAM. */
void func_ov007_020b7384(Ov007Display *d) {
    GX_LoadOAM(d->oamMain, 0, 0x400);
    GXS_LoadOAM(d->oamSub, 0, 0x400);
}
