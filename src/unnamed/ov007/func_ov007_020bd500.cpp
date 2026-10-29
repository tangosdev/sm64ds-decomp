//cpp
#include "GX.h"

namespace GXS {
void SetGraphicsMode(int a);
}
extern "C" void func_ov007_020bd500(void)
{
    GX::SetBankForSubBG(4);
    GX::SetBankForSubOBJ(0x100);
    GXS::SetGraphicsMode(0);

    *(volatile unsigned short *)0x400100a = (*(volatile unsigned short *)0x400100a & 0x43) | 0x314;
    *(volatile unsigned short *)0x400100c = (*(volatile unsigned short *)0x400100c & 0x43) | 0x84;
    *(volatile unsigned short *)0x400100e = (*(volatile unsigned short *)0x400100e & 0x43) | 0x184;
    *(volatile unsigned short *)0x4001008 = (*(volatile unsigned short *)0x4001008 & 0x43) | 0x284;
    *(volatile unsigned int *)0x4001000 = *(volatile unsigned int *)0x4001000 & 0xffcfffef;
}
