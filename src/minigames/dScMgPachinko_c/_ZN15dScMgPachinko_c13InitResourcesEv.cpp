//cpp
// @symbol _ZN15dScMgPachinko_c13InitResourcesEv
// recovered name: dScMgPachinko_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgPachinko_c.h"
/* dScMgPachinko_c::InitResources - recovered from vtable slot identity */
extern "C" {
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, void *dst);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern char *_ZN2G213GetBG2CharPtrEv(void);
extern char *_ZN2G212GetBG2ScrPtrEv(void);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern char *_ZN3G2S13GetBG2CharPtrEv(void);
extern char *_ZN3G2S12GetBG2ScrPtrEv(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void func_ov006_020feba8(char *c);
extern void func_ov006_020fc844(char *c);
extern void func_ov006_020fae90(char *c);

extern u8 data_0209d45c;
extern u8 data_0209d454;
}

s32 dScMgPachinko_c::InitResources()
{
    char *c = (char *)this;
    char *b;
    char *dst;
    volatile u16 fillScr;
    volatile u16 fillZero;
    volatile u16 fillIdx;
    int objChar;
    int objPltt;
    int f;
    int n;
    int y;
    int x;

    data_0209d45c |= 8;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e &= ~0x40;
    *(volatile u32 *)0x400001c = 0;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1210;

    f = LoadFile(0x5f);
    DecompressLZ16(f, (void *)func_02054d88());
    Deallocate((void *)f);

    f = LoadFile(0x60);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)f, 0x60, 0x1a0);
    Deallocate((void *)f);

    f = LoadFile(0x61);
    func_02056314((void *)f, 0, 0x800);
    Deallocate((void *)f);

    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000c &= ~0x40;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x5418;

    f = LoadFile(0x46);
    DecompressLZ16(f, (void *)_ZN2G213GetBG2CharPtrEv());
    Deallocate((void *)f);

    b = _ZN2G212GetBG2ScrPtrEv();
    fillScr = 0x3073;
    MultiStore16(fillScr, b, 0x1000);

    f = LoadFile(6);
    func_020563d4((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    objChar = LoadFile(0xe0);
    objPltt = LoadFile(0xe1);
    DecompressLZ16(objChar, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)objPltt, 0, 0x100);

    data_0209d454 |= 0xd;
    *(volatile u16 *)0x400100c &= ~3;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x418;

    func_ov004_020af2f8(c, 0, 2, 0);

    b = (char *)_ZN3G2S13GetBG2CharPtrEv();
    fillZero = 0;
    MultiStore16(fillZero, b, 0x6000);

    n = 0;
    for (y = 0; y < 0x18; y++) {
        for (x = 0; x < 0x20; x++) {
            dst = (char *)((u16 *)_ZN3G2S12GetBG2ScrPtrEv() + x + y * 0x20);
            fillIdx = n;
            MultiStore16(fillIdx, dst, 2);
            n++;
        }
    }

    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 2;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile u32 *)0x400101c = 0;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x208;

    f = LoadFile(0x5c);
    DecompressLZ16(f, (void *)_ZN3G2S13GetBG3CharPtrEv());
    Deallocate((void *)f);

    f = LoadFile(0x5d);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)f, 0x60, 0x1a0);
    Deallocate((void *)f);

    f = LoadFile(0x5e);
    func_020562b4((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3) | 1;
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile u32 *)0x4001010 = 0;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x608;

    f = LoadFile(5);
    func_020564f4((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    DecompressLZ16(objChar, (void *)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)objPltt, 0, 0x100);
    Deallocate((void *)objChar);
    Deallocate((void *)objPltt);

    func_ov006_020feba8(c);
    func_ov006_020fc844(c);
    func_ov006_020fae90(c);
    func_ov004_020b04d0(0x20);

    *(int *)(c + 0xa4) = 1;
    *(u16 *)(c + 0x5c1c) = 0x10;
    *(u16 *)(c + 0x5c24) = 0x60;
    *(int *)(c + 0x5c10) = 1;
    *(int *)(c + 0xa4) = 1;
    return 1;
}
