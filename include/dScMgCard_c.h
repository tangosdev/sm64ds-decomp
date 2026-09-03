/* class dScMgCard_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgCard_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). MG_CARD is the ROM profile and Picture Poker is its
 * English minigame identity; the unique factory is named
 * dScMgCard_c_classInit from that profile+RTTI evidence.
 *
 * SIZE 0x539c, from dScMgCard_c_classInit's `_ZN7fBase_cnwEj(0x539c)`.
 *
 * SHARED TABLE at 0x4f38, size 0x270 (func_ov006_020c1d80/020c1c64), same
 * as five siblings -- see include/dScMgMemory_c.h's own note. TWO fields
 * fall WITHIN it and are real matched access:
 * mShared.unk_01a and mShared.unk_1e6 are literally the ctor's own
 * `*(short*)(t+0x1a)=0`
 * and `*(short*)(t+0x1e6)=0` writes (func_ov006_020c1d80's own
 * body), same as dScMgMCarlo2_c's own instance of these two fields -- named
 * here instead of folded into the opaque table, everything else stays pad.
 *
 * THE TWO CARD HANDS, 0x51a8 and 0x5298: five 0x30-byte card objects each.
 * The element types are recovered below -- dMgCardObj_c for the hand at
 * 0x51a8, dMgDilarCardObj_c for the hand at 0x5298, both RTTI-confirmed --
 * and the members themselves carry those types here, so UpdateState's state
 * walks spell real member access (&mArray1[0].mState, mArray1[i].mState) and
 * the compiler folds each member+field address itself. The two classes
 * deliberately declare no C++ destructor: real lifecycle definitions make
 * the exact bodies but not the cartridge's required section order. The
 * scene's inline destructor therefore retains the two measured array-helper
 * ABI callbacks, the smallest surviving compiler-wall bridge.
 *
 * OWN TAIL, 0x5388..0x539c: nine fields are real matched access (five
 * src files: func_ov006_020da9c4.cpp, 020db6ec.c, 020db720.c,
 * 020db9dc.c, 020dbaf0.cpp -- five of dScMgCard_c's own vtable methods).
 *
 * THE DESTRUCTOR IS DEFINED INLINE, in the class body, and the ROM's own
 * emission order is why. Written out of line, mwcc emits the synthesized D0
 * AHEAD of the written D1; the cartridge has D1 first (0x020d95a4) and D0
 * second (0x020d9638), and rombuild's fail-closed isolate refuses a TU whose
 * licensed .text is not in ROM address order. Inline, the destructor cannot
 * be the key function; InitResources -- the next virtual declared, and
 * non-inline -- takes that role, and the TU that defines it emits
 * _ZTV11dScMgCard_c. Slots 16 and 17 name D1 then D0, odr-using both, so the
 * compiler emits the pair for us in cartridge order. It also removes the
 * homeless D2 entirely -- a base-object variant byte-identical to D1, with no
 * ROM address to claim and no inbound relocation. The two __destroy_arr calls
 * preserve the cartridge's callback ABI and reverse member order; the third
 * releases the typed shared state. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one.
 * Same recipe as include/dScMgMCarlo2_c.h, and for the same reason. */
#ifndef DSCMGCARD_C_H
#define DSCMGCARD_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c1c64(char *t); /* decl_common.h's own signature */
extern "C" void func_ov006_020c1d80(void *t);
extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void _ZN12dMgCardObj_cD1Ev(void *elem);
extern "C" void _ZN17dMgDilarCardObj_cD1Ev(void *elem);

/* The element destroy callbacks use their ABI D1 spellings because C++ cannot
   take a destructor's address. Both live beside their constructor callbacks
   and classInit in src/actors/dScMgCard_c.cpp. The classes below declare no
   destructor because no member-destructor spelling lands the pair in the
   cartridge's own shape AND section order, measured across every form:
     both written out of line
        -> mwcc never -O4 auto-inlines a callee that is not marked inline:
           the derived D1 goes out as a bl to the base D2 (0x24 bytes with a
           push/blr frame; the cartridge carries 0x1c -- two vptr stores and
           no bl), and both classes' D0 and D2 come with it -- six
           destructor symbols emitted where the cartridge carries two; and
           reordering the source cannot help, because the section groups
           follow reverse source order -- the one order that puts the base
           body ahead of the derived definition (so an inline mark could
           bite) is exactly the order that flips the groups off the
           cartridge layout;
     both marked inline (in class, on the definition, any source order)
        -> the two D1s ARE emitted, byte-matching the cartridge pair, with
           no D0/D2 anywhere -- the bodies are right. The failure is
           emission ORDER: the element pair is emitted between the scene
           class's D1 (0x020d95a4) and its D0 (0x020d9638) -- the scene D0
           slides behind the element pair -- while the cartridge carries
           the scene pair adjacent with the element pair after it, and
           rombuild's fail-closed isolate refuses a TU whose licensed
           .text is not in ROM address order. #pragma force_inline /
           #pragma inline change neither outcome.
   Construction has a second measured wall: CW 1.2 rejects class/global
   placement-new declarations and has no builtin placement new, so the folded
   allocation+construction factory cannot be expressed as semantic C++.
   The vtable/RTTI records do not follow these callbacks: this header
   declares Render -- each class's key function -- non-inline, and
   src/actors/dScMgCard_c.cpp defines it, so the key-function TU keeps
   emitting _ZTV/_ZTI/_ZTS for both classes either way. */
