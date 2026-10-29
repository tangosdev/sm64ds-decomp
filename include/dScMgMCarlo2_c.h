/* class dScMgMCarlo2_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgMCarlo2_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). The factory now carries the reconstructed
 * dScMgMCarlo2_c_classInit spelling; MgPairAGoneAndOn_Spawn is its
 * historical English-minigame alias.
 *
 * SIZE 0x5930, from dScMgMCarlo2_c_classInit in the combined class TU's
 * `_ZN7fBase_cnwEm(0x5930)`.
 *
 * THE DEALER at 0x4f38: a Dealer (include/Dealer.h, 0x270 bytes), the
 * same record the card, roulette, BSC and memory scenes embed there. This
 * scene writes and tests its mHand and writes its mReserved directly.
 *
 * ONE ARRAY at 0x51a8, 40 elements of 0x30 bytes each. The compiler lowers
 * its construction/destruction to the ROM's __cxa_vec_ctor/__cxa_vec_cleanup calls.
 * The element type is ROM-proven by _ZTI19dMgMCarlo2CardObj_c and its
 * two-slot vtable at 0x0213d728. The matched bodies below recover every
 * field and the intrusive list threaded through mPrev/mNext.
 *
 * OWN TAIL, 0x5928..0x5930: three fields are real matched access
 * (src/minigames/d_s_mg_m_carlo2.cpp).
 *
 * LIFECYCLE IS COMPILER-OWNED. The factory is `new dScMgMCarlo2_c`, which
 * builds mDealer and the card array in declaration order. The empty inline
 * scene destructor destroys them in reverse: the typed card array, the
 * dealer, then the inherited scene. That produces the cartridge's
 * D1/D0 bodies exactly. The card class's real out-of-line constructor and
 * destructor produce the cartridge C1/D1 exactly; mwccarm's unreferenced
 * C2/D2/D0 companions are explicitly proven dead and removed by the TU
 * manifest. InitResources remains the scene key function and Render the card
 * key function, so their ROM RTTI/vtables are emitted by this TU. */
#ifndef DSCMGMCARLO2_C_H
#define DSCMGMCARLO2_C_H
#include "dScMgSingle3DBase_c.h"
#include "Dealer.h"

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

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dMgMCarlo2CardObj_c_size_must_be_0x30[sizeof(dMgMCarlo2CardObj_c) == 0x30 ? 1 : -1];
#endif

struct dScMgMCarlo2_c : dScMgSingle3DBase_c {
    virtual ~dScMgMCarlo2_c() {}

    Dealer mDealer;                   /* 0x4f38 */
    dMgMCarlo2CardObj_c mArray[0x28]; /* 0x51a8..0x5928 */
    s16 mRoundState;                  /* 0x5928 -- the round's step; Behavior switches on it */
    s16 mTimer;                       /* 0x592a -- step timer (ApproachLinear2 to 0) */
    u8  pad_592c[0x2];      /* 0x592c */
    s16 mDealCount;         /* 0x592e -- cards dealt so far; Dealer_StartShuffle counts it */

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
    virtual void StartRound(int arg);               /* slot 18 */
    virtual int  CanEndState(int mode);              /* slot 19 */
    s32 CleanupResources();   /* slot 3 -- ov006 0x020f9fe0 */

    static void UpdateBoard();
    static void FlipDealtCards();
    static int  DrawCardValue();
    static int  BoardReady();
    static int  BoardBusy();
    static int  HasRemovablePair();
    static void SetupBoard(dMgMCarlo2CardObj_c *cards);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgMCarlo2_c_size_must_be_0x5930[sizeof(dScMgMCarlo2_c) == 0x5930 ? 1 : -1];
#endif

#endif
