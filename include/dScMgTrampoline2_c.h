/* dScMgTrampoline2_c -- Trampoline Terror. Draw lines on the touch screen
 * so Mario can bounce through the shapes that drop in. Child of
 * dScMgD3DBase_c, size 0x7bac from the factory's operator new.
 *
 * RTTI: dScMgTrampoline2_c : dScMgD3DBase_c; vtable ov006:0x0213fc7c.
 * Historical alias MgTrampolineTerror_Spawn. Element dtors 020d1008 and
 * 02120938 are shared with dScMgTrampoline_c, 020eed64 with dScMgJump2_c.
 * Typed arrays and the state PMF remain #2497.
 *
 * The destructor is inline and destroys the six arrays in reverse of the
 * order the factory builds them. dScMgD3DBase_c supplies operator delete.
 * The arrays stay raw bytes; their element types are not recovered.
 * 0x7acc has no matched read.
 */
#ifndef DSCMGTRAMPOLINE2_C_H
#define DSCMGTRAMPOLINE2_C_H
#include "dScMgD3DBase_c.h"

extern "C" void __cxa_vec_cleanup(void *base, int count, int stride, void *dtor);
extern "C" void *Trmp2Mario_Destruct(void);
extern "C" void TrmpLine_Destroy(void);
extern "C" void Jump2Popup_ElemDtor(void *object);
struct Model;
extern "C" int Trmp2_DestructJumperModels(char *object);
extern "C" Model *Trmp2Spinner_Destruct(Model *model);
extern "C" void Trmp_Nothing(void);

struct dScMgTrampoline2_c : dScMgD3DBase_c {
    typedef void (dScMgTrampoline2_c::*State)();

    virtual ~dScMgTrampoline2_c() {
        __cxa_vec_cleanup(mArray6, 5, 0x24, (void *)Trmp_Nothing);
        __cxa_vec_cleanup(mArray5, 0x14, 0x78, (void *)Trmp2Spinner_Destruct);
        __cxa_vec_cleanup(mArray4, 0xa, 0x24, (void *)Jump2Popup_ElemDtor);
        __cxa_vec_cleanup(mArray3, 0xa, 0x1d0, (void *)Trmp2_DestructJumperModels);
        __cxa_vec_cleanup(mArray2, 3, 0x32c, (void *)TrmpLine_Destroy);
        __cxa_vec_cleanup(mArray1, 5, 0xdc, (void *)Trmp2Mario_Destruct);
    }
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
    void StateDone();        /* ov006 0x02123b20 */
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
    u8  mArray1[0x44c];       /* 0x500c -- 5 * 0xdc,    elem dtor Trmp2Mario_Destruct */
    u8  mArray2[0x984];       /* 0x5458 -- 3 * 0x32c,   elem dtor TrmpLine_Destroy */
    u8  mArray3[0x1220];      /* 0x5ddc -- 0xa * 0x1d0, elem dtor Trmp2_DestructJumperModels */
    u8  mArray4[0x168];       /* 0x6ffc -- 0xa * 0x24,  elem dtor Jump2Popup_ElemDtor */
    u8  mArray5[0x960];       /* 0x7164 -- 0x14 * 0x78, elem dtor Trmp2Spinner_Destruct */
    u32 mParticleID;          /* 0x7ac4 -- Particle::System unique ID */
    int mRamp;                /* 0x7ac8 -- fixed point; rests at 0x14000, eased
                                 toward 0x7000 as misses come in */
    u8  pad_7acc[0x4];        /* 0x7acc -- no matched read */
    u8  mArray6[0xb4];        /* 0x7ad0 -- 5 * 0x24,    elem dtor Trmp_Nothing */
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