struct dMgCardSharedState_c {
    u8  pad_000[0x1a];
    s16 unk_01a;
    u8  pad_01c[0x1ca];
    s16 unk_1e6;
    u8  pad_1e8[0x88];
};

typedef char dMgCardSharedState_c_size_must_be_0x270[sizeof(dMgCardSharedState_c) == 0x270 ? 1 : -1];

/* --- the card element classes ---------------------------------------------
   Both names are RTTI-confirmed (build/rtti.json: _ZTS12dMgCardObj_c and
   _ZTS17dMgDilarCardObj_c at 0x0213bc8c / 0x0213bc9c). Both are polymorphic
   with EXACTLY three vtable slots, measured off the cartridge's own tables
   (_ZTV12dMgCardObj_c at 0x0213bccc, _ZTV17dMgDilarCardObj_c at
   0x0213bcf4, 0x14-byte records, tools/rtti_vtables.py): slot 0 Render,
   slot 1 Update, slot 2 GetOffscreenY. NO virtual destructor -- the records
   are three slots wide and nothing in the range claims a D0.

   The slot names and every field name below are COINED from the matched
   bodies in src/actors/dScMgCard_c.cpp -- the ROM's symbols for these
   functions are address-only (func_ov006_020d970c and friends), so no
   recovered spelling exists to confirm or contradict them. Render draws the
   card through the Hud_RenderSprite bank table; Update runs the card's own
   motion state machine; GetOffscreenY answers the off-screen y (-0x60000)
   that DealIn starts the card flying in from. The fields: the 1:4096
   fixed-point position pair (mX, mY); the deal/move target pair (mTargetX,
   mTargetY) with its approach steps (mXStep, mYStep); the return/fly-away
   destination pair (mHomeX, mHomeY) -- SetHome writes it and states 7 and
   0xa fly toward it; the raise amount and its enable flag (mLift, mLiftFlag,
   0x4000 meaning fully raised, (mLift >> 12) indexing the flip frames);
   the fly-away stagger counter (mFlyDelay); the card's value byte (mValue,
   the weighted 0..5 draw), its state byte (mState) and the combo marker
   (mComboFlag) the hand evaluation sets on the cards forming the pair.

   Objects are constructed and destroyed through the C1/D1 ABI callbacks in
   src/actors/dScMgCard_c.cpp. Their class identities are RTTI-proven; the
   callback spellings remain only at the measured factory and section-order
   walls described above. */

struct dMgCardObj_c {
    virtual void Render();       /* slot 0 -- ov006 0x020d9bdc */
    virtual void Update();       /* slot 1 -- ov006 0x020d9c5c */
    virtual s32 GetOffscreenY(); /* slot 2 -- ov006 0x020d9bd0 */

    /* hand helpers reached on one card (not virtual, no table entry) */
    void DealIn(const s32 *src); /* ov006 0x020da0ac -- fly in from src {x, y} */
    void Redeal();               /* ov006 0x020da00c -- re-fly from own target */
    void SetHome(s32 n);         /* ov006 0x020d99ec -- home slot n, state 7 */
    void FlyOff();               /* ov006 0x020d99a4 -- staggered fly-away */

    s32 mX;         /* 0x04 */
    s32 mY;         /* 0x08 */
    s32 mTargetX;   /* 0x0c */
    s32 mTargetY;   /* 0x10 */
    s32 mXStep;     /* 0x14 */
    s32 mYStep;     /* 0x18 */
    s32 mHomeX;     /* 0x1c */
    s32 mHomeY;     /* 0x20 */
    s32 mLift;      /* 0x24 */
    s16 mFlyDelay;  /* 0x28 */
    u8  mValue;     /* 0x2a */
    u8  mState;     /* 0x2b */
    u8  mLiftFlag;  /* 0x2c */
    u8  mComboFlag; /* 0x2d */
    u8  pad_2e[0x2];/* 0x2e */
};

/* The dealer's hand. Same three slots, all three overridden, no new
   fields: the record is the same 0x30 (its _ZTI at 0x0213bc58 is the
   12-byte single-inheritance form, and the 0x14-byte record at
   0x0213bcec/0x0213bcf4 confirms it adds nothing). */
struct dMgDilarCardObj_c : dMgCardObj_c {
    virtual void Render();       /* slot 0 -- ov006 0x020d970c */
    virtual void Update();       /* slot 1 -- ov006 0x020d978c */
    virtual s32 GetOffscreenY(); /* slot 2 -- ov006 0x020d9998 */
};

