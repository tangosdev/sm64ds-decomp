//cpp
#include "nitro/gx/gx_vramcnt.h"

extern "C" void MenuGfx_InitMainEngine(void) {
    GX_SetBankForTex(3);
    GX_SetBankForTexPltt(0x20);
    GX_SetBankForOBJ(0x40);

    *(volatile unsigned short*)0x4000008 &= ~0x40;
    *(volatile unsigned short*)0x400000a &= ~0x40;
    *(volatile unsigned short*)0x400000c &= ~0x40;
    *(volatile unsigned short*)0x400000e &= ~0x40;
    *(volatile unsigned short*)0x400000c &= ~3;
    *(volatile unsigned int*)0x4000000 &= 0xffcfffef;
}
