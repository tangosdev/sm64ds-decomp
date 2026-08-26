//cpp
// @symbol _ZN10dScEntry_c15graphCallback_c14GraphCallback2Ev
#include "dScEntry_c.h"

extern "C" {
unsigned short *_ZN3G2S12GetBG2ScrPtrEv();
void DecompressLZ16(void *, void *);
void Deallocate(void *);
void func_ov075_021160dc(char *);
}

int dScEntry_c::graphCallback_c::GraphCallback2()
{
    volatile unsigned short *reg = (volatile unsigned short *)0x400100c;
    unsigned short value = (unsigned short)(*reg & ~0x1f00);
    *reg = value | (bg2Priority << 8);

    if (compressedBg2Screen != 0) {
        unsigned short *screen = _ZN3G2S12GetBG2ScrPtrEv();
        DecompressLZ16(compressedBg2Screen, screen);
        Deallocate(compressedBg2Screen);
        compressedBg2Screen = 0;
    }

    if (entryScene != 0)
        func_ov075_021160dc((char *)entryScene);

    return 1;
}
