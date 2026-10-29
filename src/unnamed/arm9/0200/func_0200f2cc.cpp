//cpp
#include "types.h"
#include "GX.h"

extern "C" {

void func_0200f2cc(void)
{
    GX::DisableAllBanks();
    GX::SetBankForTex(3);
    GX::SetBankForTexPltt(0x10);
    GX::SetBankForBG(4);
    GX::SetBankForOBJ(0x60);
    GX::SetBankForSubBG(0x180);
    GX::SetBankForSubOBJ(8);

    *(volatile u32 *)0x4000000 &= ~0x7000000;
    *(volatile u32 *)0x4000000 &= ~0x38000000;
    *(volatile u16 *)0x4000304 |= 0x8000;
    GX::SetGraphicsMode(1, 0, 1);

    *(volatile u32 *)0x4000000 &= 0xffcfffef;
    *(volatile u16 *)0x400000e = (u16)((*(volatile u16 *)0x400000e & 0x43) | 0x10);
    *(volatile u16 *)0x400000e &= ~0x40;
    *(volatile u16 *)0x400000c = (u16)((*(volatile u16 *)0x400000c & 0x43) | 0xc108);
    *(volatile u16 *)0x400000c &= ~0x40;
    GXS::SetGraphicsMode(0);

    *(volatile u32 *)0x4001000 &= 0xffcfffef;
    *(volatile u16 *)0x4001008 = (u16)((*(volatile u16 *)0x4001008 & 0x43) | 4);
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile u16 *)0x400100c = (u16)((*(volatile u16 *)0x400100c & 0x43) | 0x104);
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u16 *)0x400100e = (u16)((*(volatile u16 *)0x400100e & 0x43) | 0x704);
    *(volatile u16 *)0x400100e &= ~0x40;
}
}
