//cpp
// @symbol _ZN14dScMgD3DBase_c9Virtual84Ev
/* dScMgD3DBase_c::Virtual84 - slot 33, this class's own override.  It backs
   five vtables: its own and the four children that inherit it unchanged
   (dScMgJump_c, dScMgJump2_c, dScMgTrampoline_c, dScMgTrampoline2_c).

   See the slot-33 block in include/dScMgBase_c.h for why the base slot has no
   ROM name.  Unlike slots 30 and 31 on this class, there is no misattribution
   to fix here -- this file never carried a `recovered name:` line at all.

   It is the base's engine bring-up rewritten for 3D, not a wrapper around it.
   GX::DisableAllBanks() first, then banks reassigned with texture and texture-
   palette banks in the mix (SetBankForTex, SetBankForTexPltt) where the 2D base
   only needs BG and OBJ; graphics modes come out 1/0/1 and GXS 5 rather than
   1/0/0 and 0.  It keeps the decompressed file pointers alive in
   data_ov006_02141a4c / _02141a48 instead of freeing them, runs
   InitialiseVramGlobals(), and publishes three of the object's own buffers
   (+0x466c twice and +0x4728) into data_ov006_02141a44 / _40 / _50.

   dScMgSingle3DBase_c's override at ov006:0x0210a708 is the third body of this
   slot -- same job again, and it lives inside that class's promoted
   intact-object TU, src/actors/dScMgSingle3DBase_c.cpp. */
#include "types.h"
#include "dScMgD3DBase_c.h"
extern "C" {
extern void func_ov004_020b290c(void);
extern void func_ov004_020b2980(void);
extern void _ZN2GX15DisableAllBanksEv(void);
extern void _ZN2GX13SetBankForTexEt(u16 a);
extern void _ZN2GX17SetBankForTexPlttEt(u16 a);
extern void func_02054748(int x);
extern void _ZN2GX13SetBankForOBJEt(u16 a);
extern void _ZN2GX12SetBankForBGEt(u16 a);
extern int GetGameLanguage(void);
extern void *func_ov004_020adc68(int id);
extern void DecompressLZ16(void *src, void *dst);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void InitialiseVramGlobals(void);
extern void func_ov004_020b0d30(void);
extern void func_ov006_020e7428(void);
extern void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
extern void _ZN3GXS15SetGraphicsModeEi(int a);
}

extern "C" {
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern u8 data_0209e660;
extern int data_ov006_0213c5fc[];
extern void *data_ov006_02141a4c;
extern void *data_ov006_02141a48;
extern int data_0208ee44;
extern void *data_ov006_02141a44;
extern void *data_ov006_02141a40;
extern void *data_ov006_02141a50;
extern int data_ov004_020beb74[];
extern void *data_0209d4a8;
}

void dScMgD3DBase_c::Virtual84()
{
    char *obj = (char *)this;

    void *p;

    *(vu32 *)0x4001000u |= 0x10000u;
    data_0209d45c = 0x10;
    data_0209d454 = 0x10;
    func_ov004_020b290c();
    func_ov004_020b2980();
    data_0209e660 = 1;
    _ZN2GX15DisableAllBanksEv();
    _ZN2GX13SetBankForTexEt(1);
    _ZN2GX17SetBankForTexPlttEt(0x40);
    *(vu32 *)0x4000000u &= ~0x7000000u;
    *(vu32 *)0x4000000u &= ~0x38000000u;
    func_02054748(0);
    _ZN2GX13SetBankForOBJEt(0x10);
    _ZN2GX12SetBankForBGEt(2);
    p = func_ov004_020adc68(data_ov006_0213c5fc[GetGameLanguage()]);
    data_ov006_02141a4c = p;
    {
        char *dst = (char *)0x6400000;
        dst += 0x4000;
        DecompressLZ16(p, dst);
    }
    p = func_ov004_020adc68(0xc3);
    data_ov006_02141a48 = p;
    _ZN2GX11LoadOBJPlttEPKvjj(p, 0x100u, 0x100u);
    _ZN3GXS11LoadOBJPlttEPKvjj(data_ov006_02141a48, 0x100u, 0x100u);
    InitialiseVramGlobals();
    func_ov004_020b0d30();
    func_ov006_020e7428();
    data_0208ee44 = 1;
    _ZN2GX15SetGraphicsModeEiii(1, 0, 1);
    _ZN3GXS15SetGraphicsModeEi(5);
    *(vu32 *)0x4000000u &= 0xffcfffefu;
    data_ov006_02141a44 = obj + 0x466c;
    data_ov006_02141a40 = obj + 0x466c;
    data_ov006_02141a50 = obj + 0x4728;
    data_ov004_020beb74[1] = (int)obj;
    data_0209d4a8 = (void *)data_ov004_020beb74;
}
