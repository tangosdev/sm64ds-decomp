#include "types.h"
extern void func_020541a4(void);
extern void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
extern void _ZN2GX12SetBankForBGEt(u16 x);
extern void func_02054430(int a);
extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile u16 *p, u16 a, u16 b, u16 c, u16 d);

#define BG0CNT (*(volatile u16 *)0x4000008)
#define BG1CNT (*(volatile u16 *)0x400000a)
#define BG2CNT (*(volatile u16 *)0x400000c)
#define BG3CNT (*(volatile u16 *)0x400000e)
#define DISPCNT (*(volatile u32 *)0x4000000)
#define DISPCNT_SUB (*(volatile u32 *)0x4001000)

void func_ov007_020bd648(int mode)
{
    switch (mode) {
    case 1:
        func_020541a4();
        _ZN2GX15SetGraphicsModeEiii(1, 0, 1);
        _ZN2GX12SetBankForBGEt(8);
        BG1CNT = (BG1CNT & 0x43) | 4;
        BG3CNT = (BG3CNT & 0x43) | 0x10c;
        BG2CNT = (BG2CNT & 0x43) | 0x210;
        BG0CNT = (BG0CNT & ~3) | 2;
        BG1CNT = (BG1CNT & ~3) | 3;
        BG3CNT = (BG3CNT & ~3) | 1;
        BG2CNT = (BG2CNT & ~3);
        break;
    case 2:
        func_020541a4();
        func_02054430(8);
        _ZN2GX12SetBankForBGEt(0x10);
        break;
    case 3:
        func_020541a4();
        _ZN2GX15SetGraphicsModeEiii(1, 3, 1);
        _ZN2GX12SetBankForBGEt(8);
        BG3CNT = (BG3CNT & 0x43) | 0x4284;
        BG3CNT = (BG3CNT & ~3) | 1;
        BG2CNT = (BG2CNT & 0x43) | 4;
        BG2CNT = (BG2CNT & ~3);
        _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4000050, 0, 8, 0, 0x10);
        break;
    case 4:
        BG0CNT = (BG0CNT & ~3) | 1;
        break;
    case 6:
        func_020541a4();
        _ZN2GX15SetGraphicsModeEiii(1, 0, 1);
        _ZN2GX12SetBankForBGEt(8);
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
