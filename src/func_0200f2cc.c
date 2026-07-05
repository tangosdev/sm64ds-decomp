typedef unsigned short u16;
typedef unsigned int u32;

extern void _ZN2GX15DisableAllBanksEv(void);
extern void _ZN2GX13SetBankForTexEt(u16 v);
extern void _ZN2GX17SetBankForTexPlttEt(u16 v);
extern void _ZN2GX12SetBankForBGEt(u16 v);
extern void _ZN2GX13SetBankForOBJEt(u16 v);
extern void _ZN2GX15SetBankForSubBGEt(u16 v);
extern void _ZN2GX16SetBankForSubOBJEt(u16 v);
extern void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
extern void _ZN3GXS15SetGraphicsModeEi(int a);

void func_0200f2cc(void)
{
    _ZN2GX15DisableAllBanksEv();
    _ZN2GX13SetBankForTexEt(3);
    _ZN2GX17SetBankForTexPlttEt(0x10);
    _ZN2GX12SetBankForBGEt(4);
    _ZN2GX13SetBankForOBJEt(0x60);
    _ZN2GX15SetBankForSubBGEt(0x180);
    _ZN2GX16SetBankForSubOBJEt(8);

    *(volatile u32 *)0x4000000 &= ~0x7000000;
    *(volatile u32 *)0x4000000 &= ~0x38000000;
    *(volatile u16 *)0x4000304 |= 0x8000;
    _ZN2GX15SetGraphicsModeEiii(1, 0, 1);

    *(volatile u32 *)0x4000000 &= 0xffcfffef;
    *(volatile u16 *)0x400000e = (u16)((*(volatile u16 *)0x400000e & 0x43) | 0x10);
    *(volatile u16 *)0x400000e &= ~0x40;
    *(volatile u16 *)0x400000c = (u16)((*(volatile u16 *)0x400000c & 0x43) | 0xc108);
    *(volatile u16 *)0x400000c &= ~0x40;
    _ZN3GXS15SetGraphicsModeEi(0);

    *(volatile u32 *)0x4001000 &= 0xffcfffef;
    *(volatile u16 *)0x4001008 = (u16)((*(volatile u16 *)0x4001008 & 0x43) | 4);
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile u16 *)0x400100c = (u16)((*(volatile u16 *)0x400100c & 0x43) | 0x104);
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u16 *)0x400100e = (u16)((*(volatile u16 *)0x400100e & 0x43) | 0x704);
    *(volatile u16 *)0x400100e &= ~0x40;
}
