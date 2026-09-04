#ifndef SNOWMANHEAD_H
#define SNOWMANHEAD_H

#include "dActor_c.h"
#include "Model.h"
#include "TextureSequence.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

/* The cartridge RTTI names this class daBgSnmHed_c. SnowmanHead is the
 * readable compatibility spelling already carried by every matched virtual.
 * Its __si_class_type_info record has dActor_c as the sole base at offset zero,
 * and its vtable has the same 31 slots as dActor_c. The D1/D0 pair and factory
 * independently pin the four owned subobjects and the total allocation size.
 *
 * The four state pairs are recovered from __sinit_ov072_021221f8, which copies
 * eight ROM PMF constants into data_ov072_02122c00. SetState indexes that table
 * at state * 0x10 and invokes the first PMF; Behavior invokes the second.
 * Descriptive original names are absent, so the ROM-evidenced indices remain.
 *
 * SM64DS RTTI names the implementation daBgSnmHed_c. The reconstructed
 * factory daBgSnmHed_c_classInit (historical alias
 * SnowmanHead_Spawn) constructs it for the BIG_SNOWMAN_HEAD
 * registry profile.
 */
struct SnowmanHead : dActor_c {
    u8 mPad0d0[0x4];                    /* 0x0d0 */
    Model mModel;                       /* 0x0d4 */
    TextureSequence mTextureSequence;   /* 0x124 */
    dCcAc_c mCylinder;                  /* 0x138 */
    dBgCh_Actr mWithMeshClsn;           /* 0x16c */

    typedef int (SnowmanHead::*StateFunc)();
    StateFunc *mStateFuncs;             /* 0x328 */
    Player *mTalkPlayer;                /* 0x32c */
    s32 mStateValue;                    /* 0x330 */
    u8 mSubstate;                       /* 0x334 */
    u8 mStateTimer;                     /* 0x335 */
    u8 unk_336;                         /* 0x336 */
    u8 mPad337;                         /* 0x337 */

    virtual ~SnowmanHead();             /* slots 16, 17 */

    virtual int InitResources();        /* slot  0 */
    virtual int CleanupResources();     /* slot  3 */
    virtual int Behavior();             /* slot  6 */
    virtual int Render();               /* slot  9 */
    virtual void OnPendingDestroy();    /* slot 12 */

    void UpdateModel();
    int InitState0();
    int State0();
    int InitState1();
    int State1();
    int InitState2();
    int State2();
    int InitState3();
    int State3();
    void CallStateBehavior();
    void CallStateInit();
    void SetState(int state);
};

typedef char SnowmanHead_size_must_be_0x338[
    sizeof(SnowmanHead) == 0x338 ? 1 : -1];

#endif
