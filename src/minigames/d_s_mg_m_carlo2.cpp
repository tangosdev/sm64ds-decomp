//cpp
/* Pair-match minigame scene 2 (ov006/dScMgMCarlo2_c), 25 functions,
 * enrolled and canonical. The board is 40 card objects on an intrusive
 * doubly-linked list (see the member notes).
 *
 * Source runs REVERSE of ROM (highest address first). Do not reorder.
 * `#pragma long_calls` stays bracketed at its member; opt_propagation
 * stays ON file-wide (measured: off costs four members).
 *
 * Leftover: 21 func_ov004_* + 30 data_* keep linker names (unnamed in
 *   symbols.txt); each needs a coined, behaviour-justified name.
 * Leftover: hand-rolled C1/C2/D0/D1/D2 call shapes and vptr stores
 *   stand in for real ctors.
 * Leftover: raw *(T *)(p + 0x..) offsets and 7 unk_NN names not
 *   evidenced.
 */

/* TUBUILD NOTE -- #pragma directive(s) were present in the legacy sources
 * of this TU. `#pragma long_calls` is POSITIONAL in mwccarm 2004/b56 and is
 * carried verbatim before its own member below, bracketed with `off` so it
 * cannot leak into later members (dropping it silently costs the pooled
 * cross-overlay tail-call -- a byte diff; see daObjBSwdoor_c in ov014).
 * Any OTHER pragma is FILE-GLOBAL last-wins (opt_propagation,
 * optimize_for_size). Two legacy files carried one:
 *   func_ov006_020f95f0: #pragma opt_propagation off   [NOT carried]
 *   _ZN14dScMgMCarlo2_c13OnTurnIntoEggEi: #pragma opt_propagation off   [NOT carried]
 * Neither is carried, and the measurements say the cartridge's own
 * translation unit did not have it either. opt_propagation is file-global
 * last-wins, so a TU has exactly one setting for all 25 functions, and there
 * is only one place it could go -- the top. Set there it costs four OTHER
 * members their match (ordinals 3, 5, 10 and 18), and prepending it to the
 * unmodified legacy sources of those four breaks them there too. So `off`
 * cannot be this TU's setting; it was a per-file crutch for two bodies
 * whose shape had been tuned against it one file at a time.
 *
 * Both members verify byte-exact with propagation ON once their source is
 * shaped the way the cartridge's was -- see the note on each below. That
 * is the load-bearing evidence: a single setting, no pragma, all bodies exact.
 */


#include "dScMgMCarlo2_c.h"
#include "types.h"
#include "decl_common.h"

/* THE BOARD IS 40 dMgMCarlo2CardObj_c objects on one intrusive doubly-linked
 * list. Its 0x30-byte layout and two virtual slots are corroborated by every
 * matched access and by the ROM's own RTTI/vtable records.
 *
 * data_ov006_0214257c is the head of the live list and _02142568 its tail
 * (SetupBoard builds it and leaves both set); _0214256c heads the list of
 * pieces already cleared, which UpdateBoard recycles back
 * onto the tail. */

/* The minigame's own globals. Everything already declared by
 * include/decl_common.h with a type this file agrees with is NOT repeated
 * here -- only the typed card-list pointers. */
extern "C" {
extern dMgMCarlo2CardObj_c* data_ov006_02142568;
extern dMgMCarlo2CardObj_c* data_ov006_0214256c;
extern dMgMCarlo2CardObj_c* data_ov006_02142570;
extern dMgMCarlo2CardObj_c* data_ov006_02142574;
extern dMgMCarlo2CardObj_c* data_ov006_02142578;
extern dMgMCarlo2CardObj_c* data_ov006_0214257c;
extern s16 data_ov006_02142558;     /* frames left before the pair resolves  */
extern s16 data_ov006_0214255c;     /* pieces still animating                */
extern s16 data_ov006_02142560;     /* difficulty row into data_ov006_0212e97c */
extern int data_ov006_02142580[];   /* per-face weights the picker draws from  */
extern int data_ov006_021425a8[];   /* sprite handles, indexed by 0213d770      */
extern unsigned short data_ov006_0213d770[];
extern int data_ov006_0212e954[];   /* the starting weights                     */
extern int data_ov006_0212e97c[][10]; /* per-difficulty weight top-ups          */
extern int data_0209e650[];
extern int data_020a0db0;
extern unsigned char data_0209d454;
extern unsigned char data_0209d45c;
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

int  RandomIntInternal(void* seed);
int  RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);
int  GetGameLanguage(void);
int  LoadFile(int handle);
void DecompressLZ16(int src, int dst);
void Vec2_Sub(int* out, int* a, int* b);
unsigned int func_02012790(unsigned int id);
int  func_0203d5dc(void* a, void* b);
void func_ov004_020ad90c(void* t);
void func_ov004_020adb1c(int score);
void func_ov006_0210a534(void* c);
void func_ov006_020c0aa8(void* c);
void func_ov006_020c1604(char* t, int unused, short a2, void* a3);
int  func_ov006_020c1718(void* t);
void func_ov006_020c1804(void* t);
void func_ov006_020c19d0(void* t);
int  func_ov006_020c1a88(void* t);

