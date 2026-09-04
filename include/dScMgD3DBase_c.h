/* class dScMgD3DBase_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgD3DBase_c : dScMgBase_c, single edge, offset 0 (build/rtti.json).
 * Read straight out of the ROM: the vptr value is ov006:0x0213c62c, the
 * RTTI pointer one word below it is 0x0213c5c8 = _ZTI14dScMgD3DBase_c, and
 * that record's _ZTS string reads "14dScMgD3DBase_c".
 *
 * THIS FILE WAS include/MgBounceAndPounce.h, AND THE COINED NAME WAS WRONG
 * ABOUT THE LEVEL OF THE HIERARCHY, NOT JUST THE SPELLING. That name was
 * assigned believing this was one minigame. It is the shared base of FOUR:
 * dScMgJump_c, dScMgJump2_c, dScMgTrampoline_c, dScMgTrampoline2_c.
 *
 * THE OLD HEADER INHERITED THAT ERROR AND MODELLED THIS CLASS WITH ONE
 * CHILD'S FIELDS. It declared mModel at 0x501c, a 3*0xb8 table at 0x506c
 * and a 6*0xf0 table at 0x5294, and asserted sizeof == 0x5834 "from
 * dScMgJump_c_classInit's own operator new(0x5834) -- the third and
 * strongest confirmation". All four belong to dScMgJump_c.
 *
 * dScMgJump_c_classInit IS dScMgJump_c's FACTORY, and its own body says
 * so: it writes THIS class's vtable, and then writes _ZTV11dScMgJump_c --
 * dScMgJump_c's own vtable, confirmed by RTTI -- and only AFTER that derived
 * vptr store does it construct the Model at 0x501c and the two tables.
 * Everything a factory builds after the derived vptr store belongs to the
 * derived class. 0x5834 is dScMgJump_c's size.
 *
 * WHERE THIS CLASS ACTUALLY ENDS: 0x5004.
 *
 * From below, two witnesses. Each of the four children's own D1 destroys
 * its own members, then stores THIS class's vtable, then destroys
 * mSysTracker at 0x47e4, then chains to _ZN11dScMgBase_cD2Ev -- nothing of
 * this class's is destroyed after mSysTracker, and
 * 0x47e4 + sizeof(Particle::SysTracker) (0x81c) = 0x5000 exactly. Then this
 * class's OWN BeforeInitResources writes a 4-byte 0 at 0x5000, which is the
 * only member it has past mSysTracker and the reason this file does not
 * stop at 0x5000 (the first draft of it did; the write is what caught it).
 *
 * From above, the bound is dScMgJump_c's: a Jump helper
 * (src/func_ov006_020ee2c4.c) writes 0x5004 and 0x5014, and no method of
 * THIS class touches either. Its three siblings begin at 0x500c instead, so
 * 0x5004..0x500c is Jump's alone.
 *
 * That 0x5004 boundary is the tightest reading the base's own code
 * supports, and it is deliberately the conservative one: a child can always
 * declare padding it does not use, but it cannot un-inherit a field. If a
 * later witness shows 0x5004 or 0x5008 is shared base state after all, the
 * boundary moves -- and all four children would fail build_pin at once,
 * which is exactly the safety net that makes the conservative choice cheap.
 *
 * The old size assert never caught the error because the three misplaced
 * members are raw `u8` arrays with trivial destructors -- declaring them
 * changed no byte of this class's own D1, and a `sizeof` assert only checks
 * the number against itself.
 *
 * THE DESTRUCTOR IS DEFINED INLINE, and that IS correct -- verified against
 * a real descendant rather than assumed, because this tree has been bitten
 * both ways (an out-of-line base destructor emitting a call to a D2 that
 * exists nowhere, and an inline one the ROM actually calls as a real `bl`).
 * All four children's own D1s contain this class's vptr store, mSysTracker
 * destruction and the chain to dScMgBase_c's D2 written out INLINE, with no
 * call to any dScMgD3DBase_c-specific destructor. Own copy of
 * operator delete for the same reason: mwcc only inlines a D0 route through
 * the class itself or its IMMEDIATE base.
 *
 * Note the contrast one level up, and it is not a contradiction: this class
 * inlines into ITS children, while dScMgBase_c's own destructor is NOT
 * inlined into this one (the ROM's 0x38-byte D1 here calls
 * _ZN11dScMgBase_cD2Ev as a real `bl`). mwcc inlines a trivial destructor
 * unconditionally and refuses one with real statements across a TU
 * boundary. See include/dScMgBase_c.h's own note.
 */
#ifndef DSCMGD3DBASE_C_H
#define DSCMGD3DBASE_C_H
#include "dScMgBase_c.h"
#include "Stage.h"

struct dScMgD3DBase_c : dScMgBase_c {
    /* Declared first -- see include/dScene_c.h's KEY FUNCTION note. Overrides
       slots 16 (D1) and 17 (D0). DEFINED INLINE, see the file banner. */
    virtual ~dScMgD3DBase_c() {}

