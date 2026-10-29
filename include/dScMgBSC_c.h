/* class dScMgBSC_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgBSC_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). MG_BS_CARD is the ROM profile and Lucky Stars is its
 * English minigame identity; the unique factory is named
 * dScMgBSC_c_classInit from that profile+RTTI evidence.
 *
 * SIZE 0x51d0, from dScMgBSC_c_classInit's `_ZN7fBase_cnwEm(0x51d0)`.
 *
 * THE DEALER at 0x4f38: a Dealer (include/Dealer.h, 0x270 bytes), the
 * same record the card, roulette and memory scenes embed there.
 *
 * THE CARD TABLE at 0x51a8, 2 elements of 8 bytes, spelled as two s32
 * positions per card now that this class's own state handlers name the
 * shape: StateDealCards starts each card at x = 0x80000, y = -0x40000
 * (0x021250e4). StateMoveCards approaches the chosen card's y toward
 * 0x90000 and the other card's y toward 0x30000 (0x02124e1c).
 * StateWaitDeal runs both halves through
 * Math_Function_0203b14c as coordinates (0x02124fd8), and Render reads
 * them back with `>> 12` for the dMeter_c -- 1:4096 fixed-point screen
 * positions, spelled s32 like the camera fields in
 * dScMgSingle3DBase_c.h. Each card is a Vector2 (include/Vector2.h): the
 * ROM builds and destroys the pair through that type's arm9 structors,
 * func_0203d738 and NullDestructor_0203d47c, and this TU's manifest
 * discards its own inline copies of them as duplicates of those bodies.
 *
 * OWN TAIL, 0x51b8..0x51d0, all named: mState is the state number that
 * indexes the member-pointer state table sBSCStateTable (filled at
 * startup by __sinit_ov006_0213326c from the fourteen pair globals at
 * 0x0213fd0c..0x0213fd84); the state handlers below are that table's
 * entries. mStateTimer, mChosenCard, mOtherCard, mCardsDealt, mCardValue,
 * mCardFrame and mRevealTick are the deal/reveal/judge working state
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
 * The destructor's body is empty: the compiler tears down mCardPos
 * (__cxa_vec_cleanup with Vector2's destructor) and then mDealer, the
 * cartridge's order. The factory is `new dScMgBSC_c`, which builds them in
 * declaration order. dScMgBase_c, two levels up, provides operator new and
 * delete. */
#ifndef DSCMGBSC_C_H
#define DSCMGBSC_C_H
#include "dScMgSingle3DBase_c.h"
#include "Dealer.h"
#include "Vector2.h"

struct dScMgBSC_c : dScMgSingle3DBase_c {
    virtual ~dScMgBSC_c() {}

    /* --- this class's own vtable slots, named from the table ---
       Re-overrides of slots fBase_c already owns, NOT new virtuals: the
       table stays the base's width and no field moves. Declared AFTER the
       destructor so the destructor is still the first virtual declared. */
    virtual s32 InitResources(); /* slot 0 -- ov006 0x0212551c */
    virtual s32 Behavior();      /* slot 6 -- ov006 0x021254c0 */
    virtual s32 Render();        /* slot 9 -- ov006 0x021253bc */

    /* --- event handlers, this class's vtable slots 18/19/21 ---
       Named from the table (tools/rtti_vtables.py --own dScMgBSC_c), under
       dScMgBase_c.h's slot names. Slots 18 and 19 became
       overrides when upstream dScMgBase_c.h declared them virtual
       (2026-08-31). Both are re-overrides of base-owned slots, adding
       no table entry, spelled like the base-owned re-overrides above.
       StartRound is void -- this class's own bytes measured it (an
       int return reserves r0 and shifts the tail registers; retail uses
       r0 as scratch), and the base slot's int was amended to void on
       the same evidence, Coin's `return 0;` proving to be a
       transcription artifact. CanEndState is s32, matching the
       base's measured int (its ov004 base body ends `return 1;`).
       Slot 21 is also declared by dScMgBase_c, so OnDealDone is a
       void override. */
    virtual void StartRound(int mode); /* slot 18 -- ov006 0x02125364 */
    virtual s32 CanEndState(int mode); /* slot 19 -- ov006 0x0212527c */
    virtual void OnDealDone();         /* slot 21 -- ov006 0x02125248 */

    enum State {
        STATE_DEAL_CARDS = 0,
        STATE_WAIT_DEAL,
        STATE_CHOOSE_CARD,
        STATE_MOVE_CARDS,
        STATE_WAIT_MOVE,
        STATE_REVEAL_CARDS,
        STATE_JUDGE_CARDS,
        STATE_WIN_REACTION,
        STATE_LOSE_REACTION,
        STATE_WAIT_WIN,
        STATE_WAIT_LOSE,
        STATE_RETURN_CARDS,
        STATE_WAIT_COINS,
        STATE_WAIT_RESULT
    };

    /* --- the state machine ---
       Fourteen entries of the member-pointer table sBSCStateTable,
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

    Dealer mDealer;      /* 0x4f38 */
    Vector2 mCardPos[2]; /* 0x51a8 -- 1:4096 screen positions, see the banner */
    s32 mState;          /* 0x51b8 -- index into sBSCStateTable */
    s32 mStateTimer;     /* 0x51bc */
    s32 mChosenCard;     /* 0x51c0 */
    s32 mOtherCard;      /* 0x51c4 */
    s16 mCardsDealt;     /* 0x51c8 -- counter incremented by the dealer animation */
    u8 mCardValue[2];    /* 0x51ca */
    u8 mCardFrame[2];    /* 0x51cc */
    u8 mRevealTick;      /* 0x51ce -- advances the card frame every four ticks */
    u8 pad_51cf[1];      /* 0x51cf */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgBSC_c_size_must_be_0x51d0[sizeof(dScMgBSC_c) == 0x51d0 ? 1 : -1];
#endif

#endif