void* _ZN7fBase_cnwEj(unsigned int);
void _ZN11dScMgBase_cC2Ev(void*);
void _ZN8Particle10SysTrackerC1Ev(void*);
void func_ov006_020c1d80(void*);
void __cxa_vec_ctor(void*, int, int, void*, void*);
extern int _ZTV19dScMgSingle3DBase_c;
extern int _ZTV14dScMgMCarlo2_c[];
void _ZN19dMgMCarlo2CardObj_cC1Ev(void*);
}

namespace GX { void LoadOBJPltt(const void*, unsigned int, unsigned int); }
namespace GXS { void LoadOBJPltt(const void*, unsigned int, unsigned int); }
namespace Sound { void PlayBank2_2D(unsigned int); }

/* The ROM's own symbols demangle to these, so this is how they are spelled:
 * _Z14ApproachLinearRiii is ApproachLinear(int&, int, int) and
 * _Z15ApproachLinear2Rsss is ApproachLinear2(short&, short, short). Declared
 * with C++ linkage -- the reference parameter is what mangles them. */
int ApproachLinear(int& value, int target, int step);
int ApproachLinear2(s16& value, s16 target, s16 step);

// @symbol _ZN19dMgMCarlo2CardObj_cC1Ev
dMgMCarlo2CardObj_c::dMgMCarlo2CardObj_c()
    : mPrev(0), mNext(0)
{
}

// @symbol dScMgMCarlo2_c_classInit
extern "C" void* dScMgMCarlo2_c_classInit()
{
    char* p = (char*)_ZN7fBase_cnwEj(0x5930);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        *(int*)p = (int)&_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
        *(int*)p = (int)&_ZTV14dScMgMCarlo2_c[2];
        func_ov006_020c1d80(p + 0x4f38);
        __cxa_vec_ctor(p + 0x51a8, 0x28, 0x30,
                     (void*)_ZN19dMgMCarlo2CardObj_cC1Ev,
                     (void*)_ZN19dMgMCarlo2CardObj_cD1Ev);
    }
    return p;
}

// @symbol _ZN14dScMgMCarlo2_c13InitResourcesEv
/* dScMgMCarlo2_c::InitResources -- vtable slot 0.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgMCarlo2_c, this class's own
 * slot 0 (fBase_c::InitResources). The old file's `recovered name:
 * dScMgMCarlo2_c_InitResources` agreed.
 *
 * The final call is the scene's own slot-18 reset hook. */
s32 dScMgMCarlo2_c::InitResources()
{
    char *c = (char *)this;
    int handle;
    int f1, f2;
    func_ov004_020b04d0(0x20);
    func_ov006_0210a534(c);
    handle = GetGameLanguage();
    f1 = LoadFile(data_ov006_0213d744[handle]);
    f2 = LoadFile(0xbb);
    DecompressLZ16(f1, 0x6400000);
    DecompressLZ16(f1, 0x6600000);
    GX::LoadOBJPltt((void *)f2, 0, 0x100);
    GXS::LoadOBJPltt((void *)f2, 0, 0x100);
    Deallocate((void *)f1);
    Deallocate((void *)f2);
    data_0209d45c = 0x11;
    data_0209d454 = 0x18;
    *(volatile unsigned short *)0x4000008 = (*(volatile unsigned short *)0x4000008 & ~3) | 1;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~3;
    func_ov006_020c0aa8((void *)(c + 0x4660));
    if (func_ov006_020c1a88((void *)(c + 0x4f38)) == 0) return 0;
    func_ov004_020b682c();
    OnYoshiTryEat(-1);
    this->unk_592a = 0;
    this->unk_0a8 = 0xa;
    this->unk_0ac = this->unk_0a8;
    return 1;
}

// @symbol _ZN14dScMgMCarlo2_c13OnYoshiTryEatEi
/* Resets the whole board: rebuild the 40 pieces, clear the match latch,
 * re-arm the shared table, then hand the score display a zero. */
