//cpp
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "dScene_c.h"
#include "nitro/hw/registers.h"

extern "C" {

extern void ResetCrashScreen(void);
extern void ShowCrashScreen(void);
extern u8 data_0208a178[];
extern char data_0208c178;

void InitCrashScreen(void)
{
    dScene_c::ResetHardwareRegisters();
    *(volatile short *)0x4000050 = 0;
    *(volatile short *)0x4001050 = 0;
    GX_SetBankForBG(8);
    *(volatile int *)0x4000000 = (*(volatile int *)0x4000000 & ~0x1f00) | 0x200;
    *(volatile short *)0x400000a = (unsigned short)((REG_BG1CNT & 0x43) | 0x1e10);
    GX_LoadBG1Char(data_0208a178, 0, 0x2000);
    *(volatile short *)0x5000000 = 0x7c00;
    GX_LoadBGPltt(&data_0208c178, 0x1a0, 0x60);
    ResetCrashScreen();
    ShowCrashScreen();
    GX_DispOn();
    *(volatile int *)0x4001000 = *(volatile int *)0x4001000 & ~0x10000;
}
}
