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
 *   [3] 0x020d96e0  _ZN12dMgCardObj_cD1Ev
 *   [4] 0x020d96f0  _ZN17dMgDilarCardObj_cD1Ev
 *   [5] 0x020d970c  _ZN17dMgDilarCardObj_c6RenderEv
 *   [6] 0x020d978c  _ZN17dMgDilarCardObj_c6UpdateEv
 *   [7] 0x020d9998  _ZN17dMgDilarCardObj_c13GetOffscreenYEv
 *   [8] 0x020d99a4  _ZN12dMgCardObj_c6FlyOffEv
 *   [9] 0x020d99ec  _ZN12dMgCardObj_c7SetHomeEi
 *   [10] 0x020d9a14  _ZN11dScMgCard_c9RenderHudEv
 *   [11] 0x020d9bd0  _ZN12dMgCardObj_c13GetOffscreenYEv
 *   [12] 0x020d9bdc  _ZN12dMgCardObj_c6RenderEv
 *   [13] 0x020d9c5c  _ZN12dMgCardObj_c6UpdateEv
 *   [14] 0x020da00c  _ZN12dMgCardObj_c6RedealEv
 *   [15] 0x020da0ac  _ZN12dMgCardObj_c6DealInEPKi
 *   [16] 0x020da154  _ZN11dScMgCard_c10SettleHandEP12dMgCardObj_c
 *   [17] 0x020da174  _ZN11dScMgCard_c11ArrangeHandEP12dMgCardObj_c
 *   [18] 0x020da420  _ZN11dScMgCard_c11MarkSinglesEP12dMgCardObj_c
 *   [19] 0x020da4ac  _ZN11dScMgCard_c12EvaluateHandEPK12dMgCardObj_cPs
 *   [20] 0x020da5e8  _ZN11dScMgCard_c12CompareHandsEPK12dMgCardObj_cS2_
 *   [21] 0x020da834  _ZN11dScMgCard_c9AllLiftedEPK12dMgCardObj_c
 *   [22] 0x020da860  _ZN11dScMgCard_c10AllInStateEPK12dMgCardObj_ci
 *   [23] 0x020da88c  _ZN11dScMgCard_c12CountInStateEPK12dMgCardObj_ci
 *   [24] 0x020da8b8  _ZN11dScMgCard_c11FindInStateEPK12dMgCardObj_ci
 *   [25] 0x020da8e4  _ZN11dScMgCard_c13DrawCardValueEv
 *   [26] 0x020da974  _ZN11dScMgCard_c11FillWeightsEi
 *   [27] 0x020da994  _ZN11dScMgCard_c16CleanupResourcesEv
 *   [28] 0x020da9c4  _ZN11dScMgCard_c6RenderEv
 *   [29] 0x020dabec  _ZN11dScMgCard_c8BehaviorEv
 *   [30] 0x020dac34  _ZN11dScMgCard_c11UpdateStateEv
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

/* TUBUILD CONFLICT -- alternate body of typedef 's32', from the legacy file for func_ov006_020da174, NOT applied:
typedef signed int s32;
*/

/* TUBUILD CONFLICT -- alternate body of typedef 's16', from the legacy file for _ZN11dScMgCard_c13OnTurnIntoEggEi, NOT applied:
typedef short s16;
*/

