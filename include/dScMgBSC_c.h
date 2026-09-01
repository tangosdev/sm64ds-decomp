/* class dScMgBSC_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgBSC_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). English Spawn-function name is MgLuckyStars_Spawn;
 * kept as the symbol name for the factory (already attributed), but the
 * class itself takes its real ROM identity, matching this tree's current
 * convention.
 *
 * SIZE 0x51d0, from MgLuckyStars_Spawn.cpp's own
 * `_ZN7fBase_cnwEj(0x51d0)`.
 *
 * ONE SHARED MEMBER at 0x4f38, size 0x270 (func_ov006_020c1d80/020c1c64),
 * shared verbatim by five other classes in this family -- left as raw
 * bytes, see include/dScMgMemory_c.h's own note.
 *
 * THE CARD TABLE at 0x51a8, 2 elements of 8 bytes, spelled as two s32
 * positions per card now that this class's own state handlers name the
 * shape: StateDealCards starts each card at x = 0x80000, y = -0x40000
 * (0x021250e4), StateMoveCards approaches x toward 0x90000 and y toward
 * 0x30000 (0x02124e1c), StateWaitDeal runs both halves through
 * Math_Function_0203b14c as coordinates (0x02124fd8), and Render reads
 * them back with `>> 12` for the HUD -- 1:4096 fixed-point screen
 * positions, spelled s32 like the camera fields in
 * dScMgSingle3DBase_c.h. The factory still constructs it as raw bytes
 * (`func_020733a8(p+0x51a8, 2, 8, func_0203d738,
 * NullDestructor_0203d47c)`) and the destructor still destroys it through
 * `__destroy_arr(mCardPos, 2, 8, NullDestructor_0203d47c)`; the layout
 * claim is exactly what those byte helpers already encode.
 *
 * OWN TAIL, 0x51b8..0x51d0, all named: mState is the state number that
 * indexes the member-pointer state table data_ov006_02142f94 (filled at
 * startup by __sinit_ov006_0213326c from the fourteen pair globals at
 * 0x0213fd0c..0x0213fd84); the state handlers below are that table's
 * entries. mStateTimer, mChosenCard, mOtherCard, mDealCount, mCardValue,
 * mCardFrame and mRevealStep are the deal/reveal/judge working state
 * those handlers read and write.
 *
 * THE DESTRUCTOR IS DEFINED INLINE, in the class body, and the ROM's own
 * layout is the evidence for it. This class is a leaf with no RTTI
 * descendants, so nothing forces the choice from below -- but _ZN10dScMgBSC_cD1Ev
 * sits at 0x02124908 and _ZN10dScMgBSC_cD0Ev at 0x0212497c, D1 first. An
 * out-of-line destructor does not produce that order: mwcc emits the
 * synthesized D0 ahead of the written D1, which is backwards from the
 * cartridge. Inline in the class body, the destructor cannot be the key
 * function, so InitResources (the next virtual declared, and non-inline)
 * becomes it; that TU emits _ZTV10dScMgBSC_c, whose slots 16 and 17 odr-use
 * D1 then D0, and the pair comes out in exactly the cartridge's order. Same
 * mechanism as dScMgSingle3DBase_c one level up -- see that header's note.
 *
 * The two calls in the body are member destruction, not hand-written
 * cleanup: mCardPos is declared after mTable and is destroyed first, which is
 * reverse-declaration order. They are spelled out only because mTable is
 * still a raw byte array with no element type recovered.
 *
 * No separate operator delete is needed -- dScMgBase_c, two levels up,
 * already provides one. */
#ifndef DSCMGBSC_C_H
#define DSCMGBSC_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c1c64(char *t); /* decl_common.h's own signature */
extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void NullDestructor_0203d47c(void);

struct dScMgBSC_c : dScMgSingle3DBase_c {
    virtual ~dScMgBSC_c() {
        __destroy_arr(mCardPos, 2, 8, (void *)NullDestructor_0203d47c);
        func_ov006_020c1c64((char *)mTable);
    }

    /* --- this class's own vtable slots, named from the table ---
       Re-overrides of slots fBase_c already owns, NOT new virtuals: the
       table stays the base's width and no field moves. Declared AFTER the
       destructor so the destructor is still the first virtual declared. */
    virtual s32 InitResources(); /* slot 0 -- ov006 0x0212551c */
    virtual s32 Behavior();   /* slot 6 -- ov006 0x021254c0 */
    virtual s32 Render();     /* slot 9 -- ov006 0x021253bc */

