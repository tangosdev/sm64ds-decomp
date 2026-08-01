#include "types.h"
// @symbol func_ov006_021073b0
// recovered name: dScMgPanel_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgPanel_c::InitResources - recovered from vtable slot identity */
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, void *dst);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern char *_ZN2G212GetBG2ScrPtrEv(void);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern char *_ZN3G2S12GetBG2ScrPtrEv(void);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void FreeGfxSlotsById(int arg);
extern void func_ov006_021067a4(void *p);
extern void func_ov006_021063a0(void *p);
extern void func_ov006_02106168(void *p);

extern u8 data_0209d45c;
extern u8 data_0209d454;

int func_ov006_021073b0(void *arg0)
{
    char *c = (char *)arg0;
    char *b;
    volatile u16 sp8;
    volatile u16 spA;
    int f;
    int r5;

    data_0209d45c |= 8;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e &= ~0x40;
    *(volatile u32 *)0x400001c = 0;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1210;

    f = LoadFile(0x6a);
    DecompressLZ16(f, (void *)(func_02054d88() + 0x4000));
    Deallocate((void *)f);

    f = LoadFile(0x6b);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)f, 0x1a0, 0x60);
    Deallocate((void *)f);

    f = LoadFile(0x6c);
    func_02056314((void *)f, 0, 0x800);
    Deallocate((void *)f);

    data_0209d45c |= 4;
    *(volatile u16 *)0x400000c &= ~0x40;
    *(volatile u32 *)0x4000018 = 0;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x9410;

    b = _ZN2G212GetBG2ScrPtrEv();
    sp8 = 0xf361;
    MultiStore16(sp8, b, 0x1000);

    f = LoadFile(0x1e6);
    func_020563d4((const void *)f, 0x800, 0x800);
    Deallocate((void *)f);

    r5 = LoadFile(0xe5);
    if (GetOwnerLanguage() == 5) {
        r5 = LoadFile(0xe7);
    } else if (GetOwnerLanguage() == 4) {
        r5 = LoadFile(0xe4);
    } else if (GetOwnerLanguage() == 3) {
        r5 = LoadFile(0xe3);
    } else if (GetOwnerLanguage() == 2) {
        r5 = LoadFile(0xe2);
    }

    f = LoadFile(0xe6);
    DecompressLZ16(r5, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)f, 0, 0x100);

    data_0209d454 |= 8;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 1;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile u32 *)0x400101c = 0;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 1;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x210;

    {
        int f7 = LoadFile(0x6a);
        DecompressLZ16(f7, (void *)(_ZN3G2S13GetBG3CharPtrEv() + 0x4000));
        Deallocate((void *)f7);

        f7 = LoadFile(0x6b);
        _ZN3GXS10LoadBGPlttEPKvjj((const void *)f7, 0x1a0, 0x60);
        Deallocate((void *)f7);

        f7 = LoadFile(0x69);
        func_020562b4((const void *)f7, 0, 0x800);
        Deallocate((void *)f7);

        data_0209d454 |= 4;
        *(volatile u16 *)0x400100c &= ~0x40;
        *(volatile u32 *)0x4001018 = 0;
        *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 1;
        *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x8410;

        b = _ZN3G2S12GetBG2ScrPtrEv();
        spA = 0xf361;
        MultiStore16(spA, b, 0x1000);

        f7 = LoadFile(0x1e6);
        func_02056374((const void *)f7, 0, 0x800);
        Deallocate((void *)f7);

        *(volatile u16 *)0x4001008 &= ~3;
        *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x610;

        f7 = LoadFile(0x1e5);
        func_020564f4((const void *)f7, 0, 0x800);
        Deallocate((void *)f7);
    }

    DecompressLZ16(r5, (void *)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)f, 0, 0x100);
    Deallocate((void *)r5);
    Deallocate((void *)f);

    FreeGfxSlotsById(0x1d);
    func_ov006_021067a4(arg0);
    func_ov006_02106758(c);

    *(int *)(c + 0x4cb0) = 0;
    *(u8 *)(c + 0x4fea) = 0;
    *(int *)(c + 0x4cb4) = 0xff;

    func_ov006_021063a0(arg0);
    func_ov006_02106168(arg0);
    func_ov006_02105118(c);
    func_ov006_02104b24(c);

    *(int *)(c + 0x4ca8) = 1;
    *(int *)(c + 0x4cac) = 0;
    *(u8 *)(c + 0x4fe2) = 3;

    func_ov004_020b04d0(0x20);
    func_ov004_020b0cac(0xd, 0x80, 0x40, 0, -1, 0xd);
    *(int *)(c + 0xb4) = 0;
    return 1;
}
