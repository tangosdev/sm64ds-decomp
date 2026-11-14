//cpp
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx.h"

void MenuGfx_InitMainEngine(void) {
    GX_SetBankForTex(GX_VRAM_TEX_01_AB);
    GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0_F);
    GX_SetBankForOBJ(GX_VRAM_OBJ_16_G);

    G2_BG0Mosaic(0);
    G2_BG1Mosaic(0);
    G2_BG2Mosaic(0);
    G2_BG3Mosaic(0);
    G2_SetBG2Priority(0);
    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
}
