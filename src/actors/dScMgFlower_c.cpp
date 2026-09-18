//cpp
/* Reconstructed translation unit.
 * ov006/dScMgFlower_c  (9 functions)
 *
 * The flower-petal minigame scene ("loves me, loves me not"): the player drags
 * petals off a flower with the stylus while a face watches. The class identity
 * is the cartridge's own -- tools/rtti_extract.py reads dScMgFlower_c out of
 * ov006's own __si_class_type_info record, single edge to dScMgSingle3DBase_c
 * at offset 0, and the 31-slot vtable at ov006:0x02140140 carries the matching
 * _ZTI13dScMgFlower_c one word below its address point. No name here is coined.
 *
 * FUNCTION ORDER IS ROM-ASCENDING, WHICH IS THE OPPOSITE OF THIS PROJECT'S
 * USUAL TU ORDER, AND THE PRAGMA IS THE REASON. mwccarm 2004/b56 normally
 * defers code generation to end of file and emits the deferred bodies in
 * REVERSE source order, so a merged TU is normally written highest ROM address
 * first. Deferral also makes every optimisation pragma effectively file-global
 * and last-wins: the compiler reads the whole file, then generates, so a
 * bracket around one member applies to all of them.
 *
 * This TU cannot live with that. Behavior needs common-subexpression
 * elimination OFF and no other member does -- with CSE on, mwcc coalesces the
 * adjacent touch-sample base loads (data_020a0de8/de9/dea/deb and
 * data_020a0e40) into one pooled address, which is 999 words and three wrong
 * relocation destinations away from the cartridge; with it off file-wide, two
 * other members regress instead. `#pragma defer_codegen off` is what makes the
 * bracket bind positionally, because each body is then generated where it
 * stands. Measured both ways on the whole TU: with the bracket, 9/9 MATCH;
 * with the bracket removed and nothing else changed, Behavior alone falls to
 * DIFF (999 words, 3 reloc destinations wrong) for 8/9. The pragma pair is
 * load-bearing, not decoration.
 *
 * Turning deferral off forces the ascending rewrite, because the parse-time
 * half of the emission stream comes out in source order. It also decides the
 * destructor's form, and NOT the way this class's ov006 siblings record it.
 * The cartridge has D1 at 0x0212a554 BELOW D0 at 0x0212a5c8 and no D2 at all.
 * include/dScMgRoulette_c.h reports that only an INLINE, in-class destructor
 * reaches that order, an out-of-line one coming out D2, D0, D1 -- true under
 * DEFERRED codegen, which is the regime every one of those siblings compiles
 * in. With deferral off the variant group emits D1, D0, D2 instead, so here
 * the destructor is declared in the header and DEFINED OUT OF LINE, below.
 * The four rounds behind that are written out in include/dScMgFlower_c.h.
 *
 * The D2 mwcc emits has no cartridge home in any module and nothing in the
 * object references it: D1 and D0 reproduce the ROM with the body inlined
 * rather than chained. It is licensed as a deadstrip in the manifest, and
 * objisolate proves no retained section depends on it before it is dropped.
 *
 * KEY FUNCTION. The destructor is this class's first DECLARED non-inline
 * virtual, so this TU -- and only this one -- emits _ZTV13dScMgFlower_c, its
 * _ZTI/_ZTS pair, and the whole ancestor chain's typeinfo as vague linkage.
 * All thirteen records are licensed in the manifest's compiler_only_output
 * block with their cartridge addresses; each lives outside this TU's single
 * licensed range (.text 0x0212a554-0x0212b7f8), so dsd delinks them from the
 * cartridge independently.
 *
 * SHADOW TYPES AND EXTERNAL DATA STAY INSIDE THE MEMBER THAT RECOVERED THEM,
 * except where two members' views actually conflicted; those are reconciled to
 * the most complete observation at the top of the file, never hoisted wholesale.
 * decl_common.h's signatures win over the legacy files' guesses, and the two
 * members whose bodies assumed a different one take a cast at the head of the
 * body rather than a redeclaration.
 *
 * Assembled from these legacy one-function sources (ROM address order).  They lived
 * directly under src/ and no longer exist; the manifest's per-function
 * legacy_source rows keep their full paths:
 *   [0] 0x0212a554  _ZN13dScMgFlower_cD1Ev.cpp
 *   [1] 0x0212a5c8  _ZN13dScMgFlower_cD0Ev.cpp
 *   [2] 0x0212a650  func_ov006_0212a650.c
 *   [3] 0x0212a654  func_ov006_0212a654.c
 *   [4] 0x0212a764  func_ov006_0212a764.cpp
 *   [5] 0x0212aa74  _ZN13dScMgFlower_c13OnYoshiTryEatEi.cpp
 *   [6] 0x0212aacc  _ZN13dScMgFlower_c6RenderEv.cpp
 *   [7] 0x0212ac74  _ZN13dScMgFlower_c8BehaviorEv.cpp
 *   [8] 0x0212b480  _ZN13dScMgFlower_c13InitResourcesEv.cpp
 *
 * The two destructor shards each carried their own out-of-line copy of the same
 * body; both are absorbed into the single definition below.
 *
 * The class's factory, dScMgFlower_c_classInit at 0x0212b7f8, is NOT here. It
 * sits in src/d_s_mg_flower.cpp, immediately above this range's end, and that
 * file does not include this class's header. It is left where it is.
 */

