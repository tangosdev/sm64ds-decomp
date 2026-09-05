//cpp
/* dScMgD3DBase_c -- the shared base of the four 3D minigame scenes
 * (dScMgJump_c, dScMgJump2_c, dScMgTrampoline_c, dScMgTrampoline2_c).
 *
 * ov006, .text 0x020e6c28 .. 0x020e7660, 27 functions, reconstructed from 27
 * one-function legacy sources by tools/tubuild.py create and reconciled by
 * hand.  See include/dScMgD3DBase_c.h for the class's own evidence trail --
 * where its fields end, why its destructor is inline, and why the coined name
 * "MgBounceAndPounce" was wrong about the level of the hierarchy and not just
 * the spelling.
 *
 * Seventeen of the 27 are this class's own members.  The other ten are the
 * free helpers that shared the translation unit with them: the four VRAM
 * bank-swap routines the class's own overrides call (0x020e73c4, 0x020e740c,
 * 0x020e7508, 0x020e759c), the sub-screen OAM table builder (0x020e7428), the
 * BG1-enable reset (0x020e7110), and the four sound shims
 * (0x020e6da4, 0x020e6db4, Sound_PlayBank1Panned, 0x020e6e3c).
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here.  Do not reorder.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020e6c28  src/_ZN14dScMgD3DBase_cD1Ev.cpp
 *   [1] 0x020e6c60  src/_ZN14dScMgD3DBase_cD0Ev.cpp
 *   [2] 0x020e6cac  src/_ZN14dScMgD3DBase_c25OnAimedAtWithEggReturnVecEv.cpp
 *   [3] 0x020e6d24  src/_ZN14dScMgD3DBase_c16OnAimedAtWithEggEv.cpp
 *   [4] 0x020e6d8c  src/_ZN14dScMgD3DBase_c19OnHitFromUnderneathEv.cpp
 *   [5] 0x020e6d98  src/_ZN14dScMgD3DBase_c15OnHitByMegaCharEv.cpp
 *   [6] 0x020e6da4  src/func_ov006_020e6da4.c
 *   [7] 0x020e6db4  src/func_ov006_020e6db4.c
 *   [8] 0x020e6df0  src/Sound_PlayBank1Panned.cpp
 *   [9] 0x020e6e3c  src/func_ov006_020e6e3c.c
 *   [10] 0x020e6e4c  src/_ZN14dScMgD3DBase_c24OnHitByCannonBlastedCharEv.cpp
 *   [11] 0x020e6e54  src/_ZN14dScMgD3DBase_c8OnPushedEv.cpp
 *   [12] 0x020e6e78  src/_ZN14dScMgD3DBase_c8OnKickedEv.cpp
 *   [13] 0x020e6f60  src/_ZN14dScMgD3DBase_c21AfterCleanupResourcesEj.cpp
 *   [14] 0x020e700c  src/_ZN14dScMgD3DBase_c11AfterRenderEj.cpp
 *   [15] 0x020e7040  src/_ZN14dScMgD3DBase_c12BeforeRenderEv.cpp
 *   [16] 0x020e7074  src/_ZN14dScMgD3DBase_c14BeforeBehaviorEv.cpp
 *   [17] 0x020e70c0  src/_ZN14dScMgD3DBase_c18AfterInitResourcesEj.cpp
 *   [18] 0x020e70e4  src/_ZN14dScMgD3DBase_c19BeforeInitResourcesEv.cpp
 *   [19] 0x020e7110  src/func_ov006_020e7110.c
 *   [20] 0x020e7124  src/_ZN14dScMgD3DBase_c9Virtual84Ev.cpp
 *   [21] 0x020e72c0  src/_ZN14dScMgD3DBase_c9Virtual7CEv.cpp
 *   [22] 0x020e73c4  src/func_ov006_020e73c4.cpp
 *   [23] 0x020e740c  src/func_ov006_020e740c.c
 *   [24] 0x020e7428  src/func_ov006_020e7428.c
 *   [25] 0x020e7508  src/func_ov006_020e7508.cpp
 *   [26] 0x020e759c  src/func_ov006_020e759c.c
 */

#include "dScMgD3DBase_c.h"
#include "decl_common.h"
#include "decl_Particle.h"
#include "types.h"

/* The sub-screen OAM entry the 0x020e7428 table builder writes.  Eight bytes,
   and only this TU spells it; there is no OAM.h in the tree yet (see
   include/G2x.h's note), so the shadow definition the legacy .c carried is
   kept rather than inventing one header for one caller. */
typedef struct Oam {
    u32 attr01;
    u16 attr2;
    u16 aff;
} Oam;

