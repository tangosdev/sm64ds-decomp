//cpp
#include "types.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "GX.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/gx_bgcnt.h"

extern "C" {

/* Display setup shared by the scenes in src/game/stages/d_s_entry_methods.cpp
 * and src/game/stages/d_s_mb.cpp: VRAM banks, both engines' BG modes with
 * 2D-mapped OBJ characters, and the BG controls the loaders func_0200f220
 * and func_0200f13c fill. Each BG control store keeps the priority and
 * mosaic bits (& 0x43), then clears mosaic. */
void func_0200f2cc(void)
{
    GX::DisableAllBanks();
    GX_SetBankForTex(GX_VRAM_TEX_01_AB);
    GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0123_E);
    GX_SetBankForBG(GX_VRAM_BG_128_C);
    GX_SetBankForOBJ(GX_VRAM_OBJ_32_FG);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_48_HI);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);

    GX_SetBGCharOffset(0);
    GX_SetBGScrOffset(0);
    /* Main engine on the top screen. */
    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);

    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                         GX_BG_CHARBASE_0x10000);
    G2_BG3Mosaic(0);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_512x512, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0800,
                         GX_BG_CHARBASE_0x08000);
    G2_BG2Mosaic(0);
    GXS_SetGraphicsMode(GX_BGMODE_0);

    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                      GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01);
    G2S_BG0Mosaic(0);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0800,
                          GX_BG_CHARBASE_0x04000);
    G2S_BG2Mosaic(0);
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3800,
                          GX_BG_CHARBASE_0x04000);
    G2S_BG3Mosaic(0);
}
}