#include "dScMgFlower_c.h"
#include "types.h"
#include "decl_common.h"

#pragma defer_codegen off
#pragma opt_strength_reduction off

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow typedef 'Entry' */
typedef struct Entry {
    u8 unk00; u8 unk01; u8 unk02; u8 unk03;
    s32 unk04; s32 unk08; s32 unk0c; s32 unk10;
    s32 unk14; s32 unk18; s32 unk1c;
} Entry;

/* shadow typedef 'Obj' */
typedef struct Obj {
    u8 pad0[0x4f38];
    Entry entries[22];
    u8 pad1[0x5fc8 - 0x51f8];
    s32 unk5fc8;
    u8 pad2[1];
    u8 unk5fcd;
} Obj;

/* shadow struct 'S2' */
struct S2 { int a, b; };

#define M(p) ((long long)(int)(p))
#define A1(off) ((int)M((int)M((char *)self + (int)M(i) * 32) + (off)))
#define A2(off) ((int)M((int)M((char *)self + (unsigned)i * 32) + (off)))
#define B1(off) (*(u8 *)A1(off))
#define W1(off) (*(s32 *)A1(off))
#define W2(off) (*(s32 *)A2(off))
#define LB(p) ((int)((int)(p)))
#define LA(p) ((int)(p))

extern "C" {
extern int RandomIntInternal(int *seed);
extern void func_0203d388(int *p, int angle);
extern void func_ov004_020b04d0(int n);
/* RECONCILED: the two legacy sources merged here disagreed -- func_ov006_0212a764
 * spelled the register pointer `volatile void *` and the last argument `u32`,
 * while InitResources spelled them `volatile u16 *` and `u16`. The mangled name
 * settles it without a judgement call: PVt-t-t-t-j is (volatile u16 *, u16, u16,
 * u16, unsigned int), so the pointer type comes from the second file and the
 * final argument from the first. Both call sites pass a literal register address
 * and small constants, so the reconciliation is byte-neutral. */
extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile u16 *reg, u16 a, u16 b, u16 c, u32 d);
extern int data_0209d4b8;
extern s16 data_02082214[];
void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
void func_ov004_020af770(void *a0, int a1, int a2, int a3, int a4, int a5, u16 a6);
typedef struct V2 {
int x;
int z;
} V2;
typedef struct Cell {
int x;
int z;
char pad[0x18];
} Cell;
void Vec2_Sub(V2 *out, V2 *a, V2 *b);
int Vec2_Len(V2 *p);
void FreeGfxSlotsById(int a);
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
void *func_ov004_020adc74(void *arg);
char *_ZN2G213GetBG2CharPtrEv(void);
void DecompressLZ16(int src, void *dst);
void Ov004_Deallocate(int handle);
void *_ZN3G2S13GetBG2CharPtrEv(void);
char *_ZN2G212GetBG2ScrPtrEv(void);
char *_ZN3G2S12GetBG2ScrPtrEv(void);
char *_ZN3G2S12GetBG3ScrPtrEv(void);
void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 addr, u32 size);
void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern int data_0208ee44;
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

/* The out-of-line destructor definition, written FIRST because `#pragma
   defer_codegen off` makes the variant group emit D1, D0, D2 in source
   position -- which is the cartridge's own order, D1 at 0x0212a554 then D0 at
   0x0212a5c8, with no D2 surviving. See the file banner.

   The body's two explicit calls are the ROM's order: the 0x51f8 object first
   (func_ov006_020c3e70), then mArray (__cxa_vec_cleanup over 0x16 elements of
   0x20, element dtor func_ov006_0212a650) -- the reverse of the factory's
   construction order in src/d_s_mg_flower.cpp. Everything after them in the
   emitted bytes is the compiler's own inlining of dScMgSingle3DBase_c's
   already-inline destructor: the own-vtable store, mSysTracker's destruction
   and the chain up to ~dScMgBase_c. */