    /* Own copy, same reason dScMgBase_c has one -- unlocks D0 for the four
       descendants. */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }

    /* --- overrides. 1, 2, 5, 7, 10 re-override slots dScMgBase_c already
           gave a body; 11 (AfterRender) is the first override below dScene_c's
           own default (dScMgBase_c never touched it). --- */
    virtual bool BeforeInitResources();                /* slot  1 */
    virtual void AfterInitResources(u32 vfSuccess);    /* slot  2 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot  5 */
    virtual int  BeforeBehavior();                     /* slot  7 */
    virtual int  BeforeRender();                       /* slot 10 */
    virtual void AfterRender(u32 vfSuccess);           /* slot 11 */
    virtual int  OnKicked();                           /* slot 24 */
    virtual int  OnPushed();                           /* slot 25 */
    virtual int  OnHitByCannonBlastedChar();           /* slot 26 */
    virtual void OnHitByMegaChar();                    /* slot 27 */
    virtual int  OnHitFromUnderneath();                /* slot 28 */
    virtual int  OnAimedAtWithEgg();                   /* slot 29 */
    virtual void OnAimedAtWithEggReturnVec();          /* slot 30 */
    virtual int  Virtual7C();                          /* slot 31 */
    virtual void Virtual84();                          /* slot 33 */

    /* THAT LIST IS EMPTY NOW.  This class overrides nine of dScMgBase_c's own
       18 new slots (18-35) -- 24 through 31, and 33 -- per
       tools/rtti_vtables.py --own dScMgD3DBase_c, and slot 33 above was the
       last one still waiting on its base to name it.  All nine are spelled out
       here; nothing this class overrides is left implicit, and its table is a
       byte-exact 34-slot prefix rather than a 31-slot one.

       They are the same case nine times over: dScMgJump_c and dScMgJump2_c both
       point at THIS class's body for each of them, so the declaration belongs
       here and neither child declares any of them.  Slots 26 through 31 and 33
       widen that to all four children -- dScMgTrampoline_c and
       dScMgTrampoline2_c point here too, where at 24 and 25 they had bodies of
       their own. */

    s32  unk_4660;                    /* 0x4660 */
    u16  unk_4664;                    /* 0x4664 -- both factories zero it right
                                          after the base constructor */
    u8   pad_4666[0x6];               /* 0x4666 */
    /* 0x466c..0x47e4 is TWO ELEMENTS OF 0xbc, not flat state, and all four
       factories say so: each runs `e = p + 0x466c; do { e += 0xbc; } while
       (e != p + 0x47e4);`, and 0x47e4 - 0x466c = 0x178 = 2 * 0xbc. The
       fourteen fields below prove the stride independently -- 0x470c..0x4724
       and 0x47c8..0x47e0 are the SAME seven offsets within each element
       (+0xa0..+0xb8), and all four children reference all fourteen. Kept as
       flat names for now because the eight files that read them spell them
       this way; typing it as a real array is its own change. */
    /* AND EACH ELEMENT IS A MINIGAME CAMERA. src/Camera_UpdateMatrices.c
       carries the 0xbc layout: matrices in the head, `eye` at +0xa0, `target`
       at +0xac and `angle` at +0xb8. dScMgJump_c::InitResources settles which
       element is which by calling Camera_UpdateMatrices(this + 0x466c) and
       Camera_UpdateMatrices(this + 0x4728) -- the two element bases -- so the
       fourteen fields below are those three members of each. */
    u8   pad_466c[0xa0];              /* 0x466c -- camera 0, matrices */
    s32  mCamera0EyeX;                /* 0x470c */
    s32  mCamera0EyeY;                /* 0x4710 */
    s32  mCamera0EyeZ;                /* 0x4714 */
    s32  mCamera0TargetX;             /* 0x4718 */
    s32  mCamera0TargetY;             /* 0x471c */
    s32  mCamera0TargetZ;             /* 0x4720 */
    u16  mCamera0Angle;               /* 0x4724 */
    u8   pad_4726[0xa2];              /* 0x4726 -- camera 1, matrices */
    s32  mCamera1EyeX;                /* 0x47c8 */
    s32  mCamera1EyeY;                /* 0x47cc */
    s32  mCamera1EyeZ;                /* 0x47d0 */
    s32  mCamera1TargetX;             /* 0x47d4 */
    s32  mCamera1TargetY;             /* 0x47d8 */
    s32  mCamera1TargetZ;             /* 0x47dc */
    u16  mCamera1Angle;               /* 0x47e0 */
    u8   pad_47e2[0x2];               /* 0x47e2 */
    Particle::SysTracker mSysTracker; /* 0x47e4 -- AfterInitResources initialises
                                          it, BeforeBehavior updates it, and all
                                          four children destroy it inline */
    s32  unk_5000;                    /* 0x5000 -- real matched access, this
                                          class's own BeforeInitResources writes
                                          it as a 4-byte 0 */
};

/* 0x47e4 + 0x81c = 0x5000, plus unk_5000 = 0x5004. See the file banner for
   both bounds. If this is short, all four children's fields land on the
   wrong bytes and build_pin says so at once. */
typedef char dScMgD3DBase_c_size_must_be_0x5004[sizeof(dScMgD3DBase_c) == 0x5004 ? 1 : -1];

#endif