void dScMgMCarlo2_c::OnYoshiTryEat(int /* arg */)
{
    char *c = (char *)this;

  dScMgMCarlo2_c::SetupBoard(mArray);
  data_ov006_0213d6fc = 0;
  unk_592e = 0;
  mShared.unk_1e6 = 1;
  func_ov006_020c1604(c + 0x4f38, 4, 4, c + 0x592e);
  mShared.unk_01a = 1;
  unk_592a = 0;
  func_ov004_020adb1c(0);
  unk_5928 = 1;
}

// @symbol _ZN14dScMgMCarlo2_c13OnTurnIntoEggEi
int dScMgMCarlo2_c::OnTurnIntoEgg(int /* mode */)
{
    char *self = (char *)this;

    short st = *(short *)(self + 0x5928);
    switch (st) {
    case 4:
        if (func_ov006_020c1718((int *)(self + 0x4f38)) != 0) {
            short *p = (short *)(self + 0x5928);
            *p += 1;
        }
        break;
    case 5: {
        unsigned char idx = data_020a0e40[0];
        /* Written as a nested test rather than `found = A && B`, which is what
         * the ROM's code shape says the original was: the cartridge materialises
         * `found` (mov r3,#0 ... movne r3,#1) and then tests it (cmp r3,#0).
         * Fused into one && expression the compiler folds the variable away and
         * branches straight out -- three instructions short. */
        int found = 0;
        if (data_020a0de8[idx * 4] != 0) {
            found = data_020a0de9[idx * 4] != 0;
        }
        if (found != 0) {
            short *p;
            func_02012790(0x62);
            *(short *)(self + 0x592a) = 0x1e;
            dScMgMCarlo2_c::FlipDealtCards();
            p = (short *)(self + 0x5928);
            *p += 1;
        }
        break;
    }
    case 6:
    default:
        if (ApproachLinear2(*(s16 *)(self + 0x592a), 0, 1) != 0) {
            return 1;
        }
        break;
    }
    return 0;
}

// @symbol _ZN14dScMgMCarlo2_c8BehaviorEv
/* dScMgMCarlo2_c::Behavior -- vtable slot 6, ov006 0x020fa13c.
 *
 * Attributed by the vtable: dScMgMCarlo2_c's own table is ov006 0x0213d7e8 and
 * its slot 6 relocates here. The signature is include/dScMgBase_c.h's own slot
 * 6, `virtual s32 Behavior()`.
 *
 * THE STATE COUNTER IS BUMPED THROUGH A LAUNDERED POINTER, three times, and
 * that is not a leftover: `unk_5928++` recomputes the address for the store
 * where `s16 *p = (s16 *)((int)this + 0x5928); (*p)++;` makes mwccarm CSE it
 * into a register, which is what the cartridge does. Same lever
 * notes/mwccarm-codegen.md records for compound assignment. Everything the
 * class owns outright and reads once -- unk_592e, unk_4f52, unk_511e -- reads
 * as a member.
 *
 * The 0x270-byte shared table at 0x4f38 stays opaque, as
 * include/dScMgMCarlo2_c.h's banner says: five siblings use the same one, and
 * the helpers here take its base address rather than anything inside it. The
 * two fields that ARE named within it, unk_4f52 and unk_511e, are named
 * because this function and its siblings write them by hand. */