dScMgFlower_c::~dScMgFlower_c()
{
    func_ov006_020c3e70((char *)this + 0x51f8);
    __cxa_vec_cleanup(mArray, 0x16, 0x20, (void *)func_ov006_0212a650);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- _ZN13dScMgFlower_cD0Ev 0x0212a5c8 (0x88) and
 * _ZN13dScMgFlower_cD1Ev 0x0212a554 (0x74).
 *
 * NEITHER IS DEFINED HERE. The cartridge puts D1 at 0x0212a554 BELOW D0 at
 * 0x0212a5c8 and carries no D2 at all, and the only admissible source form
 * that makes mwccarm 2004/b56 emit D1-then-D0 is an INLINE, in-class
 * destructor declared as the first member. The two legacy shards each held
 * an out-of-line copy of the same body; merged they would also have been a
 * duplicate definition -- the compiler says so in as many words. The body now
 * lives in include/dScMgFlower_c.h, where the compiler emits both variants
 * itself, in the cartridge's order. See the class header's own note. */
/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov006_0212a650, 0x0212a650, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0212a650
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0212a650(void)
{
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov006_0212a654, 0x0212a654, size 0x110 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0212a654
extern "C" {  /* .c-derived member: C linkage for the whole block */
/* SIGNATURE FROM decl_common.h, same reconciliation as func_ov006_0212a764
 * below: the shard took `Obj *`, the shared header takes `char *`, and the
 * shared header wins with the typed view recovered by a cast. */
void func_ov006_0212a654(char *p)
{
    Obj *self = (Obj *)p;
    int i;

    self->unk5fcd = 1;
    for (i = 0; i < 22; i++) {
        if (B1(0x4f38) == 0) continue;
        if (self->entries[i].unk02 == 1) { self->unk5fcd = 0; continue; }
        if (self->entries[i].unk01 == 0) { self->unk5fcd = 0; continue; }
        if (i == self->unk5fc8) continue;
        W1(0x4f48) += 0x100;
        if (W1(0x4f48) >= 0x4000) W1(0x4f48) = 0x4000;
        W2(0x4f3c) += self->entries[i].unk0c;
        W2(0x4f40) += W1(0x4f48);
        if (W2(0x4f40) >= 0x100000) B1(0x4f38) = 0;
        else self->unk5fcd = 0;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov006_0212a764, 0x0212a764, size 0x310 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0212a764
/* recovered: dScMgFlower_c round setup, ov006 0x0212a764 (784 bytes). Called by
 * InitResources and again from OnYoshiTryEat when a round restarts. Clears the
 * cursor pair, frees the 0x20 OAM slot group, wipes all 0x16 petal records,
 * rolls the petal count (1-2 when the hold counter reads exactly 0x14, else a
 * 1-in-10 chance of 20-21, otherwise 8-15), then lays the petals out on a
 * circle: each gets angle = base + i * (full turn) / count, a (0, 3.0) offset
 * rotated by that angle through func_0203d388 and translated to (8.0, 6.0),
 * and is marked active. A shuffle pass then swaps angle and position between
 * each petal and a random partner, and the round state (held petal, toggle,
 * timers, face sprite, background phase, blend alpha) is reset.
 *
 * The petal record is addressed by raw offset off `this`, and that is
 * measured, not laziness. The cartridge forms every angle/position address as
 * `add rX, <this + i*0x20>, <pool 0x4f54/0x4f3c/0x4f40>` with a zero-offset
 * access, and keeps &angle (r5) and &pos.x (r4) across the two calls. Real
 * member access folds the constant into an `add #0x4f00` / `[#0x54]` pair,
 * keeps nothing across the calls, and the whole loop drifts (20 bytes short);
 * named pointers to the members get strength-reduced, or hoisted as
 * `this + 0x4f54` with a scaled index. Converting the element address to an
 * integer before adding the field offset -- `(unsigned int)(c + i * 0x20) +
 * 0x4f54` -- is the one spelling that keeps the constant out of the
 * addressing mode. It is a plain pointer-to-integer conversion, not the u64
 * mask or the long-long round trip. The tail members DO reproduce and are
 * named.
 *
 * Three more shapes are load-bearing: the two petal pointers `pa` and `px`
 * are block locals, `px` declared after the angle store (the cartridge
 * computes &angle before the divide and &pos.x after it, and colours them
 * r5/r4 ahead of the function-scope i2/ang/pang, which take r8/r7/r6 in
 * reverse declaration order); the divisor is the live petal count, so r1
 * stays occupied through the top of the loop and the 0x4f54 pool load lands
 * in r2; and the swap temp is a Vec2, whose empty destructor keeps its 8-byte
 * stack home (frame 0x1c) while the copy itself goes through registers --
 * see the header. */
/* SIGNATURE FROM decl_common.h, NOT FROM THE LEGACY SHARD. The shard defined
 * this as `func_ov006_0212a764(dScMgFlower_c *)`, but decl_common.h -- which
 * this TU includes, and which both calling members compiled against -- spells
 * it `void func_ov006_0212a764(void *)`. One TU cannot hold both spellings
 * under C linkage, so the shared header's wins and the typed view is recovered
 * by a cast at the head of the body. Byte-neutral: the parameter is one
 * r0-width value either way, and verify re-confirms all nine members. */
extern "C" void func_ov006_0212a764(void *p)
{
    dScMgFlower_c *self = (dScMgFlower_c *)p;
    char *c = (char *)self;
    unsigned int r;
    int j;
    int i2;
    int ang;
    int *pang;
    int i;
    int angbase;

    self->mCursorX = 0;
    self->mCursorY = 0;
    self->mPrevCursorX = self->mCursorX;
    self->mPrevCursorY = self->mCursorY;
    func_ov004_020b04d0(0x20);
    for (i = 0; i < 0x16; i++) {
        *(u8 *)(c + i * 0x20 + 0x4f38) = 0;
        *(u8 *)(c + i * 0x20 + 0x4f3a) = 0;
        *(u8 *)(c + i * 0x20 + 0x4f39) = 0;
        *(u8 *)(c + i * 0x20 + 0x4f3b) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f3c) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f40) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f4c) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f50) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f44) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f48) = 0;
    }
    if (self->mHoldTimer == 0x14) {
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        self->mPetalsLeft = ((r * 2) >> 15) + 1;
    } else {
        int raw = RandomIntInternal(&data_0209d4b8);
        r = ((unsigned int)raw >> 16) & 0x7fff;
        if (((r * 10) >> 15) == 0) {
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            self->mPetalsLeft = ((r * 2) >> 15) + 0x14;
        } else {
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            self->mPetalsLeft = ((r * 8) >> 15) + 8;
        }
    }
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    angbase = (r * 0x10000) >> 15;
    i2 = 0;
    if (self->mPetalsLeft > 0) {
        pang = (int *)(c + 0x4f3c);
        ang = 0;
        do {
            s16 *pa = (s16 *)((unsigned int)(c + i2 * 0x20) + 0x4f54);
            *pa = angbase + ang / self->mPetalsLeft;
            {
                s32 *px = (s32 *)((unsigned int)(c + i2 * 0x20) + 0x4f3c);
                *px = 0;
                *(s32 *)((unsigned int)(c + i2 * 0x20) + 0x4f40) = 0x30000;
                func_0203d388(pang, *pa);
                *px += 0x80000;
                *(s32 *)((unsigned int)(c + i2 * 0x20) + 0x4f40) += 0x60000;
            }
            *(u8 *)((unsigned int)c + i2 * 0x20 + 0x4f38) = 1;
            i2++;
            ang += 0x10000;
            pang += 8;
        } while (i2 < self->mPetalsLeft);
    }
    j = 0;
    if (self->mPetalsLeft > 0) {
        do {
            int k;
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            k = (self->mPetalsLeft * r) >> 15;
            if (k != j) {
                char *ej = c + j * 0x20;
                char *ek = c + k * 0x20;
                u16 t16;
                dScMgFlower_c::Vec2 tmp;
                t16 = *(u16 *)((unsigned int)ej + 0x4f54);
                *(u16 *)((unsigned int)ej + 0x4f54) = *(u16 *)((unsigned int)ek + 0x4f54);
                *(u16 *)((unsigned int)ek + 0x4f54) = t16;
                tmp.x = *(s32 *)((unsigned int)ej + 0x4f3c);
                tmp.y = *(s32 *)((unsigned int)ej + 0x4f40);
                *(s32 *)((unsigned int)ej + 0x4f3c) = *(s32 *)((unsigned int)ek + 0x4f3c);
                *(s32 *)((unsigned int)ej + 0x4f40) = *(s32 *)((unsigned int)ek + 0x4f40);
                *(s32 *)((unsigned int)ek + 0x4f3c) = tmp.x;
                *(s32 *)((unsigned int)ek + 0x4f40) = tmp.y;
            }
            j++;
        } while (j < self->mPetalsLeft);
    }
    self->mHeldPetal = -1;
    self->mPetalToggle = 0;
    self->mState = 0;
    self->mHintTimer = 0;
    self->mResultTimer = 0;
    self->mFaceSprite = 2;
    self->unk_5fcd = 0;
    self->mBgScrollPhase = 0;
    *(volatile u16 *)0x4000050 = 0;
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4001050, 4, 8, 6, 0xa);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN13dScMgFlower_c13OnYoshiTryEatEi, 0x0212aa74, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScMgFlower_c13OnYoshiTryEatEi
// recovered name: dScMgFlower_c_OnYoshiTryEat_0212aa74
/* recovered: renamed to Class_Method, declarations from a shared header */
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
/* _ZN13dScMgFlower_c13OnYoshiTryEatEi at 0x0212aa74
 *
 * Wraps a counter at this+0x5fe4 (increments while <= 0x14, else resets
 * to 0), then updates the sub-object at this+0x51f8 (func_ov006_020c3bc8)
 * and runs func_ov006_0212a764.
 *
 * Matching notes: the increment must be the THEN arm (fall-through) with
 * the reset as the small predicated else (movgt/strgt + bgt), and it is
 * spelled as a volatile RMW through a u64-mask-laundered pointer so the
 * counter address is rematerialized from the literal pool in its own
 * block instead of folding into the this+0x5000 base and if-converting.
 */
