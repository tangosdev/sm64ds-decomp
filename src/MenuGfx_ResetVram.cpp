//cpp
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/mi/memory.h"

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
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);

    b = 0;
    MI_CpuFill32((int *)0x6800000, b, 0xa4000);

    GX_DisableBankForLCDC();

    a = 0xc0;
    MI_CpuFillFast((int *)0x7000000, a, 0x400);

    c = 0;
    MI_CpuFill32((int *)0x5000000, c, 0x400);
}