s32 dScMgMCarlo2_c::Behavior()
{
    unsigned char *c = (unsigned char *)this;

    switch (unk_5928) {
    case 1:
        {
            s16 *p = (s16 *)((int)c + 0x5928);
            (*p)++;
        }
        if (c[0xc4] == 0) {
            c[0xc3] = 1;
            c[0xc4] = 1;
            *(s16 *)(c + 0xc0) = 0;
        }
        /* fall through */
    case 2:
        data_ov006_0213d700 = unk_592e * 5 << 12;
        if (unk_592e == 4) {
            if (dScMgMCarlo2_c::BoardBusy() == 0) {
                unk_592e = 0;
                {
                    s16 *p = (s16 *)((int)c + 0x5928);
                    (*p)++;
                }
            }
        }
        break;
    case 3:
        if (dScMgMCarlo2_c::BoardReady() == 0) {
            int r5;
            if (data_ov006_0213d6fc == 1)
                data_ov006_0213d6fc = 0;
            r5 = data_ov006_0213d700 >> 12;
            if (data_ov006_0213d6f4 != 0 && r5 > 10 && r5 <= 18
                && func_ov006_020c1718(c + 0x4f38) != 0) {
                unk_592e = 0;
                mShared.unk_1e6 = 0;
                func_ov006_020c1164(c + 0x4f38, 2, &unk_592e);
                if (data_ov006_0213d6f4 == 2)
                    mShared.unk_01a = 0;
            } else {
                int lim = unk_592e + 0x12;
                if (r5 >= lim) {
                    int flag = (data_ov006_02142570 != 0 && data_ov006_02142574 != 0);
                    if (flag == 0)
                        data_ov006_0213d700 = lim << 12;
                }
            }
        } else {
            if (data_ov006_0213d6fc == 0) {
                if (dScMgMCarlo2_c::HasRemovablePair() != 0) {
                    data_ov006_0213d6fc = 1;
                } else {
                    if (data_ov006_0213d6f8 != 0) {
                        if (mShared.unk_01a == 1) {
                            mShared.unk_01a = 0;
                        } else if (func_ov006_020c16b4(c + 0x4f38) != 0) {
                            func_ov006_020c0d68(c + 0x4f38);
                            func_ov004_020b0a54(0x12);
                            c[0xc3] = 0;
                            {
                                s16 *p = (s16 *)((int)c + 0x5928);
                                (*p)++;
                            }
                        }
                    }
                    data_ov006_0213d6fc = 0;
                }
            }
        }
        break;
    }

    func_ov006_020c19d0(c + 0x4f38);
    dScMgMCarlo2_c::UpdateBoard();
    return 1;
}

// @symbol _ZN14dScMgMCarlo2_c6RenderEv
/* dScMgMCarlo2_c::Render -- vtable slot 9.
 *
 * THE TWO DRAW PASSES WALK `prev`, NOT `next`. The pre-migration file
 * modelled the element as `struct Node { virtual void m0(); Node *next;
 * char pad[0x18]; int f20; };` -- one link, at offset 0x04. Every other
 * file in this TU puts `prev` at 0x04 and `next` at 0x08, so the link this
 * function follows is `prev`: it starts from data_ov006_02142578 and walks
 * toward the head. Renaming it without re-reading the offset would have
 * changed the emitted load.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgMCarlo2_c, this class's own
 * slot 9 (dScMgBase_c::Render). The old file's `recovered name:
 * dScMgMCarlo2_c_Render` agreed. */
s32 dScMgMCarlo2_c::Render()
{
    char *c = (char *)this;
    short v;
    dMgMCarlo2CardObj_c *n6;
    int i5;
    int i6;
    dMgMCarlo2CardObj_c *n5;

    func_ov006_020c0aa8(c + 0x4660);

    v = data_ov006_02142564;
    if (v > 0x270f)
        v = 0x270f;
    func_ov004_020b1ea4(0xe8, 0x28, v, 1, -1, 0, 0);

    RenderOamMainScreen(data_ov006_02133f18, 0xe8, 0x18, -1, -1);

    if (this->unk_5928 == 5)
        func_ov004_020b0d8c(c, 0xe0, 0xa0);

    n6 = data_ov006_02142578;
    for (i5 = 0; i5 < 0x14; i5++) {
        if (n6 == 0)
            break;
        if (n6->mYStep > 0)
            n6->Render();
        n6 = n6->mPrev;
    }

    n5 = data_ov006_02142578;
    for (i6 = 0; i6 < 0x14; i6++) {
        if (n5 == 0)
            break;
        if (n5->mYStep == 0)
            n5->Render();
        n5 = n5->mPrev;
    }

    func_ov006_020c1804(c + 0x4f38);
    return 1;
}

// @symbol _ZN14dScMgMCarlo2_c16CleanupResourcesEv
/* dScMgMCarlo2_c::CleanupResources -- vtable slot 3, ov006 0x020f9fe0.
 *
 * Attributed by the vtable: dScMgMCarlo2_c's own table is ov006 0x0213d7e8 and
 * its slot 3 relocates here, so this is this class's own override and not one
 * it inherits from dScMgSingle3DBase_c. The signature is include/fBase_c.h's
 * own slot 3, `virtual s32 CleanupResources()`.
 *
 * func_ov004_020ad90c IS ARITY-AMBIGUOUS ACROSS THE FAMILY, and this file does
 * not settle it: dScMgJump2_c's and dScMgTrampoline2_c's CleanupResources both
 * call it with no argument at all, this one passes the scene. `this` is
 * already in r0 at the call, so both spellings produce the same word and
 * neither is evidence. Kept as the pre-migration file had it. */
s32 dScMgMCarlo2_c::CleanupResources()
{
    func_ov004_020ad90c(this);
    return 1;
}

