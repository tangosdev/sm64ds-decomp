//cpp
extern "C" void _ZN2GX15SetBankForSubBGEt(unsigned short a);
extern "C" void _ZN2GX16SetBankForSubOBJEt(unsigned short a);
extern "C" void _ZN3GXS15SetGraphicsModeEi(int a);

extern "C" void func_ov007_020bd500(void)
{
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX16SetBankForSubOBJEt(0x100);
    _ZN3GXS15SetGraphicsModeEi(0);

    *(volatile unsigned short *)0x400100a = (*(volatile unsigned short *)0x400100a & 0x43) | 0x314;
    *(volatile unsigned short *)0x400100c = (*(volatile unsigned short *)0x400100c & 0x43) | 0x84;
    *(volatile unsigned short *)0x400100e = (*(volatile unsigned short *)0x400100e & 0x43) | 0x184;
    *(volatile unsigned short *)0x4001008 = (*(volatile unsigned short *)0x4001008 & 0x43) | 0x284;
    *(volatile unsigned int *)0x4001000 = *(volatile unsigned int *)0x4001000 & 0xffcfffef;
}
