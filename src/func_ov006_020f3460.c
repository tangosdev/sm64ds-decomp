// @symbol func_ov006_020f3460
// @emits dScMgLuigi_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgLuigi_c::InitResources - recovered from vtable slot identity */
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern int func_020adc74(void* p);
extern void DecompressLZ16(int src, void* dst);
extern int LoadFile(int handle);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern char* _ZN2G213GetBG2CharPtrEv(void);
extern char* _ZN2G212GetBG2ScrPtrEv(void);
extern void MultiStore16(u16 val, char* dst, int nbytes);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void* p, u32 a, u32 b);
extern unsigned func_02054de8(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern char* _ZN3G2S12GetBG3ScrPtrEv(void);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void* p, u32 a, u32 b);
extern void func_ov006_020f2ec0(void* c);
extern void Ov004_Deallocate(int a);
extern u8 data_0209d45c;
extern u8 data_0209d454;

int dScMgLuigi_c_InitResources(void* arg0) {
    char* c = (char*)arg0;
    char* b;
    volatile u16 sp8;
    volatile u16 spA;
    int r6;
    int f;
    int r5;

    r6 = func_020adc74(&data_ov006_0213cfa0);
    if (r6 == 0) return 0;

    data_0209d45c |= 8;
    *(volatile u16*)0x400000e = (*(volatile u16*)0x400000e & ~3) | 2;
    *(volatile u16*)0x400000e = (*(volatile u16*)0x400000e & 0x43) | 0x1218;

    DecompressLZ16(r6, (void*)func_02054d88());

    f = LoadFile(0x48);
    _ZN2GX10LoadBGPlttEPKvjj((const void*)f, 0x60, 0x1a0);
    Deallocate((void*)f);

    f = LoadFile(0x49);
    func_02056314((void*)f, 0, 0x800);
    Deallocate((void*)f);

    data_0209d45c |= 4;
    *(volatile u16*)0x400000c &= ~3;
    *(volatile u16*)0x400000c &= ~0x40;
    *(volatile u32*)0x4000018 = 0;
    *(volatile u16*)0x400000c = (*(volatile u16*)0x400000c & 0x43) | 0x1410;

    f = LoadFile(0x4b);
    DecompressLZ16(f, (void*)(_ZN2G213GetBG2CharPtrEv() + 0x4000));
    Deallocate((void*)f);

    f = LoadFile(0x4c);
    _ZN2GX10LoadBGPlttEPKvjj((const void*)f, 0x40, 0x20);
    Deallocate((void*)f);

    b = _ZN2G212GetBG2ScrPtrEv();
    sp8 = 0x23e1;
    MultiStore16(sp8, b, 0x800);

    r5 = LoadFile(0xd3);
    f = LoadFile(0xd4);
    DecompressLZ16(r5, (void*)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void*)f, 0, 0x100);

    *(volatile u16*)0x400100c &= ~3;
    *(volatile u16*)0x400100c &= ~0x40;
    *(volatile u32*)0x4001018 = 0;
    *(volatile u16*)0x400100c = (*(volatile u16*)0x400100c & 0x43) | 0x1210;

    {
        int f8;
        f8 = LoadFile(0x4b);
        DecompressLZ16(f8, (void*)(func_02054de8() + 0x4000));
        Deallocate((void*)f8);

        f8 = LoadFile(0x4c);
        _ZN3GXS10LoadBGPlttEPKvjj((const void*)f8, 0x1e0, 0x20);
        Deallocate((void*)f8);

        f8 = LoadFile(0x4d);
        func_02056374((const void*)f8, 0, 0x800);
        Deallocate((void*)f8);

        data_0209d454 |= 8;
        *(volatile u16*)0x400100e &= ~3;
        *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & 0x43) | 0x1410;

        f8 = LoadFile(0x4a);
        b = _ZN3G2S12GetBG3ScrPtrEv();
        spA = 0xf3e1;
        MultiStore16(spA, b, 0x800);
        Deallocate((void*)f8);
    }

    DecompressLZ16(r5, (void*)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void*)f, 0, 0x100);
    Deallocate((void*)r5);
    Deallocate((void*)f);

    *(u8*)(c + 0x5000 + 0x457) = 0;
    func_ov006_020f2ec0(arg0);
    *(volatile u16*)(c + 0x5100 + 0x74) = 0xff;
    func_ov006_020f2e20(c);

    *(int*)(c + 0x4000 + 0xf78) = 0;
    Ov004_Deallocate(r6);
    func_ov004_020b04d0(0x30);
    *(volatile u16*)(c + 0x5100 + 0x72) = 0xa;
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    func_020bc888 = 0x80;
    func_020bc864 = ~0x1b;
    *(int*)(c + 0xb4) = 0;
    return 1;
}