// @symbol _ZN19dMgMCarlo2CardObj_c4InitEi
void dMgMCarlo2CardObj_c::Init(int r1){
    mSlot = (short)r1;
    if (r1 >= 0x14) {
        mDealDelay = 1;
    } else {
        mDealDelay = (short)(((r1 % 5) << 1) + 1);
    }
    mLift = 0;
    if (r1 == 0x13) data_ov006_02142578 = this;
    mVisible = 1;
    mXStep = 0;
    mYStep = 0;
    mState = 0;
    mFace = (unsigned char)dScMgMCarlo2_c::DrawCardValue();
    mNext = 0;
    mPrev = mNext;
}

// @symbol _ZN19dMgMCarlo2CardObj_c6DealInEi
void dMgMCarlo2CardObj_c::DealIn(int a)
{
    int v[3];

    if (a >= 0x14)
        return;

    if (mSlot >= 0x14) {
        mX = (0x70 - ((data_ov006_0213d6f4 >> 2) << 1)) << 12;
        mY = -0x30000;
        data_ov006_02142578 = this;
        mState = 1;
    } else if (mState == 0) {
        mX = (0x70 - ((data_ov006_0213d6f4 >> 2) << 1)) << 12;
        mY = -0x30000;
        mState = 4;
        data_ov006_0214255c++;
    } else {
        mState = 4;
        data_ov006_0214255c++;
    }

    mTargetX = ((a % 5) * 32 + 0x30) << 12;
    mTargetY = ((a / 5) * 0x30) << 12;
    mSlot = (short)a;

    Vec2_Sub(v, &mTargetX, &mX);

    mXStep = v[0];
    mYStep = v[1];
    func_0203d630(&mXStep, 0x124);

    if (mXStep < 0)
        mXStep = -mXStep;
    if (mYStep < 0)
        mYStep = -mYStep;
}

// @symbol _ZN19dMgMCarlo2CardObj_c8FlipAwayEi
void dMgMCarlo2CardObj_c::FlipAway(int n) {
    mDealDelay = (short)((4 - n % 5) * 2);
    mState = 5;
}

// @symbol _ZN19dMgMCarlo2CardObj_c10IsPairWithEPS_
int dMgMCarlo2CardObj_c::IsPairWith(dMgMCarlo2CardObj_c *b)
{
    int ai, bi, dm, dd;
    if (b->mFace != mFace)
        goto fail;
    bi = b->mSlot;
    ai = mSlot;
    dm = ai % 5 - bi % 5;
    dd = ai / 5 - bi / 5;
    if (dm < 0)
        dm = -dm;
    if (dm >= 2)
        goto fail;
    if (dd < 0)
        dd = -dd;
    if (dd >= 2)
        goto fail;
    return 1;
fail:
    return 0;
}

// @symbol _ZN19dMgMCarlo2CardObj_c7HitTestEv
int dMgMCarlo2CardObj_c::HitTest()
{
    u8 idx;
    int off;
    int has;
    int a, b;

    if (data_ov006_0213d6fc == 0) return 0;
    if (dScMgMCarlo2_c::BoardBusy() != 0) goto fail;

    idx = data_020a0e40[0];
    off = idx * 4;
    has = 0;
    if (data_020a0de8[off]) {
        if (data_020a0de9[off]) has = 1;
    }
    if (has == 0) goto fail;

    a = data_020a0dea[idx * 4] - (mX >> 12);
    b = data_020a0deb[idx * 4] - (mY >> 12);
    if (a > 7 && a < 0x28 && b > 0 && b < 0x31) return 1;
fail:
    return 0;
}

// @symbol _ZN19dMgMCarlo2CardObj_c6UpdateEi
void dMgMCarlo2CardObj_c::Update(int b)
{
    switch (mState) {
    case 0:
        {
            short *p = &mDealDelay;
            short v = *(short *)p;
            *(short *)p = (short)(v - 1);
        }
        if (mDealDelay != 0)
            return;
        data_ov006_0213d6f4--;
        DealIn(b);
        return;
    case 2:
        if (mSlot != b) {
            DealIn(b);
            return;
        }
        if (HitTest() == 0)
            return;
        if (data_ov006_02142570 == 0) {
            data_ov006_02142570 = this;
            mState = 3;
            Sound::PlayBank2_2D(0x153);
            return;
        }
        if (IsPairWith(data_ov006_02142570) != 0) {
            data_ov006_02142574 = this;
            data_ov006_02142558 = 0x20;
            mState = 3;
            Sound::PlayBank2_2D(0x154);
            return;
        }
        data_ov006_02142570->mState = 2;
        data_ov006_02142570 = 0;
        func_02012790(0xe);
        return;
    case 3:
        if (HitTest() == 0)
            return;
        mState = 2;
        if (data_ov006_02142570 != this)
            return;
        if (data_ov006_02142574 != 0)
            return;
        Sound::PlayBank2_2D(0x155);
        data_ov006_02142570 = 0;
        return;
    case 1:
    case 4:
        ApproachLinear(mX, mTargetX, mXStep);
        ApproachLinear(mY, mTargetY, mYStep);
        ApproachLinear(mLift, 0x4000, 0x300);
        if (func_0203d5dc(&mX, &mTargetX) != 0)
            return;
        if (mLift != 0x4000)
            return;
        mState = 2;
        ApproachLinear2(data_ov006_0214255c, 0, 1);
        mXStep = 0;
        mYStep = 0;
        return;
    case 5:
        if (ApproachLinear2(mDealDelay, 0, 1) == 0)
            return;
        ApproachLinear(mX, -0x30000, 0x10000);
    }
}

