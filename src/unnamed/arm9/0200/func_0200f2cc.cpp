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
    GX_SetBankForTex(3);
    GX_SetBankForTexPltt(0x10);
    GX_SetBankForBG(4);
    GX_SetBankForOBJ(0x60);
    GX_SetBankForSubBG(0x180);
    GX_SetBankForSubOBJ(8);

    GX_SetBGCharOffset(0);
    GX_SetBGScrOffset(0);
    /* Main engine on the top screen. */
    REG_POWCNT1 |= 0x8000;
    GX_SetGraphicsMode(1, 0, 1);

    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    REG_BG3CNT = (u16)((REG_BG3CNT & 0x43) | 0x10);
    G2_BG3Mosaic(0);
    REG_BG2CNT = (u16)((REG_BG2CNT & 0x43) | 0xc108);
    G2_BG2Mosaic(0);
    GXS_SetGraphicsMode(0);

    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    REG_DB_BG0CNT = (u16)((REG_DB_BG0CNT & 0x43) | 4);
    G2S_BG0Mosaic(0);
    REG_DB_BG2CNT = (u16)((REG_DB_BG2CNT & 0x43) | 0x104);
    G2S_BG2Mosaic(0);
    REG_DB_BG3CNT = (u16)((REG_DB_BG3CNT & 0x43) | 0x704);
    G2S_BG3Mosaic(0);
}
}
