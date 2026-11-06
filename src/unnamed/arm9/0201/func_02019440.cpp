//cpp
#include "types.h"
#include "GX.h"
#include "dGraph_c.h"
#include "nitro/hw/registers.h"

extern "C" {

extern dGraph_c::callback_c *data_0209d4a8; /* the scene graph callback (d_s_dsmt.cpp) */
extern int data_0209d4ac; /* the brightness fader (d_scene.cpp) */
/* the active transition fader (d_s_stage_methods.cpp wipes, KuppaScript.cpp colour fades) */
extern int data_0209d4b0;

extern void func_0203bd24(void);
extern int func_02053be0(int enable);
extern void func_02054430(int a);
extern void MultiStore32Bytes(unsigned val, void *dst, int len);
extern void func_0200f4f4(void);
extern void func_0200f46c(void);

/* Display bring-up at boot (func_0201a054), the NitroSDK demo idiom: power
   both 2D engines and the 3D engine (POWCNT1 0x20e, GX_SetPower), enable the
   V-blank IRQ (func_02053be0, GX_VBlankIntr), give every VRAM bank to the
   LCDC (func_02054430, GX_SetBankForLCDC 0x1ff) and clear all 0xa4000 bytes
   of it, switch the LCDC mapping off again (GX_DisableBankForLCDC), hide
   every OAM entry (fill 0xc0) and clear the palettes. It then drops the
   three fader/callback pointers and runs the 3D setup (func_0200f4f4). The
   volatile fill values are what the ROM loads. */
void func_02019440(void) {
    volatile int b;
    volatile int a;
    volatile int c;
    func_0203bd24();
    REG_POWCNT1 = (REG_POWCNT1 & 0xfffffdf1) | 0x20e;
    func_02053be0(1);
    func_02054430(0x1ff);
    a = 0;
    MultiStore32Bytes(a, (int *)0x6800000, 0xa4000);
    GX::DisableBankForLCDC();
    b = 0xc0;
    MultiStore32Bytes(b, (int *)0x7000000, 0x400);
    c = 0;
    MultiStore32Bytes(c, (int *)0x5000000, 0x400);
    data_0209d4a8 = 0;
    data_0209d4ac = 0;
    data_0209d4b0 = 0;
    func_0200f4f4();
    func_0200f46c();
}
}
