//cpp
// @symbol _ZN15dScMgRoulette_c13InitResourcesEv
#include "decl_common.h"
#include "dScMgRoulette_c.h"
/* dScMgRoulette_c::InitResources -- vtable slot 0.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgRoulette_c, this class's own
 * slot 0 (fBase_c::InitResources). The old file's `recovered name:
 * dScMgRoulette_c_InitResources` agreed.
 *
 * Minigame dual-screen graphics init: sets both sub BG layers, loads the
 * BG tiles/maps/palette and the shared OBJ tiles/palettes for both engines,
 * then initializes the board, cursor and slider objects.
 *
 * The final `((VtObj *)c)->m18(-1)` is a self-dispatch through this class's own
 * vtable slot 18 -- left as a raw vtable-shim call, same shape the pre-migration
 * file used, just through `this` instead of a `char *c` parameter. */
struct VtObj {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual void d4();
    virtual void d5();
    virtual void d6();
    virtual void d7();
    virtual void d8();
    virtual void d9();
    virtual void d10();
    virtual void d11();
    virtual void d12();
    virtual void d13();
    virtual void d14();
    virtual void d15();
    virtual void d16();
    virtual void d17();
    virtual void m18(int);
};

extern "C" {
extern u8 data_0209d45c;
extern u8 data_0209d454;

extern void *LoadFile(int);
extern int _ZN3G2S13GetBG2CharPtrEv(void);
extern void DecompressLZ16(void *, int);
extern int _ZN3G2S12GetBG2ScrPtrEv(void);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(void *, unsigned int, unsigned int);
extern void _ZN2GX11LoadOBJPlttEPKvjj(void *, unsigned int, unsigned int);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(void *, unsigned int, unsigned int);
extern void func_ov006_020c0aa8(char *);
extern int func_ov006_020c1a88(char *);
}

s32 dScMgRoulette_c::InitResources()
{
    char *c = (char *)this;
    void *f8, *f7, *f6, *f5;

    func_ov004_020b04d0(0x20);

    *(volatile u16 *)0x0400100c = (*(volatile u16 *)0x0400100c & 0x43) | 0x404;
    *(volatile u16 *)0x04001008 = (*(volatile u16 *)0x04001008 & 0x43) | 0x504;

    f8 = LoadFile(0x6f);
    f7 = LoadFile(0x71);
    f6 = LoadFile(0x1e4);
    f5 = LoadFile(0x70);
    DecompressLZ16(f8, _ZN3G2S13GetBG2CharPtrEv());
    DecompressLZ16(f7, _ZN3G2S12GetBG2ScrPtrEv());
    DecompressLZ16(f6, _ZN3G2S12GetBG0ScrPtrEv());
    _ZN3GXS10LoadBGPlttEPKvjj(f5, 0x1a0, 0x60);
    Deallocate(f8);
    Deallocate(f7);
    Deallocate(f6);
    Deallocate(f5);

    {
        volatile u16 *a = (volatile u16 *)0x04001008;
        *a = *a & ~0x40;
        *(volatile u16 *)0x0400100c = *(volatile u16 *)0x0400100c & ~0x40;
        SetSubBg0Offset(0, 0);
        SetSubBg2Offset(0, 0);
        *a = (*a & ~3) | 2;
        *(volatile u16 *)0x0400100c = (*(volatile u16 *)0x0400100c & ~3) | 2;
    }

    f6 = LoadFile(0xe9);
    f5 = LoadFile(0xea);
    DecompressLZ16(f6, 0x6400000);
    DecompressLZ16(f6, 0x6600000);
    _ZN2GX11LoadOBJPlttEPKvjj(f5, 0, 0x100);
    _ZN3GXS11LoadOBJPlttEPKvjj(f5, 0, 0x100);
    Deallocate(f6);
    Deallocate(f5);

    data_0209d45c = 0x11;
    data_0209d454 = 0x15;
    *(volatile u16 *)0x04000008 = (*(volatile u16 *)0x04000008 & ~3) | 1;

    func_ov006_020c0aa8(c + 0x4660);
    if (func_ov006_020c1a88(c + 0x4f38) == 0)
        return 0;
    if (func_ov006_021085c0(c + 0x530c) == 0)
        return 0;

    *(int *)(c + 0xa8) = func_ov004_020ad8b8();
    *(int *)(c + 0xac) = *(int *)(c + 0xa8);
    func_ov004_020b682c();
    func_ov006_02107b70(c + 0x52ac);
    ((VtObj *)c)->m18(-1);
    return 1;
}
