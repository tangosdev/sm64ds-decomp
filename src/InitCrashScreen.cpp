//cpp
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/g2.h"
#include "dScene_c.h"
#include "nitro/hw/registers.h"



extern void ResetCrashScreen(void);
extern void ShowCrashScreen(void);
extern u8 data_0208a178[];
extern char data_0208c178;

void InitCrashScreen(void)
{
    dScene_c::ResetHardwareRegisters();
    G2_BlendNone();
    G2S_BlendNone();
    GX_SetBankForBG(GX_VRAM_BG_128_D);
    GX_SetVisiblePlane(DISPLAY_LAYER_BG1);
    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xf000,
                     GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01);
    GX_LoadBG1Char(data_0208a178, 0, 0x2000);
    *(volatile short *)0x5000000 = 0x7c00;
    GX_LoadBGPltt(&data_0208c178, 0x1a0, 0x60);
    ResetCrashScreen();
    ShowCrashScreen();
    GX_DispOn();
    GXS_DispOff();
}
