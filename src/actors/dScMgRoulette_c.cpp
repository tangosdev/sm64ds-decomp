//cpp
/* dScMgRoulette_c -- the Mushroom Roulette minigame scene: racers are dealt
 * onto a spinning board, the countdown runs, and each racer is paid out by
 * the tile it lands on.  MG_ROULETTE in the profile registry.
 *
 * ov006, .text 0x0210788c .. 0x0210a400, 40 functions, reconstructed from 40
 * one-function legacy sources by tools/tubuild.py create and reconciled by
 * hand.  See include/dScMgRoulette_c.h for the class's own evidence trail --
 * where its fields end, why the destructor is inline, and why InitResources
 * rather than the destructor is this TU's key function.
 *
 * Eight of the 40 are this class's own vtable slots: 0 InitResources,
 * 3 CleanupResources, 6 Behavior, 9 Render, the 16/17 destructor pair, and
 * 18/19 OnYoshiTryEat/OnTurnIntoEgg.  The other 32 are the file-local helpers
 * that shared the translation unit with them -- board hit-testing, the racer
 * update and draw loops, the slider physics and the HUD strips.  None is
 * reached from outside 0x0210788c..0x0210a400 and the ROM gives none of them
 * a mangled name, so none was a class member with external linkage.
 *
 * PRODUCTION translation unit: enrolled in
 * config/arm9/overlays/ov006/delinks.txt as one complete .text range and
 * linked into the ROM build.  All 40 functions reproduce byte for byte from
 * this single object.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * emission order is a hard gate (`linkcheck [4b/8]`), not advice.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x0210788c  src/_ZN15dScMgRoulette_cD1Ev.cpp
 *   [1] 0x02107920  src/_ZN15dScMgRoulette_cD0Ev.cpp
 *   [2] 0x021079c8  src/func_ov006_021079c8.c
 *   [3] 0x021079cc  src/func_ov006_021079cc.c
 *   [4] 0x02107a6c  src/func_ov006_02107a6c.cpp
 *   [5] 0x02107b14  src/func_ov006_02107b14.c
 *   [6] 0x02107b70  src/func_ov006_02107b70.c
 *   [7] 0x02107b94  src/func_ov006_02107b94.c
 *   [8] 0x02107c50  src/func_ov006_02107c50.c
 *   [9] 0x02107d20  src/func_ov006_02107d20.c
 *   [10] 0x02107d58  src/func_ov006_02107d58.c
 *   [11] 0x02107d80  src/func_ov006_02107d80.cpp
 *   [12] 0x02107db8  src/func_ov006_02107db8.cpp
 *   [13] 0x02107ea8  src/func_ov006_02107ea8.c
 *   [14] 0x021082c4  src/func_ov006_021082c4.c
 *   [15] 0x021082fc  src/func_ov006_021082fc.c
 *   [16] 0x02108508  src/func_ov006_02108508.c
 *   [17] 0x02108524  src/func_ov006_02108524.c
 *   [18] 0x0210858c  src/func_ov006_0210858c.c
 *   [19] 0x021085c0  src/func_ov006_021085c0.c
 *   [20] 0x02108650  src/func_ov006_02108650.c
 *   [21] 0x02108b90  src/func_ov006_02108b90.c
 *   [22] 0x02108cc0  src/func_ov006_02108cc0.c
 *   [23] 0x02108d28  src/func_ov006_02108d28.c
 *   [24] 0x02108e24  src/func_ov006_02108e24.c
 *   [25] 0x02108f2c  src/func_ov006_02108f2c.c
 *   [26] 0x0210927c  src/func_ov006_0210927c.c
 *   [27] 0x021092a0  src/func_ov006_021092a0.c
 *   [28] 0x021092e8  src/func_ov006_021092e8.c
 *   [29] 0x0210935c  src/func_ov006_0210935c.c
 *   [30] 0x021094ac  src/func_ov006_021094ac.c
 *   [31] 0x02109530  src/func_ov006_02109530.c
 *   [32] 0x021095ac  src/func_ov006_021095ac.c
 *   [33] 0x021095cc  src/_ZN15dScMgRoulette_c13OnYoshiTryEatEi.cpp
 *   [34] 0x021096c8  src/_ZN15dScMgRoulette_c13OnTurnIntoEggEi.cpp
 *   [35] 0x0210980c  src/_ZN15dScMgRoulette_c16CleanupResourcesEv.cpp
 *   [36] 0x02109834  src/_ZN15dScMgRoulette_c6RenderEv.cpp
 *   [37] 0x02109aa0  src/func_ov006_02109aa0.c
 *   [38] 0x02109aac  src/_ZN15dScMgRoulette_c8BehaviorEv.cpp
 *   [39] 0x0210a194  src/_ZN15dScMgRoulette_c13InitResourcesEv.cpp
 */

/* PRAGMAS: NONE, AND THAT IS A MEASUREMENT. Two members carried a file-scope
 * pragma in their legacy shard files -- `#pragma optimize_for_size on` around
 * func_ov006_021094ac and `#pragma opt_propagation off` around
 * func_ov006_02108b90. Neither is carried here, because neither is
 * load-bearing: with both deleted outright this TU still compiles 40/40 byte
 * MATCH. At -O4,p the defaults already produce the cartridge's bytes for both
 * members, so the shards' pragmas were belt-and-braces the merged TU does not
 * need. Three controls back that up (see the manifest's boundary_evidence for
 * the full write-up):
 *   (a) drop both pragmas          -> 40/40 MATCH
 *   (b) `optimize_for_size on` at file scope with no `off`
 *                                  -> 23/40, 17 DIFF -- so the compile is live
 *                                     and this pragma really does move bytes
 *   (c) bracket `on`/`off` around ONE of those 17 size-sensitive members
 *                                  -> 40/40 MATCH, i.e. the bracket did NOT
 *                                     bind: the trailing `off` restored the
 *                                     default for the whole file. mwccarm
 *                                     2004/b56 treats it as file-global
 *                                     last-wins, exactly as writer.md says.
 * `#pragma defer_codegen off` at the top of the file changes (c) -- see the
 * manifest -- but it also reverses emission order, which this TU's descending
 * source layout is already built around, so it is not used here.
 *
 * INCLUDE ORDER IS LOAD-BEARING. common.h comes first so its FLAT
 * `struct Matrix4x3 { s32 m[12]; }` wins the MATRIX4X3_DEFINED guard, which
 * is the spelling src/func_ov006_02107db8.cpp compiled against. (This class's
 * own header does not reach math/Matrix.h today, so the order is belt and
 * braces -- but a whole-matrix assignment sits in 02107db8's body and the
 * structured spelling would split it, exactly as it did on daPgDfdr_c.)
 *
 * decl_common.h IS INCLUDED AND MATCHED. Five of the legacy files already
 * pulled it in, so the merge makes its declarations visible to all forty
 * members; every definition below therefore repeats decl_common.h's own
 * spelling where it has one, with a cast at the head of the body when the
 * legacy file wanted a different pointer type.
 */
#include "common.h"
#include "types.h"
#include "dScMgRoulette_c.h"
#include "decl_common.h"

/* ------------------------------------------------------------------------ *
 * Shadow types, reconciled across the forty legacy files.
 *
 * The legacy pile carried three different `struct S`, two different
 * `struct Obj` and two different `struct P`, plus `Mtx43`/`M48` duplicating
 * common.h's Matrix4x3 at the same 0x30 layout. Each distinct shape is
 * declared once here under a name that says what it is; the duplicates of
 * Matrix4x3 are gone entirely.
 * ------------------------------------------------------------------------ */

/* Three consecutive Fix12 words, written a component at a time. */
struct Vec3 { int x, y, z; };

/* One row of the 12-entry angle table at data_ov006_0213e334, copied whole. */
typedef struct { short v[12]; } ShortTbl12;

/* Two Fix12 words copied as a unit out of data_ov006_0213e2f8 / _0213e2e8.
 * BOTH are spelled with an array member on purpose: C++ scalarizes a
 * whole-struct assignment of `{int a, b;}` into ldr/str/ldr/str where the ROM
 * issues both loads then both stores. Measured on func_ov006_021082c4. */
struct Pair   { int w[2]; };
struct Pair2W { int w[2]; };

/* The board's per-tile centre coordinates: data_ov006_02142ab4 holds x,
 * data_ov006_02142ab8 is the same array offset to y, stride 8.
 * decl_common.h types both as `char[]`, so every structured read casts. */