// @symbol _ZN19dMgMCarlo2CardObj_c6RenderEv
void dMgMCarlo2CardObj_c::Render()
{
    unsigned char k;
    if (mVisible == 0) return;
    k = mState;
    if (k == 0) return;
    if (k == 3) {
        if (((&data_020a0db0)[0] & 8) != 0) return;
    }
    {
        int idx = (mFace + 1) * 5 + (mLift >> 12);
        unsigned short e = data_ov006_0213d770[idx];
        Hud_RenderSprite(
            (void*)data_ov006_021425a8[e],
            (mX >> 12) + 0x18,
            (mY >> 12) + 0x18,
            -1,
            -1);
    }
}

// @symbol _ZN14dScMgMCarlo2_c10SetupBoardEP19dMgMCarlo2CardObj_c
void dScMgMCarlo2_c::SetupBoard(dMgMCarlo2CardObj_c* base)
{
    do {
        {
            s16 i = 0;
            do {
                data_ov006_02142580[i] = data_ov006_0212e954[i];
                i = i + 1;
            } while (i < 10);
        }
        {
            s16 j = 0;
            dMgMCarlo2CardObj_c* p = base;
            do {
                p->Init(j);
                p = p + 1;
                j = j + 1;
            } while (j < 0x28);
        }
        data_ov006_02142568 = 0;
        data_ov006_0214257c = base;
        data_ov006_0214256c = 0;
        data_ov006_02142570 = 0;
        data_ov006_02142574 = 0;
        data_ov006_02142558 = 0;
        data_ov006_0214255c = 0;
        data_ov006_0213d6f8 = 0x28;
        data_ov006_0213d6f4 = 0x28;
        data_ov006_02142560 = 0;
        data_ov006_02142564 = 0;
        data_ov006_0213d700 = 0;
        {
            s16 i = 0;
            dMgMCarlo2CardObj_c* h = base;
            do {
                dMgMCarlo2CardObj_c* t = &base[i + 1];
                dMgMCarlo2CardObj_c* save = h->mNext;
                h->mNext = t;
                t->mPrev = h;
                {
                    dMgMCarlo2CardObj_c* u = t->mNext;
                    if (u != 0) {
                        do { t = u; u = u->mNext; } while (u != 0);
                    }
                }
                t->mNext = save;
                if (t->mNext == 0) data_ov006_02142568 = t;
                h = h + 1;
                i++;
            } while (i < 0x27);
        }
    } while (HasRemovablePair() == 0);
}

// @symbol _ZN14dScMgMCarlo2_c16HasRemovablePairEv
int dScMgMCarlo2_c::HasRemovablePair()
{
    dMgMCarlo2CardObj_c *p, *q;
    p = data_ov006_0214257c;
    while (p != 0 && p->mSlot < 0x14) {
        q = p->mNext;
        while (q != 0 && q->mSlot < 0x14) {
            if (p->IsPairWith(q) != 0) return 1;
            q = q->mNext;
        }
        p = p->mNext;
    }
    return 0;
}

// @symbol _ZN14dScMgMCarlo2_c9BoardBusyEv
int dScMgMCarlo2_c::BoardBusy() {
    int ret = 1;
    int a = (data_ov006_0213d700 << 4) >> 0x10;
    int b = data_ov006_0213d6f8;
    if (b > 0x14) b = 0x14;
    if (a == b && data_ov006_0214255c == 0) {
        if (data_ov006_02142570 == 0 || data_ov006_02142574 == 0) {
            ret = 0;
        }
    }
    return ret;
}

