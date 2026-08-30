//cpp
// @symbol _ZN11dScMgCard_c13InitResourcesEv
#include "decl_common.h"
#include "dScMgCard_c.h"
/* dScMgCard_c::InitResources -- vtable slot 0.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgCard_c, this class's own slot 0
 * (fBase_c::InitResources). The old file's `recovered name:
 * dScMgCard_c_InitResources` agreed.
 *
 * Minigame graphics init: loads/decompresses the OBJ tiles+palettes for both
 * engines, sets blending, patches the OAM attr template list, spawns the two
 * rows of 5 slot sprites, and resets the shared counters.
 *
 * The final `((VtObj *)c)->m18(-1)` is a self-dispatch through this class's own
 * vtable slot 18 -- left as a raw vtable-shim call, same shape the pre-migration
 * file used, just through `this` instead of a `char *c` parameter. */
/* One OAM attribute-template entry: the list is a u32 attr word, then the two
 * 16-bit attrs, then an 8-byte stride. Spelled as a struct rather than as casts
 * off a u32* because the ROM addresses all three off ONE base register at
 * displacements 0/4/6; `*((u16 *)e + 2)` makes b56 materialise a second base
 * (`add r2, r3, #4`), and that one extra instruction moves the literal pool. */
typedef struct OamAttrTmpl {
    u32 attr0; /* 0x0 */
    u16 attr2; /* 0x4 */
    u16 attr3; /* 0x6 */
} OamAttrTmpl;

typedef struct Slot6 {
    char b[0x30];
} Slot6;

typedef struct Obj6 {
    char _p0[0x51a8];
    Slot6 rowA[5]; /* 0x51a8 */
    Slot6 rowB[5]; /* 0x5298 */
} Obj6;

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
extern u32 *data_ov006_02134028;
/* data_ov004_020beb68 comes from dScMgBase_c.h (void*) -- not redeclared here,
   see its own use below. */

extern void func_ov006_0210a534(char *);
extern void *LoadFile(int);
extern int GetGameLanguage(void);
extern void DecompressLZ16(void *, u32);
extern void _ZN2GX11LoadOBJPlttEPKvjj(void *, u32, u32);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(void *, u32, u32);
extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile u16 *, int, int, int, int);
extern void func_ov006_020c0aa8(char *);
extern int func_ov006_020c1a88(char *);
}

s32 dScMgCard_c::InitResources()
{
    dScMgCard_c *self = this;
    char *c = (char *)this;
    void *f7, *f6, *f5, *f4;
    int v[2];

    func_ov004_020b04d0(0x20);
    func_ov006_0210a534(c);
    data_0209d45c = 0x11;
    data_0209d454 = 0x18;
    f7 = LoadFile(0xbd);
    f6 = LoadFile(0xbe);
    f5 = LoadFile(data_ov006_0213bcb0[GetGameLanguage()]);
    f4 = LoadFile(0xbb);
    DecompressLZ16(f7, 0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj(f6, 0, 0x20);
    DecompressLZ16(f5, 0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj(f4, 0, 0x100);
    Deallocate(f7);
    Deallocate(f6);
    Deallocate(f5);
    Deallocate(f4);
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x04000050, 0, 1, 0x10, 8);
    *(volatile u16 *)0x04001050 = 0;

    {
        OamAttrTmpl *e = (OamAttrTmpl *)data_ov006_02134028;
        u16 last;
        do {
            e->attr0 = (e->attr0 & ~0xc00) | 0x400;
            /* NOT `e->attr2 &= ~0xf000;`. Compound assignment makes b56 CSE the
             * field's address and materialise it (`add r2, r3, #4`), then reach
             * attr3 at +2 off that; the ROM keeps ONE base and uses +4 / +6.
             * The extra instruction moves the literal pool and shifts every
             * pc-relative load in the function -- it is the whole 4-byte size
             * difference that kept this file pinned to 1.2/base. */
            u16 a2 = e->attr2;
            e->attr2 = (u16)(a2 & ~0xf000);
            last = e->attr3;
            e++;
        } while (last != 0xffff);
    }

    *(volatile u16 *)0x04000008 = (*(volatile u16 *)0x04000008 & ~3) | 1;

    func_ov006_020c0aa8(c + 0x4660);
    if (func_ov006_020c1a88(c + 0x4f38) == 0)
        return 0;

    {
        int y = 0x2c;
        int i;
        for (i = 0; i < 5; i++) {
            v[0] = y << 12;
            v[1] = 0x80000;
            func_ov006_020da0ac(((Obj6 *)c)->rowA[i].b, v);
            v[1] = 0x10000;
            func_ov006_020da0ac(((Obj6 *)c)->rowB[i].b, v);
            y += 0x28;
        }
    }

    self->mState = 0;
    self->mStateTimer = 0;
    *(int *)(c + 0xa8) = func_ov004_020ad8b8();
    *(int *)(c + 0xac) = *(int *)(c + 0xa8);
    {
        int r = func_ov004_020ad878();
        if (data_ov004_020beb68 != 0)
            *(int *)((char *)data_ov004_020beb68 + 0xb4) = r;
    }
    self->mScore = 0;
    func_ov004_020b682c();
    ((VtObj *)c)->m18(-1);
    return 1;
}
