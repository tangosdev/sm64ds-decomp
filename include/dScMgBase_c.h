/* Base class for every minigame scene -- 15 direct RTTI children, 32
 * descendants. Field names and widths were recovered from matched bodies;
 * see notes/minigame-provenance.md for the evidence behind each one, and for
 * why the destructor is declared here but defined out of line. */
#ifndef DSCMGBASE_C_H
#define DSCMGBASE_C_H
#include "dScene_c.h"
#include "dGraph_c.h"
#include "dMgState_c.h"
#include "dMgPsOpt_c.h"

extern "C" void *data_ov004_020beb68;

struct dScMgBase_c : dScene_c {

    class graphCallback_c;
    dScMgBase_c();
    /* Declared first (key function), and deliberately NOT defined inline:
       descendants call _ZN11dScMgBase_cD2Ev as a real `bl`, and an inline
       body makes every one of them miss. Defined in
       src/_ZN11dScMgBase_cD1Ev.cpp and .../_D0Ev.cpp.
       MEASURED -- do not move the body up here. */
    virtual ~dScMgBase_c();

    /* Own copy, deliberately: mwcc only inlines a D0 route through the class
       itself or its immediate base, so descendants cannot reach dScene_c's.
       Removing this breaks D0 for all 32 of them. */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }

    /* --- overrides of dScene_c's own virtuals, same signature, in _ZTV order.
           1, 2, 5, 7, 10 re-override slots dScene_c already gave a body;
           6, 9, 12 are the first override below fBase_c's own default. --- */
    virtual bool BeforeInitResources();                /* slot  1 */
    virtual void AfterInitResources(u32 vfSuccess);    /* slot  2 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot  5 */
    virtual s32  Behavior();                           /* slot  6 */
    virtual int  BeforeBehavior();                     /* slot  7 */
    virtual s32  Render();                             /* slot  9 */
    virtual int  BeforeRender();                       /* slot 10 */
    virtual void OnPendingDestroy();                   /* slot 12 */

    /* Slots 18-35 are eighteen further virtuals new at this class; their
       signatures are not reconstructed yet, so they stay undeclared. */

    s32 unk_050;            /* 0x050 */
    s32 unk_054;            /* 0x054 */
    s32 mTimeLimit;         /* 0x058 -- 60 or 120 from the scene ID */
    s32 mFrameCounter;      /* 0x05c -- wraps every 40 frames */
    s32 unk_060;            /* 0x060 */
    s32 unk_064;            /* 0x064 */
    u8  pad_068[0x38];
    s32 unk_0a0;            /* 0x0a0 */
    s32 unk_0a4;            /* 0x0a4 */
    s32 unk_0a8;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    u8  pad_0b0[0x4];
    s32 mHudScore;          /* 0x0b4 -- zeroed by BeforeInitResources; the
                                value func_ov004_020adb1c writes into the HUD
                                counter word. Leaves keep their own tallies and
                                copy the result here */
    s32 unk_0b8;            /* 0x0b8 */
    u32 unk_0bc;            /* 0x0bc */
    u16 mPromptBlinkTimer;  /* 0x0c0 -- free-runs 0..0x2f; the prompt shows
                                during the first 0x18 frames of each cycle */
    u8  unk_0c2;            /* 0x0c2 -- constructor seeds this to 1 */
    u8  mPromptEnabled;     /* 0x0c3 -- nothing is drawn while this is 0 */
    u8  mPromptBlinkCount;  /* 0x0c4 -- after 4 blink cycles the per-language
                                prompt sprite stays up every frame */
    u8  pad_0c5[0x3];
    s32 unk_0c8;            /* 0x0c8 */
    dMgState_c mStateController; /* 0x0cc -- minigame UI state controller */
    dMgPsOpt_c mTouchOptions; /* 0x0f4 -- eight polymorphic touch icons */
    u32 mSavedMainBgBits;   /* 0x21c -- func_ov004_020af094 saves data_0209d45c
                                here and func_ov004_020aeed8 restores it */
    u32 mSavedSubBgBits;    /* 0x220 -- the same pair for data_0209d454 */
    u32 mSavedScreenSwap;   /* 0x224 -- bit 15 of POWCNT1 (0x4000304), saved and
                                restored by the same two functions */
    u8  pad_228[0x4400];
    s32 mMenuOpen;          /* 0x4628 -- the three-item overlay menu is up;
                                BeforeRender skips the normal render pass and
                                draws that instead */
    s32 unk_462c;           /* 0x462c */
    s32 unk_4630;           /* 0x4630 */
    s16 mMenuItem0X;        /* 0x4634 */
    s16 mMenuItem0Y;        /* 0x4636 */
    s16 mMenuItem1X;        /* 0x4638 */
    s16 mMenuItem1Y;        /* 0x463a */
    s16 mMenuItem2X;        /* 0x463c */
    s16 mMenuItem2Y;        /* 0x463e */
    s32 mMenuCursorPhase;   /* 0x4640 -- drives the highlighted item's pulse */
    u8  pad_4644[0x2];
    s16 mMenuCursor;        /* 0x4646 -- highlighted item, -1 for none */
    s32 unk_4648;           /* 0x4648 */
    s32 unk_464c;           /* 0x464c */
    u8  pad_4650[0x4];
    s32 unk_4654;           /* 0x4654 */
    s32 unk_4658;           /* 0x4658 */
    u8  unk_465c;           /* 0x465c */
    u8  pad_465d[0x1];
    s16 mSceneKind;         /* 0x465e -- table-selected from param1's high byte */
};

/* The nested callback class the ROM's type graph records for this scene,
   deriving from dGraph_c::callback_c (include/dGraph_c.h). It is registered
   with the owning scene as its context; mScene at +0x4 is that back-pointer,
   read by all four slots. Declaring the class adds no storage to
   dScMgBase_c -- the object itself is not a member of this class.

   The slots are declared virtual, matching dGraph_c::callback_c's own
   declarations (include/dGraph_c.h): the base supplies the vptr at +0x0,
   so the derived fields fall where the ROM puts them, and strict object
   isolation discards the vtable/RTTI passengers the function range does
   not own. */
class dScMgBase_c::graphCallback_c : public dGraph_c::callback_c {
public:
    dScMgBase_c *mScene;                             /* 0x04 */

    virtual int GraphCallback0();                            /* slot 0 */
    virtual int GraphCallback1();                            /* slot 1 */
    virtual int GraphCallback2();                            /* slot 2 */
    virtual int GraphCallback3();                            /* slot 3 */
};

/* A floor, not a claim the object ends here: 0x465c is the last field any
   matched body has observed. See notes/minigame-provenance.md. */
typedef char dScMgBase_c_size_must_be_0x4660[sizeof(dScMgBase_c) == 0x4660 ? 1 : -1];

#endif
