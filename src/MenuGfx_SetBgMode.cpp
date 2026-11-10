//cpp
#include "types.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/g2.h"

extern "C" {

#define BG0CNT (*(volatile u16 *)0x4000008)
#define BG1CNT (*(volatile u16 *)0x400000a)
#define BG2CNT (*(volatile u16 *)0x400000c)
#define BG3CNT (*(volatile u16 *)0x400000e)
#define DISPCNT (*(volatile u32 *)0x4000000)
#define DISPCNT_SUB (*(volatile u32 *)0x4001000)

void MenuGfx_SetBgMode(int mode) {
    switch (mode) {
    case 1:
        GX_ResetBankForBG();
        GX_SetGraphicsMode(1, 0, 1);
        GX_SetBankForBG(8);
        BG1CNT = (BG1CNT & 0x43) | 4;
        BG3CNT = (BG3CNT & 0x43) | 0x10c;
        BG2CNT = (BG2CNT & 0x43) | 0x210;
        BG0CNT = (BG0CNT & ~3) | 2;
        BG1CNT = (BG1CNT & ~3) | 3;
        BG3CNT = (BG3CNT & ~3) | 1;
        BG2CNT = (BG2CNT & ~3);
        break;
    case 2:
        GX_ResetBankForBG();
        GX_SetBankForLCDC(8);
        GX_SetBankForBG(0x10);
        break;
    case 3:
        GX_ResetBankForBG();
        GX_SetGraphicsMode(1, 3, 1);
        GX_SetBankForBG(8);
        BG3CNT = (BG3CNT & 0x43) | 0x4284;
        BG3CNT = (BG3CNT & ~3) | 1;
        BG2CNT = (BG2CNT & 0x43) | 4;
        BG2CNT = (BG2CNT & ~3);
        G2x_SetBlendAlpha_((volatile u16 *)0x4000050, 0, 8, 0, 0x10);
        break;
    case 4:
        BG0CNT = (BG0CNT & ~3) | 1;
        break;
    case 6:
        GX_ResetBankForBG();
        GX_SetGraphicsMode(1, 0, 1);
        GX_SetBankForBG(8);
        BG1CNT = (BG1CNT & 0x43) | 4;
        BG3CNT = (BG3CNT & 0x43) | 0x210;
        BG2CNT = (BG2CNT & 0x43) | 0x10c;
        BG0CNT = (BG0CNT & ~3) | 2;
        BG1CNT = (BG1CNT & ~3) | 3;
        BG2CNT = (BG2CNT & ~3);
        break;
    }

    switch (mode) {
    case 1:
        DISPCNT = (DISPCNT & ~0x1f00) | 0x1b00;
        DISPCNT_SUB = (DISPCNT_SUB & ~0x1f00) | 0x1c00;
        break;
    case 3:
        DISPCNT = (DISPCNT & ~0x1f00) | 0x800;
        break;
    case 4:
        DISPCNT = (DISPCNT & ~0x1f00) | 0x1900;
        break;
    case 5:
        DISPCNT = (DISPCNT & ~0x1f00) | 0x1900;
        break;
    case 6:
        DISPCNT = (DISPCNT & ~0x1f00) | 0x1300;
        DISPCNT_SUB = (DISPCNT_SUB & ~0x1f00) | 0x1700;
        break;
    }
}
}
