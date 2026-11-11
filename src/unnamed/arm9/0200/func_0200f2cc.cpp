//cpp
#include "types.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "GX.h"
#include "nitro/hw/registers.h"

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

    /* Main engine character and screen base offsets to 0. */
    REG_DISPCNT &= ~0x7000000;
    REG_DISPCNT &= ~0x38000000;
    /* Main engine on the top screen. */
    REG_POWCNT1 |= 0x8000;
    GX_SetGraphicsMode(1, 0, 1);

    /* OBJ characters 2D-mapped (bit 4 and the 1D boundary bits 20-21 clear). */
    REG_DISPCNT &= 0xffcfffef;
    REG_BG3CNT = (u16)((REG_BG3CNT & 0x43) | 0x10);
    REG_BG3CNT &= ~0x40;
    REG_BG2CNT = (u16)((REG_BG2CNT & 0x43) | 0xc108);
    REG_BG2CNT &= ~0x40;
    GXS_SetGraphicsMode(0);

    REG_DISPCNT_SUB &= 0xffcfffef;
    REG_DB_BG0CNT = (u16)((REG_DB_BG0CNT & 0x43) | 4);
    REG_DB_BG0CNT &= ~0x40;
    REG_DB_BG2CNT = (u16)((REG_DB_BG2CNT & 0x43) | 0x104);
    REG_DB_BG2CNT &= ~0x40;
    REG_DB_BG3CNT = (u16)((REG_DB_BG3CNT & 0x43) | 0x704);
    REG_DB_BG3CNT &= ~0x40;
}
}
