//cpp
// @symbol _ZN7dScMB_c13InitResourcesEv
/* recovered: named members + real C++ method */
/* dScMB_c::InitResources() -- vtable slot 0. Brings both engines up for the
 * Multi-Boot download screen, registers the GLOBAL graphCallback_c object at
 * data_020a0c68 with `this` as its context, then zeroes the state machine
 * Behavior() drives and hands its own FaderColor member to dScene_c. */
#include "dScMB_c.h"
#include "decl_common.h"

extern "C" {
void MultiStore16(u16 val, char *dst, int nbytes);
void DecompressLZ16(int src, void *dst);
void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
int func_0201a244(int a0, int a1, int a2, int a3, int a4);

extern u8 data_0209d45c[];
extern u8 data_0209d454[];
extern u8 data_0209d4a8[];
extern u8 data_0208ee44[];
extern u8 func_0201a2f8[];
}

s32 dScMB_c::InitResources()
{
    volatile u16 sp4;
    volatile u16 sp6;

    func_02053b98();
    *(volatile u32 *)0x4001000 &= ~0x10000;
    func_02030aa4(2);

    *(volatile u16 *)0x4000304 = (*(volatile u16 *)0x4000304 & ~0x20e) | 0x20e;
    func_0200f2cc();

    *(volatile u16 *)0x400000a = (*(volatile u16 *)0x400000a & 0x43) | 0x704;
    *(volatile u16 *)0x400000a &= ~0x40;
    *(volatile u16 *)0x400100a = (*(volatile u16 *)0x400100a & 0x43) | 0x408;
    *(volatile u16 *)0x400100a &= ~0x40;

    sp4 = 0xffff;
    MultiStore16(sp4, (char *)0x5000000, 2);
    sp6 = 0xffff;
    MultiStore16(sp6, (char *)0x5000400, 2);

    {
        char *vram = (char *)0x6400000;
        vram += 0x1280;
        DecompressLZ16((int)&data_0209446c, vram);
    }
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)&data_0209444c, 0, 0x20);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)&data_020945d0, 0xa0, 0x20);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)&data_0209444c, 0, 0x20);

    *(volatile u16 *)0x400000a &= ~3;
    *(volatile u16 *)0x400100a &= ~3;
    SetBg1Offset(0, 0);
    SetSubBg1Offset(0, 0);

    *(u8 *)&data_0209d45c = 0;
    *(u8 *)&data_0209d454 = 0;
    *(volatile u32 *)0x4000000 &= ~0x1f00;
    *(volatile u32 *)0x4001000 &= ~0x1f00;
    _ZN2GX6DispOnEv();

    *(volatile u32 *)0x4001000 |= 0x10000;
    func_02034b1c((void *)&data_020a0c68, (int)this);

    *(void **)&data_0209d4a8 = (void *)&data_020a0c68;
    unk_060 = 0;
    unk_064 = 0;
    dScene_c::SetFaders(&fader);

    fader.color = 0x7fff;
    *(int *)&data_0208ee44 = 1;
    *(int *)&data_020a0c64 = func_0201a244((int)&func_0201a2f8, 0, 0xf, 0, 0x1000);

    return 1;
}
