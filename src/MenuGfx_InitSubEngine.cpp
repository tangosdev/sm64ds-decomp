//cpp
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"

extern "C" void MenuGfx_InitSubEngine(void) {
    GX_SetBankForSubBG(4);
    GX_SetBankForSubOBJ(0x100);
    GXS_SetGraphicsMode(0);

    *(volatile unsigned short *)0x400100a = (*(volatile unsigned short *)0x400100a & 0x43) | 0x314;
    *(volatile unsigned short *)0x400100c = (*(volatile unsigned short *)0x400100c & 0x43) | 0x84;
    *(volatile unsigned short *)0x400100e = (*(volatile unsigned short *)0x400100e & 0x43) | 0x184;
    *(volatile unsigned short *)0x4001008 = (*(volatile unsigned short *)0x4001008 & 0x43) | 0x284;
    *(volatile unsigned int *)0x4001000 = *(volatile unsigned int *)0x4001000 & 0xffcfffef;
}
