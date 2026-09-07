//cpp
#include "Stage.h"
extern "C" {
extern unsigned char data_0209d45c[];
extern unsigned char data_0209d454[];
extern unsigned short data_0209f360[];
extern unsigned char data_0209f2b4[];
extern unsigned char data_0209f2e0[];
extern unsigned int data_0209b454[];
extern unsigned int data_0209b464[];
extern unsigned char data_0209f294[];
extern unsigned char data_0209f290[];
extern void _ZN7Message17DisplayVsExitTextEt(unsigned short n);
extern void _ZN5Stage17UpdateMenuButtonsEb(bool b);
extern void SetBg2Offset(int a, int b);
extern void SetSubBg0Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(volatile unsigned short *p, int val, short amt);

}

void Stage::VE_Init()
{
    unsigned int a;
    unsigned int b;

    a = data_0209d45c[0] ^ 0x10;
    b = data_0209d454[0] ^ 0x10;
    *(volatile unsigned short *)0x400100a = (*(volatile unsigned short *)0x400100a & 0x43) | 0x1100;
    *(volatile unsigned short *)0x400000c = (*(volatile unsigned short *)0x400000c & 0x43) | 0x1010;
    _ZN7Message17DisplayVsExitTextEt(0x1f);
    data_0209f360[0] = 0xe0;
    data_0209f360[1] = 0x1a0;
    data_0209f2b4[0] = 2;
    data_0209f2e0[0] = 0;
    _ZN5Stage17UpdateMenuButtonsEb(true);
    SetBg2Offset(0, 0);
    SetSubBg0Offset(0, 0);
    SetSubBg1Offset(0, 0);
    data_0209d45c[0] |= 4;
    data_0209d454[0] |= 3;
    data_0209b454[0] |= 0x40000000;
    data_0209b464[0] = data_0209b454[0];
    data_0209f294[0] = 1;
    data_0209f290[0] = 0;
    _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short *)0x4000050, a | 0x20, -7);
    _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short *)0x4001050, b | 0x20, -7);
}
