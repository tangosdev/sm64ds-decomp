//cpp
#include "types.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx_bgcnt.h"


void MenuGfx_SetBgMode(int mode) {
    switch (mode) {
    case 1:
        GX_ResetBankForBG();
        GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
        GX_SetBankForBG(GX_VRAM_BG_128_D);
        G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                         GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01);
        G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0800,
                             GX_BG_CHARBASE_0x0c000);
        G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                             GX_BG_CHARBASE_0x10000);
        G2_SetBG0Priority(2);
        G2_SetBG1Priority(3);
        G2_SetBG3Priority(1);
        G2_SetBG2Priority(0);
        break;
    case 2:
        GX_ResetBankForBG();
        GX_SetBankForLCDC(GX_VRAM_LCDC_D);
        GX_SetBankForBG(GX_VRAM_BG_64_E);
        break;
    case 3:
        GX_ResetBankForBG();
        GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BG0_AS_3D);
        GX_SetBankForBG(GX_VRAM_BG_128_D);
        G2_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU,
                              GX_BG_BMPSCRBASE_0x08000);
        G2_SetBG3Priority(1);
        G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                             GX_BG_CHARBASE_0x04000);
        G2_SetBG2Priority(0);
        G2_SetBlendAlpha(0, 8, 0, 0x10);
        break;
    case 4:
        G2_SetBG0Priority(1);
        break;
    case 6:
        GX_ResetBankForBG();
        GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
        GX_SetBankForBG(GX_VRAM_BG_128_D);
        G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                         GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01);
        G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                             GX_BG_CHARBASE_0x10000);
        G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0800,
                             GX_BG_CHARBASE_0x0c000);
        G2_SetBG0Priority(2);
        G2_SetBG1Priority(3);
        G2_SetBG2Priority(0);
        break;
    }

    switch (mode) {
    case 1:
        GX_SetVisiblePlane(DISPLAY_LAYER_BG0 | DISPLAY_LAYER_BG1 | DISPLAY_LAYER_BG3 |
                           DISPLAY_LAYER_OBJ);
        GXS_SetVisiblePlane(DISPLAY_LAYER_BG2 | DISPLAY_LAYER_BG3 | DISPLAY_LAYER_OBJ);
        break;
    case 3:
        GX_SetVisiblePlane(DISPLAY_LAYER_BG3);
        break;
    case 4:
        GX_SetVisiblePlane(DISPLAY_LAYER_BG0 | DISPLAY_LAYER_BG3 | DISPLAY_LAYER_OBJ);
        break;
    case 5:
        GX_SetVisiblePlane(DISPLAY_LAYER_BG0 | DISPLAY_LAYER_BG3 | DISPLAY_LAYER_OBJ);
        break;
    case 6:
        GX_SetVisiblePlane(DISPLAY_LAYER_BG0 | DISPLAY_LAYER_BG1 | DISPLAY_LAYER_OBJ);
        GXS_SetVisiblePlane(DISPLAY_LAYER_BG0 | DISPLAY_LAYER_BG1 | DISPLAY_LAYER_BG2 |
                            DISPLAY_LAYER_OBJ);
        break;
    }
}