void dScMgFlower_c::OnYoshiTryEat(int /* arg */)
{
    char *self = (char *)this;

    if (*(int *)(self + 0x5fe4) <= 0x14) {
        (*(volatile int *)(self + 0x5fe4))++;
    } else {
        *(int *)(self + 0x5fe4) = 0;
    }
    func_ov006_020c3bc8(self + 0x51f8);
    func_ov006_0212a764(self);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN13dScMgFlower_c6RenderEv, 0x0212aacc, size 0x1a8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScMgFlower_c6RenderEv
/* dScMgFlower_c::Render -- vtable slot 9.
 *
 * Attributed by the ROM's vtable; the third of the three slots (0, 6, 9) where
 * Flower's table differs from dScMgSingle3DBase_c's. The four addresses that once
 * carried `recovered name: dScMgFlower_c_*` for slots 2/5/7/10 were the parent's
 * and moved up in commit 4f7406b9c -- see include/dScMgFlower_c.h.
 *
 * Scrolls the two background layers off a sine table (0x5ff4 is the phase, stepped
 * 0xc0 a frame), draws the round's banner sprite, then one sprite per live flower.
 * The flower being dragged -- index 0x5fc8 -- is drawn with a different palette
 * argument, which is how the player sees which one is held.
 *
 * WAS A C99 FILE, so the three declarations move inside `extern "C"`; in C++ they
 * would mangle and resolve to nothing. */
s32 dScMgFlower_c::Render()
{
    char *self = (char *)this;
    int i;
    u8 (*barr)[0x20] = (u8 (*)[0x20])self;
    int (*iarr)[8] = (int (*)[8])self;
    u16 (*harr)[0x10] = (u16 (*)[0x10])self;

    *(u16*)(self + 0x5ff4) += 0xc0;
    {
        int v = data_02082214[(*(u16*)(self + 0x5ff4) >> 4) << 1];
        int t = v + 0x80;
        int off = (t + (int)((unsigned)(t >> 7) >> 24)) >> 8;
        SetSubBg2Offset(off, off + 8);
    }
    {
        int v = data_02082214[(*(u16*)(self + 0x5ff4) >> 4) << 1];
        int t = 0x80 - v;
        int off = (t + (int)((unsigned)(t >> 7) >> 24)) >> 8;
        SetSubBg3Offset(off, off);
    }
    func_ov004_020afdd0(data_ov006_0213ab94[*(int*)(self + 0x5fec)], 0x80, 0x60, -1, 1);

    for (i = 0; i < 0x16; i++) {
        if (barr[i][0x4f38] != 0) {
            struct S2 loc = *(struct S2*)data_ov006_0213abe0;
            int a4 = 1;
            loc.a = (loc.a & 0xc1fffcff) | 0x100;
            if (*(int*)(self + 0x5fc8) == i)
                a4 = 0;
            func_ov004_020af770(
                &loc,
                iarr[i][0x13cf] >> 12,
                iarr[i][0x13d0] >> 12,
                -1,
                a4,
                0x1000,
                (u16)(s16)(harr[i][0x27aa] + 0x8000));
        }
    }

    func_ov006_020c3b2c(self + 0x4660);
    func_ov006_020c3bf4(self + 0x51f8);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN13dScMgFlower_c8BehaviorEv, 0x0212ac74, size 0x80c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScMgFlower_c8BehaviorEv
/* dScMgFlower_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable; one of the three slots (0, 6, 9) where Flower's
 * table really does differ from dScMgSingle3DBase_c's. The four addresses that
 * once carried `recovered name: dScMgFlower_c_*` for slots 2/5/7/10 were the
 * parent's and moved up in commit 4f7406b9c -- see include/dScMgFlower_c.h.
 *
 * The whole minigame: pick the flower nearest the cursor, drag it, drop it, score
 * the pair, and run the between-rounds state machine. mArray at 0x4f38 is the
 * 0x16-entry flower table, spelled here by raw offset because its element type is
 * not reconstructed yet.
 *
 * LA/LB ARE LAUNDERS, NOT CASTS. Each forces the address through an integer so
 * mwccarm re-materialises it instead of common-subexpressing the field address --
 * removing one is a byte-level change. The two pragmas are load-bearing for the
 * same reason. Only the declarations move: the body is byte-for-byte the one that
 * already matched, with `this` in place of the char* parameter. */
#pragma opt_common_subs off
s32 dScMgFlower_c::Behavior()
{
    char *c = (char *)this;
    struct dScMgFlower_c *self = this;
    int i;
    int k;
    int t;
    V2 d1;
    V2 d2;
    V2 d3;

    if (self->mPromptBlinkCount == 0) {
        self->mPromptEnabled = 1;
        self->mPromptBlinkCount = 1;
        self->mPromptBlinkTimer = 0;
    }
    if (self->mHintTimer > 0) {
        if (self->mHeldPetal == -1) {
            *(int *)LA(c + 0x5fd0) -= 1;
            if (self->mHintTimer <= 0) {
                if (self->mPetalsLeft >= 1)
                    self->mFaceSprite = 2;
            }
        } else {
            self->mHintTimer = 0x3c;
        }
    }
    switch (self->mState) {
    case 0:
        self->mPrevCursorX = self->mCursorX;
        self->mPrevCursorY = self->mCursorY;
        k = data_020a0e40[0];
        if (data_020a0de8[k * 4] != 0) {
            int b = data_020a0deb[k * 4];
            int a = data_020a0dea[k * 4];
            self->mCursorX = (b ? a : a) << 12;
            self->mCursorY = b << 12;
        }
        if (self->mHoldTimer > 0x14) {
            if (self->mPetalsLeft > 0) {
                for (i = 0; i < self->mPetalsLeft; i++) {
                    *(u8 *)(c + i * 0x20 + 0x4f39) = 1;
                    *(u8 *)(c + i * 0x20 + 0x4f3b) = 1;
                }
                self->mPetalsLeft = 0;
                _ZN5Sound12PlayBank2_2DEj(0x10d);
                FreeGfxSlotsById(0x1d);
            }
        } else if (self->mHeldPetal < 0) {
            int t;
            if (data_020a0de8[k * 4] != 0 && data_020a0de9[k * 4] != 0)
                t = 1;
            else
                t = 0;
            if (t != 0) {
                int ii;
                V2 *q = (V2 *)(c + 0x4f3c);
                for (ii = 0; ii < 0x16; ii++) {
                    if (*(u8 *)(c + ii * 0x20 + 0x4f38) != 0 &&
                        *(u8 *)(c + ii * 0x20 + 0x4f3a) != 1 &&
                        *(u8 *)(c + ii * 0x20 + 0x4f39) != 1) {
                        int v;
                        Vec2_Sub(&d1, q, (V2 *)(c + 0x5fb8));
                        v = Vec2_Len(&d1) < 0x18000 ? 1 : 0;
                        if (v != 0) {
                            _ZN5Sound12PlayBank2_2DEj(0x109);
                            *(u8 *)(c + ii * 0x20 + 0x4f3a) = 1;
                            *(int *)(c + ii * 0x20 + 0x4f44) = 0;
                            *(int *)(c + ii * 0x20 + 0x4f48) = 0;
                            self->mHeldPetal = ii;
                            *(int *)(c + ii * 0x20 + 0x4f4c) = self->mCursorX;
                            *(int *)(c + ii * 0x20 + 0x4f50) = self->mCursorY;
                            FreeGfxSlotsById(0x1d);
                            self->mFaceSprite = 2;
                            break;
                        }
                    }
                    q = (V2 *)((char *)q + 0x20);
                }
                if (self->mHeldPetal < 0) {
                    int i2;
                    V2 *q2 = (V2 *)(c + 0x4f3c);
                    for (i2 = 0; i2 < 0x16; i2++) {
                        if (*(u8 *)(c + i2 * 0x20 + 0x4f38) != 0 &&
                            *(u8 *)(c + i2 * 0x20 + 0x4f3a) != 1) {
                            int v2;
                            Vec2_Sub(&d2, q2, (V2 *)(c + 0x5fb8));
                            v2 = Vec2_Len(&d2) < 0x18000 ? 1 : 0;
                            if (v2 != 0) {
                                *(u8 *)(c + i2 * 0x20 + 0x4f3a) = 1;
                                *(int *)(c + i2 * 0x20 + 0x4f44) = 0;
                                *(int *)(c + i2 * 0x20 + 0x4f48) = 0;
                                self->mHeldPetal = i2;
                                *(int *)(c + i2 * 0x20 + 0x4f4c) = self->mCursorX;
                                *(int *)(c + i2 * 0x20 + 0x4f50) = self->mCursorY;
                                break;
                            }
                        }
                        q2 = (V2 *)((char *)q2 + 0x20);
                    }
                }
            }
        } else {
            t = 0;
            if (data_020a0de8[k * 4] != 0) {
                Cell *cells;
                int j;
                Vec2_Sub(&d3, (V2 *)(c + 0x5fb8), (V2 *)(c + 0x5fc0));
                cells = (Cell *)(c + 0x4f3c);
                j = self->mHeldPetal;
                cells[j].x += d3.x;
                *(int *)LB((char *)cells + j * 0x20 + 4) += d3.z;
            } else {
                if (*(u8 *)(c + self->mHeldPetal * 0x20 + 0x4f39) == 0) {
                    *(u8 *)(c + self->mHeldPetal * 0x20 + 0x4f39) = 1;
                    if (*(u8 *)(c + self->mHeldPetal * 0x20 + 0x4f3b) == 0) {
                        t = 1;
                        *(u8 *)(c + self->mHeldPetal * 0x20 + 0x4f3b) = 1;
                        func_ov006_020c3990(c + 0x51f8);
                    }
                }
                *(u8 *)(c + self->mHeldPetal * 0x20 + 0x4f3a) = 0;
                self->mHeldPetal = -1;
            }
            if (t != 0) {
                *(int *)LA(c + 0x5fd8) -= 1;
                if (self->mPetalToggle == 1) {
                    self->mPetalToggle = 0;
                    if (self->mPetalsLeft >= 1) {
                        FreeGfxSlotsById(0x1d);
                        func_ov004_020b0cac(0x13, 0x80, 0x18, 0, -1, 0xd);
                        self->mFaceSprite = 3;
                        _ZN5Sound12PlayBank2_2DEj(0x104);
                    }
                } else {
                    self->mPetalToggle = 1;
                    if (self->mPetalsLeft >= 1) {
                        FreeGfxSlotsById(0x1d);
                        func_ov004_020b0cac(0x10, 0x80, 0x18, 0, -1, 0xd);
                        self->mFaceSprite = 1;
                        _ZN5Sound12PlayBank2_2DEj(0x103);
                    }
                }
                self->mHintTimer = 0x3c;
            }
        }
        if (self->mPetalsLeft <= 0 && self->unk_5fcd == 1) {
            if (self->mResultTimer > 0) {
                *(int *)LA(c + 0x5fd4) -= 1;
                if (self->mResultTimer <= 0) {
                    self->mState = 1;
                    func_ov004_020b0a54(0xc);
                }
                if (func_ov006_020c3b80(c + 0x51f8) != 0) {
                    if (self->mPetalToggle == 1)
                        func_ov006_020c3908(c + 0x51f8);
                    else
                        func_ov006_020c38b0(c + 0x51f8);
                }
            } else {
                FreeGfxSlotsById(0x1d);
                if (self->mHoldTimer > 0x14) {
                    if (self->unk_5fcd == 1) {
                        _ZN5Sound12PlayBank2_2DEj(0x106);
                        self->mFaceSprite = 4;
                        self->mResultTimer = 0x3c;
                    }
                } else {
                if (self->mPetalToggle == 1) {
                    self->mFaceSprite = 0;
                    *(int *)LA(c + 0x5fdc) += 1;
                    self->mLoseStreak = 0;
                    if (self->mWinStreak >= 3) {
                        func_ov004_020b0cac(0x12, 0x80, 0x18, 0, -1, 0xd);
                        _ZN5Sound12PlayBank2_2DEj(0x107);
                        *(int *)LA(c + 0x5ff0) += 3;
                        if (self->mScore > 0x270f)
                            self->mScore = 0x270f;
                    } else {
                        func_ov004_020b0cac(0x10, 0x80, 0x18, 0, -1, 0xd);
                        _ZN5Sound12PlayBank2_2DEj(0x108);
                        *(int *)LA(c + 0x5ff0) += 1;
                        if (self->mScore > 0x270f)
                            self->mScore = 0x270f;
                    }
                } else {
                    self->mFaceSprite = 4;
                    *(int *)LA(c + 0x5fe0) += 1;
                    self->mWinStreak = 0;
                    if (self->mLoseStreak >= 3) {
                        func_ov004_020b0cac(0x11, 0x80, 0x18, 0, -1, 0xd);
                        _ZN5Sound12PlayBank2_2DEj(0x105);
                    } else {
                        func_ov004_020b0cac(0x13, 0x80, 0x18, 0, -1, 0xd);
                        _ZN5Sound12PlayBank2_2DEj(0x106);
                    }
                }
                self->mResultTimer = 0x3c;
                }
            }
        }
        func_ov006_0212a654(c);
        break;
    case 1:
        if (func_ov006_020c3b80(c + 0x51f8) != 0) {
            if (self->mPetalToggle == 1)
                func_ov006_020c3908(c + 0x51f8);
            else
                func_ov006_020c38b0(c + 0x51f8);
        }
        self->mPromptEnabled = 0;
        func_ov006_0212a654(c);
        break;
    default:
        break;
    }
    func_ov006_020c3d18(c + 0x51f8);
    return 1;
}
#pragma opt_common_subs on

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN13dScMgFlower_c13InitResourcesEv, 0x0212b480, size 0x378 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScMgFlower_c13InitResourcesEv
/* dScMgFlower_c::InitResources -- vtable slot 0.
 *
 * Attributed by the ROM's vtable, and this one the old `recovered name:` comment
 * got right: slot 0 is one of the three where Flower's table really does differ
 * from dScMgSingle3DBase_c's (the others are 6 and 9). The four addresses that
 * used to claim `dScMgFlower_c_*` for slots 2/5/7/10 were the parent's and moved
 * up in commit 4f7406b9c; see include/dScMgFlower_c.h.
 *
 * Sets up both screens' BG2/BG3 layers, decompresses the tile and screen data
 * behind each, loads the palettes, then hands off to the class's own state reset.
 * The bare 0x040000xx stores are the 2D engine's BGxCNT / BGxOFS registers and
 * the 0x0640_0000 / 0x0660_0000 destinations are the two OBJ VRAM banks.
 *
 * WAS A C99 FILE, so every declaration below moves inside `extern "C"`: in C++
 * these names would mangle and resolve to nothing. The three `extern` variables
 * keep that keyword deliberately -- a variable declaration inside `extern "C" {}`
 * without it is a definition and collides with the delinked gap object. */
s32 dScMgFlower_c::InitResources()
{
    char *c = (char *)this;
    int h;

    *(int *)(c + 0x5ff0) = func_ov004_020ad8b8();

    data_0208ee44 = 1;

    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x1414;
    *(volatile u16 *)0x400000c &= ~0x40;
    *(volatile u32 *)0x4000018 = 0;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 3;

    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x1414;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 3;

    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x1414;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile u32 *)0x400101c = 0;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 3;

    h = (int)func_ov004_020adc74(&data_ov006_021401d0);
    DecompressLZ16(h, _ZN2G213GetBG2CharPtrEv());
    Ov004_Deallocate(h);

    h = (int)func_ov004_020adc74(&data_ov006_021401f4);
    DecompressLZ16(h, _ZN3G2S13GetBG2CharPtrEv());
    Ov004_Deallocate(h);

    h = (int)func_ov004_020adc74(&data_ov006_02140218);
    DecompressLZ16(h, _ZN2G212GetBG2ScrPtrEv());
    Ov004_Deallocate(h);

    h = (int)func_ov004_020adc74(&data_ov006_0214023c);
    DecompressLZ16(h, _ZN3G2S12GetBG2ScrPtrEv());
    DecompressLZ16(h, _ZN3G2S12GetBG3ScrPtrEv());
    Ov004_Deallocate(h);

    h = (int)func_ov004_020adc74(&data_ov006_02140260);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(h, 0x1a0);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
    Ov004_Deallocate(h);

    h = (int)func_ov004_020adc74(&data_ov006_02140284);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(h, 0x1a0);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
    Ov004_Deallocate(h);

    *(volatile u16 *)0x4000050 = 0;
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4001050, 4, 8, 6, 0xa);

    data_0209d45c |= 4;
    data_0209d454 |= 0xc;

    h = (int)func_ov004_020adc74(&data_ov006_021402a8);
    DecompressLZ16(h, (void *)0x6400000);
    DecompressLZ16(h, (void *)0x6600000);
    Ov004_Deallocate(h);

    h = (int)func_ov004_020adc74(&data_ov006_021402c4);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(h, 0x100);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)h, 0, 0x100);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)h, 0, 0x100);
    Ov004_Deallocate(h);

    data_ov004_020bc8a8 = 0x40;
    data_ov004_020bc898 = 0xa0;
    data_ov004_020bc86c = 0xc0;
    data_ov004_020bc8a4 = 0xa0;

    *(int *)(c + 0x5fdc) = 0;
    *(int *)(c + 0x5fe0) = 0;
    *(int *)(c + 0x5fe4) = 0;
    func_ov006_0212a764(c);

    data_0209d45c |= 1;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3) | 1;
    func_ov006_020c3d88(c + 0x51f8);
    func_ov006_020c3b2c(c + 0x4660);

    return 1;
}

