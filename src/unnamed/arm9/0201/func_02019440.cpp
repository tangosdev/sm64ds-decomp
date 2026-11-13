//cpp
#include "types.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "dGraph_c.h"
#include "nitro/hw/registers.h"
#include "nitro/mi/memory.h"
#include "nitro/gx/gx.h"



extern dGraph_c::callback_c *gGraphCallback;
 /* the scene graph callback (d_s_dsmt.cpp) */
class dFdBrightness_c;
extern dFdBrightness_c *data_0209d4ac;
 /* the brightness fader (d_scene.cpp) */
/* the active transition fader (d_s_stage_methods.cpp wipes, KuppaScript.cpp colour fades) */
extern int data_0209d4b0;
extern "C" {


extern void func_0203bd24(void);
}

extern void func_0200f4f4(void);
extern "C" {

extern void func_0200f46c(void);
}


/* Display bring-up at boot (func_0201a054), the NitroSDK demo idiom: power
   both 2D engines and the 3D engine (POWCNT1 0x20e, GX_SetPower), enable the
   V-blank IRQ (GX_VBlankIntr), give every VRAM bank to the
   LCDC (GX_SetBankForLCDC 0x1ff) and clear all 0xa4000 bytes
   of it, switch the LCDC mapping off again (GX_DisableBankForLCDC), hide
   every OAM entry (fill 0xc0) and clear the palettes. It then drops the
   three fader/callback pointers and runs the 3D setup (func_0200f4f4). The
   volatile fill values are what the ROM loads. */
void func_02019440(void) {
    volatile int b;
    volatile int a;
    volatile int c;
    func_0203bd24();
    GX_SetPower(GX_POWER_ALL);
    GX_VBlankIntr(1);
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    a = 0;
    MI_CpuFillFast((int *)0x6800000, a, 0xa4000);
    GX_DisableBankForLCDC();
    b = 0xc0;
    MI_CpuFillFast((int *)0x7000000, b, 0x400);
    c = 0;
    MI_CpuFillFast((int *)0x5000000, c, 0x400);
    gGraphCallback = 0;
    data_0209d4ac = 0;
    data_0209d4b0 = 0;
    func_0200f4f4();
    func_0200f46c();
}
