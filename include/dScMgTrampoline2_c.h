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
extern "C" void *func_ov006_020ca604(void);
extern "C" void func_ov006_020d1008(void);
extern "C" void func_ov006_020eed64(void *object);
struct Model;
extern "C" int func_ov006_021227c8(char *object);
extern "C" Model *func_ov006_02122c68(Model *model);
extern "C" void func_ov006_02120938(void);

struct dScMgTrampoline2_c : dScMgD3DBase_c {
    virtual ~dScMgTrampoline2_c() {
        __cxa_vec_cleanup(mArray6, 5, 0x24, (void *)func_ov006_02120938);
        __cxa_vec_cleanup(mArray5, 0x14, 0x78, (void *)func_ov006_02122c68);
        __cxa_vec_cleanup(mArray4, 0xa, 0x24, (void *)func_ov006_020eed64);
        __cxa_vec_cleanup(mArray3, 0xa, 0x1d0, (void *)func_ov006_021227c8);
        __cxa_vec_cleanup(mArray2, 3, 0x32c, (void *)func_ov006_020d1008);
        __cxa_vec_cleanup(mArray1, 5, 0xdc, (void *)func_ov006_020ca604);
    }
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */
    virtual int  OnAttacked2();                        /* slot 23 */
    virtual int  OnKicked();                           /* slot 24 */
    virtual int  OnPushed();                           /* slot 25 */
    virtual void Virtual88(int cx, int cy, int colour, int size); /* slot 34 */

    /* Raw eight-byte state PMF. Behavior calls through it. A typed
       global initializer emits a __sinit this ROM does not have. */
    u32 mState[2];            /* 0x5004 */
    u8  mArray1[0x44c];       /* 0x500c -- 5 * 0xdc,    elem dtor func_ov006_020ca604 */
    u8  mArray2[0x984];       /* 0x5458 -- 3 * 0x32c,   elem dtor func_ov006_020d1008 */
    u8  mArray3[0x1220];      /* 0x5ddc -- 0xa * 0x1d0, elem dtor func_ov006_021227c8 */
    u8  mArray4[0x168];       /* 0x6ffc -- 0xa * 0x24,  elem dtor func_ov006_020eed64 */
    u8  mArray5[0x960];       /* 0x7164 -- 0x14 * 0x78, elem dtor func_ov006_02122c68 */
    u32 mParticleID;          /* 0x7ac4 -- Particle::System unique ID */
    int mRamp;                /* 0x7ac8 -- fixed point; rests at 0x14000, eased
                                 toward 0x7000 as misses come in */
    u8  pad_7acc[0x4];        /* 0x7acc -- no matched read */
    u8  mArray6[0xb4];        /* 0x7ad0 -- 5 * 0x24,    elem dtor func_ov006_02120938 */
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
    u8  mTouchReleased;       /* 0x7bab -- OnAttacked2 consumes it */

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
