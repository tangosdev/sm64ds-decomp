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
 * vptr store does it construct the dExtSimpleModel_c at 0x501c and the two tables.
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
 * From above, the bound is dScMgJump_c's: a Jump helper (State_Final,
 * once a one-function source directly under src/ that no longer exists --
 * it is now a member of src/minigames/d_s_mg_jump_methods.cpp, whose manifest entry
 * config/tu_manifest.d/ov006/dScMgJump_c.json keeps the old full path in its
 * functions[].legacy_source row) writes 0x5004 and 0x5014, and no method of
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
#include "dScStage_c.h"

struct dScMgD3DBase_c : dScMgBase_c {
    /* Declared first -- see include/dScene_c.h's KEY FUNCTION note. Overrides
       slots 16 (D1) and 17 (D0). DEFINED INLINE, see the file banner. */
    virtual ~dScMgD3DBase_c() {}

    /* Inline, and that is what the four factories show: each runs
       dScMgBase_c's C2, stores this class's vtable, zeroes mDrawSubScreen,
       steps over mCameras and constructs mSysTracker, with no call to any
       dScMgD3DBase_c constructor (none exists in the cartridge). The zero is
       a mem-initializer: written in the body it lands after mSysTracker's
       construction instead of before the camera loop (measured on
       dScMgJump_c_classInit). */
    dScMgD3DBase_c() : mDrawSubScreen(0) {}

    /* Own copy, same reason dScMgBase_c has one -- unlocks D0 for the four
       descendants. */
    void operator delete(void *ptr) {
        Memory::Deallocate(ptr, (Heap *)data_020a0eac);
    }

    /* --- overrides. 1, 2, 5, 7, 10 re-override slots dScMgBase_c already
           gave a body; 11 (AfterRender) is the first override below dScene_c's
           own default (dScMgBase_c never touched it). --- */
    virtual bool BeforeInitResources();                /* slot  1 */
    virtual void AfterInitResources(u32 vfSuccess);    /* slot  2 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot  5 */
    virtual int  BeforeBehavior();                     /* slot  7 */
    virtual int  BeforeRender();                       /* slot 10 */
    virtual void AfterRender(u32 vfSuccess);           /* slot 11 */
    virtual int  OnGraphCallback2();                           /* slot 24 */
    virtual int  OnGraphCallback3();                           /* slot 25 */
    virtual int  GetDisplayMode();           /* slot 26 */
    virtual void OpenMenu();                    /* slot 27 */
    virtual int  CloseMenu();                /* slot 28 */
    virtual int  SaveScreenForMenu();                   /* slot 29 */
    virtual void RestoreScreenFromMenu();          /* slot 30 */
    virtual int  InitSubScreenBg();                          /* slot 31 */
    virtual void InitEngines();                          /* slot 33 */

    /* THAT LIST IS EMPTY NOW.  This class overrides nine of dScMgBase_c's own
       18 new slots (18-35) -- 24 through 31, and 33 -- per
       tools/rtti_vtables.py --own dScMgD3DBase_c, and slot 33 above was the
       last one still waiting on its base to name it.  All nine are spelled out
       here; nothing this class overrides is left implicit, and the emitted
       table is the WHOLE 36-slot table -- not a prefix of any length.

       THAT LAST CLAUSE USED TO READ "a byte-exact 34-slot prefix", and it was
       stale: it was written while slots 34 and 35 were still unnamed.
       dScMgBase_c.h now declares all eighteen of its own new slots through 35,
       so mwcc emits 0x98 bytes here -- two header words plus 36 code pointers
       -- and the cartridge agrees word for word.  Read straight out of ov006:
       offset-to-top 0 at 0x0213c624, _ZTI14dScMgD3DBase_c at 0x0213c628,
       address point 0x0213c62c, 36 consecutive code pointers, last slot at
       0x0213c6b8.  The word at 0x0213c6bc is 0x6f6d6168 -- the start of the
       string "hamon", which carries its own data symbol -- so there is no 37th
       slot, and no trailing null slot in which an undeclared override could
       hide.  Do not reintroduce prefix language here without re-reading those
       words; a "prefix" claim is exactly what would excuse a short table.

       They are the same case nine times over: dScMgJump_c and dScMgJump2_c both
       point at THIS class's body for each of them, so the declaration belongs
       here and neither child declares any of them.  Slots 26 through 31 and 33
       widen that to all four children -- dScMgTrampoline_c and
       dScMgTrampoline2_c point here too, where at 24 and 25 they had bodies of
       their own. */

    s32 mSavedSubObjBank; /* 0x4660 -- sub-OBJ VRAM bank saved at init, restored after */
    u16 mDrawSubScreen;   /* 0x4664 -- flips every frame: 1 draws the 3D for the sub
                             screen (D3D_CaptureToSub), 0 for the main; both
                             factories zero it after the base constructor */
    u8 pad_4666[0x6];     /* 0x4666 */
    /* 0x466c..0x47e4 is TWO ELEMENTS OF 0xbc, and all four factories say
       so: each runs `e = p + 0x466c; do { e += 0xbc; } while (e != p +
       0x47e4);`, and 0x47e4 - 0x466c = 0x178 = 2 * 0xbc. That empty loop is
       what an array of a class with an empty inline constructor and no
       destructor compiles to, which is why Camera declares one: with it,
       `new dScMgJump_c` reproduces dScMgJump_c_classInit byte for byte.
       The seven fields at +0xa0..+0xb8 prove the stride independently;
       all four children set them for both elements. */
    /* EACH ELEMENT IS A MINIGAME CAMERA. Camera_UpdateMatrices (src/minigames/unit020bfec0.cpp)
       carries the 0xbc layout: matrices in the head, `eye` at +0xa0, `target`
       at +0xac and `angle` at +0xb8. dScMgJump_c::InitResources settles which
       element is which by calling Camera_UpdateMatrices(this + 0x466c) and
       Camera_UpdateMatrices(this + 0x4728) -- the two element bases. */
    struct Camera {
        Camera() {}
        u8 pad_00[0xa0]; /* 0x00 -- matrices */
        s32 mEyeX;       /* 0xa0 */
        s32 mEyeY;       /* 0xa4 */
        s32 mEyeZ;       /* 0xa8 */
        s32 mTargetX;    /* 0xac */
        s32 mTargetY;    /* 0xb0 */
        s32 mTargetZ;    /* 0xb4 */
        u16 mAngle;      /* 0xb8 */
        u8 pad_ba[0x2];  /* 0xba */
    };
    Camera mCameras[2];               /* 0x466c -- [0] main screen, [1] sub screen */
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
#ifndef SM64DS_PLATFORM_PC
typedef char
    dScMgD3DBase_c_Camera_size_must_be_0xbc[sizeof(dScMgD3DBase_c::Camera) == 0xbc ? 1 : -1];
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgD3DBase_c_size_must_be_0x5004[sizeof(dScMgD3DBase_c) == 0x5004 ? 1 : -1];
#endif

#endif
