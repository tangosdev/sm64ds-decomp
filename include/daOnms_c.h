/* daOnms_c, the rolling crush box (profile ONIMASU). Field names are
 * recovered from the bodies in src/actors/daOnms_c.cpp; offsets and widths
 * are observed.
 *
 * param1's low two bits pick how it moves: 0..2 index data_ov092_02132294
 * for a canned sequence of moves, 3 makes it follow the path whose id is in
 * param1 bits 8..11. That choice is mMoveKind, and NextMove and
 * func_ov092_021313b0 branch on it to choose the next mMoveDir.
 *
 * SM64DS proves this class as daOnms_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings are reconstructed
 * source-style names, not recovered SM64DS symbols:
 * daOnms_c_classInit at 0x02132018 (historical alias ToxBox_Spawn) installs
 * this class's vtable and backs the ONIMASU registry profile, whose
 * descriptor at 0x021322ac is reconstructed as g_profile_ONIMASU.
 * Provenance table: notes/butterfly-tornado-provenance.md. */
#ifndef DAONMS_C_H
#define DAONMS_C_H
#include "types.h"

/* dBgActor_c.h deliberately selects the flat Matrix4x3 spelling before
 * Model.h. It must therefore be the first ownership header included here. */
#include "dBgActor_c.h"
#include "dBgCh_Actr.h"
#include "dCcAcPos_c.h"
#include "PathPtr.h"

/* The teardown and construction order identify the class: dBgActor_c is the
 * base, followed by dBgCh_Actr, dCcAcPos_c and PathPtr members at the
 * offsets daOnms_c_classInit constructs. */
struct daOnms_c : dBgActor_c {
    dActor_c *mPlayerActor;         /* 0x320 - player that knocked the box away */
    dBgCh_Actr mWithMeshClsn;       /* 0x324 */
    s16 mTumbleVelX;                /* 0x4e0 - added to mAngleX each frame */
    s16 mTumbleVelY;                /* 0x4e2 */
    s16 mTumbleVelZ;                /* 0x4e4 */
    u8 pad_4e6[0x2];
    dCcAcPos_c mdCcAcPos_c;         /* 0x4e8 */
    Matrix4x3 mBaseMtx;             /* 0x528 - untumbled model transform */
    Vector3 mRestPos;               /* 0x558 - spawn position lifted 0xfa000;
                                       y follows each landing */
    u16 mStateTimer;                /* 0x564 */
    u8 pad_566[0x2];
    s32 mMoveDir;                   /* 0x568 - state index, see the states */
    s32 *mMoveSeq;                  /* 0x56c - canned states, -1 ends */
    s32 mMoveSeqIndex;              /* 0x570 */
    u8 mMoveKind;                   /* 0x574 - canned sequence or path */
    u8 mOrientBits;                 /* 0x575 - angles X, Y, Z >> 14, 2 bits each */
    u8 mTumbling;                   /* 0x576 - rotate the model this frame */
    u8 mRollDone;                   /* 0x577 - a roll finished this frame */
    s32 mPathNodeCount;             /* 0x578 */
    s32 mPathNodeIndex;             /* 0x57c */
    Vector3 mPathNode;              /* 0x580 */
    PathPtr mPathPtr;               /* 0x58c */

    virtual ~daOnms_c();

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();

    /* The movement states, indexed by mMoveDir through the member-pointer
       table the ov092 static initializer fills. State 1 (wait,
       func_ov092_02131578) and the helpers func_ov092_021313b0 and
       func_ov092_02131a88 stay C-linkage free functions because
       include/decl_common.h declares them by those names. Every member
       name below is coined; the cartridge keeps none. */
    void StateLand();               /* 0 - coined */
    void StateRollPosZ();           /* 2 - coined */
    void StateRollNegZ();           /* 3 - coined */
    void StateRollNegX();           /* 4 - coined */
    void StateRollPosX();           /* 5 - coined */
    void StateKnocked();            /* 6 - coined */
    int  StateBounce();             /* 7 - coined */
    void StateSink();               /* 8 - coined */

    void NextMove();                                    /* coined */
    void Roll(s32 stepZ, s32 stepX, s32 pitch, s16 bank); /* coined */
    void Launch(dActor_c *player, u32 how);             /* coined */
    void CheckPlayerHit();                              /* coined */
    void UpdateModelMtx();                              /* coined */

    /* Read-only tables in ov092 .data, indexed by Launch's `how`
       (punch, kick, bump) and by mOrientBits. Coined names. */
    static s32 sLaunchVertSpeed[3];     /* 0x02132074 */
    static s32 sLaunchHorzSpeed[3];     /* 0x02132080 */
    static s8  sBankAxis[64];           /* 0x0213208c */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daOnms_c_size_must_be_0x594[sizeof(struct daOnms_c) == 0x594 ? 1 : -1];
#endif

#endif
