//cpp
/* Translation unit ov006/dScMgMCarlo2_c  (25 function(s)).
 * Reconstructed with tools/tubuild.py create, then reconciled by hand.
 *
 * ENROLLED AND CANONICAL. config/arm9/overlays/ov006/delinks.txt names this
 * one file for the whole .text range 0x020f8ef4..0x020fa75c, so every byte
 * of that range in the retail overlay is built from the source below -- the
 * 25 legacy functions it replaced are gone. The adjacent factory and element
 * constructor are included because the factory profile and this class's RTTI,
 * vtables, initializer, data, and BSS form one continuous ownership cluster.
 * byte-exact, the linked module byte-identical to the cartridge, and the full
 * ROM identical to the stock build. See the measurements in
 * config/tu_manifest.d/ov006/dScMgMCarlo2_c+MgPairAGoneAndOn.json.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020f8ef4  src/_ZN14dScMgMCarlo2_cD1Ev.cpp
 *   [1] 0x020f8f68  src/_ZN14dScMgMCarlo2_cD0Ev.cpp
 *   [2] 0x020f8ff0  src/func_ov006_020f8ff0.c
 *   [3] 0x020f9000  src/func_ov006_020f9000.cpp
 *   [4] 0x020f94f4  src/func_ov006_020f94f4.c
 *   [5] 0x020f9560  src/func_ov006_020f9560.c
 *   [6] 0x020f95f0  src/func_ov006_020f95f0.c
 *   [7] 0x020f9668  src/func_ov006_020f9668.c
 *   [8] 0x020f96e0  src/func_ov006_020f96e0.c
 *   [9] 0x020f9760  src/func_ov006_020f9760.c
 *   [10] 0x020f98dc  src/func_ov006_020f98dc.c
 *   [11] 0x020f9994  src/func_ov006_020f9994.c
 *   [12] 0x020f9bec  src/func_ov006_020f9bec.c
 *   [13] 0x020f9cbc  src/func_ov006_020f9cbc.c
 *   [14] 0x020f9d68  src/func_ov006_020f9d68.c
 *   [15] 0x020f9db8  src/func_ov006_020f9db8.c
 *   [16] 0x020f9f40  src/func_ov006_020f9f40.c
 *   [17] 0x020f9fe0  src/_ZN14dScMgMCarlo2_c16CleanupResourcesEv.cpp
 *   [18] 0x020f9ffc  src/_ZN14dScMgMCarlo2_c6RenderEv.cpp
 *   [19] 0x020fa13c  src/_ZN14dScMgMCarlo2_c8BehaviorEv.cpp
 *   [20] 0x020fa3d0  src/func_ov006_020fa3d0.c
 *   [21] 0x020fa4d4  src/func_ov006_020fa4d4.cpp
 *   [22] 0x020fa56c  src/_ZN14dScMgMCarlo2_c13InitResourcesEv.cpp
 *   [23] 0x020fa6ac  src/MgPairAGoneAndOn_Spawn.cpp
 *   [24] 0x020fa740  src/func_ov006_020fa740.c
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
 * Neither is carried. The measured source forms require propagation ON
 * to match all 25 functions in one translation unit. opt_propagation is file-global
 * last-wins, so a TU has exactly one setting for all 25 functions, and there
 * is only one place it could go -- the top. Set there it costs four OTHER
 * members their match (ordinals 3, 5, 10 and 18), and prepending it to the
 * unmodified legacy sources of those four breaks them there too. So `off`
 * cannot be this TU's setting; it was a per-file crutch for two bodies
 * whose shape had been tuned against it one file at a time.
 *
 * Both members verify byte-exact with propagation ON once their source is
 * shaped as documented below. That
 * is the load-bearing evidence: a single setting, no pragma, all bodies exact.
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
/* Remaining reconstruction: the factory still spells runtime construction
 * explicitly, the shared state and inherited camera remain partly opaque,
 * and the scene state/timer fields and data_* globals need stronger names.
 * The BoardReady branch shape and OnTurnIntoEgg touch predicate below retain
 * measured matching constraints. Promotion alone does not complete this work. */

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