// @symbol _ZN14dScMgMCarlo2_c10BoardReadyEv
/* One expression, one exit. The cartridge keeps the result pinned in r0 for
 * the whole body (mov r0,#0 up front, mov r0,#1 on the one success path);
 * spelling the guards as early `return 0`s instead lets the compiler
 * rematerialise the zero at each exit, which is the extra 8 bytes. */
int dScMgMCarlo2_c::BoardReady() {
    int a = (data_ov006_0213d700 << 4) >> 16;
    int r = 0;
    int b = data_ov006_0213d6f8;

    if (b > 0x14) {
        b = 0x14;
    }
    if (a == b && data_ov006_0214255c == 0
        && (data_ov006_02142570 == 0 || data_ov006_02142574 == 0)) {
        r = 1;
    }
    return r;
}

// @symbol _ZN14dScMgMCarlo2_c13DrawCardValueEv
int dScMgMCarlo2_c::DrawCardValue(){
    unsigned char pick = 0;
    int total = 0;
    int i;
    int r;
    for(i=0;i<0xa;i++) total += data_ov006_02142580[i];
    r = (int)(((unsigned int)RandomIntInternal(data_0209e650) & 0x7fffffff) >> 0x13);
    total = (total * r) >> 0xc;
    for(i=0;i<0xa;i++){
        total -= data_ov006_02142580[i];
        if(total < 0){
            pick = (unsigned char)i;
            data_ov006_02142580[i]--;
            break;
        }
    }
    return pick;
}

// @symbol _ZN14dScMgMCarlo2_c14FlipDealtCardsEv
void dScMgMCarlo2_c::FlipDealtCards(){
    dMgMCarlo2CardObj_c* node = data_ov006_0214257c;
    short i = 0;
    if((data_ov006_0213d700>>12) <= 0) return;
    do {
        if(node == 0) return;
        node->FlipAway(i);
        i = i+1;
        node = node->mNext;
    } while(i < (data_ov006_0213d700>>12));
}

