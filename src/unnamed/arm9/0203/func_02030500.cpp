//cpp
#include "types.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "GX.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/g2.h"

extern "C" {


extern void Disable3dEngines(void);
}

extern s32 GetLanguageIndexVS(void);
extern "C" {

extern void DecompressLZ16(const void *src, void *dst);
extern void SetBg1Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);
}


/* The visible-plane masks of the two engines, kept beside DISPCNT bits 8-12
   (BG0-BG3, OBJ): d_a_demo.cpp sets and clears bits here and writes the
   mask back to the register the same way. */
extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;
extern void *data_020927b0[];
extern void *data_020927c4[];
extern void *data_020927d8[];
extern u8 data_0209440c;
extern u8 data_0209fc58;

/* Put up the same language-indexed BG1 picture on both screens: hide every
   plane, map the VRAM banks, set engine A to graphics mode 1, load the
   characters, palette and one of two screen maps (data_0209fc58 bits 4-5
   choose) on both engines, then show BG1 alone. */
void func_02030500(void) {
    void *p;

    gMainScreenLayers = 0;
    gSubScreenLayers = 0;
    GX_SetVisiblePlane(0);
    GXS_SetVisiblePlane(0);

    Disable3dEngines();
    GX::DisableAllBanks();
    GX_SetBankForBG(GX_VRAM_BG_128_A);
    GX_SetBankForOBJ(GX_VRAM_OBJ_128_B);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);

    GX_SetBGCharOffset(0);
    GX_SetBGScrOffset(0);
    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
    G2_BlendNone();
    G2S_BlendNone();
    GX_SetVisibleWnd(GX_WNDMASK_NONE);
    GXS_SetVisibleWnd(GX_WNDMASK_NONE);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);

    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                     GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01);
    G2_BG1Mosaic(0);
    GXS_SetGraphicsMode(GX_BGMODE_0);

    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                      GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01);
    G2S_BG1Mosaic(0);

    DecompressLZ16(data_020927b0[GetLanguageIndexVS()], G2_GetBG1CharPtr());
    DecompressLZ16(data_020927b0[GetLanguageIndexVS()], G2S_GetBG1CharPtr());

    GX_LoadBGPltt(&data_0209440c, 0, 0x40);
    GXS_LoadBGPltt(&data_0209440c, 0, 0x40);

    if (data_0209fc58 & 0x30) {
        p = data_020927d8[GetLanguageIndexVS()];
    } else {
        p = data_020927c4[GetLanguageIndexVS()];
    }
    DecompressLZ16(p, G2_GetBG1ScrPtr());
    DecompressLZ16(p, G2S_GetBG1ScrPtr());

    G2_SetBG1Priority(0);
    G2S_SetBG1Priority(0);
    SetBg1Offset(0, 0);
    SetSubBg1Offset(0, 0);

    gMainScreenLayers = DISPLAY_LAYER_BG1;
    gSubScreenLayers = DISPLAY_LAYER_BG1;
    GX_SetVisiblePlane(DISPLAY_LAYER_BG1);
    GXS_SetVisiblePlane(DISPLAY_LAYER_BG1);
}