int  RandomIntInternal(int* seed);
void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);
int  GetGameLanguage(void);
int  LoadFile(int handle);
void DecompressLZ16(void* src, void* dst);
void Vec2_Sub(int* out, int* a, int* b);
unsigned int func_02012790(unsigned int id);
int  func_0203d5dc(void* a, void* b);
void func_ov004_020ad90c(void);
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
typedef void (*ArrayCtor)(void*);
typedef void (*ArrayDtor)(void*);
void __cxa_vec_ctor(void*, unsigned int, unsigned int, ArrayCtor, ArrayDtor);
extern int _ZTV19dScMgSingle3DBase_c;
extern int _ZTV14dScMgMCarlo2_c[];
void _ZN19dMgMCarlo2CardObj_cC1Ev(void*);
}

namespace GX { void LoadOBJPltt(const void*, unsigned int, unsigned int); }
namespace GXS { void LoadOBJPltt(const void*, unsigned int, unsigned int); }
namespace Sound { unsigned int PlayBank2_2D(unsigned int); }

/* The configured C++ symbol spellings correspond to these signatures:
 * _Z14ApproachLinearRiii is ApproachLinear(int&, int, int) and
 * _Z15ApproachLinear2Rsss is ApproachLinear2(short&, short, short). Declared
 * with C++ linkage -- the reference parameter is what mangles them. */
int ApproachLinear(int& value, int target, int step);
int ApproachLinear2(s16& value, s16 target, s16 step);

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- dMgMCarlo2CardObj_c::dMgMCarlo2CardObj_c, 0x020fa740 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19dMgMCarlo2CardObj_cC1Ev
dMgMCarlo2CardObj_c::dMgMCarlo2CardObj_c()
    : mPrev(0), mNext(0)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- dScMgMCarlo2_c_classInit, 0x020fa6ac, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol dScMgMCarlo2_c_classInit
extern "C" void* dScMgMCarlo2_c_classInit()
{
    char* scene = (char*)_ZN7fBase_cnwEj(0x5930);
    if (scene) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(int*)scene = (int)&_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(scene + 0x471c);
        *(int*)scene = (int)&_ZTV14dScMgMCarlo2_c[2];
        func_ov006_020c1d80(scene + 0x4f38);
        __cxa_vec_ctor(scene + 0x51a8, 0x28, 0x30,
                     _ZN19dMgMCarlo2CardObj_cC1Ev,
                     _ZN19dMgMCarlo2CardObj_cD1Ev);
    }
    return scene;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- _ZN14dScMgMCarlo2_c13InitResourcesEv, 0x020fa56c, size 0x140 */
/* -------------------------------------------------------------------------- */
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
    int language;
    int spriteData, paletteData;
    func_ov004_020b04d0(0x20);
    func_ov006_0210a534(this);
    language = GetGameLanguage();
    spriteData = LoadFile(data_ov006_0213d744[language]);
    paletteData = LoadFile(0xbb);
    DecompressLZ16((void*)spriteData, (void*)0x6400000);
    DecompressLZ16((void*)spriteData, (void*)0x6600000);
    GX::LoadOBJPltt((void *)paletteData, 0, 0x100);
    GXS::LoadOBJPltt((void *)paletteData, 0, 0x100);
    Deallocate((void *)spriteData);
    Deallocate((void *)paletteData);
    data_0209d45c = 0x11;
    data_0209d454 = 0x18;
    *(volatile unsigned short *)0x4000008 = (*(volatile unsigned short *)0x4000008 & ~3) | 1;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~3;
    func_ov006_020c0aa8((char*)this + 0x4660);
    if (func_ov006_020c1a88(&mShared) == 0) return 0;
    func_ov004_020b682c();
    OnYoshiTryEat(-1);
    this->unk_592a = 0;
    this->unk_0a8 = 0xa;
    this->unk_0ac = this->unk_0a8;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN14dScMgMCarlo2_c13OnYoshiTryEatEi, 0x020fa4d4, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c13OnYoshiTryEatEi
