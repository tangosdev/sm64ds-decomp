/* class dScMgCup_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgCup_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable sits at ov006:0x0213c154; the RTTI
 * pointer one word below it is _ZTI10dScMgCup_c, which is what ties that
 * table to this class rather than to a neighbour.
 *
 * THIS CLASS HAD NO HEADER AT ALL before this file -- unlike its twelve
 * siblings it never got an auto-generated skeleton, so there are no
 * inherited field names to carry over and no `.c` includers to convert.
 * Every offset below comes from the two structors, nothing from a prior
 * header.
 *
 * SIZE 0x5470, from the factory's own `_ZN7fBase_cnwEj(0x5470)`
 * (dScMgCup_c_classInit, now in src/actors/dScMgCup_c.cpp).
 *
 * THE FACTORY AND THE DESTRUCTOR AGREE MEMBER FOR MEMBER, which is the
 * bar this tree sets (see include/dScMgCard_c.h's own note). The factory
 * constructs, in order: the 0x4f38 table (func_ov006_020c33dc), then
 * mArray1 at 0x50e8 (0x20 elements of 0x18, element ctor
 * func_ov006_020e0634), then mArray2 at 0x53e8 (3 elements of 8, element
 * ctor func_0203d738). The destructor destroys exactly those three in
 * reverse order, with the matching element destructors
 * (NullDestructor_0203d47c, func_ov006_020deac4, func_ov006_020c3288).
 * Nothing is left over either way.
 *
 * COMPONENT at 0x4f38 has an unknown full extent. The current raw-storage
 * declaration ends at +0x19c, where three s32 declarations begin at 0x50d4.
 * StateSelect and OnYoshiTryEat access them using split offset literals.
 * This declaration boundary is not a proven component boundary: its
 * constructor func_ov006_020c33dc writes component +0x1a4 and +0x1a8,
 * corresponding to scene 0x50dc and 0x50e0. Preserve this storage layout
 * until a measured component/field reconstruction replaces it.
 *
 * OWN TAIL, 0x5400..0x5470, IS THE CLASS'S STATE MACHINE, not the
 * unclaimed trailing space the first draft of this header called it. The
 * first statement of Cup's own Behavior (vtable slot 6) is now a typed
 * pointer-to-member dispatch through data_ov006_02141870[mState];
 * InitResources
 * (src/actors/dScMgCup_c.cpp, slot 0) already carries a local struct naming
 * `ones[3]` at 0x540c, `ids[3]` at 0x5420 and `flags[3]` at 0x5465; and
 * slot 9's Render reads 0x5462 and 0x5468. State code also accesses
 * 0x5469..0x546d; the remaining byte storage below does not mean it is unused.
 *
 * The size is unaffected -- 0x5470 is the factory's literal either way --
 * but the span and the literal AGREE here, which the first draft denied.
 *
 * THE PROMOTED TU HAS ONE SOURCE-LEVEL DESTRUCTOR DEFINITION and lets
 * CodeWarrior emit D1/D0/D2 from it; D1 and D0 are enrolled from
 * src/actors/dScMgCup_c.cpp and D2 is a deadstripped compiler-only passenger.
 * No separate operator delete is needed; dScMgBase_c, two levels up, already
 * provides one.
 *
 * THE FIELD NAMES BELOW THAT ARE NOT `unk_`/`pad_` ARE INFERENCES, NOT ROM
 * FACTS. notes/data/class-facts/dScMgCup_c.json proves eleven offsets in
 * 0x5400..0x5470 and names none of them: mOnes, mIds and mFlags come from a
 * local struct in InitResources, and mState, mShuffleSound, mShuffleAngle and
 * mShuffleSpeed from behaviour. Much of the TU still uses raw offsets and
 * an Obj6e view. Typed or offset-named members need fresh byte proof, not
 * recovered English names; that remaining work belongs to issue #2492. */
