/* class dScMgMCarlo2_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgMCarlo2_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). English Spawn-function name is MgPairAGoneAndOn_Spawn;
 * kept as the symbol name for the factory (already attributed), but the
 * class itself takes its real ROM identity, matching this tree's current
 * convention.
 *
 * SIZE 0x5930, from MgPairAGoneAndOn_Spawn in the combined class TU's
 * `_ZN7fBase_cnwEj(0x5930)`.
 *
 * SHARED TABLE at 0x4f38, size 0x270 (func_ov006_020c1d80/020c1c64), same
 * as five siblings -- see include/dScMgMemory_c.h's own note. TWO fields
 * fall WITHIN it and are real matched access (dScMgMCarlo2_c.cpp):
 * mShared.unk_01a and mShared.unk_1e6 are literally the ctor's own writes
 * and `*(short*)(t+0x1e6)=0` writes (func_ov006_020c1d80's own body) --
 * named here instead of folded into the opaque table, everything else in
 * the table stays pad.
 *
 * ONE ARRAY at 0x51a8, 40 elements of 0x30 bytes each. The compiler lowers
 * its construction/destruction to the ROM's func_020733a8/__destroy_arr calls.
 * The element type is ROM-proven by _ZTI19dMgMCarlo2CardObj_c and its
 * two-slot vtable at 0x0213d728. The matched bodies below recover every
 * field and the intrusive list threaded through mPrev/mNext.
 *
 * OWN TAIL, 0x5928..0x5930: three fields are real matched access
 * (src/minigames/d_s_mg_m_carlo2.cpp).
 *
 * LIFECYCLE IS COMPILER-OWNED. The empty inline scene destructor destroys
 * members in reverse declaration order: the typed card array, the shared
 * state wrapper, then the inherited scene. That produces the cartridge's
 * D1/D0 bodies exactly. The card class's real out-of-line constructor and
 * destructor produce the cartridge C1/D1 exactly; mwccarm's unreferenced
 * C2/D2/D0 companions are explicitly proven dead and removed by the TU
 * manifest. InitResources remains the scene key function and Render the card
 * key function, so their ROM RTTI/vtables are emitted by this TU. */
#ifndef DSCMGMCARLO2_C_H
#define DSCMGMCARLO2_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c1c64(char *t); /* decl_common.h's own signature */
extern "C" void _ZN19dMgMCarlo2CardObj_cD1Ev(void *elem);

/* Shared 0x270-byte minigame state block. The type name is coined; its
   destructor behavior and layout are measured. Keeping it as a real member
   lets C++ perform the scene's reverse-order teardown: card array, then this
   block, then the inherited scene state. */
struct dMgMCarlo2SharedState_c {
    ~dMgMCarlo2SharedState_c() { func_ov006_020c1c64((char *)this); }

    u8  pad_000[0x1a];
    s16 unk_01a;
    u8  pad_01c[0x1ca];
    s16 unk_1e6;
    u8  pad_1e8[0x88];
};

typedef char dMgMCarlo2SharedState_c_size_must_be_0x270[sizeof(dMgMCarlo2SharedState_c) == 0x270 ? 1 : -1];

struct dMgMCarlo2CardObj_c {
    dMgMCarlo2CardObj_c();
    ~dMgMCarlo2CardObj_c();

    virtual void Render();
    virtual void Update(int event);

    int  HitTest();
    int  IsPairWith(dMgMCarlo2CardObj_c *other);
    void FlipAway(int n);
    void DealIn(int slot);
    void Init(int slot);

    dMgMCarlo2CardObj_c *mPrev; /* 0x04 */
    dMgMCarlo2CardObj_c *mNext; /* 0x08 */
    s32 mX;                     /* 0x0c, 20.12 fixed point */
    s32 mY;                     /* 0x10 */
    s32 mTargetX;               /* 0x14 */
    s32 mTargetY;               /* 0x18 */
    s32 mXStep;                 /* 0x1c */
    s32 mYStep;                 /* 0x20 */
    s32 mLift;                  /* 0x24 */
    s16 mDealDelay;             /* 0x28 */
    s16 mSlot;                  /* 0x2a */
    u8  mFace;                  /* 0x2c */
    u8  mState;                 /* 0x2d */
    u8  mVisible;               /* 0x2e */
    u8  pad_2f;                 /* 0x2f */
};

typedef char dMgMCarlo2CardObj_c_size_must_be_0x30[sizeof(dMgMCarlo2CardObj_c) == 0x30 ? 1 : -1];

struct dScMgMCarlo2_c : dScMgSingle3DBase_c {
    virtual ~dScMgMCarlo2_c() {}

    dMgMCarlo2SharedState_c mShared; /* 0x4f38..0x51a8 */
    dMgMCarlo2CardObj_c mArray[0x28]; /* 0x51a8..0x5928 */
    s16 unk_5928;           /* 0x5928 */
    s16 unk_592a;           /* 0x592a */
    u8  pad_592c[0x2];      /* 0x592c */
    s16 unk_592e;           /* 0x592e */

    /* --- this class's own vtable overrides, defined out of line under their
       own mangled names. Each re-uses a slot fBase_c already holds rather
       than appending one, and neither adds a field, so the size assert below
       is untouched. InitResources, the first of these and the first
       non-inline virtual declared after the inline destructor, is this
       class's KEY FUNCTION -- so the TU that defines it is the one that
       emits _ZTV14dScMgMCarlo2_c.
       Signatures are include/fBase_c.h's and include/dScMgBase_c.h's own,
       copied unchanged. --- */
    s32 InitResources();      /* slot 0 -- src/_ZN14dScMgMCarlo2_c13InitResourcesEv.cpp */
    s32 Behavior();           /* slot 6 -- ov006 0x020fa13c */
    s32 Render();             /* slot 9 -- src/_ZN14dScMgMCarlo2_c6RenderEv.cpp */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */
    s32 CleanupResources();   /* slot 3 -- ov006 0x020f9fe0 */

    static void UpdateBoard();
    static void FlipDealtCards();
    static int  DrawCardValue();
    static int  BoardReady();
    static int  BoardBusy();
    static int  HasRemovablePair();
    static void SetupBoard(dMgMCarlo2CardObj_c *cards);
};

typedef char dScMgMCarlo2_c_size_must_be_0x5930[sizeof(dScMgMCarlo2_c) == 0x5930 ? 1 : -1];

#endif
