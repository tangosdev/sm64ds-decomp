//cpp
/* ov006/dScMgCard_c -- the Dilar (card-match) minigame scene, as one C++
 * translation unit. 34 functions, ROM 0x020d95a4-0x020dbd54.
 *
 * ENROLLED AND CANONICAL. config/arm9/overlays/ov006/delinks.txt names this
 * one file for that whole .text range, so every byte the cartridge carries
 * there is compiled from here -- nothing in the range is copied out of the
 * ROM any more. The class also emits its own vtable, its six typeinfo
 * records and their name strings; config/tu_manifest.d/ov006/dScMgCard_c.json
 * lists them under "compiler_only_output" with the verdict romdata_check
 * gives each one against the cartridge.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order). The
 * destructor is not written here at all -- it is inline in the class header,
 * which is what puts the D1/D0 pair in cartridge order; see that banner.
 *
 * The 34 members, in ROM address order (the reverse of the order below).
 * Each was a separate one-function source until this commit; the manifest's
 * "functions" array keeps the old path beside every address.
 *   [1] 0x020d95a4  _ZN11dScMgCard_cD1Ev
 *   [2] 0x020d9638  _ZN11dScMgCard_cD0Ev
 *   [3] 0x020d96e0  func_ov006_020d96e0
 *   [4] 0x020d96f0  func_ov006_020d96f0
 *   [5] 0x020d970c  func_ov006_020d970c
 *   [6] 0x020d978c  func_ov006_020d978c
 *   [7] 0x020d9998  func_ov006_020d9998
 *   [8] 0x020d99a4  func_ov006_020d99a4
 *   [9] 0x020d99ec  func_ov006_020d99ec
 *   [10] 0x020d9a14  func_ov006_020d9a14
 *   [11] 0x020d9bd0  func_ov006_020d9bd0
 *   [12] 0x020d9bdc  func_ov006_020d9bdc
 *   [13] 0x020d9c5c  func_ov006_020d9c5c
 *   [14] 0x020da00c  func_ov006_020da00c
 *   [15] 0x020da0ac  func_ov006_020da0ac
 *   [16] 0x020da154  func_ov006_020da154
 *   [17] 0x020da174  func_ov006_020da174
 *   [18] 0x020da420  func_ov006_020da420
 *   [19] 0x020da4ac  func_ov006_020da4ac
 *   [20] 0x020da5e8  func_ov006_020da5e8
 *   [21] 0x020da834  func_ov006_020da834
 *   [22] 0x020da860  func_ov006_020da860
 *   [23] 0x020da88c  func_ov006_020da88c
 *   [24] 0x020da8b8  func_ov006_020da8b8
 *   [25] 0x020da8e4  func_ov006_020da8e4
 *   [26] 0x020da974  func_ov006_020da974
 *   [27] 0x020da994  _ZN11dScMgCard_c16CleanupResourcesEv
 *   [28] 0x020da9c4  _ZN11dScMgCard_c6RenderEv
 *   [29] 0x020dabec  _ZN11dScMgCard_c8BehaviorEv
 *   [30] 0x020dac34  func_ov006_020dac34
 *   [31] 0x020db6ec  _ZN11dScMgCard_c15OnGroundPoundedEv
 *   [32] 0x020db720  _ZN11dScMgCard_c13OnTurnIntoEggEi
 *   [33] 0x020db9dc  _ZN11dScMgCard_c13OnYoshiTryEatEi
 *   [34] 0x020dbaf0  _ZN11dScMgCard_c13InitResourcesEv
 */

/* The union of what the 34 legacy sources included, first-seen in
 * ROM-ascending order. */
#include "dScMgCard_c.h"
#include "types.h"
#include "decl_common.h"
#include "cMgSmartball_object_c.h"

/* Local declarations carried from the legacy sources. Each one pins the
 * call or field shape mwcc has to emit, so swapping one for a real project
 * header type is a byte-affecting change: re-run tools/rombuild.py after.
 * Reconciling them against include/*.h is readability work still open. */
/* shadow struct 'Obj' */
struct Obj {
    virtual void m00();
    virtual void m04();
    virtual int m08();
};

/* shadow typedef 's32' */
typedef int s32;

/* shadow struct 'VObj' */
struct VObj { virtual int f0(); virtual int f1(); virtual int f2(); };

/* shadow struct 'C' */
struct C {
    virtual int m0();
    virtual int m1();
    virtual int m2();
    int f4;      // +4
    int f8;      // +8
    int fc;      // +0xc
    int f10;     // +0x10
    int f14;     // +0x14
    int f18;     // +0x18
    int pad1c;   // +0x1c
    int pad20;   // +0x20
    int f24;     // +0x24
    short f28;   // +0x28
    unsigned char f2a; // +0x2a
    unsigned char f2b; // +0x2b
    unsigned char f2c; // +0x2c
    unsigned char f2d; // +0x2d
};

/* shadow struct 'Src' */
struct Src { int f0; int f4; int f8; int fc; };

/* shadow typedef 's8' */
typedef signed char s8;

/* shadow typedef 'u8' */
typedef unsigned char u8;

/* shadow typedef 's16' */
typedef signed short s16;

/* shadow typedef 'u16' */
typedef unsigned short u16;

/* shadow typedef 'u32' */
typedef unsigned int u32;

/* shadow typedef 'u64' */
typedef unsigned long long u64;

/* shadow typedef 'Struct30' */
typedef struct 
{
  u8 _pad[0x2A];
  u8 unk_2A;
  u8 _pad2B[2];
  u8 unk_2D;
  u8 _pad2E[2];
} Struct30;

/* shadow typedef 'Five' */
typedef struct 
{
  s32 v[5];
} Five;

/* shadow struct 'Node' */
struct Node {
    virtual void m0();
    char pad[0x2c];
};

/* shadow typedef 'OamAttrTmpl' */
typedef struct OamAttrTmpl {
    u32 attr0; /* 0x0 */
    u16 attr2; /* 0x4 */
    u16 attr3; /* 0x6 */
} OamAttrTmpl;

/* shadow typedef 'Slot6' */
typedef struct Slot6 {
    char b[0x30];
} Slot6;

/* shadow typedef 'Obj6' */
typedef struct Obj6 {
    char _p0[0x51a8];
    Slot6 rowA[5]; /* 0x51a8 */
    Slot6 rowB[5]; /* 0x5298 */
} Obj6;

/* shadow struct 'VtObj' */
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


/* TUBUILD CONFLICT -- alternate body of typedef 's32', from the legacy file for func_ov006_020da174, NOT applied:
typedef signed int s32;
*/

/* TUBUILD CONFLICT -- alternate body of typedef 's16', from the legacy file for _ZN11dScMgCard_c13OnTurnIntoEggEi, NOT applied:
typedef short s16;
*/

#define S16(off) (*(s16 *)(c + (off)))