#ifndef DSCMGCUP_C_H
#define DSCMGCUP_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c3288(char *t); /* decl_common.h's own signature */
extern "C" void __cxa_vec_cleanup(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_020deac4(void);
extern "C" void NullDestructor_0203d47c(void);

struct dScMgCup_c : dScMgSingle3DBase_c {
    virtual ~dScMgCup_c();

    /* The ROM vtable at ov006 0x0213c154 differs from the direct base in seven
       slots: 0, 6, 9, 16, 17, 18 and 20 (16/17 are the destructor pair).
       Overrides remain virtual whether the keyword is repeated or omitted;
       they reuse existing slots and add no field. The inherited slot names,
       including OnYoshiTryEat and Virtual50, are reconstructed interface names,
       not strings read from this vtable. The destructor remains the key
       function; compiler vtable/RTTI passengers are externalized to the
       canonical cartridge copies by the manifest. */
    /* Inferred class-anchored aliases: __sinit_ov006_021303d0 fixes this
       eight-entry PMF order; behavior fixes the roles, but the original
       source spellings are not present in the ROM. */
    void StateSetup();
    void StatePrepareShuffle();
    void StateShuffle();
    void StateWaitForInput();
    void StateSelect();
    void StateResult();
    void StateFinish();
    void StateIdle();
    s32 InitResources();  /* slot  0 -- src/actors/dScMgCup_c.cpp */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual void Virtual50();                          /* slot 20 */
    s32 Behavior();       /* slot  6 -- src/actors/dScMgCup_c.cpp */
    s32 Render();         /* slot  9 -- src/actors/dScMgCup_c.cpp */

    u8  pad_4f38[0x19c];  /* 0x4f38 -- raw component storage prefix, see banner */
    s32 unk_50d4;         /* 0x50d4 -- split-literal access, see file banner */
    s32 unk_50d8;         /* 0x50d8 */
    s32 unk_50dc;         /* 0x50dc */
    u8  pad_50e0[0x8];    /* 0x50e0 -- component ctor writes here; typing pending */
    u8  mArray1[0x300];   /* 0x50e8 -- 0x20 * 0x18, elem dtor func_ov006_020deac4 */
    u8  mArray2[0x18];    /* 0x53e8 -- 3 * 8, elem dtor NullDestructor_0203d47c.
                             InitResources already recovers the element
                             as a pair of s32; left raw here, own change. */
    u8  pad_5400[0xc];    /* 0x5400 -- live words at +0, +4, +8; typing pending */
    s32 mOnes[3];         /* 0x540c -- named `ones[3]` by InitResources' own struct */
    s32 mState;           /* 0x5418 -- Behavior's pointer-to-member dispatch index */
    u8  pad_541c[0x4];    /* 0x541c */
    s32 mIds[3];          /* 0x5420 -- named `ids[3]` by InitResources' own struct */
    u8  pad_542c[0x8];    /* 0x542c */
    s32 unk_5434[3];      /* 0x5434 -- three parallel per-cup arrays indexed */
    s32 unk_5440[3];      /* 0x5440    together by Behavior's own loop */
    s32 unk_544c[3];      /* 0x544c */
    s32 mShuffleSound;     /* 0x5458 */
    u16 mShuffleAngle;     /* 0x545c */
    s16 mShuffleSpeed;     /* 0x545e */
    u8  unk_5460;          /* 0x5460 */
    u8  unk_5461;          /* 0x5461 */
    u8  unk_5462[3];      /* 0x5462 */
    u8  mFlags[3];        /* 0x5465 -- named `flags[3]` by InitResources' own struct */
    u8  unk_5468;         /* 0x5468 */
    u8  pad_5469[0x7];    /* 0x5469 -- live bytes through 0x546d; typing pending */
};

typedef char dScMgCup_c_size_must_be_0x5470[sizeof(dScMgCup_c) == 0x5470 ? 1 : -1];

#endif
