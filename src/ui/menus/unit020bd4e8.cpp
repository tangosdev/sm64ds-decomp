//cpp
/* unit020bd4e8 -- ov007 unit 0x020bd4e8..0x020bd9e4: 5 functions, .text only.
 *
 * The title menu's display-engine setup helpers. They carry no state: each
 * programs the main/sub graphics engines, VRAM banks, background modes or
 * capture registers that the menu work dispatcher in unit020b6eb4 calls.
 * ov007 records no original file or class name for the group, so the file
 * keeps the unit<addr> name.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 */
#include "types.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/hw/mmap_global.h"
#include "nitro/hw/registers.h"
#include "nitro/mi/memory.h"

// @symbol _Z17MenuGfx_ResetVramv
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

// @symbol _Z17MenuGfx_SetBgModei
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

// @symbol _Z22MenuGfx_InitMainEnginev
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

// @symbol _Z21MenuGfx_InitSubEnginev
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

// @symbol _Z20MenuGfx_StartCapturev
void MenuGfx_StartCapture(void) {
    REG_DISPCAPCNT = 0x80370010;
}