struct TileXY { int x, y; };

/* The two BMD files behind the slider model pair, at +0x530c. */
struct ResPair { char pad[8]; void *a; void *b; };

/* A ModelAnim-shaped subobject reached only through vtable slot 5. */
struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int a);
};

/* The slider subobject as func_ov006_02107db8 sees it: a pointer-to-member
 * callback at +0x0 and a 4-virtual subobject at +0x10. */
struct ObjV {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
};
struct C;
typedef void (C::*PMF)();
struct C {
    PMF pmf;            /* 0x0 */
    char pad8[0x10 - 8];
    ObjV obj;           /* 0x10 */
};

/* One racer, 0x34 bytes, five of them at +0x51a8. The three spellings below
 * are three views of the same record kept from the files that used them. */
typedef struct Thing {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s16 unk2C;
    s16 unk2E;
    s16 unk30;
    u8 unk32;
    u8 unk33;
} Thing;

struct RacerXY {
    int pad[4];
    int x;
    int y;
    int a;
    int b;
};

struct RacerPos {
    char pad[0x18];
    int f18;
    int f1c;
    char pad2[0x2c - 0x20];
    short f2c;
};

typedef struct { int x, y; } Vec2_Fix12;

/* dScMgRoulette_c's own racer array, reached by offset from `this`. */
typedef struct Racer {
    char b[0x34];
} Racer;
typedef struct Obj9 {
    char _p0[0x51a8];
    Racer racers[3]; /* 0x51a8 */
} Obj9;

/* The self-dispatch shim InitResources uses for its own slot 18. */
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

/* ------------------------------------------------------------------------ *
 * Address launders and offset macros, carried verbatim from the legacy
 * files. THEY ARE LOAD-BEARING: they are what stops mwcc
 * common-subexpressioning a field address across a call, which the ROM does
 * not do.
 * ------------------------------------------------------------------------ */
#define AT(p,off) ((void*)(int)(((long long)(int)((char*)(p)+(off)))))
#define LNDR(e) ((int)(((long long)(e))))
#define I(o) (*(int*)(c + (o)))
#define H(o) (*(short*)(c + (o)))
#define B(o) (*(unsigned char*)(c + (o)))
#define IA(o) (*(int*)AT(c,(o)))
#define HA(o) (*(short*)AT(c,(o)))
#define PlayBank2 _ZN5Sound12PlayBank2_2DEj
/* The global singleton pointer to the active dScMgBase_c, read as bytes.
 * data_ov004_020beb68 is dScMgBase_c.h's own `void *`, not a local one. */
#define SCENE_BONUS ((data_ov004_020beb68 != 0) \
                        ? *(int *)((char *)data_ov004_020beb68 + 0xa8) : 0)

/* ------------------------------------------------------------------------ *
 * ROM symbols this TU calls or reads that decl_common.h does not already
 * declare. Mangled names are spelled mangled INSIDE the extern "C" block --
 * that is the ROM's own spelling, and a C++ spelling here would emit a name
 * that exists nowhere.
 * ------------------------------------------------------------------------ */
extern "C" {

/* _Z14ApproachLinearRiii == ApproachLinear(int&, int, int); the reference
 * spelling is the one the mangling proves, and three legacy files that wrote
 * `int *` are dereferenced at the call site instead. */
int  _Z14ApproachLinearRiii(int &v, int a, int b);
int  _ZN4cstd4fdivEii(int a, int b);
void _ZN5Model17UpdateFileOffsetsER8BMD_File(void *f);
int  _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
int  _ZN3G2S13GetBG2CharPtrEv(void);
int  _ZN3G2S12GetBG2ScrPtrEv(void);
int  _ZN3G2S12GetBG0ScrPtrEv(void);
void _ZN3GXS10LoadBGPlttEPKvjj(void *, unsigned int, unsigned int);
void _ZN2GX11LoadOBJPlttEPKvjj(void *, unsigned int, unsigned int);
void _ZN3GXS11LoadOBJPlttEPKvjj(void *, unsigned int, unsigned int);

int  GetGameLanguage(void);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5);
void Vec2_Sub(int *out, int *a, int *b);
void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *mF, int x, int y, int z);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
int  Sound_PlayIfNotActive(int a, int b, int c, int d);
int  RandomIntInternal(int *seed);
void *LoadFile(int id);
void DecompressLZ16(void *src, int dst);
void Ov004_Deallocate(void *p);
void FreeGfxSlotsById(int a);
void NullDestructor_0203d47c(void);
void Camera_UpdateMatrices(char *cam);

void func_02012790(int a);
void func_020731dc(int a, int b, void **node);
void func_0203d680(Vec2_Fix12 *out, const Vec2_Fix12 *in, int scale);
void *func_ov004_020adc68(int id);
void func_ov004_020b1b08(int c);
void func_ov004_020b1b40(int c);
void func_ov004_020b1bc8(char *, int, int, int);
void func_ov004_020b2220(int, int, int, int, int, int, int);
void func_ov004_020b56c8(void);
void func_ov004_020b65e4(void);
void func_ov004_020b66d4(void);
void func_ov006_020c0aa8(char *c);
int  func_ov006_020c0efc(char *a);
int  func_ov006_020c1718(char *c);
void func_ov006_020c1804(char *c);
void func_ov006_020c19d0(char *thiz);
int  func_ov006_020c1a88(char *c);

extern int   data_ov006_021428d0;
extern int   data_ov006_0213e2dc;
extern char *data_ov006_02142aa4[];
extern int   data_020a0db0;
extern s32   data_ov006_0212ed24[];
extern void *data_ov006_021376ec[];
extern void *data_ov006_0213e320[];
extern int   data_ov006_0212ed14[];
extern void *data_ov006_02138c3c;
extern Matrix4x3 data_020a0e68;
extern short data_02082214[];
extern short data_ov006_0213e334[];
extern short data_ov006_0212ed34[];
extern struct Pair   data_ov006_0213e2f8;
extern struct Pair2W data_ov006_0213e2e8;
extern int   data_0209e650[];
extern int   data_ov006_0212ed4c[];
extern int   data_ov006_0212ed68[];
extern short data_ov006_0212edb8[];
extern short data_ov006_0212edba[];
extern short data_ov006_0212ed88[];
extern short data_ov006_0212ed8a[];
extern short data_ov006_0212ed8c[];
extern short data_ov006_0212ed8e[];
extern int   data_ov006_021428cc;
extern int   data_ov006_021428d4[];
extern void *data_ov006_02142948;
extern u8    data_020a0e40;
extern u8    data_020a0de8[];
extern u8    data_020a0de9[];
extern u8    data_020a0dea[];
extern u8    data_020a0deb[];
extern s32   data_ov004_020b9488;
extern u16   data_ov004_020bf9e4;
extern u8    data_0209d45c;
extern u8    data_0209d454;

/* Forward declarations for every member of this TU. mwcc lays .text down in
 * REVERSE source order, so this file is written descending-ROM and nearly
 * every intra-TU call is a forward reference. Each spelling below is the
 * member's own definition, or decl_common.h's where it has one. */
