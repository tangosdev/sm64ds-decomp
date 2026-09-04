#ifndef SNOWMANBODY_H
#define SNOWMANBODY_H

#include "dActor_c.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"
#include "PathPtr.h"

/* The cartridge RTTI names this class daBgSnmBdy_c. SnowmanBody is the
 * readable compatibility spelling already carried by every matched virtual.
 * Its __si_class_type_info record has dActor_c as the sole base at offset zero,
 * and its vtable has the same 31 slots as dActor_c. The D1/D0 pair and factory
 * independently pin the four owned subobjects and the total allocation size.
 *
 * The six state pairs are not guesses from code adjacency. The static
 * initializer at 0x02122018 copies twelve ROM PMF constants into
 * data_ov072_02122b64 in the exact pairs documented below. SetState indexes
 * that table at state * 0x10 and invokes the first PMF; Behavior invokes the
 * second. Descriptive original names are absent, so the ROM-evidenced indices
 * are retained.
 *
 * SM64DS RTTI names the implementation daBgSnmBdy_c. The reconstructed
 * factory daBgSnmBdy_c_classInit (historical alias
 * SnowmanBody_Spawn) constructs it for the BIG_SNOWMAN_BODY
 * registry profile.
 */
struct SnowmanBody : dActor_c {
    u8 mPad0d0[0x4];                    /* 0x0d0 */
    Model mModel;                       /* 0x0d4 */
    ShadowModel mShadowModel;           /* 0x124 */
    dCcAc_c mCylinder;                  /* 0x14c */
    dBgCh_Actr mWithMeshClsn;           /* 0x180 */

    s32 mHomePosX;                      /* 0x33c */
    s32 mHomePosY;                      /* 0x340 */
    s32 mHomePosZ;                      /* 0x344 */
    s16 mHomeAngleX;                    /* 0x348 */
    s16 mHomeAngleY;                    /* 0x34a */
    s16 mHomeAngleZ;                    /* 0x34c */
    u8 mPad34e[0x2];                    /* 0x34e */
    Matrix4x3 mShadowMat;               /* 0x350 */
    PathPtr mPath;                      /* 0x380 */
    u32 mPathNode;                      /* 0x388 */

    typedef int (SnowmanBody::*StateFunc)();
    StateFunc *mStateFuncs;             /* 0x38c */
    Player *mTalkPlayer;                /* 0x390 */
    s32 mStateValue;                    /* 0x394 */
    s32 mRadius;                        /* 0x398 */
    u32 mSoundID;                       /* 0x39c */
    u16 mStateTimer;                    /* 0x3a0 */
    u8 mSubstate;                       /* 0x3a2 */
    u8 mPlayerReachedPath;              /* 0x3a3 */
    u8 unk_3a4;                         /* 0x3a4 */
    u8 mPad3a5[0x3];                    /* 0x3a5 */

    virtual ~SnowmanBody();             /* slots 16, 17 */

    virtual int InitResources();        /* slot  0 */
    virtual int CleanupResources();     /* slot  3 */
    virtual int Behavior();             /* slot  6 */
    virtual int Render();               /* slot  9 */
    virtual void OnPendingDestroy();    /* slot 12 */

    int InitState0();
    int State0();
    int InitState1();
    int State1();
    int InitState2();
    int State2();
    int InitState3();
    int State3();
    int InitState4();
    int State4();
    int InitState5();
    int State5();

    int IsPlayerNearCenter();
    void UpdateRollAngle();
    int AdvancePath();
    int HurtPlayer();
    void UpdateGroundCollision(dBgCh_Actr *collision);
    void UpdateModel();
    void CallStateBehavior();
    void CallStateInit();
    void SetState(int state);
};

typedef char SnowmanBody_size_must_be_0x3a8[
    sizeof(SnowmanBody) == 0x3a8 ? 1 : -1];

#endif
