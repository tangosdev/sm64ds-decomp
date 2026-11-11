//cpp
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/hw/registers.h"

extern "C" void MenuGfx_InitSubEngine(void) {
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_16_I);
    GXS_SetGraphicsMode(0);

    REG_DB_BG1CNT = (REG_DB_BG1CNT & 0x43) | 0x314;
    REG_DB_BG2CNT = (REG_DB_BG2CNT & 0x43) | 0x84;
    REG_DB_BG3CNT = (REG_DB_BG3CNT & 0x43) | 0x184;
    REG_DB_BG0CNT = (REG_DB_BG0CNT & 0x43) | 0x284;
    REG_DISPCNT_SUB = REG_DISPCNT_SUB & 0xffcfffef;
}
