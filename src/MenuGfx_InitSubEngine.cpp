//cpp
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/hw/registers.h"

void MenuGfx_InitSubEngine(void) {
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_16_I);
    GXS_SetGraphicsMode(GX_BGMODE_0);

    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1800,
                      GX_BG_CHARBASE_0x14000, GX_BG_EXTPLTT_01);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_256, GX_BG_SCRBASE_0x0000,
                          GX_BG_CHARBASE_0x04000);
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_256, GX_BG_SCRBASE_0x0800,
                          GX_BG_CHARBASE_0x04000);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_256, GX_BG_SCRBASE_0x1000,
                      GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01);
    REG_DISPCNT_SUB = REG_DISPCNT_SUB & 0xffcfffef;
}