/* RECONCILED shadow declarations.  The generator carried 60-odd of these over
 * verbatim from the 27 legacy files and flagged seven textual conflicts; the
 * block below is the reconciled union.  Everything a real header already
 * declares is gone -- decl_common.h supplies data_ov006_0213c5e8/5fc/610,
 * data_ov006_02141a40/44/4c, data_ov004_020beb74, data_0209d464,
 * CleanCommonModelDataArr, InitialiseVramGlobals, SetSubBg1Offset,
 * func_02012718, GX::SetBankForTex/SetBankForTexPltt/DisableAllBanks/
 * SetGraphicsMode and GXS::SetGraphicsMode; decl_Particle.h supplies
 * Particle::RenderAll and Particle::SysTracker::Initialise.  What remains has
 * no header to lose to.
 *
 * SEVEN CONFLICTS THE GENERATOR FLAGGED, and how each was settled.  Six are
 * spelling only -- u32 vs unsigned int, `const void *` vs `void const *`, a
 * named vs an unnamed parameter -- and the seventh, data_ov006_02141a48,
 * disagreed on `void *` vs `void *[]`.  It is a single pointer word: the two
 * spellings compile to the same load, and the array form's one call site
 * (OnAimedAtWithEgg's `data_ov006_02141a48[0]`) becomes the plain name.
 *
 * FIVE FURTHER CONFLICTS THE GENERATOR DID NOT SEE, because its detector
 * compares declaration text and not parameter types (they sat in per-function
 * `extern "C"` blocks it copied verbatim, so they would have reached the
 * compiler as hard errors):
 *
 *   GX::SetBankForSubBG / GX::SetBankForSubOBJ -- `unsigned short` in the four
 *     bank-swap helpers, `unsigned int` in Virtual7C, OnAimedAtWithEgg and
 *     OnAimedAtWithEggReturnVec.  Unified on `unsigned int`, which is the form
 *     every call site that passes a VARIABLE was compiled against; the
 *     `unsigned short` sites all pass small constants, where the two forms
 *     emit the same immediate.  (The mangled `Et` says the real parameter is
 *     u16 -- these are extern "C" spellings of the ROM symbol, so the C++
 *     signature only steers argument setup, not the name.)
 *   func_ov006_020e6da4 -- declared `int(void*)` by Sound_PlayBank1Panned and
 *     defined `int(int)` by its own legacy file.  Unified on the definition's
 *     `int`, with the one call site casting.
 *   func_ov006_020e7110 -- defined `void(void)` by its own legacy file but
 *     called WITH `this` by AfterCleanupResources.  The ROM settles it: the
 *     retail AfterCleanupResources has `mov r0, r5` at 0x020e6fa0 immediately
 *     before `bl 0x020e7110`, so the argument is real and the definition takes
 *     it.  The body ignores it and reloads r0 with a literal-pool address, so
 *     the callee's four instructions are unchanged.
 *   DecompressLZ16 -- `void *src` in Virtual84's file, `const void *src` in
 *     Virtual7C's.  Kept the const form; nothing in the ROM distinguishes
 *     them and no caller here writes through src.
 *   data_0209e660 -- `unsigned char` scalar in five files, `char[]` in
 *     0x020e7110's.  Kept the scalar; `data_0209e660[0] = 0` becomes
 *     `data_0209e660 = 0`, the same strb.
 */
extern "C" {

/* ov006's own data. */
extern void *data_ov006_02141a48;
extern void *data_ov006_02141a50;
extern Oam   data_ov006_02141a54[];
extern unsigned char data_ov006_0212e574[];

/* arm9 / ov004 globals with no project header. */
extern unsigned char data_0209d454;
extern unsigned char data_0209d45c;
extern unsigned char data_0209e660;
extern unsigned char data_0209f5f8;
extern void *data_0209d4a8;
extern unsigned int data_020a0db0;
extern int data_0208ee44;

/* arm9 / ov004 helpers with no project header. */
int   func_02053ea0(void);
int   func_02053eb0(void);
void  func_02054140(void);
void  func_02054154(void);
void  func_02054430(int);
void  func_02054748(int);
void  func_02056674(const void *src, unsigned int offset, unsigned int count);
int   func_020126e8(int a);
void  func_020126ac(int a0, int a1, int a2, int a3, int s0);
void  func_020127ec(int a0, int a1, int a2, int a3, int a4, int a5);
void *func_ov004_020adc68(int id);
void  func_ov004_020b0d30(void);
void  func_ov004_020b290c(void);
void  func_ov004_020b2980(void);
int   GetGameLanguage(void);
void  DecompressLZ16(const void *src, void *dst);
u32   LoadCompressedFileAt(unsigned int fileID, void *target);
void  Ov004_Deallocate(void *x);
void  Camera_UpdateMatrices(int arg);

/* GX / GXS / G2S / CP15 entry points.  Spelled as their raw mangled names the
   way the legacy sources did -- writing them as real `GX::SetBankForBG(2)`
   calls would need a GX.h this tree does not have, and inventing one is a
   separate change with its own byte risk.  Known debt, same as
   src/minigames/d_s_mg_single3_d_base.cpp's. */
void  _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void  _ZN2GX12SetBankForBGEt(u16 a);
void  _ZN2GX13SetBankForOBJEt(u16 a);
void  _ZN2GX15SetBankForSubBGEt(unsigned int x);
void  _ZN2GX16SetBankForSubOBJEt(unsigned int x);
unsigned int _ZN3G2S13GetBG1CharPtrEv(void);
void *_ZN3G2S12GetBG1ScrPtrEv(void);
void  _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void  _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *p, u32 len);

