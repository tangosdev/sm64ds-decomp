//cpp
#include "Ov007Display.h"
#include "CP15.h"

extern "C" {
extern int func_ov007_020be0dc(void *c);
extern void DMASyncFillTransfer(unsigned int ch, void *dst, unsigned int val, unsigned int n);
extern int func_ov007_020c0b20(Ov007OamList *list, void *oam, int slot);
extern void func_ov007_020be3bc(void);

/* Redraws both OAM buffers: steps the particle sprites
 * (func_ov007_020be0dc), clears each buffer to hidden entries, draws its
 * node list into it, copies in the affine matrices (func_ov007_020be3bc)
 * and flushes both from the data cache. */
void func_ov007_020bc9c8(Ov007Display *d) {
    func_ov007_020be0dc(d);
    DMASyncFillTransfer(3, d->oamMain, 0xc0, 0x400);
    DMASyncFillTransfer(3, d->oamSub, 0xc0, 0x400);
    func_ov007_020c0b20(d->oamListMain, d->oamMain, 0);
    func_ov007_020c0b20(d->oamListSub, d->oamSub, 0);
    func_ov007_020be3bc();
    CP15::FlushAndInvalidateDataCache((unsigned int)d->oamMain, 0x400);
    CP15::FlushAndInvalidateDataCache((unsigned int)d->oamSub, 0x400);
}
}
