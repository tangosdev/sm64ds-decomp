/* class dScMgMCarlo_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgMCarlo_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable sits at ov006:0x0213d664; the RTTI
 * pointer one word below it is _ZTI13dScMgMCarlo_c, which is what ties
 * that table to this class rather than to a neighbour.
 *
 * SIZE 0x60b0, from the factory's own `_ZN7fBase_cnwEj(0x60b0)`
 * (the classInit factory in src/actors/dScMgMCarlo_c.cpp).
 *
 * SHARED TABLE at 0x4f38, size 0x270, constructed by func_ov006_020c1d80
 * and destroyed by func_ov006_020c1c64 -- the same pair dScMgCard_c and
 * dScMgMCarlo2_c use at this same offset; see include/dScMgCard_c.h's own
 * note. The SAME two fields fall within it here as in both of those
 * siblings, mShared.unk_01a and mShared.unk_1e6, and they are the same thing there too:
 * literally the shared constructor's own `*(short*)(t+0x1a)=0` and
 * `*(short*)(t+0x1e6)=0` writes. Named here rather than folded into the
 * opaque table; everything else in the table stays pad.
 *
 * ONE ARRAY, 0x50 elements of 0x30 bytes, at 0x51a8: the factory builds it
 * with the compiler-spelled element C1 and the scene destructor destroys it
 * with the compiler-spelled element D1, ending exactly on 0x60a8 where the
 * own tail begins. The element type is RECOVERED here, the same
 * treatment dScMgCard_c's conversion gave its two hands: dMgMCarloCardObj_c,
 * confirmed by the cartridge's own RTTI pair (_ZTI at 0x0213d578, _ZTS at
 * 0x0213d5c8, build/rtti.json), and the member below carries that type, so
 * the board's list walks spell real member access. The board lives on ONE
 * intrusive doubly-linked list threaded through the elements' mPrev/mNext
 * (head data_ov006_02142500, tail data_ov006_02142504; data_ov006_021424fc /
 * _02142508 are the first/second pick).
 *
 * OWN TAIL, 0x60a8..0x60b0: three fields carried over verbatim from this
 * header's previous auto-generated form, real matched access out of
 * dScMgMCarlo_c's own methods, and the last of them closes exactly on the
 * factory's allocation literal. The two fields the old header declared
 * below 0x4f38 (0x0a8, 0x0ac) are dropped: both are dScMgBase_c's own and
 * are already declared there.
 *
 * LIFECYCLE IS COMPILER-OWNED. The empty inline scene destructor destroys
 * members in reverse declaration order: the typed card array, the shared
 * state wrapper, then the inherited scene. That produces the cartridge's
 * D1/D0 bodies exactly. The card class's real out-of-line constructor and
 * destructor produce the cartridge C1/D1 exactly; mwccarm's unreferenced
 * C2/D2/D0 companions are explicitly proven dead and removed by the TU
 * manifest. InitResources remains the scene key function and Render the card
 * key function, so their ROM RTTI/vtables are emitted by this TU. */
#ifndef DSCMGMCARLO_C_H
#define DSCMGMCARLO_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c1c64(char *t); /* decl_common.h's own signature */
extern "C" void _ZN18dMgMCarloCardObj_cD1Ev(void *elem);

/* The shared state name is reconstructed; its 0x270 layout and destructor
   behavior are measured. A real member lets C++ spell the scene teardown. */
struct dMgMCarloSharedState_c {
    ~dMgMCarloSharedState_c() { func_ov006_020c1c64((char *)this); }

    u8  pad_000[0x1a];
    s16 unk_01a;
    u8  pad_01c[0x1ca];
    s16 unk_1e6;
    u8  pad_1e8[0x88];
};

typedef char dMgMCarloSharedState_c_size_must_be_0x270[sizeof(dMgMCarloSharedState_c) == 0x270 ? 1 : -1];

/* --- the board-card element class ------------------------------------------
   Name RTTI-confirmed (build/rtti.json: _ZTS18dMgMCarloCardObj_c at
   0x0213d5c8). Polymorphic with EXACTLY two vtable slots, measured off the
   cartridge's own table (_ZTV18dMgMCarloCardObj_c at 0x0213d5ac, 0xc-byte
   slot array, tools/rtti_vtables.py): slot 0 Render, slot 1 Update. NO
   virtual destructor -- the record is two slots wide and nothing in the
   range claims a D0.

   The slot names and every field name below are COINED from the matched
   bodies in src/actors/dScMgMCarlo_c.cpp -- the ROM's symbols for these
   functions were address-only before this conversion renamed them
   (func_ov006_020f7e2c and friends), so no recovered spelling exists to
   confirm or contradict them. Render draws
   the card through the Hud_RenderSprite bank table; Update runs the card's
   own motion/selection state machine. The fields: the 1:4096 fixed-point
   position pair (mX, mY); the deal/move target pair (mTargetX, mTargetY)
   with its approach steps (mXStep, mYStep -- Vec2_Sub of target minus
   position, scaled, then clamped by abs, and exactly the step pair
   ApproachLinear consumes); the raise amount (mLift, 0x4000 meaning fully
   raised, (mLift >> 12) indexing the flip frames); the deal/flip stagger
   counter (mDealDelay: seeded (slot%5)*2+1 on Init and (4-slot%5)*2 on
   FlipAway, counted down in state 0 and approached to 0 in state 5); the
   board slot index (mSlot, its %5//5 pair driving IsPairWith's adjacency
   test); the face byte (mFace, the weighted 0..7 draw SetupBoard deals and
   IsPairWith matches); the state byte (mState: 0 waiting-to-deal, 1/4
   flying in, 2 idle on the board, 3 selected, 5 flipping away); and the
   visible flag (mVisible). The board list runs mPrev/mNext through every
   element -- see the ONE ARRAY paragraph above.

   The objects are constructed and destroyed by the real C1/D1 definitions
   in the same original TU. C++ cannot take a destructor's address, so the
   factory's array helper references D1 through the ABI declaration above. */