extern "C" {
extern int data_ov006_0213acb0[2];
extern int data_ov006_0213aca8[2];
extern int data_ov006_0213bccc[];
extern unsigned short data_ov006_0213bd64[];
extern void *data_ov006_02141774[];
extern void Hud_RenderSprite(void *sprite, int x, int y, int a3, int a4);
extern int _Z14ApproachLinearRiii(int *a, int b, int c);
extern int _Z15ApproachLinear2Rsss(s16 *a, s16 b, s16 c);
extern int func_ov006_020da8e4(void);
extern int func_ov006_020da834(char *p);
extern int func_ov006_020da860(char *p, int v);
extern void **data_ov006_0213bd30[];
extern void *data_ov006_0213406c[];
void _ZN5Sound12PlayBank2_2DEj(unsigned int n);
unsigned int func_02012790(unsigned int a);
extern int data_ov006_02141768;
extern int data_ov006_0214176c;
extern int data_ov006_02141770;
extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[][4];
extern unsigned char data_020a0deb[][4];
extern "C" void Vec2_Sub(int* o, int* a, int* b);
extern "C" void func_0203d680(int* out, int* in, int scale);
extern void func_ov006_020d99ec(char *c, int n);
extern const Five data_ov006_0213bcd8;
extern int RandomIntInternal(int *seed);
extern int data_ov006_0213bd18[];
extern int data_0209e650;
extern "C" void func_ov006_020c0aa8(void *c);
extern "C" int RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
extern "C" void func_ov004_020b1bc8(char *a0, int a1, int a2, int a3);
extern "C" void func_ov004_020b1e34(void *a0, int a1, int a2, int a3);
extern "C" void func_ov006_020c1804(void *c);
extern int data_ov006_02134028;
void func_ov006_020c19d0(void *table);
void func_ov004_020b65e4(void);
extern int func_ov006_020c0f0c(void *p);
extern int func_ov006_020c1718(void *p);
extern int func_ov006_020da88c(char *p, int v);
extern int func_ov006_020da8b8(char *p, int v);
extern void func_ov006_020c1420(void *c, s16 arg1, void *arg2);
extern void func_ov006_020da420(char *c);
extern int func_ov006_020da174(Struct30 *base);
extern void func_ov006_020da154(char *p);
extern void func_ov006_020c0d68(void *c);
extern void func_ov006_020c0c80(void *c);
extern void func_ov004_020ad79c(int a0, int a1);
extern void func_ov004_020b0cac(int a0, int a1, int a2, int a3, int a4, s16 a5);
extern void func_ov004_020adb1c(int v);
extern void func_ov004_020b0a54(int v);
extern int data_ov004_020bf9ec;
extern int data_ov006_0213bc44;
extern int data_ov004_020bfa18;
extern int data_ov006_0213bd48[];
extern void FreeGfxSlotsById(int arg);
extern void func_ov004_020b56c8(int a);
/* already the mangled Itanium name in the ROM's own symbols.txt --
extern "C" here means "use literally", not "C linkage" (see
double-mangling-defect memory note: a C++ TU re-mangles a bare
`extern` unless told not to, and this name is already the target). */
extern s16 data_ov004_020bf9e4;
extern void* data_ov004_020beb68;
extern "C" void func_ov006_020c1604(char *c, int unused, short a2, int a3);
extern "C" void func_ov004_020b66d4(void);
extern u8 data_0209d45c;
extern u8 data_0209d454;
/* data_ov004_020beb68 comes from dScMgBase_c.h (void*) -- not redeclared here,
see its own use below. */
extern void func_ov006_0210a534(char *);
extern void *LoadFile(int);
extern int GetGameLanguage(void);
extern void DecompressLZ16(void *, u32);
extern void _ZN2GX11LoadOBJPlttEPKvjj(void *, u32, u32);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(void *, u32, u32);
extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile u16 *, int, int, int, int);
extern int func_ov006_020c1a88(char *);
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov006_020d9c5c, NOT applied: int _Z14ApproachLinearRiii(int* a, int b, int c); */
/* TUBUILD CONFLICT -- alternate declaration of _Z15ApproachLinear2Rsss, from the legacy file for func_ov006_020d9c5c, NOT applied: int _Z15ApproachLinear2Rsss(short* a, short b, short c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020da8e4, from the legacy file for func_ov006_020d9c5c, NOT applied: int func_ov006_020da8e4(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020da8e4, from the legacy file for func_ov006_020da00c, NOT applied: extern "C" int func_ov006_020da8e4(void); */
/* TUBUILD CONFLICT -- alternate declaration of Vec2_Sub, from the legacy file for func_ov006_020da0ac, NOT applied: extern "C" void Vec2_Sub(int *o, int *a, int *b); */
/* TUBUILD CONFLICT -- alternate declaration of func_0203d680, from the legacy file for func_ov006_020da0ac, NOT applied: extern "C" void func_0203d680(int *out, int *in, int scale); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020da8e4, from the legacy file for func_ov006_020da0ac, NOT applied: extern "C" int func_ov006_020da8e4(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020da834, from the legacy file for func_ov006_020dac34, NOT applied: extern int func_ov006_020da834(void *p); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020da860, from the legacy file for func_ov006_020dac34, NOT applied: extern int func_ov006_020da860(void *p, int v); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN5Sound12PlayBank2_2DEj, from the legacy file for func_ov006_020dac34, NOT applied: extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020c1718, from the legacy file for _ZN11dScMgCard_c13OnTurnIntoEggEi, NOT applied: extern int func_ov006_020c1718(void* p); */
/* TUBUILD CONFLICT -- alternate declaration of _Z15ApproachLinear2Rsss, from the legacy file for _ZN11dScMgCard_c13OnTurnIntoEggEi, NOT applied: extern int _Z15ApproachLinear2Rsss(s16* r, s16 t, s16 s); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02134028, from the legacy file for _ZN11dScMgCard_c13InitResourcesEv, NOT applied: extern u32 *data_ov006_02134028; */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020c0aa8, from the legacy file for _ZN11dScMgCard_c13InitResourcesEv, NOT applied: extern void func_ov006_020c0aa8(char *); */
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- _ZN11dScMgCard_c13InitResourcesEv, 0x020dbaf0, size 0x264 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c13InitResourcesEv
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- _ZN11dScMgCard_c13OnYoshiTryEatEi, 0x020db9dc, size 0x114 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c13OnYoshiTryEatEi
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
/* recovered: renamed to Class_Method, RTTI class fields named */
// recovered name: dScMgCard_c_OnYoshiTryEat_020db9dc
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
void dScMgCard_c::OnYoshiTryEat(int /* arg */)
{
    char *c = (char *)this;

    dScMgCard_c *self = (dScMgCard_c *)(void *)c;
    int i;
    char *p1, *p2;

    self->unk_538c = 0;
    self->unk_538e = 6;
    self->unk_5390 = 6;
    self->unk_5392 = 6;
    self->unk_5394 = 6;
    func_ov006_020c1604(c + 0x4f38, 4, 5, (int)(c + 0x538c));

    self->unk_511e = 1;
    self->unk_4f52 = 2;
    func_ov006_020da974(5);

    p1 = c + 0x51a8;
    p2 = c + 0x5298;
    for (i = 0; i < 5; i++) {
        func_ov006_020da00c(p1);
        func_ov006_020da00c(p2);
        p1 += 0x30;
        p2 += 0x30;
    }

    func_ov004_020b66d4();

    data_ov004_020bc7d4 = 1;
    data_ov006_0214176c = 0;
    data_ov006_02141768 = 0;
    data_ov006_02141770 = 0;
    self->unk_539a = 0;
    data_ov006_0213bc44 = 1;
    self->mState = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- _ZN11dScMgCard_c13OnTurnIntoEggEi, 0x020db720, size 0x2bc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c13OnTurnIntoEggEi
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
/* recovered: renamed to Class_Method, RTTI class fields named */
// recovered name: dScMgCard_c_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* dScMgCard_c::OnTurnIntoEgg - recovered from vtable slot identity */
int dScMgCard_c::OnTurnIntoEgg(int /* mode */)
{
    char *c = (char *)this;

    struct dScMgCard_c *self = (struct dScMgCard_c *)(void *)c;
    switch (self->mState) {
    case 0xe: {
        int r = func_ov006_020da5e8(c + 0x51a8, c + 0x5298);
        if (r == -1)
            FreeGfxSlotsById(0xa);
        else if (r == 1)
            FreeGfxSlotsById(9);
        self->mStateTimer = 0;
        *(s16*)(((int)c + 0x5388)) += 1;
        break;
    }
    case 0xf:
        if (func_ov006_020c1718(c + 0x4f38) != 0) {
            int r = func_ov006_020da5e8(c + 0x51a8, c + 0x5298);
            if (r == -1) {
                func_ov004_020b5ed0();
                self->mStateTimer = 0;
            } else if (r == 1) {
                int v = data_ov006_0213bd48[func_ov006_020da4ac(c + 0x51a8, 0)];
                func_ov004_020b56c8(v * data_ov004_020bfa18);
                self->mStateTimer = 0;
            } else {
                func_ov004_020b5d74();
                self->mStateTimer = 0x1e;
            }
            *(s16*)(((int)c + 0x5388)) += 1;
        }
        break;
    case 0x10:
        if (data_ov004_020bf9e4 == 1) {
            int r = func_ov006_020da5e8(c + 0x51a8, c + 0x5298);
            func_ov006_020da4ac(c + 0x51a8, 0);
            if (r == -1) {
                *(s16*)(((int)c + 0x5388)) += 1;
            } else {
                return 1;
            }
        } else {
            if (_Z15ApproachLinear2Rsss((s16*)(c + 0x538a), 0, 1) != 0)
                FreeGfxSlotsById(0x1d);
        }
        break;
    case 0x11: {
        int i;
        char* a;
        char* b;
        void* p = data_ov004_020beb68;
        if ((p != 0 ? *(int*)((char*)p + 0xa8) : 0) != 0)
            return 1;
        a = c + 0x51a8;
        b = c + 0x5298;
        for (i = 0; i < 5; i++) {
            func_ov006_020d99a4(a);
            func_ov006_020d99a4(b);
            a += 0x30;
            b += 0x30;
        }
        self->mStateTimer = 0x3c;
        *(s16*)(((int)c + 0x5388)) += 1;
        break;
    }
    case 0x12:
        if (_Z15ApproachLinear2Rsss((s16*)(c + 0x538a), 0, 1) != 0)
            return 1;
        break;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- _ZN11dScMgCard_c15OnGroundPoundedEv, 0x020db6ec, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c15OnGroundPoundedEv
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
/* recovered: renamed to Class_Method, RTTI class fields named */
// recovered name: dScMgCard_c_OnGroundPounded
/* recovered: renamed to Class_Method */
/* dScMgCard_c::OnGroundPounded - recovered from vtable slot identity */
void dScMgCard_c::OnGroundPounded()
{
    void *thisPtr = (void *)this;

    struct dScMgCard_c *self = (struct dScMgCard_c *)(void *)thisPtr;
    int x = self->mHudScore;
    int v;
    if (x < 5) {
        v = 1;
    } else if (x < 10) {
        v = 2;
    } else if (x < 15) {
        v = 3;
    } else {
        v = 4;
    }
    func_ov004_020b6324(v);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov006_020dac34, 0x020dac34, size 0xab8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020dac34
/* dScMgCard_c Behavior @ 0x020dac34 */
extern "C" void func_ov006_020dac34(unsigned char *c)
{
    int cnt3;
    int i;
    int j;
    int k;
    int n;
    unsigned char *p;
    unsigned char *p1;
    unsigned char *p2;
    int ok;
    int idx;
    int v5;
    int v4;
    int r;
    int a;
    int b;
    int t4;
    int t5;
    void *g;
    int a0;
    int a1;
    s16 cnt;

    switch (S16(0x5388)) {
    case 1:
        if (c[0xc4] == 0) {
            c[0xc3] = 1;
            c[0xc4] = 1;
            S16(0xc0) = 0;
        }
        if (func_ov006_020c0f0c(c + 0x4f38) != 0)
            S16(0x5388)++;
        break;
    case 2:
        if (func_ov006_020c1718(c + 0x4f38) != 0) {
            S16(0x538a) = 0x10;
            S16(0x5388)++;
        } else {
            cnt = S16(0x538c);
            if (cnt > 0) {
                idx = cnt - 1;
                if (cnt != 0) {
                    idx = idx * 0x30;
                    p = c + 0x51d3;
                    if ((u8)p[idx] == 0) {
                        p[idx] = 1;
                        (c + idx + 0x5000)[0x2c3] = 1;
                    }
                }
            }
        }
        break;
    case 3:
        S16(0x538a)--;
        if (S16(0x538a) == 0)
            S16(0x5388)++;
        break;
    case 4:
        ok = 0;
        idx = data_020a0e40 * 4;
        if (data_020a0de8[idx] != 0 && data_020a0de9[idx] != 0)
            ok = 1;
        if (ok != 0 && func_ov006_020da834((char *)(c + 0x51a8)) != 0 && data_ov006_02141768 == 0) {
            v5 = data_020a0dea[data_020a0e40][0];
            v4 = data_020a0deb[data_020a0e40][0];
            if (func_ov006_020da860((char *)(c + 0x51a8), 2) != 0) {
                r = v5 - 0x80;
                if (r < 0)
                    r = -r;
                if (r < 0x48) {
                    r = v4 - 0x58;
                    if (r < 0)
                        r = -r;
                    if (r < 0x14) {
                        data_ov006_0214176c = 1;
                        data_ov006_02141768 = 1;
                        S16(0x5388)++;
                        data_ov004_020bf9ec = 1;
                        _ZN5Sound12PlayBank2_2DEj(0x151);
                        g = data_ov004_020beb68;
                        a0 = g ? *(int *)((char *)g + 0xa8) : 0;
                        a1 = g ? *(int *)((char *)g + 0xb4) : 0;
                        func_ov004_020ad79c(a0, a1);
                    }
                }
            } else {
                r = v5 - 0x80;
                if (r < 0)
                    r = -r;
                if (r < 0x48) {
                    r = v4 - 0x58;
                    if (r < 0)
                        r = -r;
                    if (r < 0x14) {
                        data_ov006_0214176c = 1;
                        data_ov006_02141768 = 1;
                        S16(0x5388)++;
                        data_ov004_020bf9ec = 1;
                        _ZN5Sound12PlayBank2_2DEj(0x151);
                        g = data_ov004_020beb68;
                        a0 = g ? *(int *)((char *)g + 0xa8) : 0;
                        a1 = g ? *(int *)((char *)g + 0xb4) : 0;
                        func_ov004_020ad79c(a0, a1);
                        cnt3 = 0;
                        i = cnt3;
                        p = c;
                        do {
                            r = (u8)(p + 0x5000)[0x1d3];
                            i++;
                            p += 0x30;
                            if (r == 3)
                                cnt3++;
                        } while (i < 5);
                        if (cnt3 < 2)
                            _ZN5Sound12PlayBank2_2DEj(0x144);
                        else
                            _ZN5Sound12PlayBank2_2DEj(0x145);
                    }
                }
            }
        }
        break;
    case 5:
        if (data_ov006_02141768 != 0 && func_ov006_020da860((char *)(c + 0x51a8), 2) != 0
            && func_ov006_020c1718(c + 0x4f38) != 0) {
            S16(0x5388)++;
        } else if (func_ov006_020c1718(c + 0x4f38) != 0) {
            r = func_ov006_020da88c((char *)(c + 0x51a8), 6);
            if (r != 0) {
                S16(0x538c) = 0;
                S16(0x511e) = 0;
                func_ov006_020c1420(c + 0x4f38, (s16)r, c + 0x538c);
            }
        } else {
            r = func_ov006_020da8b8((char *)(c + 0x51a8), 6);
            if (r > -1 && S16(0x538c) != 0) {
                (c + r * 0x30 + 0x5000)[0x1d3] = 5;
                S16(0x538c)--;
            }
        }
        break;
    case 6:
        func_ov006_020da420((char *)(c + 0x5298));
        S16(0x5388)++;
        break;
    case 7:
        if (func_ov006_020da860((char *)(c + 0x5298), 1) != 0 && func_ov006_020c1718(c + 0x4f38) != 0) {
            r = data_ov006_0213bc44 - 1;
            data_ov006_0213bc44 = r;
            if (r == 0) {
                S16(0x5388)++;
            } else {
                S16(0x5388) = 4;
                data_ov006_0214176c = 0;
                data_ov006_02141768 = 0;
                data_ov006_02141770 = 0;
            }
        } else if (func_ov006_020c1718(c + 0x4f38) != 0) {
            r = func_ov006_020da88c((char *)(c + 0x5298), 6);
            if (r != 0) {
                S16(0x538c) = 0;
                S16(0x511e) = 0;
                func_ov006_020c1420(c + 0x4f38, (s16)r, c + 0x538c);
            }
        } else {
            r = func_ov006_020da8b8((char *)(c + 0x5298), 6);
            if (r > -1 && S16(0x538c) != 0) {
                (c + r * 0x30 + 0x5000)[0x2c3] = 5;
                S16(0x538c)--;
            }
        }
        break;
    case 8:
        j = 0;
        p = c;
        c[0xc3] = (unsigned char)j;
        do {
            j++;
            (p + 0x5000)[0x2c3] = 2;
            p += 0x30;
        } while (j < 5);
        S16(0x5388)++;
        break;
    case 9:
        a = func_ov006_020da174((Struct30 *)(c + 0x51a8));
        b = func_ov006_020da174((Struct30 *)(c + 0x5298));
        if (a != 0 || b != 0)
            _ZN5Sound12PlayBank2_2DEj(0x152);
        S16(0x5388)++;
        break;
    case 10:
        if (func_ov006_020da860((char *)(c + 0x51a8), 8) != 0 && func_ov006_020da860((char *)(c + 0x5298), 8) != 0) {
            S16(0x538a) = 0x14;
            S16(0x5388)++;
        }
        break;
    case 11:
        S16(0x538a)--;
        if (S16(0x538a) == 0) {
            p = c;
            k = 0;
            do {
                (p + 0x5000)[0x1d4] = 1;
                k++;
                (p + 0x5000)[0x2c4] = 1;
                p += 0x30;
            } while (k < 5);
            func_ov006_020da154((char *)(c + 0x51a8));
            func_ov006_020da154((char *)(c + 0x5298));
            _ZN5Sound12PlayBank2_2DEj(0x147);
            S16(0x538a) = 0x3c;
            S16(0x5388)++;
        }
        break;
    case 12:
        S16(0x538a)--;
        if (S16(0x538a) == 0) {
            t4 = func_ov006_020da4ac((char *)(c + 0x5298), (s16 *)(c + 0x5392));
            t5 = func_ov006_020da4ac((char *)(c + 0x51a8), (s16 *)(c + 0x538e));
            if (data_ov006_02141768 != 0 && data_ov006_02141768 != 0) {
                func_ov004_020b0cac(t4 + 0x16, 0x80, 0x48, 0, -1, 1);
                func_ov004_020b0cac(t5 + 0x16, 0x80, 0x78, 0, -1, 2);
                if (t4 != t5) {
                    S16(0x5392) = 6;
                    S16(0x5394) = 6;
                    S16(0x538e) = 6;
                    S16(0x5390) = 6;
                } else if (S16(0x5392) != S16(0x538e)) {
                    S16(0x5394) = 6;
                    S16(0x5390) = 6;
                }
            }
            r = func_ov006_020da5e8((char *)(c + 0x51a8), (char *)(c + 0x5298));
            if (r == -1) {
                g = data_ov004_020beb68;
                a0 = g ? *(int *)((char *)g + 0xa8) : 0;
                a1 = g ? *(int *)((char *)g + 0xb4) : 0;
                func_ov004_020ad79c(a0, a1 - 1);
                S16(0x538a) = 0x5a;
            } else if (r == 1) {
                g = data_ov004_020beb68;
                {
                    int *tbl = data_ov006_0213bd48;
                    int scale;
                    int prod;
                    a0 = g ? *(int *)((char *)g + 0xa8) : 0;
                    r = tbl[t5];
                    scale = data_ov004_020bfa18;
                    prod = r * scale + a0;
                    r = g ? *(int *)((char *)g + 0xb4) : 0;
                    func_ov004_020ad79c(prod, r + 1);
                }
                S16(0x538a) = 0x1e;
            } else {
                S16(0x538a) = 0x5a;
            }
            S16(0x5388)++;
        }
        break;
    case 13:
        S16(0x538a)--;
        if (S16(0x538a) == 0) {
            r = func_ov006_020da5e8((char *)(c + 0x51a8), (char *)(c + 0x5298));
            if (r == -1) {
                func_ov006_020c0d68(c + 0x4f38);
                g = data_ov004_020beb68;
                if (g != 0) {
                    if (*(int *)((char *)g + 0xb4) > 0) {
                        *(int *)((char *)g + 0xb4) = *(int *)((char *)g + 0xb4) - 1;
                    }
                }
                func_ov004_020b0a54(5);
            } else if (r == 1) {
                func_ov006_020c0c80(c + 0x4f38);
                g = data_ov004_020beb68;
                if (g != 0) {
                    if (*(int *)((char *)g + 0xb4) < 0x270f) {
                        int *fld = (int *)((char *)g + 0xb4);
                        *fld = *fld + 1;
                    }
                    a1 = *(int *)((char *)g + 0xb4);
                    if (a1 > *(int *)((char *)g + 0xb8))
                        *(int *)((char *)g + 0xb8) = a1;
                }
                func_ov004_020adb1c(*(int *)(c + 0xb4));
                func_ov004_020b0a54(4);
            } else {
                func_ov004_020b0a54(7);
                _ZN5Sound12PlayBank2_2DEj(0x140);
                _ZN5Sound12PlayBank2_2DEj(0x138);
            }
            c[0xc3] = 0;
            S16(0x538a) = 0;
            S16(0x5388)++;
        }
        break;
    }

    p1 = c + 0x51a8;
    p2 = c + 0x5298;
    n = 0;
    do {
        ((cMgSmartball_object_c *)p1)->Update();
        ((cMgSmartball_object_c *)p2)->Update();
        n++;
        p1 += 0x30;
        p2 += 0x30;
    } while (n < 5);
    data_ov006_0214176c = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- _ZN11dScMgCard_c8BehaviorEv, 0x020dabec, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c8BehaviorEv
/* dScMgCard_c::Behavior -- vtable slot 6, ov006 0x020dabec.
 *
 * Named from the table: 0x020dabec is the word slot 6 of _ZTV11dScMgCard_c
 * holds where its base's table holds something else, so it is this class's
 * own override of the virtual fBase_c declares.
 *
 * mFrameCounter is the frame counter the header's "own tail" note lists among the
 * nine fields five of this class's methods already touch; pad_4f38 is the
 * start of the 0x270-byte table six classes in this family share, which
 * func_ov006_020c19d0 steps. */
s32 dScMgCard_c::Behavior()
{
    mFrameCounter += 1;
    func_ov006_020c19d0(pad_4f38);
    func_ov006_020dac34((unsigned char *)this);
    func_ov004_020b65e4();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- _ZN11dScMgCard_c6RenderEv, 0x020da9c4, size 0x228 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c6RenderEv
/* dScMgCard_c::Render -- vtable slot 9.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgCard_c, this class's own slot 9
 * (dScMgBase_c::Render). The old file's `recovered name: dScMgCard_c_Render` agreed. */
s32 dScMgCard_c::Render()
{
    char *c = (char *)this;
    int skip;
    short v;
    int i;
    int sb;

    func_ov006_020c0aa8(c + 0x4660);

    v = this->mState;
    if (v > 3 && v < 0x11) {
        skip = -1;
        if (v > 0xe) {
            if (this->mFrameCounter & 8) {
                if (func_ov006_020da5e8(c + 0x51a8, c + 0x5298) == 1) {
                    skip = 6 - func_ov006_020da4ac(c + 0x51a8, 0);
                }
            }
        }

        i = 0;
        sb = 0xc;
        for (; i < 6; i++) {
            if (i != skip)
                RenderOamMainScreen(data_ov006_02134010[i], 0x28, sb, -1, -1);
            sb += 0x10;
        }

        RenderOamMainScreen(data_ov006_02134028, 0x28, 0x38, -1, -1);
    }

    func_ov004_020b1bc8(c, 0xc, 0xc, 0);
    func_ov004_020b1e34(c, 0xe0, 0x14, 1);

    if (this->mScore < this->mHudScore)
        this->mScore = this->mHudScore;
    func_ov004_020adb1c(this->mScore);

    func_ov004_020b6430();

    func_ov006_020d9a14(c);

    {
        Node *bank = (Node *)(c + 0x51a8);
        int cnt = 4;
        char *cfgp = c + 0xc0;
        Node *node = &bank[4];
        do {
            unsigned char cfg = *(unsigned char *)(cfgp + 0x5000 + 0x1d2);
            if (this->unk_538e == cfg)
                goto chk1;
            if (this->unk_5390 != cfg)
                goto docall1;
        chk1:
            if (!(this->mFrameCounter & 8))
                goto skip1;
        docall1:
            node->m0();
        skip1:
            cfgp -= 0x30;
            node--;
        } while (--cnt >= 0);
    }

    {
        Node *bank = (Node *)(c + 0x5298);
        int cnt = 4;
        char *cfgp = c + 0xc0;
        Node *node = &bank[4];
        do {
            unsigned char cfg = *(unsigned char *)(cfgp + 0x5000 + 0x2c2);
            if (this->unk_5392 == cfg)
                goto chk2;
            if (this->unk_5394 != cfg)
                goto docall2;
        chk2:
            if (!(this->mFrameCounter & 8))
                goto skip2;
        docall2:
            node->m0();
        skip2:
            cfgp -= 0x30;
            node--;
        } while (--cnt >= 0);
    }

    func_ov006_020c1804(c + 0x4f38);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- _ZN11dScMgCard_c16CleanupResourcesEv, 0x020da994, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c16CleanupResourcesEv
/* dScMgCard_c::CleanupResources -- vtable slot 3, ov006 0x020da994.
 *
 * Named from the table: 0x020da994 is the word slot 3 of _ZTV11dScMgCard_c
 * holds where its base's table holds something else, so it is this class's
 * own override of the virtual fBase_c declares.
 *
 * The body never touches the instance -- it only clears three ov006 globals
 * -- which is why the pre-migration stub could take `void` and still match:
 * `this` arrives in r0 and is simply not read. */
s32 dScMgCard_c::CleanupResources()
{
    data_ov006_0214176c = 0;
    data_ov006_02141768 = 0;
    data_ov006_02141770 = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov006_020da974, 0x020da974, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020da974
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020da974(int v) {
    int i;
    for (i = 0; i < 6; i++) {
        data_ov006_0213bd18[i] = v;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov006_020da8e4, 0x020da8e4, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020da8e4
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020da8e4(void)
{
  int r5 = 0;
  int sum = 0;
  int i;
  for (i = 0; i < 6; i++)
  {
    sum += data_ov006_0213bd18[i];
  }

  sum = (sum * ((int) ((((unsigned int) RandomIntInternal(&data_0209e650)) & 0x7fffffff) >> 0x13))) >> 0xc;
  for (i = 0; i < 6; i++)
  {
    sum -= data_ov006_0213bd18[i];
    if (sum < 0)
    {
      r5 = i & 0xff;
      data_ov006_0213bd18[i] -= 1;
      break;
    }
  }

  return r5;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov006_020da8b8, 0x020da8b8, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020da8b8
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020da8b8(char *p, int v)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (v == *(unsigned char *)(p + 0x2b))
            return i;
        p += 0x30;
    }
    return -1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov006_020da88c, 0x020da88c, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020da88c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020da88c(char *p, int v)
{
    int count = 0;
    int i;
    for (i = 0; i < 5; i++) {
        if (v == *(unsigned char *)(p + 0x2b))
            count++;
        p += 0x30;
    }
    return count;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov006_020da860, 0x020da860, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020da860
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020da860(char *p, int v)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (v != *(unsigned char *)(p + 0x2b))
            return 0;
        p += 0x30;
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov006_020da834, 0x020da834, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020da834
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020da834(char *p)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (*(int *)(p + 0x24) < 0x4000)
            return 0;
        p += 0x30;
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov006_020da5e8, 0x020da5e8, size 0x24c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020da5e8
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020da5e8(char *a, char *b);
int func_ov006_020da5e8(char *a, char *b) {
    short arr1[6];
    short arr2[6];
    short *p1 = (short *)(((int)arr1));
    short *p2 = (short *)(((int)arr2));
    int max1, sec1;
    short max1i, sec1i;
    int max2, sec2;
    short max2i, sec2i;
    int i;
    short tmp;

    p1[0] = 0; p1[1] = 0; p1[2] = 0; p1[3] = 0; p1[4] = 0; p1[5] = 0;
    p2[0] = 0; p2[1] = 0; p2[2] = 0; p2[3] = 0; p2[4] = 0; p2[5] = 0;

    max1 = 0; max1i = 6; sec1 = 0; sec1i = 6;
    max2 = 0; sec2 = 0; max2i = 6; sec2i = 6;

    for (i = 0; i < 5; i++) {
        arr1[*(unsigned char *)(a + 0x2a)]++;
        a += 0x30;
        arr2[*(unsigned char *)(b + 0x2a)]++;
        b += 0x30;
    }

    for (i = 0; i < 6; i++) {
        if (max1 <= arr1[i]) { sec1 = max1; sec1i = max1i; max1 = arr1[i]; max1i = i; }
        else if (sec1 <= arr1[i]) { sec1 = arr1[i]; sec1i = i; }
        if (max2 <= arr2[i]) { sec2 = max2; sec2i = max2i; max2 = arr2[i]; max2i = i; }
        else if (sec2 <= arr2[i]) { sec2 = arr2[i]; sec2i = i; }
    }

    if (max1 == sec1 && max1i < sec1i) { tmp = max1i; max1i = sec1i; sec1i = tmp; }
    if (max2 == sec2 && max2i < sec2i) { tmp = max2i; max2i = sec2i; sec2i = tmp; }

    if (max1 > max2) return 1;
    if (max1 < max2) return -1;
    if (sec1 > sec2) return 1;
    if (sec1 < sec2) return -1;
    if (max1 == 1 && max2 == 1) return 0;
    if (max1i > max2i) return 1;
    if (max1i < max2i) return -1;
    if (sec1 == 1 && sec2 == 1) return 0;
    if (sec1i > sec2i) return 1;
    if (sec1i < sec2i) return -1;
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov006_020da4ac, 0x020da4ac, size 0x13c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020da4ac
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020da4ac(char *self, s16 *out)
{
    s16 counts[6] = {0};
    int i;
    int j;
    int result = 0;
    int maxVal = 0;
    int secVal = 0;
    s16 maxIdx = 6;
    s16 secIdx = maxIdx;

    for (i = 0; i < 5; i++) {
        counts[*(u8 *)(self + 0x2a)]++;
        self += 0x30;
    }

    for (j = 0; j < 6; j++) {
        s16 v = counts[j];
        if (maxVal <= v) {
            secVal = maxVal;
            secIdx = maxIdx;
            maxVal = v;
            maxIdx = j;
        } else if (secVal <= v) {
            secVal = v;
            secIdx = j;
        }
    }

    if (maxVal == secVal && maxIdx < secIdx) {
        s16 t = maxIdx;
        maxIdx = secIdx;
        secIdx = t;
    }

    switch (maxVal) {
    case 2:
        result = (secVal == 2) ? 2 : 1;
        break;
    case 3:
        result = (secVal == 2) ? 4 : 3;
        break;
    case 4:
        result = 5;
        break;
    case 5:
        result = 6;
        break;
    default:
        break;
    }

    if (out != 0) {
        if (maxVal > 1)
            out[0] = (s16)maxIdx;
        if (secVal > 1)
            out[1] = (s16)secIdx;
    }
    return result;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov006_020da420, 0x020da420, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020da420
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020da420(char *c);
void func_ov006_020da420(char *c){
    int i;
    char *p;
    short arr[6];
    short *a = (short *)(((int)arr));
    a[0] = 0;
    a[1] = 0;
    a[2] = 0;
    a[3] = 0;
    a[4] = 0;
    a[5] = 0;
    p = c;
    for (i = 0; i < 5; i++) {
        arr[*(unsigned char*)(p + 0x2a)]++;
        p += 0x30;
    }
    for (i = 0; i < 5; i++) {
        if (arr[*(unsigned char*)(c + 0x2a)] == 1)
            *(unsigned char*)(c + 0x2b) = 4;
        c += 0x30;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov006_020da174, 0x020da174, size 0x2ac */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020da174
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020da174(Struct30 *base)
{
  s16 hist[6];
  Five order;
  s16 i;
  s32 j;
  s32 t;
  s32 n;
  s32 neg1;
  s32 neg2;
  s32 zero;
  Struct30 *p;
  s16 *h;
  s32 max_val;
  s32 second_val;
  s32 max_idx;
  s32 second_idx;
  max_val = 0;
  h = (s16 *) ((u32) ((u64) ((u32) hist)));
  h[0] = max_val;
  h[1] = max_val;
  h[2] = max_val;
  h[3] = max_val;
  h[4] = max_val;
  max_idx = 6;
  second_val = max_val;
  second_idx = max_idx;
  i = max_val;
  h[5] = max_val;
  zero = max_val;
  for (i = 0; i < 5; i++)
  {
    s32 idx = base[i].unk_2A;
    hist[idx] = (s16) (hist[idx] + 1);
  }
  p = base;

  for (i = 0; i < 6; i++)
  {
    s32 v = hist[i];
    if (max_val <= v)
    {
      second_val = max_val;
      second_idx = max_idx;
      max_val = v;
      max_idx = i;
    }
    else
      if (second_val <= v)
    {
      second_val = v;
      second_idx = i;
    }
  }

  if (max_val == second_val)
  {
    if (max_val > 1)
    {
      if (second_val > 1)
      {
        p = base;
        for (t = 0; t < 5; t++)
        {
          s32 c = p->unk_2A;
          if (max_idx == c)
          {
            break;
          }
          if (second_idx == c)
          {
            s32 tmp = second_idx;
            second_idx = max_idx;
            max_idx = tmp;
            break;
          }
          p = (Struct30 *) (((u32) p) + 0x30);
        }

      }
    }
  }
  order = data_ov006_0213bcd8;
  n = 4;
  zero = 0;
  neg1 = -1;
  neg2 = -2;

  {
    do
    {
      s32 k = 0;
      while (k < n)
      {
        s32 v1 = order.v[k];
        s32 stride = 0x30;
        Struct30 *ptr1 = (Struct30 *) (((u32) base) + (v1 * stride));
        s32 u1 = ptr1->unk_2A;
        s32 next = k + 1;
        s32 v2;
        Struct30 *ptr2 = (Struct30 *) (((u32) base) + (order.v[next] * stride));
        s32 u2 = ptr2->unk_2A;
        if (u1 == max_idx)
        {
          if (max_val > 1)
          {
            u1 = neg2;
            goto u1_done;
          }
        }
        if (u1 == second_idx)
        {
          if (second_val > 1)
          {
            u1 = neg1;
            goto u1_done;
          }
        }
        u1 = zero;
        u1_done:
        if (u2 == max_idx)
        {
          if (max_val > 1)
          {
            u2 = neg2;
            goto u2_done;
          }
        }

        if (u2 == second_idx)
        {
          if (second_val > 1)
          {
            u2 = neg1;
            goto u2_done;
          }
        }
        u2 = zero;
        u2_done:
        if (u1 > u2)
        {
          order.v[k] = order.v[next];
          order.v[next] = v1;
        }

        k++;
      }

      n = n - 1;
    }
    while (n > 0);
  }

  {
    Struct30 *q = base;
    s32 one = 1;
    s32 z;
    j = 0;
    z = j;
    for (; j < 5; j++)
    {
      s32 c = q->unk_2A;
      if (max_idx == c)
      {
        if (max_val > 1)
        {
          goto store1c;
        }
      }
      if (second_idx == c)
      {
        if (second_val <= 1)
        {
          goto store0c;
        }
      }
      else
      {
        goto store0c;
      }
      store1c:
      q->unk_2D = one;

      goto flag_nextc;
      store0c:
      q->unk_2D = z;

      flag_nextc:
      q = (Struct30 *) (((u32) q) + 0x30);

    }

  }
  for (j = 0; j < 5; j++)
  {
    s32 v = order.v[j];
    Struct30 *ptr = (Struct30 *) (((u32) base) + (v * 0x30));
    func_ov006_020d99ec((char *) ptr, j);
  }

  for (j = 0; j < 5; j++)
  {
    if (j != order.v[j])
    {
      return 1;
    }
  }

  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov006_020da154, 0x020da154, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020da154
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020da154(char *p) {
    int i;
    for (i = 0; i < 5; i++) {
        *(unsigned char *)(p + 0x2b) = 9;
        p += 0x30;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov006_020da0ac, 0x020da0ac, size 0xa8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020da0ac
// Copy a 4-word source block into the object, refresh two fields from a virtual call
// and a fixed-point transform pair, take the absolute value of one result, then reset a
// few trailing fields. Sibling of func_ov006_020da00c.
extern "C" void func_ov006_020da0ac(char *p, int *src)
{
    C *c = (C *)p;
    Src *s = (Src *)src;
    int buf[4];
    c->f4 = s->f0;
    c->f8 = s->f4;
    {
        int *b = (int *)(((int)c + 4));
        c->fc = b[0];
        c->f10 = b[1];
    }
    c->f8 = c->m2();
    c->f4 = 0x80000;
    Vec2_Sub(&buf[0], &c->fc, &c->f4);
    func_0203d680(&buf[2], &buf[0], 0x100);
    c->f14 = buf[2];
    c->f18 = buf[3];
    if (c->f14 < 0) c->f14 = -c->f14;
    c->f24 = 0;
    c->f2c = 0;
    c->f2a = (unsigned char)func_ov006_020da8e4();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov006_020da00c, 0x020da00c, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020da00c
extern "C" void func_ov006_020da00c(char *p)
{
    C *c = (C *)p;
    int buf[4];
    c->f4 = c->fc;
    c->f8 = c->f10;
    c->f8 = c->m2();
    c->f4 = 0x80000;
    Vec2_Sub(&buf[0], &c->fc, &c->f4);
    func_0203d680(&buf[2], &buf[0], 0x100);
    c->f14 = buf[2];
    c->f18 = buf[3];
    if (c->f14 < 0) c->f14 = -c->f14;
    c->f2b = 0;
    c->f24 = 0;
    c->f2c = 0;
    c->f28 = 0;
    c->f2d = 0;
    c->f2a = (unsigned char)func_ov006_020da8e4();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov006_020d9c5c, 0x020d9c5c, size 0x3b0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020d9c5c
extern "C" void func_ov006_020d9c5c(void* self)
{
    char* c = (char*)self;
    int r5;

    if (*(unsigned char*)(c + 0x2b) == 0)
        return;

    if (*(unsigned char*)(c + 0x2b) == 1) {
        if (*(short*)(c + 0x28) != 0) {
            _Z15ApproachLinear2Rsss((short*)(c + 0x28), 0, 1);
            return;
        }
        r5 = _Z14ApproachLinearRiii((int*)(c + 8), *(int*)(c + 0x10), *(int*)(c + 0x18));
        if (_Z14ApproachLinearRiii((int*)(c + 4), *(int*)(c + 0xc), *(int*)(c + 0x14)) != 0 && r5 != 0) {
            _ZN5Sound12PlayBank2_2DEj(0x144);
            *(unsigned char*)(c + 0x2b) = 2;
            *(unsigned char*)(c + 0x2c) = 1;
        }
    }

    if (*(unsigned char*)(c + 0x2c) != 0)
        _Z14ApproachLinearRiii((int*)(c + 0x24), 0x4000, 0x200);
    else
        _Z14ApproachLinearRiii((int*)(c + 0x24), 0, 0x200);

    {
        int st = *(unsigned char*)(c + 0x2b);
        if (st == 3) {
            _Z14ApproachLinearRiii((int*)(c + 8), *(int*)(c + 0x10) - 0x10000, 0x2000);
        } else if (st == 5) {
            r5 = _Z14ApproachLinearRiii((int*)(c + 8), *(int*)(c + 0x10), *(int*)(c + 0x18));
            if (_Z14ApproachLinearRiii((int*)(c + 4), *(int*)(c + 0xc), *(int*)(c + 0x14)) != 0 && r5 != 0) {
                _ZN5Sound12PlayBank2_2DEj(0x144);
                *(unsigned char*)(c + 0x2b) = 2;
                *(unsigned char*)(c + 0x2c) = 1;
            }
        } else if (st == 2) {
            _Z14ApproachLinearRiii((int*)(c + 8), *(int*)(c + 0x10), 0x4000);
        } else if (st == 4 && *(int*)(c + 0x24) == 0) {
            int result = ((VObj*)c)->f2();
            if (_Z14ApproachLinearRiii((int*)(c + 8), result, 0x8000) != 0) {
                *(int*)(c + 4) = 0x80000;
                *(unsigned char*)(c + 0x2b) = 6;
                *(unsigned char*)(c + 0x2a) = (unsigned char)func_ov006_020da8e4();
            }
        } else if (st == 7) {
            if (_Z14ApproachLinearRiii((int*)(c + 4), *(int*)(c + 0x1c), 0x6000) != 0)
                *(unsigned char*)(c + 0x2b) = 8;
        } else if (st == 9) {
            _Z14ApproachLinearRiii((int*)(c + 8), 0x80000, 0x6000);
        } else if (st == 0xa) {
            if (_Z15ApproachLinear2Rsss((short*)(c + 0x28), 0, 1) != 0)
                _Z14ApproachLinearRiii((int*)(c + 4), *(int*)(c + 0x1c), *(int*)(c + 0x14));
        }
    }

    if (data_ov006_0214176c != 0) {
        if (*(unsigned char*)(c + 0x2b) == 3) {
            *(unsigned char*)(c + 0x2b) = 4;
            *(unsigned char*)(c + 0x2c) = 0;
        }
        return;
    }
    if (data_ov006_02141768 != 0)
        return;

    {
        int idx = data_020a0e40;
        unsigned char e8;
        int flag = 0;
        e8 = data_020a0de8[idx * 4];
        if (e8 != 0 && data_020a0de9[idx * 4] != 0)
            flag = 1;
        if (flag == 0)
            return;
        {
            unsigned char st = *(unsigned char*)(c + 0x2b);
            if ((unsigned char)(st + 0xfe) > 1)
                return;
            if (*(int*)(c + 0x24) != 0x4000)
                return;
            {
                int a = (int)data_020a0dea[idx][0] - (*(int*)(c + 4) >> 12);
                int b = (int)data_020a0deb[idx][0] - (*(int*)(c + 8) >> 12);
                if (a <= 7)
                    return;
                if (a >= 0x29)
                    return;
                if (b <= -1)
                    return;
                if (b >= 0x31)
                    return;
                if (st == 2) {
                    func_02012790(0x14f);
                    *(unsigned char*)(c + 0x2b) = 3;
                } else {
                    func_02012790(0x150);
                    *(unsigned char*)(c + 0x2b) = 2;
                }
            }
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov006_020d9bdc, 0x020d9bdc, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020d9bdc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020d9bdc(char *c)
{
    int idx;
    unsigned short e;
    if (*(unsigned char *)(c + 0x2b) == 0) return;
    idx = (*(unsigned char *)(c + 0x2a) + 1) * 5 + (*(int *)(c + 0x24) >> 12);
    e = data_ov006_0213bd64[idx];
    Hud_RenderSprite(
        data_ov006_02141774[e],
        (*(int *)(c + 4) >> 12) + 0x18,
        (*(int *)(c + 8) >> 12) + 0x18,
        -1, -1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov006_020d9bd0, 0x020d9bd0, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020d9bd0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020d9bd0(void)
{
    return -0x60000;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov006_020d9a14, 0x020d9a14, size 0x1bc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020d9a14
extern "C" {  /* .c-derived member: C linkage for the whole block */
/* The nine-field tail the header names from 0x5388 on, reached by raw offset because this member kept C linkage. */
#define F(off) (*(short *)(c + 0x5300 + (off)))
void func_ov006_020d9a14(char *c)
{
    short v = F(0x88);
    if (v > 3 && v < 0x11) {
        int sb = 0xb0;
        int i;
        for (i = 0; i < 6; i++) {
            int sel = 0;
            if ((i == F(0x92) && i == F(0x8e)) || (i == F(0x94) && i == F(0x90))) {
                if (F(0x96) & 0x10) sel = 1; else sel = 2;
            } else if (i == F(0x92) || i == F(0x94)) {
                if (F(0x96) & 0x10) sel = 2;
            } else if (i == F(0x8e) || i == F(0x90)) {
                if (F(0x96) & 0x10) sel = 1;
            }
            Hud_RenderSprite(data_ov006_0213bd30[i][sel], 0x14, sb, -1, -1);
            sb -= 0x10;
        }
    }
    if (F(0x88) != 4) return;
    if (func_ov006_020da834(c + 0x51a8) == 0) return;
    if (func_ov006_020da860(c + 0x51a8, 2) == 0)
        Hud_RenderSprite(data_ov006_0213406c[0], 0x80, 0x58, -1, -1);
    else
        Hud_RenderSprite(data_ov006_0213406c[1], 0x80, 0x58, -1, -1);
}
}

#undef F

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov006_020d99ec, 0x020d99ec, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020d99ec
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020d99ec(char *c, int n){
  *(int*)(c+0x1c) = (n*0x28+0x2c)<<0xc;
  *(int*)(c+0x20) = 0x80000;
  *(char*)(c+0x2b) = 7;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov006_020d99a4, 0x020d99a4, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020d99a4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020d99a4(void* c){
  char* p=(char*)c;
  *(int*)(p+0x1c)=-0x28000;
  *(int*)(p+0x20)=*(int*)(p+8);
  int t=*(int*)(p+4);
  unsigned ip=0x80000-t;
  *(short*)(p+0x28)=(short)((int)ip/0x10000);
  *(int*)(p+0x14)=0x10000;
  *(int*)(p+0x18)=0;
  p[0x2b]=0xa;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov006_020d9998, 0x020d9998, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020d9998
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020d9998(void)
{
    return -0x60000;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov006_020d978c, 0x020d978c, size 0x20c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020d978c
extern "C" void func_ov006_020d978c(void *arg0)
{
    char *c = (char *)arg0;

    if (*(u8 *)(c + 0x2b) == 0)
        return;

    if (*(u8 *)(c + 0x2c) != 0)
        _Z14ApproachLinearRiii((int *)(c + 0x24), 0x4000, 0x200);
    else
        _Z14ApproachLinearRiii((int *)(c + 0x24), 0, 0x200);

    u8 state = *(u8 *)(c + 0x2b);

    if (state == 5) {
        int r5 = _Z14ApproachLinearRiii((int *)(c + 8), *(s32 *)(c + 0x10), *(s32 *)(c + 0x18));
        int r0 = _Z14ApproachLinearRiii((int *)(c + 4), *(s32 *)(c + 0xc), *(s32 *)(c + 0x14));
        if (r5 == 0)
            return;
        if (r0 != 0)
            *(u8 *)(c + 0x2b) = 1;
        return;
    }

    if (state == 4 && *(s32 *)(c + 0x24) == 0) {
        int v = ((struct Obj *)c)->m08();
        int r0 = _Z14ApproachLinearRiii((int *)(c + 8), v, 0x8000);
        if (r0 == 0)
            return;
        *(s32 *)(c + 4) = 0x80000;
        *(u8 *)(c + 0x2b) = 6;
        *(u8 *)(c + 0x2a) = func_ov006_020da8e4();
        return;
    }

    if (state == 7) {
        int r0 = _Z14ApproachLinearRiii((int *)(c + 4), *(s32 *)(c + 0x1c), 0x6000);
        if (r0 != 0)
            *(u8 *)(c + 0x2b) = 8;
        return;
    }

    if (state == 9) {
        _Z14ApproachLinearRiii((int *)(c + 8), 0x10000, 0x6000);
        return;
    }

    if (state == 1) {
        if (*(s16 *)(c + 0x28) != 0) {
            _Z15ApproachLinear2Rsss((s16 *)(c + 0x28), 0, 1);
            return;
        }
        _Z14ApproachLinearRiii((int *)(c + 8), *(s32 *)(c + 0x10), *(s32 *)(c + 0x18));
        _Z14ApproachLinearRiii((int *)(c + 4), *(s32 *)(c + 0xc), *(s32 *)(c + 0x14));
        return;
    }

    if (state != 0xa)
        return;

    if (_Z15ApproachLinear2Rsss((s16 *)(c + 0x28), 0, 1) == 0)
        return;

    _Z14ApproachLinearRiii((int *)(c + 4), *(s32 *)(c + 0x1c), *(s32 *)(c + 0x14));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov006_020d970c, 0x020d970c, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020d970c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020d970c(unsigned char* c){
  int k;
  if(*(unsigned char*)(c+0x2b)==0) return;
  k=data_ov006_0213bd64[(*(unsigned char*)(c+0x2a)+1)*5+(*(int*)(c+0x24)>>12)];
  Hud_RenderSprite(
    data_ov006_02141774[k],
    (*(int*)(c+4)>>12)+0x18,
    (*(int*)(c+8)>>12)+0x18,
    -1,-1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov006_020d96f0, 0x020d96f0, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020d96f0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020d96f0(void *c)
{
  extern int data_ov006_0213bcf4[];
  extern int data_ov006_0213bccc[];
  *(int*)c=(int)data_ov006_0213bcf4;
  *(int*)c=(int)data_ov006_0213bccc;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov006_020d96e0, 0x020d96e0, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020d96e0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020d96e0(void *p) { *(int *)p = (int)data_ov006_0213bccc; }
}

/* The destructor is DEFINED INLINE in include/dScMgCard_c.h. mwcc emits the
 * D1/D0 pair at ROM ordinals 1 and 2 (0x020d95a4 size 0x94, 0x020d9638 size
 * 0xa8) from the vtable slots that name them, in cartridge order -- written
 * out of line here it emitted D0 ahead of D1 and rombuild's fail-closed
 * isolate refused the TU. The header's own banner carries the full reason. */
