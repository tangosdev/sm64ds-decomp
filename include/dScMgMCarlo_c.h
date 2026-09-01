/* class dScMgMCarlo_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgMCarlo_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable sits at ov006:0x0213d664; the RTTI
 * pointer one word below it is _ZTI13dScMgMCarlo_c, which is what ties
 * that table to this class rather than to a neighbour.
 *
 * SIZE 0x60b0, from the factory's own `_ZN7fBase_cnwEj(0x60b0)`
 * (src/func_ov006_020f8e44.cpp).
 *
 * SHARED TABLE at 0x4f38, size 0x270, constructed by func_ov006_020c1d80
 * and destroyed by func_ov006_020c1c64 -- the same pair dScMgCard_c and
 * dScMgMCarlo2_c use at this same offset; see include/dScMgCard_c.h's own
 * note. The SAME two fields fall within it here as in both of those
 * siblings, unk_4f52 and unk_511e, and they are the same thing there too:
 * literally the shared constructor's own `*(short*)(t+0x1a)=0` and
 * `*(short*)(t+0x1e6)=0` writes. Named here rather than folded into the
 * opaque table; everything else in the table stays pad.
 *
 * ONE ARRAY, 0x50 elements of 0x30 bytes, at 0x51a8: the factory builds it
 * with element ctor func_ov006_020f8ed8 and the destructor destroys it with
 * the hand-written destroy stub declared above, ending exactly on 0x60a8
 * where the own tail begins. The element type is RECOVERED here, the same
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
 * THE DESTRUCTOR IS DEFINED INLINE, in the class body, and the ROM's own
 * emission order is why. Written out of line, mwcc emits the synthesized D0
 * AHEAD of the written D1; the cartridge has D1 first (0x020f7634) and D0
 * second (0x020f76a8), and linkcheck's pre-link audit refuses a TU whose
 * licensed .text is not in ROM address order. Inline, the destructor cannot
 * be the key function; InitResources -- the next virtual declared, and
 * non-inline -- takes that role, and the TU that defines it emits
 * _ZTV13dScMgMCarlo_c. Slots 16 and 17 name D1 then D0, odr-using both, so
 * the compiler emits the pair for us in cartridge order. It also removes
 * the homeless D2 entirely. The two calls below are ordinary
 * reverse-declaration member destruction, spelled out because the element
 * class declares no destructor at all (so the compiler's own member
 * destruction is empty, and the stub carries the cartridge's destroy call
 * alone); the second call releases the shared table, since two of its
 * words stay named. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one.
 * Same recipe as include/dScMgCard_c.h, and for the same reason. */
#ifndef DSCMGMCARLO_C_H
#define DSCMGMCARLO_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c1c64(char *t); /* decl_common.h's own signature */
extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);

/* The element destroy routine, spelled by its mangled D1 name: C++ source
   cannot take a destructor's address, so the __destroy_arr call below
   references it through an extern-"C" declaration of the mangled symbol --
   the double-mangling defect this tree already documents. The routine is
   HAND-WRITTEN in src/actors/dScMgMCarlo_c.cpp as an extern-"C" definition
   of this name -- the destroy-side twin of this hand's hand-written ctor
   stub in src/func_ov006_020f8ed8.c -- and the class below declares NO
   destructor at all, because no member-destructor spelling lands the pair
   in the cartridge's own shape and order. That was measured across every
   form for dScMgCard_c's two element classes (include/dScMgCard_c.h:60-95
   carries the enumeration), and the same two failures apply to the same
   layout here -- the scene pair 0x020f7634/0x020f76a8 is adjacent in the
   cartridge with the element stub after it at 0x020f7730:
     written out of line -> the derived D1 goes out as a bl to the base D2
       with a push/blr frame where the cartridge carries a bare 0x10
       two-store-free form, and D0 and D2 come with it -- destructor
       symbols emitted where the cartridge carries one;
     marked inline (in class, on the definition, any source order, under
       #pragma force_inline / inline) -> the D1 IS emitted, byte-matching,
       but between the scene D1 and the scene D0, sliding the scene D0
       behind it, off the cartridge's ROM address order, which
       rombuild's fail-closed isolate refuses.
   The stub form is the ratified family idiom (adjudicated 2026-08-31,
   notes/minigame-family-decisions.md): the destroy-side twin of the
   hand-written ctor stubs the factory uses, disclosed in the manifest's
   goal block. The vtable/RTTI records do not follow the destructor: this
   header declares Render -- the element class's key function --
   non-inline, and src/actors/dScMgMCarlo_c.cpp defines it, so the
   key-function TU keeps emitting _ZTV/_ZTI/_ZTS for the element class
   either way. */
extern "C" void _ZN18dMgMCarloCardObj_cD1Ev(void *elem);

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

   The objects are CONSTRUCTED by this hand's factory (src/
   func_ov006_020f8e44.cpp: func_020733a8 with func_ov006_020f8ed8 as the
   element ctor -- a stub in the factory's own TU, which is why no
   constructor is declared here); and they are DESTROYED by the
   hand-written routine declared above (the mangled D1 name), the
   destroy-side twin of that ctor stub -- which is why no destructor is
   declared here either. */

struct dMgMCarloCardObj_c {
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
    virtual ~dScMgMCarlo_c() {
        __destroy_arr(mArray, 0x50, 0x30, (void *)_ZN18dMgMCarloCardObj_cD1Ev);
        func_ov006_020c1c64((char *)pad_4f38);
    }

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

    u8  pad_4f38[0x1a];    /* 0x4f38 -- shared table start, see file banner */
    s16 unk_4f52;          /* 0x4f52 -- within shared table */
    u8  pad_4f54[0x1ca];   /* 0x4f54 */
    s16 unk_511e;          /* 0x511e -- within shared table */
    u8  pad_5120[0x88];    /* 0x5120 -- rest of shared table (ends 0x51a8) */
    dMgMCarloCardObj_c mArray[0x50]; /* 0x51a8 -- dtor _ZN18dMgMCarloCardObj_cD1Ev
                                        (destroy stub) */
    s16 unk_60a8;          /* 0x60a8 */
    s16 unk_60aa;          /* 0x60aa */
    u8  pad_60ac[0x2];     /* 0x60ac */
    s16 unk_60ae;          /* 0x60ae */
};

typedef char dScMgMCarlo_c_size_must_be_0x60b0[sizeof(dScMgMCarlo_c) == 0x60b0 ? 1 : -1];

#endif