/* Base-class bodies this class's overrides chain to.  decl_common.h already
   carries dScMgBase_c's BeforeInitResources, AfterInitResources,
   BeforeBehavior, OnPushed, OnAimedAtWithEgg and OnAimedAtWithEggReturnVec;
   these six do not appear in any header yet.  The last is the one
   Particle::SysTracker entry point decl_Particle.h does not carry. */
int  _ZN11dScMgBase_c12BeforeRenderEv(void *self);
int  _ZN11dScMgBase_c8OnKickedEv(void *self);
void _ZN11dScMgBase_c15OnHitByMegaCharEv(void);
void _ZN11dScMgBase_c19OnHitFromUnderneathEv(void);
void _ZN11dScMgBase_c21AfterCleanupResourcesEj(void *a, int b);
void _ZN8dScene_c11AfterRenderEj(void *self, unsigned int result);
int  _ZN8Particle10SysTracker6UpdateEv(void *self);

/* This TU's own free helpers, forward-declared because they are called from
   members written above their definitions. */
void func_ov006_020e73c4(void);
void func_ov006_020e740c(void);
void func_ov006_020e7428(void);
void func_ov006_020e7508(void);
void func_ov006_020e759c(void);
void func_ov006_020e7110(void *self);
int  func_ov006_020e6da4(int a);

}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov006_020e759c, 0x020e759c, size 0xc4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e759c
extern "C" void func_ov006_020e759c(void) {
    *(volatile unsigned short *)0x4000304 |= 0x8000;
    func_02054140();
    _ZN2GX15SetBankForSubBGEt(4);
    func_02054430(8);
    *(volatile unsigned int *)0x4000064 = 0x80330010;
    data_0209d454 &= ~0x10;
    data_0209d454 |= 4;
    *(volatile unsigned int *)0x4001000 =
        (*(volatile unsigned int *)0x4001000 & ~0x1f00) | (data_0209d454 << 8);
    *(volatile unsigned short *)0x400100c =
        (*(volatile unsigned short *)0x400100c & 0x43) | 0x4284;
    *(volatile unsigned short *)0x400100c &= ~3;
    *(volatile unsigned short *)0x400100c &= ~0x40;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov006_020e7508, 0x020e7508, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e7508
extern "C" void func_ov006_020e7508(void)
{
    *(volatile unsigned short *)0x4000304 &= ~0x8000;
    func_02054154();
    _ZN2GX16SetBankForSubOBJEt(8);
    func_02054430(4);
    *(volatile int *)0x4000064 = 0x80360010;
    _ZN3GXS15SetGraphicsModeEi(5);
    data_0209d454 |= 0x10;
    data_0209d454 &= ~4;
    *(volatile int *)0x4001000 = (*(volatile int *)0x4001000 & ~0x1f00) | (data_0209d454 << 8);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov006_020e7428, 0x020e7428, size 0xe0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e7428
extern "C" void func_ov006_020e7428(void)
{
    int i;
    int x;
    int y;
    int k;
    int ty;
    int tx;
    u32 reg;

    reg = *(volatile u32 *)0x4001000;
    *(volatile u32 *)0x4001000 = (reg & 0xffbfff9f) | 0x20;

    k = 0;
    for (i = 0; i < 0x80; i++) {
        data_ov006_02141a54[i].attr01 = 0;
        *(u32 *)&data_ov006_02141a54[(int)(((long long)i))].attr2 = 0;
    }

    y = 0;
    ty = 0;
    for (; y < 0xc0; y += 0x40) {
        x = 0;
        tx = 0;
        for (; x < 0x100; x += 0x40) {
            data_ov006_02141a54[k].attr01 = ((y & 0xff) | 0xc0000c00) | ((x & 0x1ff) << 16);
            data_ov006_02141a54[k].attr2 = (u16)(((int)(((long long)tx)) + ((int)(((long long)ty)) << 5)) | 0xf000);
            tx += 8;
            k++;
        }
        ty += 8;
    }

    _ZN4CP1527FlushAndInvalidateDataCacheEjj(data_ov006_02141a54, 0x400);
    func_02056674(data_ov006_02141a54, 0, 0x400);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov006_020e740c, 0x020e740c, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e740c
extern "C" void func_ov006_020e740c(void)
{
    func_02056674(data_ov006_02141a54, 0, 0x400u);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov006_020e73c4, 0x020e73c4, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e73c4
extern "C" void func_ov006_020e73c4(void)
{
    func_02054154();
    func_02054140();
    _ZN2GX16SetBankForSubOBJEt(8);
    _ZN2GX15SetBankForSubBGEt(4);
    *(unsigned int *)0x4001000 &= ~0x300010;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN14dScMgD3DBase_c9Virtual7CEv, 0x020e72c0, size 0x104 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgD3DBase_c9Virtual7CEv
// recovered name: dScMgTrampoline2_c_Kill  -- WRONG twice, see below
/* dScMgD3DBase_c::Virtual7C - slot 31.

   The `recovered name:` line above is wrong in both halves, and both are worth
   reading.  The CLASS is wrong because this address backs FIVE vtables --
   _ZTV14dScMgD3DBase_c and the four children that inherit it, dScMgJump_c,
   dScMgJump2_c, dScMgTrampoline_c and dScMgTrampoline2_c -- so it belongs to
   the base of that group, not to the last child that happened to name it.
   That is the eighth such misattribution on this class; it also included
   dScMgTrampoline2_c.h and cast `this` to that type, both fixed here.  The
   METHOD is wrong because dScMgBase_c's slot 31 has no ROM name to override:
   `Kill` came from dBgActor_c, a different branch of the hierarchy that shares
   only fBase_c's first eighteen slots.  See include/dScMgBase_c.h's slot-31
   block.

   It is a real body, not a veneer.  It does the base's BG1 setup -- BG1CNT
   reduced to 0x800, scroll reset, BG1 cleared from the sub BG-enable shadow, a
   language-indexed character file and the shared screen map installed -- and
   wraps it in the VRAM bank juggling only the 3D minigames need, saving each
   bank's prior owner in unk_4660 / unk_0a0 and handing it back afterwards. */
int dScMgD3DBase_c::Virtual7C()
{
    char *c = (char *)this;

    struct dScMgD3DBase_c *self = (struct dScMgD3DBase_c *)(void *)c;
  self->unk_4660 = func_02053ea0();
  _ZN2GX16SetBankForSubOBJEt(0x100);
  LoadCompressedFileAt(data_ov006_0213c5fc[GetGameLanguage()], (void*)0x6600000);
  {
    char *dst = (char*)0x6600000; dst += 0x2000;
    DecompressLZ16((void*)data_ov006_0213c5e8[GetGameLanguage()], dst);
  }
  _ZN2GX16SetBankForSubOBJEt(self->unk_4660);
  self->unk_0a0 = func_02053eb0();
  _ZN2GX15SetBankForSubBGEt(0x80);
  {
    volatile u16 *p = (volatile u16*)0x400100a;
    *p = (*p & 0x43) | 0x800;
    *p = *p & ~3;
    *p = *p & ~0x40;
  }
  SetSubBg1Offset(0, 0);
  data_0209d454 &= ~2;
  {
    s32 i = GetGameLanguage();
    LoadCompressedFileAt(data_ov006_0213c610[i], (void*)_ZN3G2S13GetBG1CharPtrEv());
  }
  LoadCompressedFileAt(0x5b, _ZN3G2S12GetBG1ScrPtrEv());
  _ZN2GX15SetBankForSubBGEt(self->unk_0a0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN14dScMgD3DBase_c9Virtual84Ev, 0x020e7124, size 0x19c */
/* -------------------------------------------------------------------------- */
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
   slot -- same job again, and it lives inside that class's promoted TU,
   src/minigames/d_s_mg_single3_d_base.cpp. */
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
    data_ov006_02141a44 = (int)(obj + 0x466c);
    data_ov006_02141a40 = obj + 0x466c;
    data_ov006_02141a50 = obj + 0x4728;
    data_ov004_020beb74[1] = (int)obj;
    data_0209d4a8 = (void *)data_ov004_020beb74;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov006_020e7110, 0x020e7110, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e7110
/* Clears the 3D minigames' "sub BG1 owned" flag.  Takes the scene pointer and
   ignores it -- see the reconciliation note at the top of the file; the ROM's
   AfterCleanupResources passes it and this body reloads r0 from the literal
   pool, so the four instructions are the same either way. */
extern "C" void func_ov006_020e7110(void *) { data_0209e660 = 0; }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN14dScMgD3DBase_c19BeforeInitResourcesEv, 0x020e70e4, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgD3DBase_c19BeforeInitResourcesEv
/* THIS IS THE TU'S KEY FUNCTION -- the first DECLARED non-inline virtual of
   dScMgD3DBase_c (the destructor is declared before it but is defined inline
   in the class body, so it cannot be the key function).  Defining it here is
   what makes mwcc emit _ZTV14dScMgD3DBase_c, the class's _ZTI/_ZTS and its
   four ancestors', and -- through vtable slots 16 and 17 -- the out-of-line
   D1/D0 pair at the bottom of the ROM range.  See the closing comment. */
bool dScMgD3DBase_c::BeforeInitResources()
{
  if(_ZN11dScMgBase_c19BeforeInitResourcesEv(((void*)this))==0) return 0;
  unk_5000 = 0;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN14dScMgD3DBase_c18AfterInitResourcesEj, 0x020e70c0, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgD3DBase_c18AfterInitResourcesEj
void dScMgD3DBase_c::AfterInitResources(unsigned int)
{
    _ZN11dScMgBase_c18AfterInitResourcesEj(this);
    _ZN8Particle10SysTracker10InitialiseEv(&mSysTracker);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN14dScMgD3DBase_c14BeforeBehaviorEv, 0x020e7074, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgD3DBase_c14BeforeBehaviorEv
int dScMgD3DBase_c::BeforeBehavior()
{
  if(_ZN11dScMgBase_c14BeforeBehaviorEv(((void*)this))==0) return 0;
  if(data_020a0db0 & 1)
    _ZN8Particle10SysTracker6UpdateEv((char*)&mSysTracker);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN14dScMgD3DBase_c12BeforeRenderEv, 0x020e7040, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgD3DBase_c12BeforeRenderEv
int dScMgD3DBase_c::BeforeRender()
{
    if (_ZN11dScMgBase_c12BeforeRenderEv(this) == 0) {
        return 0;
    }

    _ZN8Particle9RenderAllEv();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- _ZN14dScMgD3DBase_c11AfterRenderEj, 0x020e700c, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgD3DBase_c11AfterRenderEj
void dScMgD3DBase_c::AfterRender(unsigned int arg)
{
    volatile unsigned short *reg = (volatile unsigned short *)0x04000006;
    int v = *reg;

    if (v > 0xb9 && v <= 0xc0) {
        while ((int)*reg < 0xc0) {
        }
    }

    _ZN8dScene_c11AfterRenderEj(this, arg);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- _ZN14dScMgD3DBase_c21AfterCleanupResourcesEj, 0x020e6f60, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgD3DBase_c21AfterCleanupResourcesEj
void dScMgD3DBase_c::AfterCleanupResources(unsigned int b_)
{
    int b = (int)b_;

    if (b == 2) {
        CleanCommonModelDataArr();
        *(int*)0x40004c8 = 0x296a5800;
        *(int*)0x40004cc = 0x7fff;
        *(int*)0x40004c8 = 0x696a5800;
        *(int*)0x40004cc = 0x40007fff;
        func_ov006_020e7110(((void*)this));
        Ov004_Deallocate(data_ov006_02141a4c);
        Ov004_Deallocate(data_ov006_02141a48);
    }
    data_0209f5f8 = 0;
    _ZN11dScMgBase_c21AfterCleanupResourcesEj(((void*)this), b);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- _ZN14dScMgD3DBase_c8OnKickedEv, 0x020e6e78, size 0xe8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgD3DBase_c8OnKickedEv
// recovered name: dScMgJump2_c_OnKicked  -- WRONG, see below
/* dScMgD3DBase_c::OnKicked - slot 24.
 *
 * THE RECOVERED NAME ABOVE NAMES THE WRONG CLASS, and it is kept only so
 * the correction is visible.  This body is at ov006:0x020e6e78, and word
 * 24 of THREE vtables points here: _ZTV14dScMgD3DBase_c,
 * _ZTV11dScMgJump_c and _ZTV12dScMgJump2_c.  A body that appears in a
 * class's table AND in both its children's is supplied by that class, so
 * it is dScMgD3DBase_c::OnKicked and the two children inherit it.  The
 * recovered name was assigned from one child's table without looking at
 * the other two.
 *
 * The chain below is the same argument from the bodies' own calls: this
 * one calls dScMgBase_c's 0x020ae140, and dScMgTrampoline_c and
 * dScMgTrampoline2_c -- the other two children -- call THIS one. */
int dScMgD3DBase_c::OnKicked()
{
    char *self = (char *)this;

    if (_ZN11dScMgBase_c8OnKickedEv(self) == 0) return 0;
    if (*(int*)(self + 0x4628) == 0) {
        if (data_0209d464 == 0) return 0;
        if (*(unsigned short*)(self + 0x4664) == 0)
            *(unsigned short*)(self + 0x4664) = 1;
        else
            *(unsigned short*)(self + 0x4664) = 0;
        int v = (int)(self + 0x466c + (*(unsigned short*)(self + 0x4664)) * 0xbc);
        data_ov006_02141a44 = v;
        Camera_UpdateMatrices(v);
        if (*(unsigned short*)(self + 0x4664) == 1) {
            func_ov006_020e7508();
        } else {
            func_ov006_020e759c();
            if (data_0209f5f8 == 0) {
                *(int*)0x4001000 &= ~0xe000;
                data_0209f5f8 = 1;
            }
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- _ZN14dScMgD3DBase_c8OnPushedEv, 0x020e6e54, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgD3DBase_c8OnPushedEv
// recovered name: dScMgJump2_c_OnPushed  -- WRONG, see below
/* dScMgD3DBase_c::OnPushed - slot 25.
 *
 * THE RECOVERED NAME ABOVE NAMES THE WRONG CLASS, and it is kept only so
 * the correction is visible -- the second time on this class, after
 * 0x020e6e78 at slot 24.  This body is at ov006:0x020e6e54, and word 25
 * of THREE vtables points here: _ZTV14dScMgD3DBase_c, _ZTV11dScMgJump_c
 * and _ZTV12dScMgJump2_c.  A body that appears in a class's table AND in
 * both its children's is supplied by that class, so it is
 * dScMgD3DBase_c::OnPushed and the two children inherit it.
 *
 * The bodies say the same thing: this one calls dScMgBase_c's 0x020ae128,
 * and dScMgTrampoline_c and dScMgTrampoline2_c -- the other two children
 * -- call THIS one. */
int dScMgD3DBase_c::OnPushed()
{
    void *t = (void *)this;
 return _ZN11dScMgBase_c8OnPushedEv(t) != 0; }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN14dScMgD3DBase_c24OnHitByCannonBlastedCharEv, 0x020e6e4c, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgD3DBase_c24OnHitByCannonBlastedCharEv
// recovered name: dScMgTrampoline2_c_OnHitByCannonBlastedChar  -- WRONG, see below
/* dScMgD3DBase_c::OnHitByCannonBlastedChar - slot 26.
 *
 * THE RECOVERED NAME ABOVE NAMES THE WRONG CLASS, and it is kept only so the
 * correction is visible -- the third on this class, after 0x020e6e78 at slot
 * 24 and 0x020e6e54 at slot 25.  This body is at ov006:0x020e6e4c, and word
 * 26 of FIVE vtables points here: _ZTV14dScMgD3DBase_c and all four of its
 * children's -- _ZTV11dScMgJump_c, _ZTV12dScMgJump2_c, _ZTV17dScMgTrampoline_c
 * and _ZTV18dScMgTrampoline2_c.  A body that appears in a class's table AND in
 * every child's is supplied by that class, so it is dScMgD3DBase_c's and the
 * four children inherit it.
 *
 * This also disposes of a guess made while reviewing slot 25: the two earlier
 * misattributions both landed on dScMgJump2_c, which looked like a recovery
 * pass reading each shared body's tables in name order and keeping the last.
 * This one names dScMgTrampoline2_c, which that rule does not produce.
 *
 * The body is `return 2;` where dScMgBase_c's is `return 0;` and
 * dScMgSingle3DBase_c's is `return 1;` -- three distinct constants, which is
 * what pins the return type to int rather than void. */
int dScMgD3DBase_c::OnHitByCannonBlastedChar()
{
    return 2;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov006_020e6e3c, 0x020e6e3c, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e6e3c
/* Thunk: func_02012718(a, b + 0x80000).  decl_common.h types func_02012718 as
   returning void, so the forwarded r0 falls out of the tail call rather than
   being spelled as a `return` -- the same instruction either way. */
extern "C" int func_ov006_020e6e3c(int a, int b)
{
    func_02012718(a, b + 0x80000);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- Sound_PlayBank1Panned, 0x020e6df0, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol Sound_PlayBank1Panned
extern "C" void Sound_PlayBank1Panned(int a0, char *a1, void *a2) {
    a1 += data_ov006_0212e574[a0];
    int r = func_ov006_020e6da4((int)a2);
    func_020127ec(1, (int)a1, 4, 0, 0, r);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov006_020e6db4, 0x020e6db4, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e6db4
extern "C" void func_ov006_020e6db4(int a0, int a1, int a2) {
    int s0 = func_020126e8(a1 + 0x80000);
    func_020126ac(a0, 6, 0, a2, s0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov006_020e6da4, 0x020e6da4, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e6da4
/* Thunk: func_020126e8(a + 0x80000). */
extern "C" int func_ov006_020e6da4(int a)
{
    return func_020126e8(a + 0x80000);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN14dScMgD3DBase_c15OnHitByMegaCharEv, 0x020e6d98, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgD3DBase_c15OnHitByMegaCharEv
// recovered name: dScMgTrampoline2_c_OnHitByMegaChar  -- WRONG, see below
/* dScMgD3DBase_c::OnHitByMegaChar - slot 27.
 *
 * THE RECOVERED NAME ABOVE NAMES THE WRONG CLASS, kept so the correction stays
 * visible.  It is the fourth on this class -- after 0x020e6e78 at slot 24,
 * 0x020e6e54 at slot 25 and 0x020e6e4c at slot 26 -- and the fifth in the
 * campaign.  Same argument as those three: word 27 of FIVE vtables points at
 * this body, _ZTV14dScMgD3DBase_c and all four of its children's, and a body
 * that appears in a class's table AND in every child's is supplied by that
 * class.
 *
 * AND THE TWELVE BYTES ARE NOT A LINKER ARTIFACT, which is worth stating here
 * because they look exactly like one: ldr ip, [pc]; bx ip; .word -- a
 * long-branch veneer from ov006 into ov004, and a forwarding override whose
 * one-instruction tail call is all mwldarm has to work with compiles to
 * precisely this.  The tell that it is a real function symbol rather than a
 * stub the linker interposed is in the OTHER twenty-six tables: the classes
 * that do not override slot 27 hold 0x020af27c, dScMgBase_c's body, directly.
 * Vtable words are data, not branches, so nothing in them needs veneering.  A
 * veneer appears here only because a real function here calls across the
 * overlay boundary. */
void dScMgD3DBase_c::OnHitByMegaChar()
{
    _ZN11dScMgBase_c15OnHitByMegaCharEv();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN14dScMgD3DBase_c19OnHitFromUnderneathEv, 0x020e6d8c, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgD3DBase_c19OnHitFromUnderneathEv
// recovered name: dScMgTrampoline2_c_OnHitFromUnderneath  -- WRONG, see below
/* dScMgD3DBase_c::OnHitFromUnderneath - slot 28.
 *
 * THE RECOVERED NAME ABOVE NAMES THE WRONG CLASS, kept so the correction stays
 * visible.  It is the fifth on this class -- after 0x020e6e78 at slot 24,
 * 0x020e6e54 at slot 25, 0x020e6e4c at slot 26 and 0x020e6d98 at slot 27 --
 * and the sixth in the campaign.  Same argument as those four: word 28 of FIVE
 * vtables points at this body, _ZTV14dScMgD3DBase_c and all four of its
 * children's, and a body that appears in a class's table AND in every child's
 * is supplied by that class.
 *
 * AND THE TWELVE BYTES ARE NOT A LINKER ARTIFACT, worth restating because they
 * look exactly like one: ldr ip, [pc]; bx ip; .word 0x020af04c -- a
 * long-branch veneer from ov006 into ov004, and a forwarding override whose
 * one-instruction tail call is all mwldarm has to work with compiles to
 * precisely this.  The tell that it is a real function symbol rather than a
 * stub the linker interposed is in the OTHER twenty-six tables: the classes
 * that do not override slot 28 hold 0x020af04c, dScMgBase_c's body, directly.
 * Vtable words are data, not branches, so nothing in them needs veneering.  A
 * veneer appears here only because a real function here calls across the
 * overlay boundary. */
int dScMgD3DBase_c::OnHitFromUnderneath()
{
    _ZN11dScMgBase_c19OnHitFromUnderneathEv();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN14dScMgD3DBase_c16OnAimedAtWithEggEv, 0x020e6d24, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgD3DBase_c16OnAimedAtWithEggEv
// recovered name: dScMgTrampoline2_c_OnAimedAtWithEgg  -- WRONG, see below
/* dScMgD3DBase_c::OnAimedAtWithEgg - recovered from vtable slot identity.

   THE RECOVERED NAME ABOVE IS WRONG and is left in place so the correction
   is visible rather than silent.  It is the seventh of its kind in this
   campaign and the sixth on this class, all naming a CHILD where the body
   belongs to a shared base.  FIVE vtables hold 0x020e6d24 at slot 29 --
   dScMgD3DBase_c's own and all four of its children's, dScMgJump_c,
   dScMgJump2_c, dScMgTrampoline_c and dScMgTrampoline2_c -- so the body
   cannot be any one child's.  It is this class's.

   Unlike the corrections at slots 26, 27 and 28, this one is not a
   twelve-byte forwarding veneer.  It is a real 0x68-byte body: it loads a
   sub-screen OBJ palette and claims two VRAM banks, and only then calls
   dScMgBase_c's.  So the fix was not the symbol name alone -- the include
   and the type `this` is cast to had to move with it.  Both fields it writes
   survive the change unmoved: unk_0a0 is dScMgBase_c's at +0x0a0, and
   unk_4660 is this class's own first field, the word immediately past
   dScMgBase_c's 0x4660 span. */
int dScMgD3DBase_c::OnAimedAtWithEgg()
{
    char *c = (char *)this;

    struct dScMgD3DBase_c *self = (struct dScMgD3DBase_c *)(void *)c;
    func_ov006_020e73c4();
    _ZN3GXS11LoadOBJPlttEPKvjj(data_ov006_02141a48, 0x100, 0x100);
    data_0209e660 = 0;
    self->unk_0a0 = func_02053eb0();
    _ZN2GX15SetBankForSubBGEt(0x80);
    self->unk_4660 = func_02053ea0();
    _ZN2GX16SetBankForSubOBJEt(0x100);
    _ZN11dScMgBase_c16OnAimedAtWithEggEv(c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN14dScMgD3DBase_c25OnAimedAtWithEggReturnVecEv, 0x020e6cac, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgD3DBase_c25OnAimedAtWithEggReturnVecEv
// recovered name: dScMgTrampoline2_c_OnAimedAtWithEggReturnVec  -- WRONG, see below
/* dScMgD3DBase_c::OnAimedAtWithEggReturnVec - slot 30.

   The `recovered name:` line above is kept visible because it is wrong and the
   correction is worth reading.  This address backs FIVE vtables --
   _ZTV14dScMgD3DBase_c and the four children that inherit it, dScMgJump_c,
   dScMgJump2_c, dScMgTrampoline_c and dScMgTrampoline2_c -- so it belongs to
   the base of that group, not to the last child that happened to name it.
   That is the seventh such misattribution on this class.  It also included
   dScMgTrampoline2_c.h and cast `this` to that type; both are fixed here.

   It is a real body, not a veneer: it hands the sub-screen BG and OBJ VRAM
   banks back before delegating to dScMgBase_c's, which is the half of the
   menu-down sequence that only the 3D minigames need. */
void dScMgD3DBase_c::OnAimedAtWithEggReturnVec()
{
    char *c = (char *)this;

    struct dScMgD3DBase_c *self = (struct dScMgD3DBase_c *)(void *)c;
    _ZN2GX15SetBankForSubBGEt(self->unk_0a0);
    _ZN2GX16SetBankForSubOBJEt(self->unk_4660);
    data_0209e660 = 1;
    func_ov006_020e740c();
    _ZN11dScMgBase_c25OnAimedAtWithEggReturnVecEv(c);
    if (self->unk_4664 == 1) {
        func_ov006_020e7508();
    } else {
        func_ov006_020e759c();
        if (data_0209f5f8 == 0) data_0209f5f8 = 1;
    }
}

/* --------------------------------------------------------------------------
 * ROM ordinals 1 and 0 -- _ZN14dScMgD3DBase_cD0Ev at 0x020e6c60 (0x4c)
 *                     and _ZN14dScMgD3DBase_cD1Ev at 0x020e6c28 (0x38).
 *
 * Neither is written out here, and neither needs a forcing helper.
 *
 * ~dScMgD3DBase_c() is defined inline in the class body
 * (include/dScMgD3DBase_c.h) and must stay there: all four children write
 * this class's vptr store, mSysTracker destruction and the chain to
 * dScMgBase_c's D2 out INLINE in their own D1s, and
 * _ZN14dScMgD3DBase_cD2Ev exists nowhere in the ROM, so an out-of-line
 * definition would leave every child with an undefined external.
 *
 * What emits the out-of-line D1/D0 pair is this TU's KEY FUNCTION.  The first
 * DECLARED non-inline virtual of the class is BeforeInitResources (the inline
 * destructor is declared first but, being inline, cannot be the key
 * function), and this TU defines it -- so mwcc emits _ZTV14dScMgD3DBase_c
 * right here.  Slots 16 and 17 of that table name D1 and D0, which odr-uses
 * both, and the compiler emits the pair out of line, D1 first.  That is
 * exactly the mechanism the retail ROM used, and it is why 0x020e6c28 and
 * 0x020e6c60 exist at all, in that order.
 *
 * The two `..._EmitDestructor` / `..._EmitDeletingDestructor` helpers that
 * src/_ZN14dScMgD3DBase_cD1Ev.cpp and src/_ZN14dScMgD3DBase_cD0Ev.cpp carried
 * are therefore deleted, not ported: they were scaffolding for two files that
 * had no key function to lean on.  Keeping them would add two unlicensed
 * .text symbols and change the vtable's data.  Same call as
 * src/minigames/d_s_mg_single3_d_base.cpp made one level across.
 * -------------------------------------------------------------------------- */