// @symbol _ZN14dScMgMCarlo2_c11UpdateBoardEv
void dScMgMCarlo2_c::UpdateBoard()
{
    dMgMCarlo2CardObj_c* head = data_ov006_0214257c;

    if (data_ov006_02142570 != 0 && data_ov006_02142574 != 0) {
        data_ov006_02142558 -= 1;
        if (data_ov006_02142558 == 0) {
            dMgMCarlo2CardObj_c* a;
            dMgMCarlo2CardObj_c* b;
            dMgMCarlo2CardObj_c* t;
            dMgMCarlo2CardObj_c* u;
            s16 kb = data_ov006_02142574->mSlot;
            s16 ka = data_ov006_02142570->mSlot;
            if (ka > kb)
                data_ov006_0213d700 = kb << 12;
            else
                data_ov006_0213d700 = ka << 12;

            a = data_ov006_02142570;
            if (data_ov006_0214257c == a) data_ov006_0214257c = a->mNext;
            if (data_ov006_0214256c == a) data_ov006_0214256c = a->mNext;
            if (data_ov006_02142578 == a) data_ov006_02142578 = a->mPrev;
            if (data_ov006_02142568 == a) data_ov006_02142568 = a->mPrev;
            if (a->mPrev != 0) a->mPrev->mNext = a->mNext;
            if (a->mNext != 0) a->mNext->mPrev = a->mPrev;
            a->mNext = 0;
            a->mPrev = a->mNext;

            b = data_ov006_02142574;
            if (data_ov006_0214257c == b) data_ov006_0214257c = b->mNext;
            if (data_ov006_0214256c == b) data_ov006_0214256c = b->mNext;
            if (data_ov006_02142578 == b) data_ov006_02142578 = b->mPrev;
            if (data_ov006_02142568 == b) data_ov006_02142568 = b->mPrev;
            if (b->mPrev != 0) b->mPrev->mNext = b->mNext;
            if (b->mNext != 0) b->mNext->mPrev = b->mPrev;
            b->mNext = 0;
            b->mPrev = b->mNext;

            a = data_ov006_02142570;
            t = data_ov006_0214256c;
            if (t == 0) {
                data_ov006_0214256c = a;
            } else {
                u = t->mNext;
                if (u != 0) {
                    do { t = u; u = u->mNext; } while (u != 0);
                }
                t->mNext = a;
                a->mPrev = t;
                a->mNext = 0;
            }

            b = data_ov006_02142574;
            t = data_ov006_0214256c;
            if (t == 0) {
                data_ov006_0214256c = b;
            } else {
                u = t->mNext;
                if (u != 0) {
                    do { t = u; u = u->mNext; } while (u != 0);
                }
                t->mNext = b;
                b->mPrev = t;
                b->mNext = 0;
            }

            data_ov006_02142574 = 0;
            data_ov006_02142570 = 0;
            ApproachLinear2(data_ov006_02142564, 0x270f, 2);
            ApproachLinear2(data_ov006_0213d6f8, 0, 2);
            func_ov004_020adb1c(data_ov006_02142564);

            if (data_ov006_0213d6f8 <= 0x14) {
                {
                    int i = 0;
                    s16 idx = *(volatile s16*)&data_ov006_02142560;
                    int* row = (int*)((char*)data_ov006_0212e97c + idx * 0x28);
                    do {
                        data_ov006_02142580[i] = data_ov006_02142580[i] + row[i];
                        i += 1;
                    } while (i < 10);
                }
                {
                    dMgMCarlo2CardObj_c* p = data_ov006_0214256c;
                    if (p != 0) {
                        do {
                            dMgMCarlo2CardObj_c* nxt = p->mNext;
                            if (data_ov006_0214257c == p) data_ov006_0214257c = nxt;
                            if (data_ov006_0214256c == p) data_ov006_0214256c = p->mNext;
                            if (data_ov006_02142578 == p) data_ov006_02142578 = p->mPrev;
                            if (data_ov006_02142568 == p) data_ov006_02142568 = p->mPrev;
                            if (p->mPrev != 0) p->mPrev->mNext = p->mNext;
                            if (p->mNext != 0) p->mNext->mPrev = p->mPrev;
                            p->mNext = 0;
                            p->mPrev = p->mNext;
                            p->Init((s16)(data_ov006_02142568->mSlot + 1));
                            {
                                dMgMCarlo2CardObj_c* h = data_ov006_02142568;
                                dMgMCarlo2CardObj_c* save = h->mNext;
                                h->mNext = p;
                                p->mPrev = h;
                                if (p->mNext != 0) {
                                    do { p = *(dMgMCarlo2CardObj_c* volatile*)&p->mNext; } while (p->mNext != 0);
                                }
                                p->mNext = save;
                                if (p->mNext == 0) data_ov006_02142568 = p;
                            }
                            p = nxt;
                        } while (p != 0);
                    }
                }
                ApproachLinear2(data_ov006_02142560, 9, 1);
                data_ov006_0213d6f4 += 0x14;
                data_ov006_0213d6f8 += 0x14;
            }
        }
        {
            s16 i = 0;
            if ((data_ov006_0213d700 >> 12) <= 0) return;
            do {
                if (head == 0) return;
                if (head->mState == 1)
                    head->Update(i);
                i = i + 1;
                head = head->mNext;
            } while (i < (data_ov006_0213d700 >> 12));
        }
    } else {
        int v = data_ov006_0213d6f8;
        if (v > 0x14) v = 0x14;
        ApproachLinear(data_ov006_0213d700, v << 12, 0x800);
        {
            s16 i = 0;
            if ((data_ov006_0213d700 >> 12) <= 0) return;
            do {
                if (head == 0) return;
                head->Update(i);
                i = i + 1;
                head = head->mNext;
            } while (i < (data_ov006_0213d700 >> 12));
        }
    }
}

// @symbol _ZN19dMgMCarlo2CardObj_cD1Ev
dMgMCarlo2CardObj_c::~dMgMCarlo2CardObj_c()
{
}

/* --------------------------------------------------------------------------
 * ROM ordinals 0 and 1 -- _ZN14dScMgMCarlo2_cD1Ev at 0x020f8ef4 (0x74)
 *                     and _ZN14dScMgMCarlo2_cD0Ev at 0x020f8f68 (0x88).
 *
 * Neither is written out here. ~dScMgMCarlo2_c() is defined inline in the
 * class body (include/dScMgMCarlo2_c.h), so it cannot be this TU's key
 * function; InitResources, the next virtual declared and non-inline, is, and
 * this TU defines it. That emits _ZTV14dScMgMCarlo2_c right here, whose slots
 * 16 and 17 name D1 and D0, odr-using both, so the compiler emits the pair out
 * of line for us -- in cartridge order. D0's extra 0x14 bytes over D1 are the
 * `operator delete` tail dScMgBase_c already provides.
 *
 * Written out of line instead, mwcc emits the synthesized D0 AHEAD of the
 * written D1, and linkcheck's pre-link audit refuses a TU whose licensed .text
 * is not in ROM address order. Inlining also removes the homeless D2 that an
 * out-of-line definition would emit with no ROM address to claim.
 * -------------------------------------------------------------------------- */
