#include "types.h"
// @symbol func_ov006_02103ed0
// @emits dScMgPachinko2_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgPachinko2_c::InitResources - recovered from vtable slot identity */
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, void *dst);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern unsigned _ZN3G2S13GetBG2CharPtrEv(void);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern char *_ZN3G2S12GetBG2ScrPtrEv(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);

extern u8 data_0209d45c;
extern u8 data_0209d454;

int dScMgPachinko2_c_InitResources(void *arg0)
{
    char *c = (char *)arg0;
    char *b;
    char *dst;
    volatile u16 spC;
    volatile u16 spE;
    int sp4;
    int sp8;
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

    sp4 = LoadFile(0xe0);
    sp8 = LoadFile(0xe1);
    DecompressLZ16(sp4, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)sp8, 0, 0x100);

    data_0209d454 |= 0xd;
    *(volatile u16 *)0x400100c &= ~3;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x418;

    b = (char *)_ZN3G2S13GetBG2CharPtrEv();
    spC = 0;
    MultiStore16(spC, b, 0x6000);

    n = 0;
    for (y = 0; y < 0x18; y++) {
        for (x = 0; x < 0x20; x++) {
            dst = (char *)((u16 *)_ZN3G2S12GetBG2ScrPtrEv() + x + y * 0x20);
            spE = n;
            MultiStore16(spE, dst, 2);
            n++;
        }
    }

    func_ov004_020af2f8(c, 0, 2, 0);

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

    DecompressLZ16(sp4, (void *)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)sp8, 0, 0x100);
    Deallocate((void *)sp4);
    Deallocate((void *)sp8);

    func_ov006_02103bfc(c);
    func_ov006_02100084(c);
    func_ov006_021024e0((int)c);
    func_ov006_020fffec(c);
    func_ov004_020b04d0(0x20);

    *(int *)(c + 0xa4) = 1;
    *(u16 *)(c + 0x566c) = 0x10;
    *(int *)(c + 0x5660) = 1;
    return 1;
}
