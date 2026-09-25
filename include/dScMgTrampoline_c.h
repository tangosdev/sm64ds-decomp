/* dScMgTrampoline_c -- Trampoline Time. Draw up to three lines on the touch
 * screen; Mario bounces off them toward the lit door. Three misses end
 * the round. Child of dScMgD3DBase_c, size 0x5dc8 from the factory's
 * operator new.
 *
 * RTTI: dScMgTrampoline_c : dScMgD3DBase_c (tools/rtti_extract.py); vtable
 * ov006:0x0213fb34. Factory dScMgTrampoline_c_classInit (historical alias
 * MgTrampolineTime_Spawn) installs it for the MG_TRAMPOLINE profile. The
 * factory builds 0x500c, 0x534c and 0x5cd0; the destructor tears them down
 * in exact reverse.
 *
 * The destructor is defined in the class body: D1 0x021207dc sits below D0
 * 0x02120880. An out-of-line definition emits D0 before D1, plus a D2 this
 * ROM does not have, so the TU (.text 0x021207dc..0x02122490) could not be
 * linked in address order. InitResources is the first virtual not defined
 * here and stays the key function. dScMgD3DBase_c supplies operator delete.
 *
 * The three element arrays stay raw: their private types are not recovered.
 * From 0x5d84 the tail is the flashing door mark, the scroll, the stylus
 * sample, and the timer that moves the lit door.
 */
#ifndef DSCMGTRAMPOLINE_C_H
#define DSCMGTRAMPOLINE_C_H
#include "dScMgD3DBase_c.h"

extern "C" void __cxa_vec_cleanup(void *base, int count, int stride, void *dtor);
extern "C" void *func_ov006_020ccfc8(void);
extern "C" void func_ov006_020d1008(void);
extern "C" void func_ov006_02120938(void);

/* The flashing mark on the lit door, 0xc bytes at 0x5d84. The helpers that
   drive it keep short* / char* parameters so they still match decl_common.h. */
struct dScMgTrampoline_DoorMark {
    s16 row;            /* 0x00 -- which door the flash marks, 0 or 1 */
    s16 timer;          /* 0x02 -- counts down from 0x3c while it blinks */
    s32 active;         /* 0x04 -- nonzero while that blink is running */
    s32 shown;          /* 0x08 -- toggles with timer / 10 */
};
typedef char dScMgTrampoline_DoorMark_size_must_be_0xc[sizeof(dScMgTrampoline_DoorMark) == 0xc ? 1 : -1];

struct dScMgTrampoline_c : dScMgD3DBase_c {
    typedef void (dScMgTrampoline_c::*State)();

    /* In the class body so D1 is emitted before D0 and no D2 is emitted.
       A virtual destructor is still reached through the vtable, not inlined
       into callers. */
    virtual ~dScMgTrampoline_c() {
        __cxa_vec_cleanup(mArray3, 5, 0x24, (void *)func_ov006_02120938);
        __cxa_vec_cleanup(mArray2, 3, 0x32c, (void *)func_ov006_020d1008);
        __cxa_vec_cleanup(mArray1, 4, 0xd0, (void *)func_ov006_020ccfc8);
    }

    virtual s32 InitResources();     /* slot 0 */
    virtual s32 CleanupResources();  /* slot 3 -- ov006 0x021212e0 */
    virtual s32 Behavior();          /* slot 6 -- ov006 0x021214f8 */
    virtual s32 Render();            /* slot 9 -- ov006 0x021212fc */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */
    virtual int  OnAttacked2();                        /* slot 23 */
    virtual int  OnKicked();                           /* slot 24 */
    virtual int  OnPushed();                           /* slot 25 */
    virtual void Virtual88(int cx, int cy, int colour, int size); /* slot 34 */

    /* State callbacks stored in mState. The PMF descriptors at
       ov006:0x0213faa0..0x0213fad0 show these are no-argument members of
       this class; Behavior dispatches the same eight-byte pointer-to-member
       every frame. The State/Begin/Update names are coined from the
       matched bodies; the ROM symbols were address-only. */
    void StateDone();      /* ov006 0x02121774 */
    void StateWaitExit();  /* ov006 0x02121778 */
    void StateResults();   /* ov006 0x02121848 */
    void StatePlay();      /* ov006 0x021218fc */
    void StateIntro();     /* ov006 0x02121d64 */

    void BeginResults();   /* ov006 0x021218c4 */
    void BeginPlay();      /* ov006 0x02121cf4 */
    void BeginIntro();     /* ov006 0x02121f04 */

    void UpdateTouchInput(); /* ov006 0x0212157c */
    void UpdateScroll();     /* ov006 0x02121bc8 */

    u32 mState[2];          /* 0x5004 -- raw eight-byte State encoding; a
                               typed global PMF emits a non-ROM __sinit */
    u8  mArray1[0x340];     /* 0x500c -- 4 * 0xd0; StatePlay launches one */
    u8  mArray2[0x984];     /* 0x534c -- 3 * 0x32c, elem dtor func_ov006_020d1008 */
    u8  mArray3[0xb4];      /* 0x5cd0 -- 5 * 0x24 floating scores (Desc) */
    dScMgTrampoline_DoorMark mDoorMark; /* 0x5d84 */
    int mTimer;             /* 0x5d90 -- countdown gating state transitions */
    s32 mScrollY;           /* 0x5d94 -- approached toward mScrollTargetY by 2 a
                               tick; the BG2 offset is mScrollY + mScrollOffsetY */
    s32 mScrollTargetY;     /* 0x5d98 */
    s32 mScrollHoldTimer;   /* 0x5d9c -- 0x78, run down to 0 before the target
                               may move again */
    s32 mScrollOffsetY;     /* 0x5da0 -- added to mScrollY everywhere it is used */
    s32 mArrow1X;           /* 0x5da4 -- the two guide arrows Render draws at
                               n + 0xf0; each slides between 0 and 0x20 */
    s32 mArrow2X;           /* 0x5da8 */
    s32 mDragSoundHandle;   /* 0x5dac -- returned by the positional drag-sound
                               updater and fed back on the next frame */
    s16 mTouchX;            /* 0x5db0 -- the current stylus sample */
    s16 mTouchY;            /* 0x5db2 */
    s16 mTouchStartX;       /* 0x5db4 -- copied from mTouch on the press edge and
                               left alone; the swipe is measured against it */
    s16 mTouchStartY;       /* 0x5db6 */
    s16 mInputEnabled;      /* 0x5db8 -- while 0 the touch handler clears
                               mTouching and returns at once */
    s16 mDoorSide;          /* 0x5dba -- 0 or 1, the lit door; the arrows slide
                               toward it and the mark copies it */
    s16 mDoorSwitchTimer;   /* 0x5dbc -- counts down to the next door change */
    s16 mDoorBlinkTimer;    /* 0x5dbe -- 0..0xa once the switch is within 0x3c */
    s16 mDoorBlinkPhase;    /* 0x5dc0 -- while set, the lit door's arrow uses
                               the other frame */
    s16 mRoundOver;         /* 0x5dc2 -- set once results clear the screen;
                               Render then skips the playfield */
    u8  mTouching;          /* 0x5dc4 -- a drag is in progress */
    u8  mTouchReleased;     /* 0x5dc5 -- set on the release edge; OnAttacked2
                               consumes and clears it after the swipe test */
    u8  pad_5dc6[0x2];      /* 0x5dc6 -- rounds up to the 0x5dc8 boundary */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgTrampoline_c_size_must_be_0x5dc8[sizeof(dScMgTrampoline_c) == 0x5dc8 ? 1 : -1];
#endif

#endif
