#include "Ov007Display.h"
#include "nitro/gx/gx_load.h"

/* Copies the two 0x400-byte OAM buffers into the main (0x07000000) and sub
 * (0x07000400) OAM. */
void func_ov007_020b7384(Ov007Display *d) {
    _Z10GX_LoadOAMPKvjj(d->oamMain, 0, 0x400);
    _Z11GXS_LoadOAMPKvjj(d->oamSub, 0, 0x400);
}