// recovered name: dScMgMCarlo2_c_OnYoshiTryEat_020fa4d4
/* Resets the whole board: rebuild the 40 pieces, clear the match latch,
 * re-arm the shared table, then hand the score display a zero. */
void dScMgMCarlo2_c::OnYoshiTryEat(int /* arg */)
{
    dScMgMCarlo2_c::SetupBoard(mArray);
    data_ov006_0213d6fc = 0;
    unk_592e = 0;
    mShared.unk_1e6 = 1;
    func_ov006_020c1604((char*)&mShared, 4, 4, &unk_592e);
    mShared.unk_01a = 1;
    unk_592a = 0;
    func_ov004_020adb1c(0);
    unk_5928 = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN14dScMgMCarlo2_c13OnTurnIntoEggEi, 0x020fa3d0, size 0x104 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c13OnTurnIntoEggEi
int dScMgMCarlo2_c::OnTurnIntoEgg(int /* mode */)
{
    short state = unk_5928;
    switch (state) {
    case 4:
        if (func_ov006_020c1718(&mShared) != 0) {
            ++unk_5928;
        }
        break;
    case 5: {
        unsigned char inputIndex = data_020a0e40[0];
        /* Keep the materialized touch predicate. With this typed TU under
         * 2004/b56, combining the two tests into && grows this function from
         * 0x104 to 0x108 bytes and changes its branches. */
        int found = 0;
        if (data_020a0de8[inputIndex * 4] != 0) {
            found = data_020a0de9[inputIndex * 4] != 0;
        }
        if (found != 0) {
            func_02012790(0x62);
            unk_592a = 0x1e;
            dScMgMCarlo2_c::FlipDealtCards();
            ++unk_5928;
        }
        break;
    }
    case 6:
    default:
        if (ApproachLinear2(unk_592a, 0, 1) != 0) {
            return 1;
        }
        break;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN14dScMgMCarlo2_c8BehaviorEv, 0x020fa13c, size 0x294 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c8BehaviorEv
/* dScMgMCarlo2_c::Behavior -- vtable slot 6, ov006 0x020fa13c.
 *
 * Attributed by the vtable: dScMgMCarlo2_c's own table is ov006 0x0213d7e8 and
 * its slot 6 relocates here. The signature is include/dScMgBase_c.h's own slot
 * 6, `virtual s32 Behavior()`.
 *
 * The shared 0x270-byte state at 0x4f38 is represented by mShared. Only
 * unk_01a and unk_1e6 are modeled fields; helper calls receive its base.
 * Direct increments of unk_5928 now reproduce the unchanged production object
 * under 2004/b56; the older pointer-increment workaround is unnecessary in
 * this typed source form. */
s32 dScMgMCarlo2_c::Behavior()
{
    switch (unk_5928) {
    case 1:
        ++unk_5928;
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        /* fall through */
    case 2:
        data_ov006_0213d700 = unk_592e * 5 << 12;
        if (unk_592e == 4) {
            if (dScMgMCarlo2_c::BoardBusy() == 0) {
                unk_592e = 0;
                ++unk_5928;
            }
        }
        break;
    case 3:
        if (dScMgMCarlo2_c::BoardReady() == 0) {
            int dealtCount;
            if (data_ov006_0213d6fc == 1)
                data_ov006_0213d6fc = 0;
            dealtCount = data_ov006_0213d700 >> 12;
            if (data_ov006_0213d6f4 != 0 && dealtCount > 10 && dealtCount <= 18
                && func_ov006_020c1718(&mShared) != 0) {
                unk_592e = 0;
                mShared.unk_1e6 = 0;
                func_ov006_020c1164(&mShared, 2, &unk_592e);
                if (data_ov006_0213d6f4 == 2)
                    mShared.unk_01a = 0;
            } else {
                int dealLimit = unk_592e + 0x12;
                if (dealtCount >= dealLimit) {
                    int pairSelected = (data_ov006_02142570 != 0 && data_ov006_02142574 != 0);
                    if (pairSelected == 0)
                        data_ov006_0213d700 = dealLimit << 12;
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
                        } else if (func_ov006_020c16b4(&mShared) != 0) {
                            func_ov006_020c0d68(&mShared);
                            func_ov004_020b0a54(0x12);
                            mPromptEnabled = 0;
                            ++unk_5928;
                        }
                    }
                    data_ov006_0213d6fc = 0;
                }
            }
        }
        break;
    }

    func_ov006_020c19d0(&mShared);
    dScMgMCarlo2_c::UpdateBoard();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN14dScMgMCarlo2_c6RenderEv, 0x020f9ffc, size 0x140 */
/* -------------------------------------------------------------------------- */
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
    short score;
    dMgMCarlo2CardObj_c *movingCard;
    int movingSlot;
    int settledSlot;
    dMgMCarlo2CardObj_c *settledCard;

    func_ov006_020c0aa8((char*)this + 0x4660);

    score = data_ov006_02142564;
    if (score > 0x270f)
        score = 0x270f;
    func_ov004_020b1ea4(0xe8, 0x28, score, 1, -1, 0, 0);

    RenderOamMainScreen(data_ov006_02133f18, 0xe8, 0x18, -1, -1);

    if (this->unk_5928 == 5)
        func_ov004_020b0d8c(this, 0xe0, 0xa0);

    movingCard = data_ov006_02142578;
    for (movingSlot = 0; movingSlot < 0x14; movingSlot++) {
        if (movingCard == 0)
            break;
        if (movingCard->mYStep > 0)
            movingCard->Render();
        movingCard = movingCard->mPrev;
    }

    settledCard = data_ov006_02142578;
    for (settledSlot = 0; settledSlot < 0x14; settledSlot++) {
        if (settledCard == 0)
            break;
        if (settledCard->mYStep == 0)
            settledCard->Render();
        settledCard = settledCard->mPrev;
    }

    func_ov006_020c1804(&mShared);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN14dScMgMCarlo2_c16CleanupResourcesEv, 0x020f9fe0, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c16CleanupResourcesEv
/* dScMgMCarlo2_c::CleanupResources -- vtable slot 3, ov006 0x020f9fe0.
 *
 * Attributed by the vtable: dScMgMCarlo2_c's own table is ov006 0x0213d7e8 and
 * its slot 3 relocates here, so this is this class's own override and not one
 * it inherits from dScMgSingle3DBase_c. The signature is include/fBase_c.h's
 * own slot 3, `virtual s32 CleanupResources()`.
 *
 * func_ov004_020ad90c uses the current scene global. Its existing definition
 * takes no argument; the call here follows that contract. */
s32 dScMgMCarlo2_c::CleanupResources()
{
    func_ov004_020ad90c();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- dMgMCarlo2CardObj_c::Init, 0x020f9f40, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19dMgMCarlo2CardObj_c4InitEi
void dMgMCarlo2CardObj_c::Init(int slot){
    mSlot = (short)slot;
    if (slot >= 0x14) {
        mDealDelay = 1;
    } else {
        mDealDelay = (short)(((slot % 5) << 1) + 1);
    }
    mLift = 0;
    if (slot == 0x13) data_ov006_02142578 = this;
    mVisible = 1;
    mXStep = 0;
    mYStep = 0;
    mState = 0;
    mFace = (unsigned char)dScMgMCarlo2_c::DrawCardValue();
    mNext = 0;
    mPrev = mNext;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- dMgMCarlo2CardObj_c::DealIn, 0x020f9db8, size 0x188 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19dMgMCarlo2CardObj_c6DealInEi
void dMgMCarlo2CardObj_c::DealIn(int slot)
{
    int targetDelta[3];

    if (slot >= 0x14)
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

    mTargetX = ((slot % 5) * 32 + 0x30) << 12;
    mTargetY = ((slot / 5) * 0x30) << 12;
    mSlot = (short)slot;

    Vec2_Sub(targetDelta, &mTargetX, &mX);

    mXStep = targetDelta[0];
    mYStep = targetDelta[1];
    func_0203d630(&mXStep, 0x124);

    if (mXStep < 0)
        mXStep = -mXStep;
    if (mYStep < 0)
        mYStep = -mYStep;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- dMgMCarlo2CardObj_c::FlipAway, 0x020f9d68, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19dMgMCarlo2CardObj_c8FlipAwayEi
void dMgMCarlo2CardObj_c::FlipAway(int slot) {
    mDealDelay = (short)((4 - slot % 5) * 2);
    mState = 5;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- dMgMCarlo2CardObj_c::IsPairWith, 0x020f9cbc, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19dMgMCarlo2CardObj_c10IsPairWithEPS_
int dMgMCarlo2CardObj_c::IsPairWith(dMgMCarlo2CardObj_c *other)
{
    int slot, otherSlot, columnDistance, rowDistance;
    if (other->mFace != mFace)
        goto fail;
    otherSlot = other->mSlot;
    slot = mSlot;
    columnDistance = slot % 5 - otherSlot % 5;
    rowDistance = slot / 5 - otherSlot / 5;
    if (columnDistance < 0)
        columnDistance = -columnDistance;
    if (columnDistance >= 2)
        goto fail;
    if (rowDistance < 0)
        rowDistance = -rowDistance;
    if (rowDistance >= 2)
        goto fail;
    return 1;
fail:
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- dMgMCarlo2CardObj_c::HitTest, 0x020f9bec, size 0xd0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19dMgMCarlo2CardObj_c7HitTestEv
int dMgMCarlo2CardObj_c::HitTest()
{
    u8 inputIndex;
    int inputOffset;
    int touchActive;
    int localX, localY;

    if (data_ov006_0213d6fc == 0) return 0;
    if (dScMgMCarlo2_c::BoardBusy() != 0) goto fail;

    inputIndex = data_020a0e40[0];
    inputOffset = inputIndex * 4;
    touchActive = 0;
    if (data_020a0de8[inputOffset]) {
        if (data_020a0de9[inputOffset]) touchActive = 1;
    }
    if (touchActive == 0) goto fail;

    localX = data_020a0dea[inputIndex * 4] - (mX >> 12);
    localY = data_020a0deb[inputIndex * 4] - (mY >> 12);
    if (localX > 7 && localX < 0x28 && localY > 0 && localY < 0x31) return 1;
fail:
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- dMgMCarlo2CardObj_c::Update, 0x020f9994, size 0x258 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19dMgMCarlo2CardObj_c6UpdateEi
void dMgMCarlo2CardObj_c::Update(int slot)
{
    switch (mState) {
    case 0:
        --mDealDelay;
        if (mDealDelay != 0)
            return;
        data_ov006_0213d6f4--;
        DealIn(slot);
        return;
    case 2:
        if (mSlot != slot) {
            DealIn(slot);
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- dMgMCarlo2CardObj_c::Render, 0x020f98dc, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19dMgMCarlo2CardObj_c6RenderEv
void dMgMCarlo2CardObj_c::Render()
{
    unsigned char state;
    if (mVisible == 0) return;
    state = mState;
    if (state == 0) return;
    if (state == 3) {
        if ((data_020a0db0 & 8) != 0) return;
    }
    {
        int frameIndex = (mFace + 1) * 5 + (mLift >> 12);
        unsigned short spriteIndex = data_ov006_0213d770[frameIndex];
        Hud_RenderSprite(
            (void*)data_ov006_021425a8[spriteIndex],
            (mX >> 12) + 0x18,
            (mY >> 12) + 0x18,
            -1,
            -1);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- dScMgMCarlo2_c::SetupBoard, 0x020f9760, size 0x17c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c10SetupBoardEP19dMgMCarlo2CardObj_c
void dScMgMCarlo2_c::SetupBoard(dMgMCarlo2CardObj_c* cards)
{
    do {
        {
            s16 index = 0;
            do {
                data_ov006_02142580[index] = data_ov006_0212e954[index];
                index = index + 1;
            } while (index < 10);
        }
        {
            s16 slot = 0;
            dMgMCarlo2CardObj_c* card = cards;
            do {
                card->Init(slot);
                card = card + 1;
                slot = slot + 1;
            } while (slot < 0x28);
        }
        data_ov006_02142568 = 0;
        data_ov006_0214257c = cards;
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
            s16 index = 0;
            dMgMCarlo2CardObj_c* card = cards;
            do {
                dMgMCarlo2CardObj_c* tail = &cards[index + 1];
                dMgMCarlo2CardObj_c* savedNext = card->mNext;
                card->mNext = tail;
                tail->mPrev = card;
                {
                    dMgMCarlo2CardObj_c* next = tail->mNext;
                    if (next != 0) {
                        do { tail = next; next = next->mNext; } while (next != 0);
                    }
                }
                tail->mNext = savedNext;
                if (tail->mNext == 0) data_ov006_02142568 = tail;
                card = card + 1;
                index++;
            } while (index < 0x27);
        }
    } while (HasRemovablePair() == 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- dScMgMCarlo2_c::HasRemovablePair, 0x020f96e0, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c16HasRemovablePairEv
int dScMgMCarlo2_c::HasRemovablePair()
{
    dMgMCarlo2CardObj_c *card, *other;
    card = data_ov006_0214257c;
    while (card != 0 && card->mSlot < 0x14) {
        other = card->mNext;
        while (other != 0 && other->mSlot < 0x14) {
            if (card->IsPairWith(other) != 0) return 1;
            other = other->mNext;
        }
        card = card->mNext;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- dScMgMCarlo2_c::BoardBusy, 0x020f9668, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c9BoardBusyEv
int dScMgMCarlo2_c::BoardBusy() {
    int busy = 1;
    int dealtCount = (data_ov006_0213d700 << 4) >> 0x10;
    int visibleCount = data_ov006_0213d6f8;
    if (visibleCount > 0x14) visibleCount = 0x14;
    if (dealtCount == visibleCount && data_ov006_0214255c == 0) {
        if (data_ov006_02142570 == 0 || data_ov006_02142574 == 0) {
            busy = 0;
        }
    }
    return busy;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- dScMgMCarlo2_c::BoardReady, 0x020f95f0, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c10BoardReadyEv
/* One expression, one exit. The cartridge keeps the result pinned in r0 for
 * the whole body (mov r0,#0 up front, mov r0,#1 on the one success path);
 * spelling the guards as early `return 0`s instead lets the compiler
 * rematerialise the zero at each exit, which is the extra 8 bytes. */
int dScMgMCarlo2_c::BoardReady() {
    int dealtCount = (data_ov006_0213d700 << 4) >> 16;
    int ready = 0;
    int visibleCount = data_ov006_0213d6f8;

    if (visibleCount > 0x14) {
        visibleCount = 0x14;
    }
    if (dealtCount == visibleCount && data_ov006_0214255c == 0
        && (data_ov006_02142570 == 0 || data_ov006_02142574 == 0)) {
        ready = 1;
    }
    return ready;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- dScMgMCarlo2_c::DrawCardValue, 0x020f9560, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c13DrawCardValueEv
int dScMgMCarlo2_c::DrawCardValue(){
    unsigned char pick = 0;
    int total = 0;
    int face;
    int randomFraction;
    for(face=0;face<0xa;face++) total += data_ov006_02142580[face];
    randomFraction = (int)(((unsigned int)RandomIntInternal(data_0209e650) & 0x7fffffff) >> 0x13);
    total = (total * randomFraction) >> 0xc;
    for(face=0;face<0xa;face++){
        total -= data_ov006_02142580[face];
        if(total < 0){
            pick = (unsigned char)face;
            data_ov006_02142580[face]--;
            break;
        }
    }
    return pick;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- dScMgMCarlo2_c::FlipDealtCards, 0x020f94f4, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c14FlipDealtCardsEv
void dScMgMCarlo2_c::FlipDealtCards(){
    dMgMCarlo2CardObj_c* card = data_ov006_0214257c;
    short slot = 0;
    if((data_ov006_0213d700>>12) <= 0) return;
    do {
        if(card == 0) return;
        card->FlipAway(slot);
        slot = slot+1;
        card = card->mNext;
    } while(slot < (data_ov006_0213d700>>12));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- dScMgMCarlo2_c::UpdateBoard, 0x020f9000, size 0x4f4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMCarlo2_c11UpdateBoardEv
void dScMgMCarlo2_c::UpdateBoard()
{
    dMgMCarlo2CardObj_c* cardCursor = data_ov006_0214257c;

    if (data_ov006_02142570 != 0 && data_ov006_02142574 != 0) {
        data_ov006_02142558 -= 1;
        if (data_ov006_02142558 == 0) {
            dMgMCarlo2CardObj_c* firstCard;
            dMgMCarlo2CardObj_c* secondCard;
            dMgMCarlo2CardObj_c* tail;
            dMgMCarlo2CardObj_c* next;
            s16 secondSlot = data_ov006_02142574->mSlot;
            s16 firstSlot = data_ov006_02142570->mSlot;
            if (firstSlot > secondSlot)
                data_ov006_0213d700 = secondSlot << 12;
            else
                data_ov006_0213d700 = firstSlot << 12;

            firstCard = data_ov006_02142570;
            if (data_ov006_0214257c == firstCard) data_ov006_0214257c = firstCard->mNext;
            if (data_ov006_0214256c == firstCard) data_ov006_0214256c = firstCard->mNext;
            if (data_ov006_02142578 == firstCard) data_ov006_02142578 = firstCard->mPrev;
            if (data_ov006_02142568 == firstCard) data_ov006_02142568 = firstCard->mPrev;
            if (firstCard->mPrev != 0) firstCard->mPrev->mNext = firstCard->mNext;
            if (firstCard->mNext != 0) firstCard->mNext->mPrev = firstCard->mPrev;
            firstCard->mNext = 0;
            firstCard->mPrev = firstCard->mNext;

            secondCard = data_ov006_02142574;
            if (data_ov006_0214257c == secondCard) data_ov006_0214257c = secondCard->mNext;
            if (data_ov006_0214256c == secondCard) data_ov006_0214256c = secondCard->mNext;
            if (data_ov006_02142578 == secondCard) data_ov006_02142578 = secondCard->mPrev;
            if (data_ov006_02142568 == secondCard) data_ov006_02142568 = secondCard->mPrev;
            if (secondCard->mPrev != 0) secondCard->mPrev->mNext = secondCard->mNext;
            if (secondCard->mNext != 0) secondCard->mNext->mPrev = secondCard->mPrev;
            secondCard->mNext = 0;
            secondCard->mPrev = secondCard->mNext;

            firstCard = data_ov006_02142570;
            tail = data_ov006_0214256c;
            if (tail == 0) {
                data_ov006_0214256c = firstCard;
            } else {
                next = tail->mNext;
                if (next != 0) {
                    do { tail = next; next = next->mNext; } while (next != 0);
                }
                tail->mNext = firstCard;
                firstCard->mPrev = tail;
                firstCard->mNext = 0;
            }

            secondCard = data_ov006_02142574;
            tail = data_ov006_0214256c;
            if (tail == 0) {
                data_ov006_0214256c = secondCard;
            } else {
                next = tail->mNext;
                if (next != 0) {
                    do { tail = next; next = next->mNext; } while (next != 0);
                }
                tail->mNext = secondCard;
                secondCard->mPrev = tail;
                secondCard->mNext = 0;
            }

            data_ov006_02142574 = 0;
            data_ov006_02142570 = 0;
            ApproachLinear2(data_ov006_02142564, 0x270f, 2);
            ApproachLinear2(data_ov006_0213d6f8, 0, 2);
            func_ov004_020adb1c(data_ov006_02142564);

            if (data_ov006_0213d6f8 <= 0x14) {
                {
                    int face = 0;
                    s16 difficulty = *(volatile s16*)&data_ov006_02142560;
                    int* weights = data_ov006_0212e97c[difficulty];
                    do {
                        data_ov006_02142580[face] = data_ov006_02142580[face] + weights[face];
                        face += 1;
                    } while (face < 10);
                }
                {
                    dMgMCarlo2CardObj_c* card = data_ov006_0214256c;
                    if (card != 0) {
                        do {
                            dMgMCarlo2CardObj_c* nextCard = card->mNext;
                            if (data_ov006_0214257c == card) data_ov006_0214257c = nextCard;
                            if (data_ov006_0214256c == card) data_ov006_0214256c = card->mNext;
                            if (data_ov006_02142578 == card) data_ov006_02142578 = card->mPrev;
                            if (data_ov006_02142568 == card) data_ov006_02142568 = card->mPrev;
                            if (card->mPrev != 0) card->mPrev->mNext = card->mNext;
                            if (card->mNext != 0) card->mNext->mPrev = card->mPrev;
                            card->mNext = 0;
                            card->mPrev = card->mNext;
                            card->Init((s16)(data_ov006_02142568->mSlot + 1));
                            {
                                dMgMCarlo2CardObj_c* oldTail = data_ov006_02142568;
                                dMgMCarlo2CardObj_c* savedNext = oldTail->mNext;
                                oldTail->mNext = card;
                                card->mPrev = oldTail;
                                if (card->mNext != 0) {
                                    do { card = *(dMgMCarlo2CardObj_c* volatile*)&card->mNext; } while (card->mNext != 0);
                                }
                                card->mNext = savedNext;
                                if (card->mNext == 0) data_ov006_02142568 = card;
                            }
                            card = nextCard;
                        } while (card != 0);
                    }
                }
                ApproachLinear2(data_ov006_02142560, 9, 1);
                data_ov006_0213d6f4 += 0x14;
                data_ov006_0213d6f8 += 0x14;
            }
        }
        {
            s16 slot = 0;
            if ((data_ov006_0213d700 >> 12) <= 0) return;
            do {
                if (cardCursor == 0) return;
                if (cardCursor->mState == 1)
                    cardCursor->Update(slot);
                slot = slot + 1;
                cardCursor = cardCursor->mNext;
            } while (slot < (data_ov006_0213d700 >> 12));
        }
    } else {
        int visibleCount = data_ov006_0213d6f8;
        if (visibleCount > 0x14) visibleCount = 0x14;
        ApproachLinear(data_ov006_0213d700, visibleCount << 12, 0x800);
        {
            s16 slot = 0;
            if ((data_ov006_0213d700 >> 12) <= 0) return;
            do {
                if (cardCursor == 0) return;
                cardCursor->Update(slot);
                slot = slot + 1;
                cardCursor = cardCursor->mNext;
            } while (slot < (data_ov006_0213d700 >> 12));
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- dMgMCarlo2CardObj_c::~dMgMCarlo2CardObj_c, 0x020f8ff0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19dMgMCarlo2CardObj_cD1Ev
dMgMCarlo2CardObj_c::~dMgMCarlo2CardObj_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN14dScMgMCarlo2_cD0Ev, 0x020f8f68, size 0x88            */
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
