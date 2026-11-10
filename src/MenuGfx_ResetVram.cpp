//cpp
#include "nitro/gx/gx_vramcnt.h"

extern "C" {
extern void MultiStore_Int(int val, int *dst, int len);
extern void MultiStore32Bytes(unsigned val, void *dst, int len);

void MenuGfx_ResetVram(void) {
    volatile int a;
    volatile int b;
    volatile int c;

    GX_ResetBankForBG();
    GX_ResetBankForOBJ();
    GX_ResetBankForTex();
    GX_ResetBankForTexPltt();
    GX_ResetBankForSubBG();
    GX_ResetBankForSubOBJ();
    GX_SetBankForLCDC(0x1ff);

    b = 0;
    MultiStore_Int(b, (int *)0x6800000, 0xa4000);

    GX_DisableBankForLCDC();

    a = 0xc0;
    MultiStore32Bytes(a, (int *)0x7000000, 0x400);

    c = 0;
    MultiStore_Int(c, (int *)0x5000000, 0x400);
}
}