typedef char dMgCardObj_c_size_must_be_0x30[sizeof(dMgCardObj_c) == 0x30 ? 1 : -1];
typedef char dMgDilarCardObj_c_size_must_be_0x30[sizeof(dMgDilarCardObj_c) == 0x30 ? 1 : -1];

struct dScMgCard_c : dScMgSingle3DBase_c {
    virtual ~dScMgCard_c() {
        __destroy_arr(mArray2, 5, 0x30, (void *)_ZN17dMgDilarCardObj_cD1Ev);
        __destroy_arr(mArray1, 5, 0x30, (void *)_ZN12dMgCardObj_cD1Ev);
        func_ov006_020c1c64((char *)&mShared);
    }

    /* --- this class's own vtable slots, named from the table ---
       Re-overrides of slots fBase_c already owns, NOT new virtuals: the
       table stays the base's width and no field moves. Declared AFTER the
       destructor so the destructor is still the first virtual declared. */
    virtual s32 InitResources();     /* slot 0 -- ov006 0x020dbaf0 */
    virtual s32 CleanupResources();  /* slot 3 -- ov006 0x020da994 */
    virtual s32 Behavior();          /* slot 6 -- ov006 0x020dabec */
    virtual s32 Render();             /* slot 9 -- ov006 0x020da9c4 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */

    virtual void OnGroundPounded();   /* slot 21 -- ov006 0x020db6ec */

    /* --- the round state machine and the HUD column ---
       UpdateState is the one long switch on mState that Behavior drives
       every frame; RenderHud draws the chip column and the prompt sprite.
       Names coined from the bodies -- the ROM's symbols are address-only. */
    void UpdateState();               /* ov006 0x020dac34 */
    void RenderHud();                 /* ov006 0x020d9a14 */

    /* --- hand helpers, static: they take the hand they work on ---
       The register shape is the evidence: r0/r1 carry the hand pointers,
       not a this. Names coined from the bodies; see each definition in
       src/actors/dScMgCard_c.cpp. */
    static int  CompareHands(const dMgCardObj_c *a, const dMgCardObj_c *b); /* 0x020da5e8 */
    static int  EvaluateHand(const dMgCardObj_c *hand, s16 *out);           /* 0x020da4ac */
    static void MarkSingles(dMgCardObj_c *hand);                            /* 0x020da420 */
    static int  ArrangeHand(dMgCardObj_c *hand);                            /* 0x020da174 */
    static void SettleHand(dMgCardObj_c *hand);                            /* 0x020da154 */
    static int  AllLifted(const dMgCardObj_c *hand);                        /* 0x020da834 */
    static int  AllInState(const dMgCardObj_c *hand, int state);           /* 0x020da860 */
    static int  CountInState(const dMgCardObj_c *hand, int state);         /* 0x020da88c */
    static int  FindInState(const dMgCardObj_c *hand, int state);          /* 0x020da8b8 */
    static int  DrawCardValue();                                           /* 0x020da8e4 */
    static void FillWeights(int v);                                       /* 0x020da974 */

    dMgCardSharedState_c mShared; /* 0x4f38..0x51a8 */
    dMgCardObj_c     mArray1[5];  /* 0x51a8 -- dtor _ZN12dMgCardObj_cD1Ev (hand stub) */
    dMgDilarCardObj_c mArray2[5]; /* 0x5298 -- dtor _ZN17dMgDilarCardObj_cD1Ev (hand stub) */
    s16 mState;              /* 0x5388 -- the round's step counter; UpdateState
                                is one long switch on it and mostly just `++`s it */
    s16 mStateTimer;         /* 0x538a -- reloaded (0x10, 0x14, 0x1e, 0x3c,
                                0x5a) on each step and run down to 0 before
                                mState advances */
    s16 unk_538c;            /* 0x538c */
    s16 unk_538e;            /* 0x538e -- the mArray1 (0x51a8) highlight pair:
                                Render blinks the card whose config byte equals
                                either of the two, and draws every other card
                                every frame. 6 means "none" */
    s16 unk_5390;            /* 0x5390 -- cleared back to 6 when the two banks
                                disagree, while unk_538e keeps its value */
    s16 unk_5392;            /* 0x5392 -- the same pair for mArray2 (0x5298) */
    s16 unk_5394;            /* 0x5394 */
    s16 mFrameCounter;       /* 0x5396 -- Behavior's only own statement is
                                `+= 1`; Render blinks on bit 3 */
    s16 mScore;              /* 0x5398 -- a high-water mark of dScMgBase_c's own
                                0xb4, pushed to the HUD counter every frame */
    s8  unk_539a;             /* 0x539a */
    u8  pad_539b[0x1];        /* 0x539b */
};

typedef char dScMgCard_c_size_must_be_0x539c[sizeof(dScMgCard_c) == 0x539c ? 1 : -1];

#endif