void func_ov006_021079c8(void);
void func_ov006_021079cc(void);
void func_ov006_02107d20(int *p, int val);
void func_ov006_02107d58(int *p);
void func_ov006_02107db8(C *c);
void func_ov006_02107ea8(char *self);
void func_ov006_021082c4(char *p);
void func_ov006_021082fc(char *self);
void func_ov006_02108508(char *p);
int  func_ov006_0210858c(ResPair *o);
short func_ov006_02108650(int x, int y);
int  func_ov006_02108b90(short *r0, int r1);
void func_ov006_02108d28(int *p);
int  func_ov006_02108e24(int *p);
void func_ov006_02108f2c(Thing *t);
void func_ov006_0210927c(char *a);
void func_ov006_021092e8(void *self);
void func_ov006_0210935c(RacerPos *self, int idx);
void func_ov006_021094ac(RacerXY *self, int *src);
void func_ov006_02109530(int *out, int *a, int scale);
void func_ov006_021095ac(int *a, int *b);
void func_ov006_02107b94(void **sl);
void func_ov006_02107c50(short *a0, int a1);

}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 39 -- _ZN15dScMgRoulette_c13InitResourcesEv, 0x0210a194, size 0x26c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15dScMgRoulette_c13InitResourcesEv
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- _ZN15dScMgRoulette_c8BehaviorEv, 0x02109aac, size 0x6e8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15dScMgRoulette_c8BehaviorEv
/* dScMgRoulette_c::Behavior -- vtable slot 6, ov006 0x02109aac.
 *
 * Named from the table: 0x02109aac is the word slot 6 of
 * _ZTV15dScMgRoulette_c holds where its base's table holds something else, so
 * it is this class's own override of the virtual fBase_c declares.
 *
 * The race's five phases, selected by mPhase: 1 deals the racers out one at
 * a time, 2 runs the countdown and scores the board, 3 pays out per landed
 * tile, 4 announces win/lose/draw, then the per-racer update loops run every
 * frame regardless of phase.
 *
 * THE `AT`/`LNDR` LAUNDERS AND THE ADDRESS MACROS ARE LOAD-BEARING and are
 * inherited verbatim from the pre-migration file: they are what stops mwcc
 * common-subexpressioning a field address across a call, which the ROM does
 * not do. Only the receiver changes -- `c` is now `this` cast to the same
 * `char *` the macros already assume, so every offset reads the same bytes.
 *
 * data_ov004_020beb68 IS THE HEADER'S, NOT A LOCAL ONE. The pre-migration file
 * declared it `extern char *data_ov004_020beb68` for its own convenience;
 * include/dScMgBase_c.h -- which this file now reaches through the class --
 * declares it `void *`, and two incompatible extern "C" declarations of one
 * name in one translation unit is what mwcc rejects as "illegal function
 * overloading" (see include/dActor_c.h's own note on that). The pointer
 * arithmetic is spelled through a `char *` cast instead; same address, same
 * bytes. */