    /* --- event handlers, this class's vtable slots 18/19/21 ---
       Named from the table (tools/rtti_vtables.py --own dScMgBSC_c); the
       legacy recovered names OnYoshiTryEat / OnTurnIntoEgg /
       OnGroundPounded stand uncontradicted. Slots 18 and 19 became
       overrides when upstream dScMgBase_c.h declared them virtual
       (2026-08-31). Both are re-overrides of base-owned slots, adding
       no table entry, spelled like the base-owned re-overrides above.
       OnYoshiTryEat is void -- this class's own bytes measured it (an
       int return reserves r0 and shifts the tail registers; retail uses
       r0 as scratch), and the base slot's int was amended to void on
       the same evidence, Coin's `return 0;` proving to be a
       transcription artifact. OnTurnIntoEgg is s32, matching the
       base's measured int (its ov004 base body ends `return 1;`).
       Slot 21 is also declared by dScMgBase_c, so OnGroundPounded is a
       void override. */
    virtual void OnYoshiTryEat(int mode);  /* slot 18 -- ov006 0x02125364 */
    virtual s32 OnTurnIntoEgg(int mode);  /* slot 19 -- ov006 0x0212527c */
    virtual void OnGroundPounded(); /* slot 21 -- ov006 0x02125248 */

    /* --- the state machine ---
       Fourteen entries of the member-pointer table data_ov006_02142f94,
       indexed by mState and dispatched from Behavior. The names are
       coined from what each state does: the ROM's own symbols here are
       address-only (func_ov006_02124a04 and friends), so no recovered
       spelling exists to confirm or contradict them. States 7 and 8
       share StateTableReaction, 9 and 10 share StateWaitTable, 12 and 13
       share StateDone -- that pairing is the table's own, read off
       __sinit_ov006_0213326c's copy of the fourteen pair globals. */
    void StateDealCards();     /* mState 0      -- ov006 0x021250e4 */
    void StateWaitDeal();      /* mState 1      -- ov006 0x02124fd8 */
    void StateChooseCard();    /* mState 2      -- ov006 0x02124ec4 */
    void StateMoveCards();     /* mState 3      -- ov006 0x02124e1c */
    void StateWaitMove();      /* mState 4      -- ov006 0x02124dc0 */
    void StateRevealCards();   /* mState 5      -- ov006 0x02124cb4 */
    void StateJudgeCards();    /* mState 6      -- ov006 0x02124bb4 */
    void StateTableReaction(); /* mState 7, 8   -- ov006 0x02124b58 */
    void StateWaitTable();     /* mState 9, 10  -- ov006 0x02124ae4 */
    void StateReturnCards();   /* mState 11     -- ov006 0x02124a08 */
    void StateDone();          /* mState 12, 13 -- ov006 0x02124a04 */

    /* A card's fixed-point screen position, 1:4096 -- the scale Render
       takes back with `>> 12`. */
    struct CardPos {
        s32 x; /* 0x00 */
        s32 y; /* 0x04 */
    };

    u8  mTable[0x270];    /* 0x4f38 -- ctor func_ov006_020c1d80, dtor func_ov006_020c1c64 */
    CardPos mCardPos[2];  /* 0x51a8 -- ctor func_020733a8 via func_0203d738, dtor NullDestructor_0203d47c */
    s32 mState;          /* 0x51b8 -- index into data_ov006_02142f94 */
    s32 mStateTimer;     /* 0x51bc */
    s32 mChosenCard;     /* 0x51c0 */
    s32 mOtherCard;      /* 0x51c4 */
    s16 mDealCount;      /* 0x51c8 */
    u8  mCardValue[2];   /* 0x51ca */
    u8  mCardFrame[2];   /* 0x51cc */
    u8  mRevealStep;     /* 0x51ce */
    u8  pad_51cf[1];      /* 0x51cf */
};

typedef char dScMgBSC_CardPos_size_must_be_0x8[sizeof(dScMgBSC_c::CardPos) == 0x8 ? 1 : -1];
typedef char dScMgBSC_c_size_must_be_0x51d0[sizeof(dScMgBSC_c) == 0x51d0 ? 1 : -1];

#endif
