/* dScMgTrampoline2_c -- Trampoline Terror. Draw lines on the touch screen
 * so Mario can bounce through the shapes that drop in. Child of
 * dScMgD3DBase_c, size 0x7bac from the factory's operator new.
 *
 * RTTI: dScMgTrampoline2_c : dScMgD3DBase_c; vtable ov006:0x0213fc7c.
 * Historical alias MgTrampolineTerror_Spawn. The element classes TrmpLine and
 * TrmpScorePopup are shared with dScMgTrampoline_c, Jump2Popup with
 * dScMgJump2_c. The state PMF remains #2497.
 *
 * The factory is `new dScMgTrampoline2_c`: the implicit constructor builds
 * the six element arrays through __cxa_vec_ctor in member order, and the
 * empty inline destructor's member teardown runs them through
 * __cxa_vec_cleanup in exact reverse (mScorePopups first, mMarios last), which
 * is the cartridge's D1/D0 order. dScMgD3DBase_c supplies operator delete.
 * 0x7acc has no matched read.
 */
#ifndef DSCMGTRAMPOLINE2_C_H
#define DSCMGTRAMPOLINE2_C_H
#include "dScMgD3DBase_c.h"
#include "dMgTrmpln2Mario_c.h"
#include "TrmpLine.h"
#include "TrmpItem.h"
#include "Jump2Popup.h"
#include "Trmp2Spinner.h"
#include "TrmpScorePopup.h"

struct dScMgTrampoline2_c : dScMgD3DBase_c {
    typedef void (dScMgTrampoline2_c::*State)();

    virtual ~dScMgTrampoline2_c() {}
    virtual void StartRound(int arg);               /* slot 18 */
    virtual int  CanEndState(int mode);              /* slot 19 */
    virtual int  OnGraphCallback0();                        /* slot 23 */
    virtual int  OnGraphCallback2();                           /* slot 24 */
    virtual int  OnGraphCallback3();                           /* slot 25 */
    virtual void PaintSquare(int cx, int cy, int colour, int size); /* slot 34 */

    /* State callbacks stored in mState, the same scheme dScMgTrampoline_c
       uses: eight-byte PMF records in .data, dispatched by Behavior every
       frame. The State/Begin names are coined from the matched bodies; the
       ROM symbols were address-only. */
    static void StateDone(); /* ov006 0x02123b20 */
    void StateWaitExit();    /* ov006 0x02123b24 */
    void StateResults();     /* ov006 0x02123bf4 */
    void StatePlay();        /* ov006 0x02123cb4 */
    void StateIntro();       /* ov006 0x02124088 */

    void BeginResults();     /* ov006 0x02123c78 */
    void BeginPlay();        /* ov006 0x02124040 */
    void BeginIntro();       /* ov006 0x02124228 */

    void UpdateTouchInput(); /* ov006 0x02123938 */
    void SpawnWave();        /* ov006 0x02123428 -- one random wave of shapes */
    void InitDisplay();      /* ov006 0x021245a8 -- loads BG/OBJ screens */
    void InitBrush(int layer); /* ov006 0x02122e20 -- fills the stroke layer */

    /* Raw eight-byte state PMF. Behavior calls through it. A typed
       global initializer emits a __sinit this ROM does not have. */
    u32 mState[2];            /* 0x5004 */
    dMgTrmpln2Mario_c mMarios[5]; /* 0x500c -- 5 * 0xdc */
    TrmpLine mLines[3];           /* 0x5458 -- 3 * 0x32c */
    TrmpItem mItems[0xa];         /* 0x5ddc -- 0xa * 0x1d0 */
    Jump2Popup mPopups[0xa];      /* 0x6ffc -- 0xa * 0x24 */
    Trmp2Spinner mSpinners[0x14]; /* 0x7164 -- 0x14 * 0x78 */
    u32 mParticleID;          /* 0x7ac4 -- Particle::System unique ID */
    int mRamp;                /* 0x7ac8 -- fixed point; rests at 0x14000, eased
                                 toward 0x7000 as misses come in */
    u8  pad_7acc[0x4];        /* 0x7acc -- no matched read */
    TrmpScorePopup mScorePopups[5]; /* 0x7ad0 -- 5 * 0x24 */
    int mTimer;               /* 0x7b84 -- countdown between waves and states */
    int mPattern;             /* 0x7b88 -- scripted spawn-table index, toward 6 */
    int mDragSoundHandle;     /* 0x7b8c -- positional drag sound, fed back */
    int mScoreGate;           /* 0x7b90 -- score at which the tier advances */
    int mScriptDone;          /* 0x7b94 -- set once mPattern reaches 6 */
    int mWaveStep;            /* 0x7b98 -- counts to 4 between random waves */
    s16 mTouchX;              /* 0x7b9c -- current stylus sample */
    s16 mTouchY;              /* 0x7b9e */
    s16 mTouchStartX;         /* 0x7ba0 -- press edge; the swipe is measured
                                 against it */
    s16 mTouchStartY;         /* 0x7ba2 */
    s16 mInputEnabled;        /* 0x7ba4 -- while 0 the touch handler clears
                                 mTouching and returns */
    u8  pad_7ba6[2];          /* 0x7ba6 */
    s16 mRoundOver;           /* 0x7ba8 -- Render skips the playfield once set */
    u8  mTouching;            /* 0x7baa */
    u8  mTouchReleased;       /* 0x7bab -- OnGraphCallback0 consumes it */

    /* These definitions override existing base slots without adding fields.
       The production TU defines the non-inline virtuals and emits the vtable;
       its manifest owns and verifies that table. The shared base supplies
       reconstructed callback names and signatures, not original-name proof. */
    s32 InitResources();      /* slot 0 -- src/minigames/d_s_mg_trampoline2.cpp */
    s32 CleanupResources();   /* slot 3 -- ov006 0x0212318c */
    s32 Behavior();           /* slot 6 -- ov006 0x02123340 */
    s32 Render();             /* slot 9 -- ov006 0x021231ac */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgTrampoline2_c_size_must_be_0x7bac[sizeof(dScMgTrampoline2_c) == 0x7bac ? 1 : -1];
#endif

#endif
