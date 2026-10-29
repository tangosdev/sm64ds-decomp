//cpp
#include "GX.h"
#include "dScene_c.h"

extern "C" {

extern void ResetCrashScreen(void);
extern void ShowCrashScreen(void);
extern char data_0208a178;
extern char data_0208c178;

void InitCrashScreen(void)
{
    dScene_c::ResetHardwareRegisters();
    *(volatile short *)0x4000050 = 0;
    *(volatile short *)0x4001050 = 0;
    GX::SetBankForBG(8);
    *(volatile int *)0x4000000 = (*(volatile int *)0x4000000 & ~0x1f00) | 0x200;
    *(volatile short *)0x400000a = (unsigned short)((*(volatile unsigned short *)0x400000a & 0x43) | 0x1e10);
    GX::LoadBG1Char(&data_0208a178, 0, 0x2000);
    *(volatile short *)0x5000000 = 0x7c00;
    GX::LoadBGPltt(&data_0208c178, 0x1a0, 0x60);
    ResetCrashScreen();
    ShowCrashScreen();
    GX::DispOn();
    *(volatile int *)0x4001000 = *(volatile int *)0x4001000 & ~0x10000;
}
}
