//cpp
#include "Ov007Display.h"
#include "nitro/os/cache.h"
#include "nitro/mi/dma.h"

extern "C" {

extern int func_ov007_020be0dc(void);
extern int func_ov007_020c0b20(Ov007OamList *list, void *oam, int slot);
extern void func_ov007_020be3bc(void);

/* Redraws both OAM buffers: steps the particle sprites
 * (func_ov007_020be0dc), clears each buffer to hidden entries, draws its
 * node list into it, copies in the affine matrices (func_ov007_020be3bc)
 * and flushes both from the data cache. */
void Display_DrawOam(Ov007Display *d) {
    func_ov007_020be0dc();
    MI_DmaFill32(3, d->oamMain, 0xc0, 0x400);
    MI_DmaFill32(3, d->oamSub, 0xc0, 0x400);
    func_ov007_020c0b20(d->oamListMain, d->oamMain, 0);
    func_ov007_020c0b20(d->oamListSub, d->oamSub, 0);
    func_ov007_020be3bc();
    DC_FlushRange(d->oamMain, 0x400);
    DC_FlushRange(d->oamSub, 0x400);
}
}
