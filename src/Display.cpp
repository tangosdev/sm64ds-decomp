//cpp
/* Frame-start/display-commit helpers, BG scroll shadows, 3D engine power
 * switches, and the sound-mode byte. mwccarm emits the bodies in reverse,
 * so the high sound-mode and display-bringup functions come first. */
#include "types.h"
#include "OAM.h"
#include "dGraph_c.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/g3x.h"
#include "nitro/gx/g3imm_cmd.h"
#include "nitro/hw/registers.h"
#include "nitro/hw/mmap_global.h"
#include "nitro/mi/memory.h"

extern "C" void func_02011d5c(int mode);
extern "C" void func_0203bd24(void);
extern "C" void func_0200f468(void);
extern "C" void func_0200f46c(void);
extern void func_0200f4b4(void);
extern void func_0200f4f4(void);
extern void func_02018ec0(void);
extern void func_02018efc(void);
extern void func_0202345c(void);
extern "C" int func_02019018(void);

class dFdBrightness_c;

extern u8 data_0208ee00;

u8 gSubScreenLayers;
u8 data_0209d458;
u8 gMainScreenLayers;
u8 data_0209d460;
u8 data_0209d464;
short gBg0HOffset;
short gBg0VOffset;
short gSubBg2VOffset;
short gSubBg3HOffset;
short gBg2VOffset;
short gSubBg2HOffset;
short gSubBg1VOffset;
short gSubBg1HOffset;
short gSubBg3VOffset;
short gBg3HOffset;
short gBg3VOffset;
short gSubBg0HOffset;
short gSubBg0VOffset;
short gBg2HOffset;
short gBg1VOffset;
short gBg1HOffset;
dGraph_c::callback_c *gGraphCallback;
dFdBrightness_c *data_0209d4ac;
dFdBrightness_c *data_0209d4b0;
union SoundModeStorage {
    u8 byte;
    u32 word;
};
union SoundModeStorage data_0209d4b4;

extern "C" int GetSoundMode(void) {
    return data_0209d4b4.byte;
}

extern "C" void SetSoundMode(int mode) {
    data_0209d4b4.byte = mode;
    switch (mode) {
    case 0:
        func_02011d5c(1);
        break;
    case 1:
        func_02011d5c(2);
        break;
    default:
        func_02011d5c(0);
        break;
    }
}

void func_02019440(void) {
    volatile int b;
    volatile int a;
    volatile int c;
    func_0203bd24();
    GX_SetPower(GX_POWER_ALL);
    GX_VBlankIntr(1);
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    a = 0;
    MI_CpuFillFast((void *)HW_LCDC_VRAM, a, HW_LCDC_VRAM_SIZE);
    GX_DisableBankForLCDC();
    b = 0xc0;
    MI_CpuFillFast((void *)HW_OAM, b, HW_OAM_SIZE);
    c = 0;
    MI_CpuFillFast((void *)HW_PLTT, c, HW_PLTT_SIZE);
    gGraphCallback = 0;
    data_0209d4ac = 0;
    data_0209d4b0 = 0;
    func_0200f4f4();
    func_0200f46c();
}

void func_02019404(void) {
    dGraph_c::callback_c *callback = gGraphCallback;
    if (!callback)
        return;
    callback->GraphCallback1();
}

void func_02019390(void) {
    dGraph_c::callback_c *callback = gGraphCallback;
    if (callback != 0) {
        if (callback->GraphCallback0() == 0) {
            func_0202345c();
            func_02018efc();
            return;
        }
    }
    OAM::Reset();
    if (func_02019018() != 0) {
        func_0200f4b4();
    }
    func_0200f468();
    func_0202345c();
    func_02018efc();
    func_02018ec0();
}

void func_02019144(void) {
    dGraph_c::callback_c *callback = gGraphCallback;
    if (callback != 0) {
        if (callback->GraphCallback2() == 0)
            return;
    }
    OAM::Flush();
    OAM::Load();
    GX_SetVisiblePlane(gMainScreenLayers);
    GXS_SetVisiblePlane(gSubScreenLayers);
    if (!(REG_DISPCNT & 8)) {
        G2_SetBG0Offset(gBg0HOffset, gBg0VOffset);
    } else {
        G3X_SetHOffset(gBg0HOffset);
    }
    G2_SetBG1Offset(gBg1HOffset, gBg1VOffset);
    G2_SetBG2Offset(gBg2HOffset, gBg2VOffset);
    G2_SetBG3Offset(gBg3HOffset, gBg3VOffset);
    REG_DISPCNT_SUB;
    G2S_SetBG0Offset(gSubBg0HOffset, gSubBg0VOffset);
    G2S_SetBG1Offset(gSubBg1HOffset, gSubBg1VOffset);
    G2S_SetBG2Offset(gSubBg2HOffset, gSubBg2VOffset);
    G2S_SetBG3Offset(gSubBg3HOffset, gSubBg3VOffset);
}

extern "C" void func_02019100(void) {
    dGraph_c::callback_c *o = gGraphCallback;
    if (o)
        o->GraphCallback3();
    data_0209d464 = 0;
}

extern "C" void func_020190b8(void) {
    if (func_02019018() == 0)
        return;
    G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);
    data_0209d464 = 1;
}

extern "C" void Enable3dEngines(void) {
    func_020190b8();
    REG_POWCNT1 |= 8;
    REG_POWCNT1 |= 4;
    data_0208ee00 = 1;
}

extern "C" void Disable3dEngines(void) {
    func_020190b8();
    REG_POWCNT1 &= ~8;
    REG_POWCNT1 &= ~4;
    data_0208ee00 = 0;
}

extern "C" int func_02019018(void) {
    return data_0208ee00;
}

extern "C" void SetBg0Offset(int a, int b) {
    gBg0HOffset = a;
    gBg0VOffset = b;
}

extern "C" void SetBg1Offset(int a, int b) {
    gBg1HOffset = a;
    gBg1VOffset = b;
}

extern "C" void SetBg2Offset(int a, int b) {
    gBg2HOffset = a;
    gBg2VOffset = b;
}

extern "C" void SetBg3Offset(int a, int b) {
    gBg3HOffset = a;
    gBg3VOffset = b;
}

extern "C" void SetSubBg0Offset(int a, int b) {
    gSubBg0HOffset = a;
    gSubBg0VOffset = b;
}

extern "C" void SetSubBg1Offset(int a, int b) {
    gSubBg1HOffset = a;
    gSubBg1VOffset = b;
}

extern "C" void SetSubBg2Offset(int a, int b) {
    gSubBg2HOffset = a;
    gSubBg2VOffset = b;
}

extern "C" void SetSubBg3Offset(int a, int b) {
    gSubBg3HOffset = a;
    gSubBg3VOffset = b;
}
