//cpp
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx.h"

extern "C" void MenuGfx_InitMainEngine(void) {
    GX_SetBankForTex(3);
    GX_SetBankForTexPltt(0x20);
    GX_SetBankForOBJ(0x40);

    G2_BG0Mosaic(0);
    G2_BG1Mosaic(0);
    G2_BG2Mosaic(0);
    G2_BG3Mosaic(0);
    G2_SetBG2Priority(0);
    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
}