extern "C" {
extern int data_ov006_0213acb0[2];
extern int data_ov006_0213aca8[2];
/* the two element vtables, by the symbols.txt rows (0x0213bccc / 0x0213bcf4):
   this same TU emits them (Render -- each class's key function -- is defined
   below), and the hand-written destroy stubs near the file's end reference
   them. The names are already the mangled Itanium names in symbols.txt, so
   extern "C" here means "use literally" */
extern int _ZTV12dMgCardObj_c[];
extern int _ZTV17dMgDilarCardObj_c[];
extern unsigned short data_ov006_0213bd64[];
extern void *data_ov006_02141774[];
extern void Hud_RenderSprite(void *sprite, int x, int y, int a3, int a4);
extern void **data_ov006_0213bd30[];
extern void *data_ov006_0213406c[];
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
extern void func_ov006_020c1420(void *c, s16 arg1, void *arg2);
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
extern int func_ov006_020c1a88(char *);
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020da8e4, from the legacy file for func_ov006_020d9c5c, NOT applied: int func_ov006_020da8e4(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020da8e4, from the legacy file for func_ov006_020da00c, NOT applied: extern "C" int func_ov006_020da8e4(void); */
/* TUBUILD CONFLICT -- alternate declaration of Vec2_Sub, from the legacy file for func_ov006_020da0ac, NOT applied: extern "C" void Vec2_Sub(int *o, int *a, int *b); */
/* TUBUILD CONFLICT -- alternate declaration of func_0203d680, from the legacy file for func_ov006_020da0ac, NOT applied: extern "C" void func_0203d680(int *out, int *in, int scale); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020da8e4, from the legacy file for func_ov006_020da0ac, NOT applied: extern "C" int func_ov006_020da8e4(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020da834, from the legacy file for func_ov006_020dac34, NOT applied: extern int func_ov006_020da834(void *p); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020da860, from the legacy file for func_ov006_020dac34, NOT applied: extern int func_ov006_020da860(void *p, int v); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020c1718, from the legacy file for _ZN11dScMgCard_c13OnTurnIntoEggEi, NOT applied: extern int func_ov006_020c1718(void* p); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02134028, from the legacy file for _ZN11dScMgCard_c13InitResourcesEv, NOT applied: extern u32 *data_ov006_02134028; */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020c0aa8, from the legacy file for _ZN11dScMgCard_c13InitResourcesEv, NOT applied: extern void func_ov006_020c0aa8(char *); */
}

/* --- the engine helpers, by their true names -------------------------------
   These six calls used to be spelled by their mangled ROM names, declared
   in the extern "C" block above. They are the real C++ names now -- plain
   file-scope declarations, never extern "C", the idiom dScMgBSC_c's
   conversion established -- and each one's mwcc mangling IS that same ROM
   symbol, so every call below compiles to the same bl target as before:
     Sound::PlayBank2_2D        _ZN5Sound12PlayBank2_2DEj
     GX::LoadOBJPltt            _ZN2GX11LoadOBJPlttEPKvjj
     GXS::LoadOBJPltt           _ZN3GXS11LoadOBJPlttEPKvjj
     G2x::SetBlendAlpha         _ZN3G2x13SetBlendAlphaEPVttttj
     ApproachLinear             _Z14ApproachLinearRiii
     ApproachLinear2            _Z15ApproachLinear2Rsss
   The parameter types are read off the mangled names, not chosen (see
   include/G2x.h's own banner for why that matters): `j` unsigned int,
   `t` unsigned short, `s` short, `Riii` int&, `Rsss` short&. Sound is a
   namespace per include/Sound.h; GX/GXS are spelled as namespaces the same
   way, since a namespace and a class-only-statics mangle identically and
   the calls here are static either way. */
namespace Sound { void PlayBank2_2D(u32 id); }
namespace GX { void LoadOBJPltt(const void *plt, u32 base, u32 size); }
namespace GXS { void LoadOBJPltt(const void *plt, u32 base, u32 size); }
namespace G2x { void SetBlendAlpha(volatile u16 *reg, u16 a, u16 b, u16 c, u32 d); }
int  ApproachLinear(int &value, int target, int step);
int  ApproachLinear2(s16 &value, s16 target, s16 step);

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
 * The final `OnYoshiTryEat(-1)` is a self-dispatch through this class's own
 * vtable slot 18 -- the adjudicated eat handler whose body sits below; the
 * reload needs the whole round reset it does. */
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
    GX::LoadOBJPltt(f6, 0, 0x20);
    DecompressLZ16(f5, 0x6600000);
    GXS::LoadOBJPltt(f4, 0, 0x100);
    Deallocate(f7);
    Deallocate(f6);
    Deallocate(f5);
    Deallocate(f4);
    G2x::SetBlendAlpha((volatile u16 *)0x04000050, 0, 1, 0x10, 8);
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

    func_ov006_020c0aa8(pad_4660);
    if (func_ov006_020c1a88((char *)pad_4f38) == 0)
        return 0;

    {
        int y = 0x2c;
        int i;
        for (i = 0; i < 5; i++) {
            v[0] = y << 12;
            v[1] = 0x80000;
            ((dMgCardObj_c *)((Obj6 *)c)->rowA[i].b)->DealIn(v);
            v[1] = 0x10000;
            ((dMgCardObj_c *)((Obj6 *)c)->rowB[i].b)->DealIn(v);
            y += 0x28;
        }
    }

    self->mState = 0;
    self->mStateTimer = 0;
    unk_0a8 = func_ov004_020ad8b8();
    unk_0ac = unk_0a8;
    {
        int r = func_ov004_020ad878();
        if (data_ov004_020beb68 != 0)
            ((dScMgBase_c *)data_ov004_020beb68)->mHudScore = r;
    }
    self->mScore = 0;
    func_ov004_020b682c();
    OnYoshiTryEat(-1);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- _ZN11dScMgCard_c13OnYoshiTryEatEi, 0x020db9dc, size 0x114 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c13OnYoshiTryEatEi
/* dScMgCard_c::OnYoshiTryEat -- vtable slot 18, the eat handler: reset the
 * whole round. The name is the legacy recovered vtable name
 * (dActor_c.h:131, corroborated by ov006's symbols.txt), standing
 * uncontradicted; what the family adjudication of 2026-08-31 settled is the
 * slot's RETURN TYPE, void (notes/minigame-family-decisions.md). The body
 * agrees: it clears the card-marked counter and both highlight pairs
 * (6 means "none"), re-arms the shared-table pair the round readout uses,
 * refills the six-face weights, redeals both hands, resets the chip
 * globals, sets the round counter to 1 and restarts the state machine at
 * step 1. No destructor does any of that. */
void dScMgCard_c::OnYoshiTryEat(int mode)
{
    int i;
    char *p1, *p2;

    unk_538c = 0;
    unk_538e = 6;
    unk_5390 = 6;
    unk_5392 = 6;
    unk_5394 = 6;
    func_ov006_020c1604((char *)pad_4f38, 4, 5, (int)&unk_538c);

    unk_511e = 1;
    unk_4f52 = 2;
    dScMgCard_c::FillWeights(5);

    p1 = (char *)mArray1;
    p2 = (char *)mArray2;
    for (i = 0; i < 5; i++) {
        ((dMgCardObj_c *)p1)->Redeal();
        ((dMgCardObj_c *)p2)->Redeal();
        p1 += 0x30;
        p2 += 0x30;
    }

    func_ov004_020b66d4();

    data_ov004_020bc7d4 = 1;
    data_ov006_0214176c = 0;
    data_ov006_02141768 = 0;
    data_ov006_02141770 = 0;
    unk_539a = 0;
    data_ov006_0213bc44 = 1;
    mState = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- _ZN11dScMgCard_c13OnTurnIntoEggEi, 0x020db720, size 0x2bc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c13OnTurnIntoEggEi
/* dScMgCard_c::OnTurnIntoEgg -- vtable slot 19, the eaten exit: the round
 * ends because the player was swallowed, and these steps walk the scene
 * out. 0xe frees the result sprite the comparison picks; 0xf waits for the
 * touch poll, plays the result sound (win scales by the hand's payout
 * row), and steps on; 0x10 settles a forced result or waits the timer out
 * and frees slot 0x1d; 0x11 waits for the player object to leave, flies
 * both hands off, and arms the last timer; 0x12 runs that timer down and
 * reports the exit done. Returns 1 the moment the exit finishes. */
int dScMgCard_c::OnTurnIntoEgg(int mode)
{
    switch (mState) {
    case 0xe: {
        int r = dScMgCard_c::CompareHands((const dMgCardObj_c *)mArray1, (const dMgCardObj_c *)mArray2);
        if (r == -1)
            FreeGfxSlotsById(0xa);
        else if (r == 1)
            FreeGfxSlotsById(9);
        mStateTimer = 0;
        mState += 1;
        break;
    }
    case 0xf:
        if (func_ov006_020c1718(pad_4f38) != 0) {
            int r = dScMgCard_c::CompareHands((const dMgCardObj_c *)mArray1, (const dMgCardObj_c *)mArray2);
            if (r == -1) {
                func_ov004_020b5ed0();
                mStateTimer = 0;
            } else if (r == 1) {
                int v = data_ov006_0213bd48[dScMgCard_c::EvaluateHand((const dMgCardObj_c *)mArray1, 0)];
                func_ov004_020b56c8(v * data_ov004_020bfa18);
                mStateTimer = 0;
            } else {
                func_ov004_020b5d74();
                mStateTimer = 0x1e;
            }
            mState += 1;
        }
        break;
    case 0x10:
        if (data_ov004_020bf9e4 == 1) {
            int r = dScMgCard_c::CompareHands((const dMgCardObj_c *)mArray1, (const dMgCardObj_c *)mArray2);
            dScMgCard_c::EvaluateHand((const dMgCardObj_c *)mArray1, 0);
            if (r == -1) {
                mState += 1;
            } else {
                return 1;
            }
        } else {
            if (ApproachLinear2(mStateTimer, 0, 1) != 0)
                FreeGfxSlotsById(0x1d);
        }
        break;
    case 0x11: {
        int i;
        char* a;
        char* b;
        void* p = data_ov004_020beb68;
        if ((p != 0 ? ((dScMgBase_c *)p)->unk_0a8 : 0) != 0)
            return 1;
        a = (char *)mArray1;
        b = (char *)mArray2;
        for (i = 0; i < 5; i++) {
            ((dMgCardObj_c *)a)->FlyOff();
            ((dMgCardObj_c *)b)->FlyOff();
            a += 0x30;
            b += 0x30;
        }
        mStateTimer = 0x3c;
        mState += 1;
        break;
    }
    case 0x12:
        if (ApproachLinear2(mStateTimer, 0, 1) != 0)
            return 1;
        break;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- _ZN11dScMgCard_c15OnGroundPoundedEv, 0x020db6ec, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c15OnGroundPoundedEv
/* dScMgCard_c::OnGroundPounded -- vtable slot 21, the ground-pound reaction:
 * the base family's coin-drop call, fed by this scene's HUD score -- the
 * prize tier rises with the score (1..4). This overrides the void slot
 * declared by dScMgBase_c. Recovered from the vtable slot identity. */
void dScMgCard_c::OnGroundPounded() {
    int x = mHudScore;
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
/* ROM ordinal 30 -- _ZN11dScMgCard_c11UpdateStateEv, 0x020dac34, size 0xab8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c11UpdateStateEv
/* dScMgCard_c::UpdateState -- the round's own state machine, one long
 * switch on mState that Behavior drives every frame; the tail past the
 * switch Updates all ten cards, both hands, every frame. Steps 1-5 arm
 * the prompt and read the touched slot, mirroring the pick onto the
 * matching card of BOTH hands and waiting for the lift; 6-8 run the
 * dealer's reveal the same way; 9-13 settle the round -- arrange both
 * hands, flip and lift them, evaluate and compare them, then pay the
 * win out or take the loss's chip. mState mostly just `++`s and
 * mStateTimer carries each step's countdown; the two highlight pairs
 * and the card states carry the rest. The singleton
 * data_ov004_020beb68 is the active dScMgBase_c (see its own note in
 * notes/minigame-provenance.md): unk_0a8 is the coin counter,
 * mHudScore the chip tally, unk_0b8 its high-water mark. Coined name;
 * the ROM's symbol is address-only. */
void dScMgCard_c::UpdateState()
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

    switch (mState) {
    case 1:
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        if (func_ov006_020c0f0c(pad_4f38) != 0)
            mState++;
        break;
    case 2:
        if (func_ov006_020c1718(pad_4f38) != 0) {
            mStateTimer = 0x10;
            mState++;
        } else {
            cnt = unk_538c;
            if (cnt > 0) {
                idx = cnt - 1;
                if (cnt != 0) {
                    idx = idx * 0x30;
                    p = &mArray1[0].mState;
                    if (p[idx] == 0) {
                        p[idx] = 1;
                        ((dMgCardObj_c *)((char *)mArray2 + idx))->mState = 1;
                    }
                }
            }
        }
        break;
    case 3:
        mStateTimer--;
        if (mStateTimer == 0)
            mState++;
        break;
    case 4:
        ok = 0;
        idx = data_020a0e40 * 4;
        if (data_020a0de8[idx] != 0 && data_020a0de9[idx] != 0)
            ok = 1;
        if (ok != 0 && dScMgCard_c::AllLifted((const dMgCardObj_c *)mArray1) != 0 && data_ov006_02141768 == 0) {
            v5 = data_020a0dea[data_020a0e40][0];
            v4 = data_020a0deb[data_020a0e40][0];
            if (dScMgCard_c::AllInState((const dMgCardObj_c *)mArray1, 2) != 0) {
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
                        mState++;
                        data_ov004_020bf9ec = 1;
                        Sound::PlayBank2_2D(0x151);
                        g = data_ov004_020beb68;
                        a0 = g ? ((dScMgBase_c *)g)->unk_0a8 : 0;
                        a1 = g ? ((dScMgBase_c *)g)->mHudScore : 0;
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
                        mState++;
                        data_ov004_020bf9ec = 1;
                        Sound::PlayBank2_2D(0x151);
                        g = data_ov004_020beb68;
                        a0 = g ? ((dScMgBase_c *)g)->unk_0a8 : 0;
                        a1 = g ? ((dScMgBase_c *)g)->mHudScore : 0;
                        func_ov004_020ad79c(a0, a1);
                        cnt3 = 0;
                        i = cnt3;
                        do {
                            r = mArray1[i].mState;
                            i++;
                            if (r == 3)
                                cnt3++;
                        } while (i < 5);
                        if (cnt3 < 2)
                            Sound::PlayBank2_2D(0x144);
                        else
                            Sound::PlayBank2_2D(0x145);
                    }
                }
            }
        }
        break;
    case 5:
        if (data_ov006_02141768 != 0 && dScMgCard_c::AllInState((const dMgCardObj_c *)mArray1, 2) != 0
            && func_ov006_020c1718(pad_4f38) != 0) {
            mState++;
        } else if (func_ov006_020c1718(pad_4f38) != 0) {
            r = dScMgCard_c::CountInState((const dMgCardObj_c *)mArray1, 6);
            if (r != 0) {
                unk_538c = 0;
                unk_511e = 0;
                func_ov006_020c1420(pad_4f38, (s16)r, &unk_538c);
            }
        } else {
            r = dScMgCard_c::FindInState((const dMgCardObj_c *)mArray1, 6);
            if (r > -1 && unk_538c != 0) {
                ((dMgCardObj_c *)mArray1)[r].mState = 5;
                unk_538c--;
            }
        }
        break;
    case 6:
        dScMgCard_c::MarkSingles((dMgCardObj_c *)mArray2);
        mState++;
        break;
    case 7:
        if (dScMgCard_c::AllInState((const dMgCardObj_c *)mArray2, 1) != 0 && func_ov006_020c1718(pad_4f38) != 0) {
            r = data_ov006_0213bc44 - 1;
            data_ov006_0213bc44 = r;
            if (r == 0) {
                mState++;
            } else {
                mState = 4;
                data_ov006_0214176c = 0;
                data_ov006_02141768 = 0;
                data_ov006_02141770 = 0;
            }
        } else if (func_ov006_020c1718(pad_4f38) != 0) {
            r = dScMgCard_c::CountInState((const dMgCardObj_c *)mArray2, 6);
            if (r != 0) {
                unk_538c = 0;
                unk_511e = 0;
                func_ov006_020c1420(pad_4f38, (s16)r, &unk_538c);
            }
        } else {
            r = dScMgCard_c::FindInState((const dMgCardObj_c *)mArray2, 6);
            if (r > -1 && unk_538c != 0) {
                ((dMgCardObj_c *)mArray2)[r].mState = 5;
                unk_538c--;
            }
        }
        break;
    case 8:
        j = 0;
        mPromptEnabled = (unsigned char)j;
        do {
            mArray2[j].mState = 2;
            j++;
        } while (j < 5);
        mState++;
        break;
    case 9:
        a = dScMgCard_c::ArrangeHand((dMgCardObj_c *)mArray1);
        b = dScMgCard_c::ArrangeHand((dMgCardObj_c *)mArray2);
        if (a != 0 || b != 0)
            Sound::PlayBank2_2D(0x152);
        mState++;
        break;
    case 10:
        if (dScMgCard_c::AllInState((const dMgCardObj_c *)mArray1, 8) != 0 && dScMgCard_c::AllInState((const dMgCardObj_c *)mArray2, 8) != 0) {
            mStateTimer = 0x14;
            mState++;
        }
        break;
    case 11:
        mStateTimer--;
        if (mStateTimer == 0) {
            k = 0;
            do {
                mArray1[k].mLiftFlag = 1;
                mArray2[k].mLiftFlag = 1;
                k++;
            } while (k < 5);
            dScMgCard_c::SettleHand((dMgCardObj_c *)mArray1);
            dScMgCard_c::SettleHand((dMgCardObj_c *)mArray2);
            Sound::PlayBank2_2D(0x147);
            mStateTimer = 0x3c;
            mState++;
        }
        break;
    case 12:
        mStateTimer--;
        if (mStateTimer == 0) {
            t4 = dScMgCard_c::EvaluateHand((const dMgCardObj_c *)mArray2, &unk_5392);
            t5 = dScMgCard_c::EvaluateHand((const dMgCardObj_c *)mArray1, &unk_538e);
            if (data_ov006_02141768 != 0 && data_ov006_02141768 != 0) {
                /* the hand-value fanfares sit at sound bank 22 up */
                func_ov004_020b0cac(t4 + 22, 0x80, 0x48, 0, -1, 1);
                func_ov004_020b0cac(t5 + 22, 0x80, 0x78, 0, -1, 2);
                if (t4 != t5) {
                    unk_5392 = 6;
                    unk_5394 = 6;
                    unk_538e = 6;
                    unk_5390 = 6;
                } else if (unk_5392 != unk_538e) {
                    unk_5394 = 6;
                    unk_5390 = 6;
                }
            }
            r = dScMgCard_c::CompareHands((const dMgCardObj_c *)mArray1, (const dMgCardObj_c *)mArray2);
            if (r == -1) {
                g = data_ov004_020beb68;
                a0 = g ? ((dScMgBase_c *)g)->unk_0a8 : 0;
                a1 = g ? ((dScMgBase_c *)g)->mHudScore : 0;
                func_ov004_020ad79c(a0, a1 - 1);
                mStateTimer = 0x5a;
            } else if (r == 1) {
                g = data_ov004_020beb68;
                {
                    int *tbl = data_ov006_0213bd48;
                    int scale;
                    int prod;
                    a0 = g ? ((dScMgBase_c *)g)->unk_0a8 : 0;
                    r = tbl[t5];
                    scale = data_ov004_020bfa18;
                    prod = r * scale + a0;
                    r = g ? ((dScMgBase_c *)g)->mHudScore : 0;
                    func_ov004_020ad79c(prod, r + 1);
                }
                mStateTimer = 0x1e;
            } else {
                mStateTimer = 0x5a;
            }
            mState++;
        }
        break;
    case 13:
        mStateTimer--;
        if (mStateTimer == 0) {
            r = dScMgCard_c::CompareHands((const dMgCardObj_c *)mArray1, (const dMgCardObj_c *)mArray2);
            if (r == -1) {
                func_ov006_020c0d68(pad_4f38);
                g = data_ov004_020beb68;
                if (g != 0) {
                    if (((dScMgBase_c *)g)->mHudScore > 0) {
                        ((dScMgBase_c *)g)->mHudScore = ((dScMgBase_c *)g)->mHudScore - 1;
                    }
                }
                func_ov004_020b0a54(5);
            } else if (r == 1) {
                func_ov006_020c0c80(pad_4f38);
                g = data_ov004_020beb68;
                if (g != 0) {
                    if (((dScMgBase_c *)g)->mHudScore < 0x270f) {
                        int *fld = &((dScMgBase_c *)g)->mHudScore;
                        *fld = *fld + 1;
                    }
                    a1 = ((dScMgBase_c *)g)->mHudScore;
                    if (a1 > ((dScMgBase_c *)g)->unk_0b8)
                        ((dScMgBase_c *)g)->unk_0b8 = a1;
                }
                func_ov004_020adb1c(mHudScore);
                func_ov004_020b0a54(4);
            } else {
                func_ov004_020b0a54(7);
                Sound::PlayBank2_2D(0x140);
                Sound::PlayBank2_2D(0x138);
            }
            mPromptEnabled = 0;
            mStateTimer = 0;
            mState++;
        }
        break;
    }

    p1 = (unsigned char *)mArray1;
    p2 = (unsigned char *)mArray2;
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
    UpdateState();
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

    func_ov006_020c0aa8(pad_4660);

    v = this->mState;
    if (v > 3 && v < 0x11) {
        skip = -1;
        if (v > 0xe) {
            if (this->mFrameCounter & 8) {
                if (dScMgCard_c::CompareHands((const dMgCardObj_c *)mArray1, (const dMgCardObj_c *)mArray2) == 1) {
                    skip = 6 - dScMgCard_c::EvaluateHand((const dMgCardObj_c *)mArray1, 0);
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

    RenderHud();

    {
        /* The player's five cards, element 4 first: a card outside the marked
         * pair always draws, a marked one draws on the blink phase only.
         * Spelled as indexed member access -- mValue read, Render() called
         * through the element's own vtable, k walking 4 down to 0 -- which
         * matches the cartridge byte-identically (0x228/0x228). The folded
         * one-walker member form (a typed pointer stepping back 0x30 a
         * card) is the spelling that measurably moves bytes (0x210), and is
         * not used. */
        int k;
        for (k = 4; k >= 0; k--) {
            unsigned char cfg = mArray1[k].mValue;
            if (this->unk_538e == cfg)
                goto chk1;
            if (this->unk_5390 != cfg)
                goto docall1;
        chk1:
            if (!(this->mFrameCounter & 8))
                goto skip1;
        docall1:
            mArray1[k].Render();
        skip1: ;
        }
    }

    {
        /* the dealer's five: same walk, the dealer's pair */
        int k;
        for (k = 4; k >= 0; k--) {
            unsigned char cfg = mArray2[k].mValue;
            if (this->unk_5392 == cfg)
                goto chk2;
            if (this->unk_5394 != cfg)
                goto docall2;
        chk2:
            if (!(this->mFrameCounter & 8))
                goto skip2;
        docall2:
            mArray2[k].Render();
        skip2: ;
        }
    }

    func_ov006_020c1804(pad_4f38);
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
/* ROM ordinal 26 -- _ZN11dScMgCard_c11FillWeightsEi, 0x020da974, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c11FillWeightsEi
/* dScMgCard_c::FillWeights -- sets every one of the six card-face weights to
 * the same value: the deal code passes 5 ("all six faces equally likely"),
 * the win/lose branches pass 0 (the deck goes dead). Coined name; the ROM's
 * symbol is address-only. */
void dScMgCard_c::FillWeights(int v) {
    int i;
    for (i = 0; i < 6; i++) {
        data_ov006_0213bd18[i] = v;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- _ZN11dScMgCard_c13DrawCardValueEv, 0x020da8e4, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c13DrawCardValueEv
/* dScMgCard_c::DrawCardValue -- one weighted draw from the six-face deck:
 * roll the seeded RNG scaled by the weight total, then walk the weights
 * subtracting until the running sum goes negative -- that face is the draw,
 * and its weight is spent. Coined name; the ROM's symbol is address-only. */
int dScMgCard_c::DrawCardValue(void)
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- _ZN11dScMgCard_c11FindInStateEPK12dMgCardObj_ci, 0x020da8b8, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c11FindInStateEPK12dMgCardObj_ci
/* dScMgCard_c::FindInState -- index of the first card in the hand sitting in
 * the given state, or -1 when no card is. Coined name; the ROM's symbol is
 * address-only. */
int dScMgCard_c::FindInState(const dMgCardObj_c *hand, int state)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (state == hand->mState)
            return i;
        hand++;
    }
    return -1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- _ZN11dScMgCard_c12CountInStateEPK12dMgCardObj_ci, 0x020da88c, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c12CountInStateEPK12dMgCardObj_ci
/* dScMgCard_c::CountInState -- how many of the hand's five cards sit in the
 * given state. Coined name; the ROM's symbol is address-only. */
int dScMgCard_c::CountInState(const dMgCardObj_c *hand, int state)
{
    int count = 0;
    int i;
    for (i = 0; i < 5; i++) {
        if (state == hand->mState)
            count++;
        hand++;
    }
    return count;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- _ZN11dScMgCard_c10AllInStateEPK12dMgCardObj_ci, 0x020da860, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c10AllInStateEPK12dMgCardObj_ci
/* dScMgCard_c::AllInState -- true only when every card of the hand sits in
 * the given state. Coined name; the ROM's symbol is address-only. */
int dScMgCard_c::AllInState(const dMgCardObj_c *hand, int state)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (state != hand->mState)
            return 0;
        hand++;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN11dScMgCard_c9AllLiftedEPK12dMgCardObj_c, 0x020da834, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c9AllLiftedEPK12dMgCardObj_c
/* dScMgCard_c::AllLifted -- true only when every card of the hand is raised
 * to the full 0x4000 lift. Coined name; the ROM's symbol is address-only. */
int dScMgCard_c::AllLifted(const dMgCardObj_c *hand)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (hand->mLift < 0x4000)
            return 0;
        hand++;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN11dScMgCard_c12CompareHandsEPK12dMgCardObj_cS2_, 0x020da5e8, size 0x24c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c12CompareHandsEPK12dMgCardObj_cS2_
/* dScMgCard_c::CompareHands -- poker comparison of the two five-card hands:
 * histogram each hand's face counts, find each hand's strongest count and
 * its runner-up (keeping each one's face as the tiebreak, the earlier card
 * winning an equal pair), then compare strongest count, runner-up count,
 * strongest face, runner-up face, in that order. Coined name; the ROM's
 * symbol is address-only. */
int dScMgCard_c::CompareHands(const dMgCardObj_c *a, const dMgCardObj_c *b) {
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
        arr1[a->mValue]++;
        a++;
        arr2[b->mValue]++;
        b++;
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN11dScMgCard_c12EvaluateHandEPK12dMgCardObj_cPs, 0x020da4ac, size 0x13c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c12EvaluateHandEPK12dMgCardObj_cPs
/* dScMgCard_c::EvaluateHand -- score one hand on the poker scale (nothing,
 * one pair, two pair, three of a kind, full house, four, five), and when out
 * is non-null, name the faces of the winning combination into it. Coined
 * name; the ROM's symbol is address-only. */
int dScMgCard_c::EvaluateHand(const dMgCardObj_c *hand, s16 *out)
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
        counts[hand->mValue]++;
        hand++;
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN11dScMgCard_c11MarkSinglesEP12dMgCardObj_c, 0x020da420, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c11MarkSinglesEP12dMgCardObj_c
/* dScMgCard_c::MarkSingles -- stamp every card whose face appears exactly
 * once in the hand to state 4: the settle step for the cards left out of
 * the winning combination. Coined name; the ROM's symbol is address-only. */
void dScMgCard_c::MarkSingles(dMgCardObj_c *hand){
    int i;
    dMgCardObj_c *p;
    short arr[6];
    short *a = (short *)(((int)arr));
    a[0] = 0;
    a[1] = 0;
    a[2] = 0;
    a[3] = 0;
    a[4] = 0;
    a[5] = 0;
    p = hand;
    for (i = 0; i < 5; i++) {
        arr[p->mValue]++;
        p++;
    }
    for (i = 0; i < 5; i++) {
        if (arr[hand->mValue] == 1)
            hand->mState = 4;
        hand++;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN11dScMgCard_c11ArrangeHandEP12dMgCardObj_c, 0x020da174, size 0x2ac */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c11ArrangeHandEP12dMgCardObj_c
/* dScMgCard_c::ArrangeHand -- the sort that pulls a scored hand together:
 * histogram the faces, decide the winning combination's two faces (the
 * stronger face first), then bubble the five cards by combination rank
 * through the order table at data_ov006_0213bcd8, stamp the combination
 * cards' mComboFlag, and SetHome every card to its sorted slot. Returns
 * whether the order changed at all. Coined name; the ROM's symbol is
 * address-only. */
int dScMgCard_c::ArrangeHand(dMgCardObj_c *hand)
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
  dMgCardObj_c *p;
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
    s32 idx = hand[i].mValue;
    hist[idx] = (s16) (hist[idx] + 1);
  }
  p = hand;

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
        p = hand;
        for (t = 0; t < 5; t++)
        {
          s32 c = p->mValue;
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
          p++;
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
        dMgCardObj_c *ptr1 = (dMgCardObj_c *) (((u32) hand) + (v1 * stride));
        s32 u1 = ptr1->mValue;
        s32 next = k + 1;
        s32 v2;
        dMgCardObj_c *ptr2 = (dMgCardObj_c *) (((u32) hand) + (order.v[next] * stride));
        s32 u2 = ptr2->mValue;
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
    dMgCardObj_c *q = hand;
    s32 one = 1;
    s32 z;
    j = 0;
    z = j;
    for (; j < 5; j++)
    {
      s32 c = q->mValue;
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
      q->mComboFlag = one;

      goto flag_nextc;
      store0c:
      q->mComboFlag = z;

      flag_nextc:
      q++;

    }

  }
  for (j = 0; j < 5; j++)
  {
    s32 v = order.v[j];
    dMgCardObj_c *ptr = (dMgCardObj_c *) (((u32) hand) + (v * 0x30));
    ptr->SetHome(j);
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN11dScMgCard_c10SettleHandEP12dMgCardObj_c, 0x020da154, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c10SettleHandEP12dMgCardObj_c
/* dScMgCard_c::SettleHand -- the round's settle step: stamp all five cards
 * of one hand to state 9. Coined name; the ROM's symbol is address-only. */
void dScMgCard_c::SettleHand(dMgCardObj_c *hand) {
    int i;
    for (i = 0; i < 5; i++) {
        hand->mState = 9;
        hand++;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN12dMgCardObj_c6DealInEPKi, 0x020da0ac, size 0xa8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dMgCardObj_c6DealInEPKi
/* dMgCardObj_c::DealIn -- deal one fresh card in: park it at the {x, y} the
 * source pair names, then retarget the approach steps off GetOffscreenY's
 * start height through the Vec2_Sub/func_0203d680 transform pair (the same
 * 1:256 fixed-point rounding both hands use), reset the raise state, and
 * draw its face. The mid-body alias reads the just-stored {x, y} back out
 * through memory into the target pair -- spelled exactly so, because a
 * plain `mTargetX = mX` would let the registers be reused and the reloads
 * would vanish. Coined name; the ROM's symbol is address-only. */
void dMgCardObj_c::DealIn(const s32 *src)
{
    int buf[4];
    mX = src[0];
    mY = src[1];
    {
        int *b = (int *)(((int)this + 4));
        mTargetX = b[0];
        mTargetY = b[1];
    }
    mY = GetOffscreenY();
    mX = 0x80000;
    Vec2_Sub(&buf[0], &mTargetX, &mX);
    func_0203d680(&buf[2], &buf[0], 0x100);
    mXStep = buf[2];
    mYStep = buf[3];
    if (mXStep < 0) mXStep = -mXStep;
    mLift = 0;
    mLiftFlag = 0;
    mValue = (unsigned char)dScMgCard_c::DrawCardValue();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- _ZN12dMgCardObj_c6RedealEv, 0x020da00c, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dMgCardObj_c6RedealEv
/* dMgCardObj_c::Redeal -- re-fly the card from its own target: the same
 * retarget off GetOffscreenY's start height as DealIn, but from the target
 * pair already on the card, and a wider reset -- the state, the raise
 * amount and its flag, the fly-away stagger, the combo marker -- before
 * drawing a fresh face. Coined name; the ROM's symbol is address-only. */
void dMgCardObj_c::Redeal()
{
    int buf[4];
    mX = mTargetX;
    mY = mTargetY;
    mY = GetOffscreenY();
    mX = 0x80000;
    Vec2_Sub(&buf[0], &mTargetX, &mX);
    func_0203d680(&buf[2], &buf[0], 0x100);
    mXStep = buf[2];
    mYStep = buf[3];
    if (mXStep < 0) mXStep = -mXStep;
    mState = 0;
    mLift = 0;
    mLiftFlag = 0;
    mFlyDelay = 0;
    mComboFlag = 0;
    mValue = (unsigned char)dScMgCard_c::DrawCardValue();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- _ZN12dMgCardObj_c6UpdateEv, 0x020d9c5c, size 0x3b0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dMgCardObj_c6UpdateEv
/* dMgCardObj_c::Update -- vtable slot 1, the player's card motion state
 * machine. mState: 1 fly in from off-screen and settle (2, lift raising, on
 * both axes arriving -- the one card sound); 3 retreat a row (the touched
 * card dodging); 4 deal a fresh card at the deck position (6) after asking
 * GetOffscreenY for the start height; 7 fly home to the mHomeX slot (8);
 * 9 settle into the dealt row; 0xa fly off home after the mFlyDelay stagger.
 * The lift runs every frame below the state block (raised toward 0x4000 =
 * one flip frame per 0x1000 of mLift when mLiftFlag is set). The tail is
 * the touch hit-test: the engine globals gate it, then the card must be
 * mid-lift (state 2 or 3, via the st + 254 (0xfe) wrap trick), fully raised (mLift
 * 0x4000), and inside the touch's on-screen window, with 0x14f picking the
 * card up (state 3) and 0x150 putting it back (state 2). */
void dMgCardObj_c::Update()
{
    int r5;

    if (mState == 0)
        return;

    if (mState == 1) {
        if (mFlyDelay != 0) {
            ApproachLinear2(mFlyDelay, 0, 1);
            return;
        }
        r5 = ApproachLinear(mY, mTargetY, mYStep);
        if (ApproachLinear(mX, mTargetX, mXStep) != 0 && r5 != 0) {
            Sound::PlayBank2_2D(0x144);
            mState = 2;
            mLiftFlag = 1;
        }
    }

    if (mLiftFlag != 0)
        ApproachLinear(mLift, 0x4000, 0x200);
    else
        ApproachLinear(mLift, 0, 0x200);

    {
        int st = mState;
        if (st == 3) {
            ApproachLinear(mY, mTargetY - 0x10000, 0x2000);
        } else if (st == 5) {
            r5 = ApproachLinear(mY, mTargetY, mYStep);
            if (ApproachLinear(mX, mTargetX, mXStep) != 0 && r5 != 0) {
                Sound::PlayBank2_2D(0x144);
                mState = 2;
                mLiftFlag = 1;
            }
        } else if (st == 2) {
            ApproachLinear(mY, mTargetY, 0x4000);
        } else if (st == 4 && mLift == 0) {
            int result = GetOffscreenY();
            if (ApproachLinear(mY, result, 0x8000) != 0) {
                mX = 0x80000;
                mState = 6;
                mValue = (unsigned char)dScMgCard_c::DrawCardValue();
            }
        } else if (st == 7) {
            if (ApproachLinear(mX, mHomeX, 0x6000) != 0)
                mState = 8;
        } else if (st == 9) {
            ApproachLinear(mY, 0x80000, 0x6000);
        } else if (st == 0xa) {
            if (ApproachLinear2(mFlyDelay, 0, 1) != 0)
                ApproachLinear(mX, mHomeX, mXStep);
        }
    }

    if (data_ov006_0214176c != 0) {
        if (mState == 3) {
            mState = 4;
            mLiftFlag = 0;
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
            unsigned char st = mState;
            if ((unsigned char)(st + 254) > 1)
                return;
            if (mLift != 0x4000)
                return;
            {
                int a = (int)data_020a0dea[idx][0] - (mX >> 12);
                int b = (int)data_020a0deb[idx][0] - (mY >> 12);
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
                    mState = 3;
                } else {
                    func_02012790(0x150);
                    mState = 2;
                }
            }
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- _ZN12dMgCardObj_c6RenderEv, 0x020d9bdc, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dMgCardObj_c6RenderEv
/* dMgCardObj_c::Render -- vtable slot 0, the player's card. Same table and
 * same index math as the dealer's Render above; the temporaries are spelled
 * out (idx, then e) where the dealer's stay one, because the two bodies'
 * register allocations differ and each spelling is the one its own bytes
 * need. */
void dMgCardObj_c::Render()
{
    int idx;
    unsigned short e;
    if (mState == 0) return;
    idx = (mValue + 1) * 5 + (mLift >> 12);
    e = data_ov006_0213bd64[idx];
    Hud_RenderSprite(
        data_ov006_02141774[e],
        (mX >> 12) + 24,
        (mY >> 12) + 24,
        -1, -1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- _ZN12dMgCardObj_c13GetOffscreenYEv, 0x020d9bd0, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dMgCardObj_c13GetOffscreenYEv
/* dMgCardObj_c::GetOffscreenY -- vtable slot 2. The y the card starts flying
 * in from: 6 screens above the table in 1:4096 fixed point. DealIn reads it
 * through the virtual slot, so the dealer's override carries the same value. */
s32 dMgCardObj_c::GetOffscreenY()
{
    return -0x60000;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN11dScMgCard_c9RenderHudEv, 0x020d9a14, size 0x1bc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgCard_c9RenderHudEv
/* dScMgCard_c::RenderHud -- the HUD column and the hold prompt. States 4
 * through 0x10 stack six face sprites at x=0x14 from y=0xb0 down; the two
 * hands' highlight pairs (unk_538e/unk_5390 for the player's hand,
 * unk_5392/unk_5394 for the dealer's) name the rows drawn in a highlight
 * variant -- 1 for the player's values, 2 for the dealer's, and when both
 * hands name the same row the two variants alternate on mFrameCounter bit
 * 4. State 4, once the whole player hand is lifted, adds the prompt
 * sprite at (0x80, 0x58), its variant picking whether every card has
 * reached state 2. Coined name; the ROM's symbol is address-only. */
void dScMgCard_c::RenderHud()
{
    short v = mState;
    if (v > 3 && v < 0x11) {
        int sb = 0xb0;
        int i;
        for (i = 0; i < 6; i++) {
            int sel = 0;
            if ((i == unk_5392 && i == unk_538e) || (i == unk_5394 && i == unk_5390)) {
                if (mFrameCounter & 0x10) sel = 1; else sel = 2;
            } else if (i == unk_5392 || i == unk_5394) {
                if (mFrameCounter & 0x10) sel = 2;
            } else if (i == unk_538e || i == unk_5390) {
                if (mFrameCounter & 0x10) sel = 1;
            }
            Hud_RenderSprite(data_ov006_0213bd30[i][sel], 0x14, sb, -1, -1);
            sb -= 0x10;
        }
    }
    if (mState != 4) return;
    if (dScMgCard_c::AllLifted((const dMgCardObj_c *)mArray1) == 0) return;
    if (dScMgCard_c::AllInState((const dMgCardObj_c *)mArray1, 2) == 0)
        Hud_RenderSprite(data_ov006_0213406c[0], 0x80, 0x58, -1, -1);
    else
        Hud_RenderSprite(data_ov006_0213406c[1], 0x80, 0x58, -1, -1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN12dMgCardObj_c7SetHomeEi, 0x020d99ec, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dMgCardObj_c7SetHomeEi
/* dMgCardObj_c::SetHome -- aim the card at home slot n (the row pitch is
 * 0x28, the first slot sits at 0x2c) and put it into state 7, the fly-home
 * step. Coined name; the ROM's symbol is address-only. */
void dMgCardObj_c::SetHome(s32 n){
  mHomeX = (n*0x28+0x2c)<<0xc;
  mHomeY = 0x80000;
  mState = 7;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN12dMgCardObj_c6FlyOffEv, 0x020d99a4, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dMgCardObj_c6FlyOffEv
/* dMgCardObj_c::FlyOff -- the end-of-round fly-away: home becomes the
 * off-screen left, the y step tracks the card's own height, and the
 * x step's frame count becomes the stagger -- one card leaves per frame,
 * counted down from how high the card sits. State 0xa is the fly-away
 * step. Coined name; the ROM's symbol is address-only. */
void dMgCardObj_c::FlyOff(){
  mHomeX = -0x28000;
  mHomeY = mY;
  int t = mX;
  unsigned ip = 0x80000 - t;
  mFlyDelay = (s16)((int)ip / 0x10000);
  mXStep = 0x10000;
  mYStep = 0;
  mState = 0xa;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN17dMgDilarCardObj_c13GetOffscreenYEv, 0x020d9998, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17dMgDilarCardObj_c13GetOffscreenYEv
/* dMgDilarCardObj_c::GetOffscreenY -- vtable slot 2, the same answer the base
 * gives: both hands' cards fly in from 6 screens above the table (-0x60000
 * in 1:4096 fixed point), and the override exists so the dealer's class
 * carries the full set of its own slots. */
s32 dMgDilarCardObj_c::GetOffscreenY()
{
    return -0x60000;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN17dMgDilarCardObj_c6UpdateEv, 0x020d978c, size 0x20c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17dMgDilarCardObj_c6UpdateEv
/* dMgDilarCardObj_c::Update -- vtable slot 1, the dealer's card motion.
 * Same states as the player's card (see dMgCardObj_c::Update below) minus
 * the touch handling -- the dealer's card is never touched -- with state 9
 * approaching the row the card was dealt into (0x10000, the dealer's row,
 * where the base flies its player's cards to 0x80000) and the early-return
 * chain kept in exactly this order for the register allocation. */
void dMgDilarCardObj_c::Update()
{
    if (mState == 0)
        return;

    if (mLiftFlag != 0)
        ApproachLinear(mLift, 0x4000, 0x200);
    else
        ApproachLinear(mLift, 0, 0x200);

    u8 state = mState;

    if (state == 5) {
        int r5 = ApproachLinear(mY, mTargetY, mYStep);
        int r0 = ApproachLinear(mX, mTargetX, mXStep);
        if (r5 == 0)
            return;
        if (r0 != 0)
            mState = 1;
        return;
    }

    if (state == 4 && mLift == 0) {
        int v = GetOffscreenY();
        int r0 = ApproachLinear(mY, v, 0x8000);
        if (r0 == 0)
            return;
        mX = 0x80000;
        mState = 6;
        mValue = dScMgCard_c::DrawCardValue();
        return;
    }

    if (state == 7) {
        int r0 = ApproachLinear(mX, mHomeX, 0x6000);
        if (r0 != 0)
            mState = 8;
        return;
    }

    if (state == 9) {
        ApproachLinear(mY, 0x10000, 0x6000);
        return;
    }

    if (state == 1) {
        if (mFlyDelay != 0) {
            ApproachLinear2(mFlyDelay, 0, 1);
            return;
        }
        ApproachLinear(mY, mTargetY, mYStep);
        ApproachLinear(mX, mTargetX, mXStep);
        return;
    }

    if (state != 0xa)
        return;

    if (ApproachLinear2(mFlyDelay, 0, 1) == 0)
        return;

    ApproachLinear(mX, mHomeX, mXStep);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN17dMgDilarCardObj_c6RenderEv, 0x020d970c, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17dMgDilarCardObj_c6RenderEv
/* dMgDilarCardObj_c::Render -- vtable slot 0, the dealer's card face down.
 * The sprite bank table data_ov006_0213bd64 is indexed by the card's face
 * (mValue + 1, the +1 skipping the card-back row) and its flip frame
 * (mLift >> 12), and Hud_RenderSprite takes the 1:4096 fixed-point mX/mY
 * back down to pixels, plus the 24-pixel hand margin. Kept compact -- one index
 * temporary -- because that is the register shape the cartridge carries;
 * the base class's Render below spells its temporaries out instead, and
 * the two spellings are not interchangeable. */
void dMgDilarCardObj_c::Render()
{
  int k;
  if(mState==0) return;
  k=data_ov006_0213bd64[(mValue+1)*5+(mLift>>12)];
  Hud_RenderSprite(
    data_ov006_02141774[k],
    (mX>>12)+24,
    (mY>>12)+24,
    -1,-1);
}

/* ROM ordinal 4 -- _ZN17dMgDilarCardObj_cD1Ev, 0x020d96f0, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17dMgDilarCardObj_cD1Ev
extern "C" {  /* the mangled name is the ROM's own symbol, used literally */
/* The dealer's card, destroyed one element at a time through __destroy_arr
 * (the mangled-name extern "C" declaration in the header -- C++ cannot take
 * a destructor's address). HAND-WRITTEN, not a member destructor: this is
 * the destroy-side twin of this hand's hand-written ctor stubs in
 * src/actors/MgPicturePoker.cpp, and no member-destructor spelling
 * reproduces the cartridge pair -- include/dScMgCard_c.h's note above the
 * two class definitions carries the measurement. Two vptr stores and no
 * bl: the derived class's own address point 0x0213bcf4 as the routine
 * enters the class, then the base class's 0x0213bccc beneath it, exactly
 * as a destructor walks down the chain. */
void _ZN17dMgDilarCardObj_cD1Ev(void *elem)
{
  *(int *)elem = (int)_ZTV17dMgDilarCardObj_c;
  *(int *)elem = (int)_ZTV12dMgCardObj_c;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN12dMgCardObj_cD1Ev, 0x020d96e0, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dMgCardObj_cD1Ev
extern "C" {  /* the mangled name is the ROM's own symbol, used literally */
/* The player's card, same reach, same idiom: one vptr store -- this class's
 * own address point 0x0213bccc -- and nothing else. */
void _ZN12dMgCardObj_cD1Ev(void *elem)
{
  *(int *)elem = (int)_ZTV12dMgCardObj_c;
}
}

/* The destructor is DEFINED INLINE in include/dScMgCard_c.h. mwcc emits the
 * D1/D0 pair at ROM ordinals 1 and 2 (0x020d95a4 size 0x94, 0x020d9638 size
 * 0xa8) from the vtable slots that name them, in cartridge order -- written
 * out of line here it emitted D0 ahead of D1 and rombuild's fail-closed
 * isolate refused the TU. The header's own banner carries the full reason. */