s32 dScMgRoulette_c::Behavior()
{
    char *c = (char *)this;

    switch (H(0x53e6)) {
    case 0:
        break;
    case 1:
        if (func_ov006_020c0efc(c + 0x4f38) != 0) {
            if (H(0x53d8) == 0) {
                H(0x53d8) = 1;
            }
        }
        if (B(0xc4) == 0) {
            B(0xc3) = 1;
            B(0xc4) = 1;
            H(0xc0) = 0;
        }
        if (func_ov006_020c0efc(c + 0x4f38) != 0) {
            HA(0x53e8) -= 1;
            if (H(0x53e8) == 0) {
                H(0x53e4) = 1;
                if (I(0x53f8) < I(0x53fc)) {
                    int v[2];
                    int w;
                    w = LNDR(0 - (I(0x53f8) << 16)) + 0x80000;
                    v[0] = 0x20000;
                    v[1] = w;
                    func_ov006_021095ac((int *)(c + 0x51a8 + I(0x53f8) * 0x34), v);
                    IA(0x53f8) += 1;
                    H(0x53e8) = 8;
                    func_ov004_020b1b08(1);
                    PlayBank2(0x14d);
                } else {
                    int i = 0;
                    if (I(0x53fc) > 0) {
                        char *p = c;
                        do {
                            *(unsigned char *)(p + 0x51da) = 1;
                            i++;
                            p += 0x34;
                        } while (i < I(0x53fc));
                    }
                    H(0x53e8) = 0x258;
                    func_ov006_020c1764(c + 0x4f38);
                    HA(0x53e6) += 1;
                    I(0x53f8) = 0;
                    PlayBank2(0x15b);
                }
            }
        }
        break;
    case 2: {
        int h = H(0x53e8);
        if (h % 60 == 0 && h > 0 && h <= 0xb4) {
            PlayBank2(0xa6);
        }
        HA(0x53e8) -= 1;
        if (H(0x53e8) == 0) {
            {
                short type;
                int b;
                int i = 0;
                if (I(0x53fc) > 0) {
                    char *p1 = c;
                    char *p2 = c + 0x51a8;
                    do {
                        *(short *)(p1 + 0x51d4) = func_ov006_02108650(*(int *)(p1 + 0x51c0) >> 12, *(int *)(p1 + 0x51c4) >> 12);
                        b = 0;
                        type = *(short *)(p1 + 0x51d4);
                        if (type >= 0xc && type <= 0x1c)
                            b = 1;
                        if (b != 0)
                            goto lab530;
                        if (type >= 0x1d && type <= 0x22)
                            b = 1;
                        else
                            b = 0;
                        if (b == 0)
                            goto lab35c;
                    lab530:
                        func_ov006_02109530((int *)p2, (int *)&data_ov006_02142ab4[type * 8], 0x100);
                        goto labnext;
                    lab35c:
                        func_ov006_0210935c((RacerPos *)p2, type);
                    labnext:
                        i++;
                        p1 += 0x34;
                        p2 += 0x34;
                    } while (i < I(0x53fc));
                }
            }
            I(0x53f8) = 0;
            HA(0x53e4) += 1;
            HA(0x53e6) += 1;
            I(0x53dc) = 1;
            H(0x53e8) = 1;
            PlayBank2(0x15a);
            {
                int off6 = 0x53f6;
                int found = 0;
                int j = 0;
                if (I(0x53fc) > 0) {
                    char *q1 = c;
                    char *q2 = c + 0x51a8;
                    do {
                        if (*(short *)(q1 + 0x51d4) == 0x25) {
                            func_ov006_021092e8(q2);
                            if (found == 0) {
                                PlayBank2(0x15c);
                                found = 1;
                            }
                        } else {
                            *(unsigned char *)(q1 + 0x51da) = 3;
                            *(short *)(c + off6) += 1;
                        }
                        j++;
                        q1 += 0x34;
                        q2 += 0x34;
                    } while (j < I(0x53fc));
                }
                func_ov004_020ad79c(I(0x53fc) - H(0x53f6) + SCENE_BONUS, 0);
            }
        }
        break;
    }
    case 3: {
        int b;
        short t;
        int off2;
        int d;
        int j;
        int r;
        char *p;
        b = (I(0x53c4) == 0) ? 1 : 0;
        if (LNDR(b) != 0) {
            j = 0;
            B(0xc3) = 0;
            t = H(0x53e8);
            HA(0x53e8) -= 1;
            if (t == 0) {
                d = H(0x53d6);
                H(0x53f2) = 0;
                if (I(0x53fc) > 0) {
                    p = c + 0x51a8;
                    off2 = 0x53f2;
                    do {
                        r = func_ov006_02108b90((short *)p, d);
                        if (r == 2) {
                            func_ov006_02107d20((int *)(c + 0x52f4), (int)p);
                        } else if (r == 3) {
                            func_ov006_02107d20((int *)(c + 0x52dc), (int)p);
                        } else if (r == 6) {
                            func_ov006_02107d20((int *)(c + 0x52c4), (int)p);
                        } else if (r == 0xc) {
                            func_ov006_02107d20((int *)(c + 0x52ac), (int)p);
                        }
                        *(short *)(c + off2) += r;
                        j++;
                        p += 0x34;
                    } while (j < I(0x53fc));
                }
                func_ov004_020ad79c(H(0x53f2) + SCENE_BONUS, 0);
                H(0x53f4) = 0;
                I(0x53f8) = 0;
                H(0x53e8) = 0x5a;
                HA(0x53e6) += 1;
            }
        }
        break;
    }
    case 4: {
        HA(0x53e8) -= 1;
        if (H(0x53e8) == 0) {
            short f6 = H(0x53f6);
            short f2 = H(0x53f2);
            if (f2 > f6) {
                func_ov006_020c0c80(c + 0x4f38);
                func_ov004_020b0a54(4);
            } else if (f2 < f6) {
                func_ov006_020c0d68(c + 0x4f38);
                func_ov004_020b0a54(5);
            } else {
                func_ov004_020b0a54(8);
            }
            H(0x53e4) = 0;
            HA(0x53e6) += 1;
        }
        break;
    }
    }

    {
        int k = 0;
        if (I(0x53fc) > 0) {
            char *p = c + 0x51a8;
            do {
                func_ov006_02108f2c((Thing *)p);
                k++;
                p += 0x34;
            } while (k < I(0x53fc));
        }
    }
    {
        int k = 0;
        if (I(0x53fc) > 0) {
            char *p = c + 0x51a8;
            do {
                func_ov006_02108d28((int *)p);
                k++;
                p += 0x34;
            } while (k < I(0x53fc));
        }
    }
    func_ov006_02107db8((C *)(c + 0x530c));
    func_ov006_020c19d0(c + 0x4f38);
    func_ov004_020b65e4();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- func_ov006_02109aa0, 0x02109aa0, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02109aa0
/* func_ov006_02109aa0 @ 0x2109aa0 (ov006) -- tail-call veneer to func_ov006_021079cc (0x21079cc).
 * ldr ip, [pc]; bx ip; .word 0x21079cc
 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02109aa0(char *) {
    func_ov006_021079cc();
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- _ZN15dScMgRoulette_c6RenderEv, 0x02109834, size 0x26c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15dScMgRoulette_c6RenderEv
/* dScMgRoulette_c::Render -- vtable slot 9, ov006 0x02109834.
 *
 * Named from the table: 0x02109834 is the word slot 9 of
 * _ZTV15dScMgRoulette_c holds where its base's table holds something else, so
 * it is this class's own override of the virtual fBase_c declares. The
 * pre-migration file had already retyped the receiver as dScMgRoulette_c and
 * named its fields; only the symbol was still a func_ov006_ one.
 *
 * Race minigame frame update: updates the racers back-to-front, plays the
 * countdown beep (volume ramps over the last 3 seconds), shows the winner
 * banner with per-rank colors, and refreshes the board.
 *
 * `c` and `self` are BOTH kept from the pre-migration file rather than folded
 * into `this`. The byte offsets that go through `c` -- 0x4660, 0x4f38, 0x51a8,
 * 0x530c -- land inside spans include/dScMgRoulette_c.h holds as opaque
 * members (mTable, mArray) or inside the base's own body, so spelling them as
 * members would be a layout claim this rename has no evidence for. */
s32 dScMgRoulette_c::Render()
{
    char *c = (char *)this;
    struct dScMgRoulette_c *self = (struct dScMgRoulette_c *)(void *)c;
    func_ov004_020b1bc8(c, 0xc, 0xc, 0);
    func_ov004_020b6430();

    {
        int i = self->mRacerCount - 1;
        for (; i >= 0; i--)
            func_ov006_02108cc0(((Obj9 *)c)->racers[i].b);
    }

    func_ov006_02109aa0(c);

    {
        int idle = (int)(((long long)(self->unk_53c4 == 0)));
        if (idle != 0) {
            if (self->mPhase < 8) {
                if (data_020a0db0 & 8) {
                    s16 idx = self->mSelectedTile;
                    Hud_RenderSprite((void *)data_ov006_02138c18,
                                        *(int *)(data_ov006_02142ab4 + idx * 8) >> 12,
                                        *(int *)(data_ov006_02142ab8 + idx * 8) >> 12,
                                        -1, 1);
                }
            }
        }
    }

    if (self->mPhase == 2) {
        int t = self->mPhaseTimer;
        if (t <= 0xb4) {
            int u = t + 0x3b;
            s16 rem = u % 60;
            int q = u / 60;
            int vol = rem * 0x88 + 0x1000;
            if (vol > 0x2000)
                vol = 0x2000;
            func_ov004_020b2220(0x80, 0x60, q, 1, -1, func_02053200(vol), 0);
        }
    }

    {
        s16 k = self->mCameraPreset;
        if (k != 0) {
            int off = k * 0xc;
            s16 hval = *(s16 *)(data_ov006_0213e2e0 + k * 2);
            {
                int d1 = *(int *)(data_ov006_0213e354 + off);
                int b1 = *(int *)(data_ov006_0213e350 + off);
                int a1 = *(int *)(data_ov006_0213e34c + off);
                self->mCameraTargetX = a1;
                self->mCameraTargetY = b1;
                self->mCameraTargetZ = d1;
            }
            {
                int f2 = *(int *)(data_ov006_0213e378 + off);
                int e2 = *(int *)(data_ov006_0213e374 + off);
                int d2 = *(int *)(data_ov006_0213e370 + off);
                self->mCameraEyeX = d2;
                self->mCameraEyeY = e2;
                self->mCameraEyeZ = f2;
            }
            self->mCameraAngle = hval;
            Camera_UpdateMatrices(c + 0x4660);
        } else {
            func_ov006_020c0aa8(c + 0x4660);
        }
        if (k != 0)
            func_ov006_02107d80(c + 0x530c);
        else
            func_ov006_020c1804(c + 0x4f38);
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- _ZN15dScMgRoulette_c16CleanupResourcesEv, 0x0210980c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15dScMgRoulette_c16CleanupResourcesEv
/* dScMgRoulette_c::CleanupResources -- vtable slot 3, ov006 0x0210980c.
 *
 * Named from the table: 0x0210980c is the word slot 3 of
 * _ZTV15dScMgRoulette_c holds where its base's table holds something else, so
 * it is this class's own override of the virtual fBase_c declares.
 *
 * 0x530c falls inside the header's pad_52ac span, which has no named field
 * yet, so the one subobject this tears down is still reached by offset. */
s32 dScMgRoulette_c::CleanupResources()
{
    func_ov006_0210858c((ResPair *)((char *)this + 0x530c));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- _ZN15dScMgRoulette_c13OnTurnIntoEggEi, 0x021096c8, size 0x144 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15dScMgRoulette_c13OnTurnIntoEggEi
// recovered name: dScMgRoulette_c_OnTurnIntoEgg
/* recovered: renamed to Class_Method, declarations from a shared header */
/* recovered: renamed to Class_Method */
/* dScMgRoulette_c::OnTurnIntoEgg - recovered from vtable slot identity */
int dScMgRoulette_c::OnTurnIntoEgg(int /* mode */)
{
    char *self = (char *)this;

    switch (*(s16 *)(self + 0x53e6)) {
    case 5:
        FreeGfxSlotsById(0x1d);
        if (func_ov006_02107a6c() != 0)
            (*(s16 *)(self + 0x53e6))++;
        break;
    case 6:
        if (func_ov006_020c1718(self + 0x4f38) != 0) {
            *(u16 *)(self + 0x53e8) = 0x3c;
            *(int *)(self + 0x53f8) = 0;
            (*(s16 *)(self + 0x53e6))++;
        }
        break;
    case 7:
        (*(s16 *)(self + 0x53e8))--;
        if (*(s16 *)(self + 0x53e8) == 0) {
            if (*(s16 *)(self + 0x53f2) != 0)
                func_ov004_020b56c8();
            (*(s16 *)(self + 0x53e6))++;
        }
        break;
    case 8:
        (*(s16 *)(self + 0x53e6))++;
        /* fall through */
    case 9:
    default:
        if (data_ov004_020bf9e4 <= 1)
            return 1;
        break;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- _ZN15dScMgRoulette_c13OnYoshiTryEatEi, 0x021095cc, size 0xfc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15dScMgRoulette_c13OnYoshiTryEatEi
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
/* recovered: renamed to Class_Method, RTTI class fields named */
// recovered name: dScMgRoulette_c_OnYoshiTryEat_021095cc
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
/* data_ov004_020beb68 is declared `void *` by dScMgBase_c.h; cast at the use site. */
void dScMgRoulette_c::OnYoshiTryEat(int /* arg */)
{
    char *c = (char *)this;

    dScMgRoulette_c *self = (dScMgRoulette_c *)(void *)c;
    int i;
    int *e;
    int n;

    func_ov006_02107b14();
    func_ov006_020c0e8c((int *)(c + 0x4f38));

    self->mScore = 0;
    self->mPhaseTimer = 0xa;

    e = (int *)(c + 0x51a8);
    for (i = 0; i < 5; i++) {
        func_ov006_021092a0(e);
        e = (int *)((char *)e + 0x34);
    }

    self->mDealIndex = 0;
    self->mTargetScore = 0;
    n = 0;
    if (data_ov004_020beb68 != 0) n = ((int *)data_ov004_020beb68)[0xa8 / 4];
    if (n >= 5) n = 5;
    self->mRacerCount = n;

    func_ov006_02108524(c + 0x530c);

    data_ov006_021428c8 = 0;
    self->mCameraPreset = 0;
    self->unk_53e0 = 0x100;
    self->unk_53ea = 0;
    self->unk_53ec = 0;
    self->unk_53ed = 0;
    self->unk_53ee = 0;
    self->unk_53ef = 0;
    self->unk_53f0 = 0;
    self->mPhase = 1;

    func_ov004_020b66d4();
    data_ov004_020bc7d4 = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov006_021095ac, 0x021095ac, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_021095ac
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_021095ac(int* a, int* b)
{
    a[8] = b[0];
    a[9] = b[1];
    func_ov006_02109530(a, b, 0x80);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov006_02109530, 0x02109530, size 0x7c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02109530
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02109530(int* out, int* a, int scale){
  Vec2_Fix12 v1;
  Vec2_Fix12 v2;
  out[6]=a[0];
  out[7]=a[1];
  Vec2_Sub((int*)&v1, out+6, out);
  func_0203d680(&v2, &v1, scale);
  out[4]=v2.x;
  out[5]=v2.y;
  if(out[4]<0) out[4]=-out[4];
  if(out[5]<0) out[5]=-out[5];
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov006_021094ac, 0x021094ac, size 0x84 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_021094ac
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_021094ac(RacerXY *self, int *src) {
    int v[2];
    self->a = src[0];
    self->b = src[1];
    Vec2_Sub(v, &self->a, (int *)self);
    self->x = v[0];
    self->y = v[1];
    if (func_0203d434(&self->x)) {
        func_0203d630(&self->x, 0x18000);
    }
    if (self->x < 0) self->x = -self->x;
    if (self->y < 0) self->y = -self->y;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov006_0210935c, 0x0210935c, size 0x150 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210935c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210935c(RacerPos *self, int idx)
{
    int buf[7];
    int f18 = self->f18;
    int a, b, lr, t, hi;
    buf[0] = f18;
    buf[1] = self->f1c;
    if ((unsigned short)(short)(idx - 0x23) <= 1) {
        a = ((TileXY *)data_ov006_02142ab4)[idx].x;
        b = ((TileXY *)data_ov006_02142ab8)[idx].x;
        buf[2] = a;
        buf[3] = b;
        lr = a + 0x8000;
        hi = a + 0x18000;
        if (f18 >= lr) {
            if (f18 <= hi)
                hi = f18;
            lr = hi;
        }
        b = buf[3];
        t = buf[1];
        {
            int lo = b - 0x20000;
            buf[0] = lr;
            lr = lo;
            hi = b + 0x20000;
            if (t >= lo) {
                if (t <= hi)
                    hi = t;
                lr = hi;
            }
        }
        buf[1] = lr;
        func_ov006_02109530((int *)self, buf, 0x100);
        return;
    }
    {
        int early = (self->f2c <= 0xb) ? 1 : 0;
        if (LNDR(early) == 0)
            return;
    }
    a = ((TileXY *)data_ov006_02142ab4)[idx].x;
    b = ((TileXY *)data_ov006_02142ab8)[idx].x;
    buf[4] = a;
    buf[5] = b;
    lr = a - 0x10000;
    hi = a + 0x10000;
    if (buf[0] >= lr) {
        if (buf[0] <= hi)
            hi = buf[0];
        lr = hi;
    }
    b = buf[5];
    t = buf[1];
    buf[0] = lr;
    lr = b - 0xc000;
    hi = b + 0xc000;
    if (t >= lr) {
        if (t <= hi)
            hi = t;
        lr = hi;
    }
    buf[1] = lr;
    func_ov006_02109530((int *)self, buf, 0x100);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov006_021092e8, 0x021092e8, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_021092e8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_021092e8(void* self)
{
    int b = data_ov006_021428cc & 1;
    if (b == 0) {
        data_ov006_021428d4[0] = 0xc000;
        data_ov006_021428d4[1] = 0xc000;
        func_020731dc((int)data_ov006_021428d4, (int)NullDestructor_0203d47c, &data_ov006_02142948);
        data_ov006_021428cc |= 1;
    }
    func_ov006_02109530((int *)self, data_ov006_021428d4, 0x80);
    *(unsigned char*)((char*)self + 0x32) = 7;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov006_021092a0, 0x021092a0, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_021092a0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_021092a0(int *o) {
    o[0] = 0xc000;
    o[1] = 0xc000;
    o[6] = o[0];
    o[7] = o[1];
    o[4] = 0;
    o[5] = 0;
    *(unsigned char*)((char*)o + 0x32) = 6;
    *(short*)((char*)o + 0x2c) = 0x25;
    *(short*)((char*)o + 0x2e) = 0;
    o[10] = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov006_0210927c, 0x0210927c, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210927c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210927c(char* a)
{
    *(short*)(a + 0x2c) = 0x25;
    *(a + 0x32) = 1;
    func_ov006_02109530((int *)a, (int *)(a + 0x20), 0x100);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov006_02108f2c, 0x02108f2c, size 0x350 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02108f2c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02108f2c(Thing* t)
{
    u8 state;
    int b;
    s16 type;

    state = t->unk32;
    if (state == 0 || state == 3) {
        _Z14ApproachLinearRiii(t->unk0, t->unk18, t->unk10);
        _Z14ApproachLinearRiii(t->unk4, t->unk1C, t->unk14);
    }

    state = t->unk32;
    if (state == 1) {
        func_ov006_02108e24((int *)t);
        _Z14ApproachLinearRiii(t->unk0, t->unk18, t->unk10);
        _Z14ApproachLinearRiii(t->unk4, t->unk1C, t->unk14);
    } else if (state == 7) {
        s32 out[2];
        _Z14ApproachLinearRiii(t->unk0, t->unk18, t->unk10);
        _Z14ApproachLinearRiii(t->unk4, t->unk1C, t->unk14);
        Vec2_Sub(out, &t->unk18, (int *)t);
        if (out[0] != 0)
            return;
        if (out[1] != 0)
            return;
        t->unk32 = 6;
        func_ov004_020b1b40(1);
    } else if (state == 4) {
        s32 out[2];
        _Z14ApproachLinearRiii(t->unk0, t->unk18, t->unk10);
        _Z14ApproachLinearRiii(t->unk4, t->unk1C, t->unk14);
        Vec2_Sub(out, &t->unk18, (int *)t);
        if (out[0] != 0)
            return;
        if (out[1] == 0) {
            t->unk30 = 0x14;
            t->unk32 = 5;
        }
    } else if (state == 5) {
        s32 d;
        s32* p;
        s32 q;
        d = data_ov004_020b9488;
        p = (s32*)(((s32)t + 4));
        q = -((t->unk30 - (d >> 1)) << 12) / d - 0x600;
        *p += (s32)((((s64)q << 14) + 0x800) >> 12);
        *(s16*)(((s32)t + 0x30)) -= 1;
        if (t->unk30 == 0)
            t->unk32 = 6;
    } else if (state == 2) {
        s32 vec[2];
        u8 idx = data_020a0e40;
        if (data_020a0de8[idx * 4] != 0) {
            vec[0] = data_020a0dea[idx * 4] << 12;
            vec[1] = data_020a0deb[idx * 4] << 12;
            func_ov006_021094ac((RacerXY *)t, vec);
        } else {
            type = func_ov006_02108650(t->unk18 >> 12, t->unk1C >> 12);
            t->unk2C = type;
            if (type != 0x25) {
                b = 0;
                type = t->unk2C;
                if (type >= 0xc && type <= 0x1c)
                    b = 1;
                if (b != false)
                    goto use_table;
                if (type >= 0x1d && type <= 0x22)
                    b = 1;
                else
                    b = 0;
                if (b == false)
                    goto no_table;
            use_table:
                func_ov006_02109530((int *)t, (int *)&data_ov006_02142ab4[type * 8], 0x100);
                goto snd;
            no_table:
                func_ov006_0210935c((RacerPos *)t, type);
            snd:
                _ZN5Sound12PlayBank2_2DEj(0x15e);
            } else {
                _ZN5Sound12PlayBank2_2DEj(0x15f);
                func_ov006_0210927c((char *)t);
            }
            data_ov006_021428c8 = 0;
            t->unk32 = 1;
        }
        _Z14ApproachLinearRiii(t->unk0, t->unk18, t->unk10);
        _Z14ApproachLinearRiii(t->unk4, t->unk1C, t->unk14);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov006_02108e24, 0x02108e24, size 0x108 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02108e24
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_02108e24(int *p)
{
  int idx;
  int flag;
  int new_var;
  int dx;
  int dz;
  new_var = 4;
  if (data_ov006_021428c8 != 0)
  {
    return 0;
  }
  idx = data_020a0e40;
  flag = 0;
  if (data_020a0de8[idx * 4])
  {
    if (data_020a0de9[idx * 4] != 0)
    {
      flag = 1;
    }
  }
  if (flag == 0)
  {
    return 0;
  }
  idx = data_020a0e40;
  dx = (p[0] >> 0xc) - data_020a0dea[idx * new_var];
  dz = (p[1] >> 0xc) - data_020a0deb[idx * 4];
  if ((((dx >= (-8)) && (dx < 8)) && (dz >= (-8))) && (dz < 8))
  {
    _ZN5Sound12PlayBank2_2DEj(0x15d);
    data_ov006_021428c8 = (void *)p;
    *((u8 *) (((char *) p) + 0x32)) = 2;
    p[2] = dx << 0xc;
    p[3] = dz << 0xc;
    return 1;
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov006_02108d28, 0x02108d28, size 0xfc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02108d28
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02108d28(int *p)
{
  int idx;
  int flag;
  int new_var;
  int dx;
  int dy;
  int px, py, bx, by;
  s16 ang;
  new_var = 4;
  if (data_ov006_021428c8 != 0) return;
  idx = data_020a0e40;
  flag = 0;
  if (data_020a0de8[idx * 4])
  {
    if (data_020a0de9[idx * 4] != 0)
    {
      flag = 1;
    }
  }
  if (flag == 0) return;
  if (*((u8 *)(((char *)p) + 0x32)) != 1) return;
  idx = data_020a0e40;
  px = p[0];
  py = p[1];
  bx = data_020a0dea[idx * new_var];
  ang = *((s16 *)(((char *)p) + 0x2c));
  px >>= 12;
  by = data_020a0deb[idx * 4];
  py >>= 12;
  dx = px - bx;
  dy = py - by;
  if (ang != 0x25) return;
  if (func_ov006_02108650(bx, by) == 0x25) return;
  _ZN5Sound12PlayBank2_2DEj(0x15d);
  data_ov006_021428c8 = (void *)p;
  *((u8 *)(((char *)p) + 0x32)) = 2;
  p[2] = dx << 12;
  p[3] = dy << 12;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov006_02108cc0, 0x02108cc0, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02108cc0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02108cc0(char *c) {
    if (*(unsigned char *)(c + 0x32) == 6) return;
    RenderOamBothScreens(data_ov006_02138c3c,
                        *(int *)c >> 0xc,
                        *(int *)(c + 4) >> 0xc,
                        -1,
                        *(int *)(c + 0x28),
                        (void *)0);
    *(int *)(c + 0x28) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov006_02108b90, 0x02108b90, size 0x130 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02108b90
/* recovered: the three range tests are MATERIALIZED booleans -- `sel = cond ? 1 : 0`
   held in a scalar and tested. The shard file carried `#pragma opt_propagation off`
   for this (notes/mwccarm-codegen.md 6r); measured here, it is not needed -- this
   body matches byte for byte at the default -O4,p, with or without the pragma.
   The 0x23 arm tests `r1 < 6`, not `>= 6` -- the two tail cases have OPPOSITE
   conditions and share one `r2 = 2`, which is why they read as a single || chain. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_02108b90(short *r0, int r1)
{
    int r3 = *(short *)((char *)r0 + 0x2c);
    int r2 = 0;
    int ip;

    ip = (r3 <= 0xb) ? 1 : 0;
    if (ip != 0) {
        if (r3 == r1) r2 = 0xc;
    } else {
        ip = (r3 >= 0xc && r3 <= 0x1c) ? 1 : 0;
        if (ip != 0) {
            ip = (r3 - 0xc) << 2;
            if (r1 == *(short *)((char *)data_ov006_0212edb8 + ip) ||
                r1 == *(short *)((char *)data_ov006_0212edba + ip))
                r2 = 6;
        } else {
            ip = (r3 >= 0x1d && r3 <= 0x22) ? 1 : 0;
            if (ip != 0) {
                ip = (r3 - 0x1d) << 3;
                if (r1 == *(short *)((char *)data_ov006_0212ed88 + ip) ||
                    r1 == *(short *)((char *)data_ov006_0212ed8a + ip) ||
                    r1 == *(short *)((char *)data_ov006_0212ed8c + ip) ||
                    r1 == *(short *)((char *)data_ov006_0212ed8e + ip))
                    r2 = 3;
            } else if ((r3 == 0x23 && r1 < 6) || (r3 == 0x24 && r1 >= 6)) {
                r2 = 2;
            }
        }
    }
    *(short *)((char *)r0 + 0x2e) = r2;
    return r2;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov006_02108650, 0x02108650, size 0x540 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02108650
extern "C" {  /* .c-derived member: C linkage for the whole block */
short func_ov006_02108650(int x, int y)
{
    short r = 0x25;

    if (x > data_ov006_0212ed4c[0] && x <= data_ov006_0212ed4c[1]) {
        if (y > 0x10 && y <= 0x60) {
            r = 0x23;
        } else if (y > 0x60 && y <= 0xb0) {
            r = 0x24;
        }
    } else if (x > data_ov006_0212ed4c[1] && x <= data_ov006_0212ed4c[2]) {
        if (y > data_ov006_0212ed68[0] && y <= data_ov006_0212ed68[1]) {
            r = 0;
        } else if (y > data_ov006_0212ed68[1] && y <= data_ov006_0212ed68[2]) {
            r = 0x14;
        } else if (y > data_ov006_0212ed68[2] && y <= data_ov006_0212ed68[3]) {
            r = 3;
        } else if (y > data_ov006_0212ed68[3] && y <= data_ov006_0212ed68[4]) {
            r = 0x17;
        } else if (y > data_ov006_0212ed68[4] && y <= data_ov006_0212ed68[5]) {
            r = 6;
        } else if (y > data_ov006_0212ed68[5] && y <= data_ov006_0212ed68[6]) {
            r = 0x1a;
        } else if (y > data_ov006_0212ed68[6] && y <= data_ov006_0212ed68[7]) {
            r = 9;
        }
    } else if (x > data_ov006_0212ed4c[2] && x <= data_ov006_0212ed4c[3]) {
        if (y > data_ov006_0212ed68[0] && y <= data_ov006_0212ed68[1]) {
            r = 0xc;
        } else if (y > data_ov006_0212ed68[1] && y <= data_ov006_0212ed68[2]) {
            r = 0x1d;
        } else if (y > data_ov006_0212ed68[2] && y <= data_ov006_0212ed68[3]) {
            r = 0xe;
        } else if (y > data_ov006_0212ed68[3] && y <= data_ov006_0212ed68[4]) {
            r = 0x1f;
        } else if (y > data_ov006_0212ed68[4] && y <= data_ov006_0212ed68[5]) {
            r = 0x10;
        } else if (y > data_ov006_0212ed68[5] && y <= data_ov006_0212ed68[6]) {
            r = 0x21;
        } else if (y > data_ov006_0212ed68[6] && y <= data_ov006_0212ed68[7]) {
            r = 0x12;
        }
    } else if (x > data_ov006_0212ed4c[3] && x <= data_ov006_0212ed4c[4]) {
        if (y > data_ov006_0212ed68[0] && y <= data_ov006_0212ed68[1]) {
            r = 1;
        } else if (y > data_ov006_0212ed68[1] && y <= data_ov006_0212ed68[2]) {
            r = 0x15;
        } else if (y > data_ov006_0212ed68[2] && y <= data_ov006_0212ed68[3]) {
            r = 4;
        } else if (y > data_ov006_0212ed68[3] && y <= data_ov006_0212ed68[4]) {
            r = 0x18;
        } else if (y > data_ov006_0212ed68[4] && y <= data_ov006_0212ed68[5]) {
            r = 7;
        } else if (y > data_ov006_0212ed68[5] && y <= data_ov006_0212ed68[6]) {
            r = 0x1b;
        } else if (y > data_ov006_0212ed68[6] && y <= data_ov006_0212ed68[7]) {
            r = 0xa;
        }
    } else if (x > data_ov006_0212ed4c[4] && x <= data_ov006_0212ed4c[5]) {
        if (y > data_ov006_0212ed68[0] && y <= data_ov006_0212ed68[1]) {
            r = 0xd;
        } else if (y > data_ov006_0212ed68[1] && y <= data_ov006_0212ed68[2]) {
            r = 0x1e;
        } else if (y > data_ov006_0212ed68[2] && y <= data_ov006_0212ed68[3]) {
            r = 0xf;
        } else if (y > data_ov006_0212ed68[3] && y <= data_ov006_0212ed68[4]) {
            r = 0x20;
        } else if (y > data_ov006_0212ed68[4] && y <= data_ov006_0212ed68[5]) {
            r = 0x11;
        } else if (y > data_ov006_0212ed68[5] && y <= data_ov006_0212ed68[6]) {
            r = 0x22;
        } else if (y > data_ov006_0212ed68[6] && y <= data_ov006_0212ed68[7]) {
            r = 0x13;
        }
    } else if (x > data_ov006_0212ed4c[5] && x <= data_ov006_0212ed4c[6]) {
        if (y > data_ov006_0212ed68[0] && y <= data_ov006_0212ed68[1]) {
            r = 2;
        } else if (y > data_ov006_0212ed68[1] && y <= data_ov006_0212ed68[2]) {
            r = 0x16;
        } else if (y > data_ov006_0212ed68[2] && y <= data_ov006_0212ed68[3]) {
            r = 5;
        } else if (y > data_ov006_0212ed68[3] && y <= data_ov006_0212ed68[4]) {
            r = 0x19;
        } else if (y > data_ov006_0212ed68[4] && y <= data_ov006_0212ed68[5]) {
            r = 8;
        } else if (y > data_ov006_0212ed68[5] && y <= data_ov006_0212ed68[6]) {
            r = 0x1c;
        } else if (y > data_ov006_0212ed68[6] && y <= data_ov006_0212ed68[7]) {
            r = 0xb;
        }
    }
    return r;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov006_021085c0, 0x021085c0, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_021085c0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_021085c0(char *c){
    *(void**)(c + 8) = func_ov004_020adc68(0x21d);
    *(void**)(c + 0xc) = func_ov004_020adc68(0x21e);
    _ZN5Model17UpdateFileOffsetsER8BMD_File(*(void**)(c + 8));
    _ZN5Model17UpdateFileOffsetsER8BMD_File(*(void**)(c + 0xc));
    if(_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x10, *(void**)(c + 8), 1, -1) == 0)
        return 0;
    if(_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x60, *(void**)(c + 0xc), 1, -1) == 0)
        return 0;
    *(short*)(c + 0xcc) = 0;
    *(int*)(c + 0xbc) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov006_0210858c, 0x0210858c, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210858c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_0210858c(ResPair *o)
{
    if (o->a)
        Ov004_Deallocate(o->a);
    if (o->b)
        Ov004_Deallocate(o->b);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov006_02108524, 0x02108524, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02108524
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02108524(char *c) {
    unsigned r;
    *(int *)(c + 0xb0) = 0x4200;
    *(int *)(c + 0xb4) = 0;
    *(short *)(c + 0xc0) = 0;
    *(short *)(c + 0xc2) = 0;
    *(short *)(c + 0xc6) = 0x400;
    *(short *)(c + 0xc8) = -0x200;
    r = (unsigned)RandomIntInternal(data_0209e650);
    *(int *)(c + 0xb8) = ((r & 0x7fffffff) >> 0x13) + 0x2000;
    *(int *)(c + 0xd0) = 0;
    func_ov006_02108508(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov006_02108508, 0x02108508, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02108508
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02108508(char *p) { *(Pair2W *)(p + 0x0) = data_ov006_0213e2e8; }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov006_021082fc, 0x021082fc, size 0x20c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_021082fc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_021082fc(char *self)
{
    int A = *(int *)(self + 0xb8);
    int r4 = _ZN4cstd4fdivEii((int)(((s64)A * A + 0x800) >> 12), *(int *)(self + 0xb0)) - 0xc00;

    A = *(int *)(self + 0xb8);
    int fm2 = (int)(((s64)A * 0x500 + 0x800) >> 12);
    int t2 = _ZN4cstd4fdivEii(fm2, *(int *)(self + 0xb0));
    *(s16 *)(self + 0xc6) = (s16)t2;

    s16 *pC0 = (s16 *)(((int)self + 0xc0));
    *pC0 = *pC0 + *(s16 *)(self + 0xc6);

    *(int *)(self + 0xb4) = (int)(((s64)*(int *)(self + 0xb4) * 0xfe0 + 0x800) >> 12);

    *(int *)(((int)self + 0xb4)) = *(int *)(((int)self + 0xb4)) + r4;
    *(int *)(((int)self + 0xb0)) = *(int *)(((int)self + 0xb0)) + *(int *)(self + 0xb4);

    int B = *(int *)(self + 0xb0);
    if (B > 0x4200) {
        B = 0x4200;
        *(int *)(self + 0xb0) = B;
        *(int *)(self + 0xb4) = 0;
    } else if (B < 0x2c00) {
        _Z14ApproachLinearRiii(*(int *)(self + 0xb8), 0xfffff667, 0x28);
        func_ov006_021082c4(self);
    } else {
        if (*(s16 *)(self + 0xcc) == 1) {
            *(int *)(self + 0xbc) = 0;
            *(s16 *)(self + 0xcc) = 2;
        }
    }

    if (*(int *)(self + 0xd0) != 0) {
        _Z14ApproachLinearRiii(*(int *)(self + 0xb8), 0, 0x10);
    }

    u16 angle = *(u16 *)(self + 0xc0);
    int dist = *(int *)(self + 0xb0);
    int idx = angle >> 4;
    s16 val1 = data_02082214[idx * 2 + 1];
    s16 val0 = data_02082214[idx * 2];

    int x, y, z;
    struct Vec3 t;
    x = (int)(((s64)dist * val1 + 0x800) >> 12);
    y = (int)(((s64)dist * 0x400 + 0x800) >> 12) + 0x780;
    z = (int)(((s64)dist * val0 + 0x800) >> 12);
    ((int *)&t)[0] = x; ((int *)&t)[1] = y; ((int *)&t)[2] = z;
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, ((int *)&t)[0], ((int *)&t)[1], ((int *)&t)[2]);

    *(Matrix4x3 *)(self + 0x7c) = data_020a0e68;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov006_021082c4, 0x021082c4, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_021082c4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_021082c4(char *p)
{
    *(int *)(p + 0xb8) += 0x140;
    *(short *)(p + 0xc4) = -*(short *)(p + 0xc2);
    *(struct Pair *)p = data_ov006_0213e2f8;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov006_02107ea8, 0x02107ea8, size 0x41c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02107ea8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02107ea8(char *self)
{
    int absv;
    int d1;
    short sum;
    ShortTbl12 buf;
    short diff;
    short idx;
    int extra;
    int angidx, idx2;
    short t0, t1;
    int x, y, z;
    Vec3 t;

    absv = *(int *)(self + 0xb8) - 0x140;
    if (absv < 0) absv = -absv;
    d1 = _ZN4cstd4fdivEii((int)((((long long)absv * absv) + 0x800) >> 12), *(int *)(self + 0xb0)) - 0xc00;

    *(short *)(self + 0xc6) = _ZN4cstd4fdivEii(
        (int)((((long long)(*(int *)(self + 0xb8)) * 0x500) + 0x800) >> 12),
        *(int *)(self + 0xb0));

    *(short *)(((int)self + 0xc0)) += *(short *)(self + 0xc6);

    *(int *)(self + 0xb4) = (int)((((long long)(*(int *)(self + 0xb4)) * 0x600) + 0x800) >> 12);
    *(int *)(((int)self + 0xb4)) += d1;
    *(int *)(((int)self + 0xb0)) += *(int *)(self + 0xb4);

    if (*(int *)(self + 0xb0) > 0x2c00) {
        *(int *)(self + 0xb0) = 0x2c00;
        *(int *)(self + 0xb4) = 0;
    } else if (*(int *)(self + 0xb0) < 0x1600) {
        *(int *)(self + 0xb0) = 0x1600;
        *(int *)(self + 0xb4) = 0;
    }

    sum = *(short *)(self + 0xc0) + *(short *)(self + 0xc4);

    buf = *(ShortTbl12 *)data_ov006_0213e334;

    for (idx = 0; idx < 12; idx++) {
        short ad;
        diff = sum - buf.v[idx];
        ad = diff < 0 ? (short)(-diff) : diff;
        if (ad < 0xaab) break;
    }

    *(short *)(self + 0xca) = data_ov006_0212ed34[idx];

    extra = 0;
    if (*(int *)(self + 0xb0) < 0x2000) {
        if (diff < -0x400 && (diff - *(short *)(self + 0xc6)) >= -0x400) {
            *(short *)(self + 0xc0) = (buf.v[idx] - 0x400) - *(short *)(self + 0xc4);
            *(int *)(self + 0xb8) = -(int)((((long long)(*(int *)(self + 0xb8)) * 0xf00) + 0x800) >> 12);
            _Z14ApproachLinearRiii(*(int *)(self + 0xb8), 0xfffff667, 0x30);
            if (*(short *)(self + 0xcc) != 0) {
                _ZN5Sound12PlayBank2_2DEj(0x159);
                *(short *)(self + 0xcc) = 0;
            }
        } else if (diff > 0x400 && (diff - *(short *)(self + 0xc6)) <= 0x400) {
            *(short *)(self + 0xc0) = (buf.v[idx] + 0x400) - *(short *)(self + 0xc4);
            *(int *)(self + 0xb8) = -(int)((((long long)(*(int *)(self + 0xb8)) * 0xe00) + 0x800) >> 12);
            if (*(short *)(self + 0xcc) != 0) {
                _ZN5Sound12PlayBank2_2DEj(0x159);
                *(short *)(self + 0xcc) = 0;
            }
        } else {
            if (diff < 0) diff = -diff;
            if (diff > 0x400) {
                *(int *)(self + 0xb0) = 0x2000;
                *(int *)(self + 0xb4) = 0;
            }
        }
        extra = (int)((((long long)(*(int *)(self + 0xb0) - 0x2000) * 0x100) + 0x800) >> 12);
    }

    _Z14ApproachLinearRiii(*(int *)(self + 0xb8), 0, 0x30);

    angidx = ((unsigned short)(sum + *(short *)(self + 0xc2))) >> 4;
    idx2 = angidx * 2;
    t0 = data_02082214[idx2 + 1];
    t1 = data_02082214[idx2];

    x = (int)((((long long)(*(int *)(self + 0xb0)) * t0) + 0x800) >> 12);
    y = (int)((((long long)(*(int *)(self + 0xb0)) * 0x400) + 0x800) >> 12) + 0x780 + extra;
    z = (int)((((long long)(*(int *)(self + 0xb0)) * t1) + 0x800) >> 12);
    ((int *)&t)[0] = x; ((int *)&t)[1] = y; ((int *)&t)[2] = z;
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, ((int *)&t)[0], ((int *)&t)[1], ((int *)&t)[2]);

    *(Matrix4x3 *)(self + 0x7c) = data_020a0e68;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov006_02107db8, 0x02107db8, size 0xf0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02107db8
/* recovered: shared common types */
extern "C" void func_ov006_02107db8(C *c)
{
    char *b = (char *)c;
    short *p = (short *)(((int)b + 0xc2));
    *p = (short)(*p + *(short *)(b + 0xc8));
    *(short *)(*(int *)(b + 0x20) + 0x82) = (short)(-*(short *)(b + 0xc2));
    c->obj.v3();
    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
    *(Matrix4x3 *)(b + 0x2c) = *(Matrix4x3 *)&data_020a0e68;
    (c->*(c->pmf))();
    {
        short s = *(short *)(b + 0xcc);
        if (s == 0)
            return;
        if (s == 2)
            *(int *)(b + 0xbc) = Sound_PlayIfNotActive(*(int *)(b + 0xbc), 2, 0x158, 0);
        else
            *(int *)(b + 0xbc) = Sound_PlayIfNotActive(*(int *)(b + 0xbc), 2, 0x157, 0);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov006_02107d80, 0x02107d80, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02107d80
extern "C" void func_ov006_02107d80(char* c){
    ((Sub*)(c + 0x10))->m(0);
    ((Sub*)(c + 0x60))->m(0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov006_02107d58, 0x02107d58, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02107d58
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02107d58(int *p){
  int i;
  for(i=0;i<5;i++) p[i]=0;
  *(short*)((char*)p+0x14)=4;
  *(short*)((char*)p+0x16)=0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov006_02107d20, 0x02107d20, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02107d20
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02107d20(int *p, int val)
{
    int i;
    *(short *)((char *)p + 0x16) += 1;
    for (i = 0; i < 5; i++) {
        if (p[i] == 0) {
            p[i] = val;
            return;
        }
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov006_02107c50, 0x02107c50, size 0xd0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02107c50
/* func_ov006_02107c50 at 0x02107c50
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02107c50(short* a0, int a1)
{
    int idx = GetGameLanguage();
    void* p = *(void**)((char*)data_ov006_0213e320[idx] + 0x50);
    Hud_RenderSprite(p, 0xc, a1, -1, -1);

    func_ov004_020b2444(0x1e, a1, data_ov006_0212ed14[*(short*)((char*)a0 + 0x14)], 0, -1, 2, 0);

    {
        int b = a1 + 0x10;
        RenderOamBothScreens(data_ov006_02138c3c, 0x20, b, -1, 1, (void*)0);
        func_ov004_020b2444(0x2e, b, *(short*)((char*)a0 + 0x16), 0, -1, 1, 0);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov006_02107b94, 0x02107b94, size 0xbc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02107b94
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02107b94(void **sl)
{
    int i;
    for (i = 0; i < 5; i++) {
        char *obj = (char *)sl[i];
        int m, idx, r0, a1, a2;
        if (obj == 0) return;
        *(int *)(obj + 0x28) = 0;
        r0 = *(short *)((char *)sl + 0x14);
        m = *(short *)(obj + 0x2c);
        idx = data_ov006_0212ed24[r0];
        if (data_020a0db0 & 8) idx++;
        if (m >= 0xc && m <= 0x13) idx += 2;
        a1 = ((TileXY *)data_ov006_02142ab4)[m].x >> 12;
        a2 = ((TileXY *)data_ov006_02142ab4)[m].y >> 12;
        if (r0 == 3) a1 += 0x58;
        Hud_RenderSprite(data_ov006_021376ec[idx], a1, a2, -1, -1);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov006_02107b70, 0x02107b70, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02107b70
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02107b70(char* p) {
    int i;
    for (i = 0; i < 4; i++) {
        data_ov006_02142aa4[i] = p;
        p += 0x18;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov006_02107b14, 0x02107b14, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02107b14
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02107b14(void) {
    int i;
    for (i = 0; i < 4; i++) {
        func_ov006_02107d58((int *)data_ov006_02142aa4[i]);
        *(short*)((char*)data_ov006_02142aa4[i] + 0x14) = (short)i;
    }
    data_ov006_0213e2dc = -1;
    data_ov006_021428d0 = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov006_02107a6c, 0x02107a6c, size 0xa8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_02107a6c
extern "C" int func_ov006_02107a6c(void)
{
    if (_Z14ApproachLinearRiii(data_ov006_021428d0, 0, 1) != 0) {
        int idx;
        data_ov006_0213e2dc = data_ov006_0213e2dc + 1;
        idx = data_ov006_0213e2dc;
        data_ov006_021428d0 = 0x78;
        while (idx < 4) {
            if (*(short*)(data_ov006_02142aa4[idx] + 0x16) != 0) break;
            data_ov006_0213e2dc = data_ov006_0213e2dc + 1;
            idx = data_ov006_0213e2dc;
        }
        if (idx < 4) {
            func_02012790(0x26);
        }
    }
    return (data_ov006_0213e2dc >= 4) ? 1 : 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov006_021079cc, 0x021079cc, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_021079cc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_021079cc(void){
    int n = data_ov006_0213e2dc;
    int arg;
    int i;
    if (n < 0) return;
    n = n + 1;
    if (n > 4) n = 4;
    arg = 0x28;
    for (i = 0; i < n; i++) {
        int* o = (int *)data_ov006_02142aa4[i];
        if (*(short*)((char*)o + 0x16) > 0) {
            func_ov006_02107c50((short *)o, arg);
            arg += 0x24;
        }
    }
    if (data_ov006_0213e2dc >= 4) return;
    func_ov006_02107b94((void **)data_ov006_02142aa4[data_ov006_0213e2dc]);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov006_021079c8, 0x021079c8, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_021079c8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_021079c8(void)
{
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- _ZN15dScMgRoulette_cD0Ev 0x02107920 (0xa8) and
 * _ZN15dScMgRoulette_cD1Ev 0x0210788c (0x94).
 *
 * NEITHER IS DEFINED HERE. The cartridge puts D1 at 0x0210788c BELOW D0 at
 * 0x02107920 and carries no D2 at all, and the only admissible source form
 * that makes mwccarm 2004/b56 emit D1-then-D0 is an INLINE, in-class
 * destructor declared as the first member. The two legacy shards each held
 * an out-of-line copy of the same body; merged they would also have been a
 * duplicate definition. The body now lives in include/dScMgRoulette_c.h,
 * where the compiler emits both variants itself, in the cartridge's order.
 * See the class header's own note. */
