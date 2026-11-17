//cpp
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/mi/memory.h"
#include "nitro/hw/mmap_global.h"

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
    MI_CpuFill32((void *)HW_LCDC_VRAM, b, HW_LCDC_VRAM_SIZE);

    GX_DisableBankForLCDC();

    a = 0xc0;
    MI_CpuFillFast((void *)HW_OAM, a, HW_OAM_SIZE);

    c = 0;
    MI_CpuFill32((void *)HW_PLTT, c, HW_PLTT_SIZE);
}