struct dMgMCarloCardObj_c {
    dMgMCarloCardObj_c();
    ~dMgMCarloCardObj_c();

    virtual void Render();          /* slot 0 -- ov006 0x020f7e2c */
    virtual void Update(int event); /* slot 1 -- ov006 0x020f7ee4 */

    /* board helpers reached on one card (not virtual, no table entry) */
    int  HitTest();                                  /* ov006 0x020f8154 */
    int  IsPairWith(dMgMCarloCardObj_c *other);      /* ov006 0x020f8224 */
    void FlipAway(int n);                            /* ov006 0x020f82d0 */
    void DealIn(int m);                              /* ov006 0x020f8320 */
    void Init(int n);                                /* ov006 0x020f84a8 */

    dMgMCarloCardObj_c *mPrev; /* 0x04 -- the board list runs tail-to-head */
    dMgMCarloCardObj_c *mNext; /* 0x08                                  */
    s32 mX;                   /* 0x0c -- 1:4096 fixed-point */
    s32 mY;                   /* 0x10 */
    s32 mTargetX;             /* 0x14 -- board slot m's column/row target */
    s32 mTargetY;             /* 0x18 */
    s32 mXStep;               /* 0x1c -- ApproachLinear's per-frame step */
    s32 mYStep;               /* 0x20 */
    s32 mLift;                /* 0x24 -- 0x4000 = fully raised */
    s16 mDealDelay;           /* 0x28 -- deal/flip stagger countdown */
    s16 mSlot;                /* 0x2a -- board slot, %5//5 = col/row */
    u8  mFace;                /* 0x2c -- weighted 0..7 draw, pair match key */
    u8  mState;               /* 0x2d -- 0 wait, 1/4 fly-in, 2 idle, 3 pick, 5 flip-away */
    u8  mVisible;             /* 0x2e */
    u8  pad_2f;               /* 0x2f */
};

typedef char dMgMCarloCardObj_c_size_must_be_0x30[sizeof(dMgMCarloCardObj_c) == 0x30 ? 1 : -1];

struct dScMgMCarlo_c : dScMgSingle3DBase_c {
    virtual ~dScMgMCarlo_c() {}

    /* This class's own override, read off the ROM's vtable: the one slot where
       the table differs from dScMgSingle3DBase_c's. Spelled WITHOUT the `virtual`
       keyword, the way include/daObjMarioCap_c.h and include/daObjRc_Dorifu_c.h
       spell theirs -- an override of a virtual an ancestor already declares is
       implicitly virtual either way, so it reuses slot 6 rather than adding one
       and adds no field, and the 0x60b0 assert below still holds. InitResources,
       the first of these and the first non-inline virtual declared after the
       inline destructor, is this class's KEY FUNCTION -- so the TU that defines
       it is the one that emits _ZTV13dScMgMCarlo_c. */
    s32 InitResources();   /* slot  0 -- ov006 0x020f8d08 */
    s32 Render();          /* slot  9 -- ov006 0x020f85b0 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */
    s32 Behavior();        /* slot  6 -- ov006 0x020f869c */

    /* --- the round state machine's helpers, all static: none reads this.
       The register shape is the evidence -- r0 is either untouched or
       carries a hand/board argument, never a scene pointer (RenderHud
       below is the one member: the ROM passes this and its body never
       reads it, the honest-parameter form). Names coined from the bodies;
       see each definition in src/actors/dScMgMCarlo_c.cpp. */
    static void UpdateBoard();                         /* ov006 0x020f7740 */
    static void FlipDealtCards();                      /* ov006 0x020f7994 */
    static int  DrawCardValue();                       /* ov006 0x020f7a00 */
    static int  BoardReady();                          /* ov006 0x020f7a90 */
    static int  BoardBusy();                           /* ov006 0x020f7b10 */
    static int  HasRemovablePair();                    /* ov006 0x020f7b90 */
    static void SetupBoard(dMgMCarloCardObj_c *cards); /* ov006 0x020f7c10 */
    void RenderHud();                                  /* ov006 0x020f8540 */

    dMgMCarloSharedState_c mShared; /* 0x4f38..0x51a8 */
    dMgMCarloCardObj_c mArray[0x50]; /* 0x51a8..0x60a8 */
    s16 unk_60a8;          /* 0x60a8 */
    s16 unk_60aa;          /* 0x60aa */
    u8  pad_60ac[0x2];     /* 0x60ac */
    s16 unk_60ae;          /* 0x60ae */
};

typedef char dScMgMCarlo_c_size_must_be_0x60b0[sizeof(dScMgMCarlo_c) == 0x60b0 ? 1 : -1];

#endif
