#ifndef DAIBL_C_H
#define DAIBL_C_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN7daIbl_cD1Ev` destroys its nontrivial members, and
 * `daIbl_c_classInit` constructs the same types at the same offsets before
 * storing `_ZTV7daIbl_c`. Everything this header used to restate below
 * 0x110 belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The following constructor/destructor calls witness these member layouts:
 *
 *     0x110 dBgCh_Actr               0x1bc   -> 0x2cc
 *     0x2cc dExtSimpleModel_c                      0x50    -> 0x31c
 *     0x31c dExtShadowModel_c                0x28    -> 0x344
 *     0x374 dCcAc_c         0x34    -> 0x3a8
 *     0x3f4 PathPtr                    0x8     -> 0x3fc
 *
 * PathPtr has no destructor, so only the factory witnesses that member:
 * daIbl_c_classInit constructs a PathPtr at 0x3f4 that the destructor
 * never destroys, PathPtr's being trivial. 0x3f4 + 8 is 0x3fc, exactly the
 * allocation literal -- the layout does not close without it. Reading only
 * the destructor leaves the class eight bytes short.
 *
 * The draw-scale group at 0x3ac is separately inferred from the three scalar
 * stores and the pointer passed to dExtSimpleModel_c::Render. Its Vector3 type and name
 * are reconstruction choices, not original spelling proved by RTTI. The empty
 * Vector3 constructor/destructor adds real subobject lifetimes; unchanged
 * emitted lifecycle code is checked separately from that source distinction.
 *
 * Every function of the class reproduces, in one translation unit:
 * src/game/actors/d_a_ibl.cpp is the whole linker unit 0x02141f04..0x021431c4,
 * factory included.
 *
 * SIZE IS THE ROM'S OWN: `daIbl_c_classInit` calls
 * `fBase_c::operator new(1020)` -- 0x3fc -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 *
 * SM64DS RTTI names the implementation daIbl_c: _ZTS7daIbl_c at
 * 0x02147f40, _ZTI7daIbl_c at 0x02147f4c, and the vtable _ZTV7daIbl_c at
 * 0x02147f7c that the factory and destructor store. The reconstructed
 * factory daIbl_c_classInit (historical alias
 * RollingIronBall_Spawn) constructs it for the IRONBALL
 * registry profile.
 */

#include "dEnemyBase_c.h"
#include "dExtSimpleModel_c.h"
#include "dCcAc_c.h"
#include "PathPtr.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

/* Behavior's table is indexed by mVariant (param1's low nibble). The names say
 * what each kind's handler does, not labels recovered from the cartridge.
 *
 *   0  func_ov100_02142b90: never drawn (Render skips it); a timer-driven
 *      spawner that creates another IRONBALL actor while the closest player is
 *      inside its distance band.
 *   1  func_ov100_02142918: rolls freely, bouncing off walls and speeding up
 *      down slopes.
 *   2  func_ov100_021424c0: follows the path, speed changing with the slope.
 *   3  func_ov100_0214272c: rolls on with no path until it drops below
 *      mKillY or runs into a wall, then breaks up. A path follower turns into
 *      this kind when its path index wraps.
 *   4  func_ov100_021424c0 again: the same handler as 2, but on the ground its
 *      speed climbs linearly to the 35 units/frame cap instead of following
 *      the floor normal.
 */
enum daIbl_Kind {
    daIbl_KIND_SPAWNER      = 0,
    daIbl_KIND_FREE_ROLL    = 1,
    daIbl_KIND_PATH_SLOPE   = 2,
    daIbl_KIND_ROLL_OFF     = 3,
    daIbl_KIND_PATH_RAMP    = 4
};

struct daIbl_c : dEnemyBase_c {
    dBgCh_Actr                 mWithMeshClsn;         /* 0x110 */
    dExtSimpleModel_c mModel; /* 0x2cc -- mModel.mat4x3 is the model matrix */
    dExtShadowModel_c                  mShadowModel;          /* 0x31c */
    /* The shadow's Matrix4x3, as twelve words (words 9..11 are the
       translation). func_ov100_02142264 copies the model matrix into it and then
       overwrites the Y translation. Kept as plain words because a Matrix4x3
       member would bring Vector3's destructor into this class's own. */
    s32                          mShadowMtx[12];        /* 0x344 */
    dCcAc_c           mdCcAc_c;   /* 0x374 -- hitFlags/otherOwner at 0x394/0x398 name what touched the ball */
    /* On a ball made by a spawner (kind 0): the spawner. The spawner stores
       itself here right after Spawn returns, and CleanupResources gives its
       slot back through mLiveBalls. InitResources zeroes it, so a spawner's own
       word stays 0. */
    daIbl_c                     *mDispenser;            /* 0x3a8 */
    /* Inferred draw-scale vector: Render passes this actual object to dExtSimpleModel_c.
       InitResources writes 0x1000 (1.0) or 0x800 (0.5) component by component
       for drawn variants; the empty default constructor does not initialize it.
       func_ov100_02142264 reads x for the shadow's size and offset. */
    Vector3                      mDrawScale;            /* 0x3ac..0x3b7 */
    u8  pad_3b8[0x2];
    /* The heading from the ball to mNextNodePos: func_ov100_0214233c stores
       atan2(node.x - pos.x, node.z - pos.z) here on every call, the path
       followers steer mPrevAngleY toward it, and InitResources copies it
       straight into mPrevAngleY once. */
    s16                          mHeadingToNode;        /* 0x3ba */
    u8  pad_3bc[0x4];
    /* Spawner (kind 0) only: the horizontal distance band to the closest player
       inside which it will spawn, both ends inclusive. InitResources picks
       them per level. */
    s32                          mMinSpawnDist;         /* 0x3c0 */
    s32                          mMaxSpawnDist;         /* 0x3c4 */
    /* Kind 3's Y floor: below it, or on touching a wall, the ball breaks up.
       InitResources seeds it from data_02092138 (STAR_CAP_MIN_POS_Y in
       symbols/verified.tsv) and overrides it in three levels. */
    s32                          mKillY;                /* 0x3c8 */
    /* Sound::PlayLong's return, fed back on the next call (the rolling sound);
       zeroed on a newly hit wall and on touch-down, after which PlayLong is
       called with a 0 handle. */
    s32                          mRollSoundHandle;      /* 0x3cc */
    /* param1's low nibble, consumed immediately (param1 is then shifted down by
       four so the next nibble is the path ID). The kind picks the handler
       Behavior runs (see daIbl_Kind), InitResources switches on it, and
       Render skips kind 0. */
    u8                           mVariant;              /* 0x3d0 */
    /* Set by func_ov100_02142130 when the ball ran into a wall it is heading
       into this frame. */
    u8                           mHitWall;              /* 0x3d1 */
    /* Spawner only: how many balls it has out. The spawner adds one after each
       Spawn that returns an actor, and each ball's CleanupResources takes one
       back. */
    u8                           mLiveBalls;            /* 0x3d2 */
    u8  pad_3d3[0x1];
    s32                          mNumPathNodes;         /* 0x3d4 -- PathPtr::NumNodes() */
    s32                          mPathNodeIndex;        /* 0x3d8 -- index passed to PathPtr::GetNode */
    /* The path node the ball last passed. InitResources seeds it with the actor's
       own position and func_ov100_0214233c overwrites it with mNextNodePos each
       time the ball passes the node it was heading for. */
    s32                          mPrevNodePosX;         /* 0x3dc */
    s32                          mPrevNodePosY;         /* 0x3e0 */
    s32                          mPrevNodePosZ;         /* 0x3e4 */
    /* Where PathPtr::GetNode writes the node it was asked for -- the node the
       ball is heading for. InitResources compares it against the actor's own
       position to decide whether to skip ahead one node. */
    s32                          mNextNodePosX;         /* 0x3e8 */
    s32                          mNextNodePosY;         /* 0x3ec */
    s32                          mNextNodePosZ;         /* 0x3f0 */
    PathPtr                      mPathPtr;              /* 0x3f4 */

    /* --- vtable --- */
    virtual ~daIbl_c();

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    void func_ov100_02141fb0();
    void func_ov100_02142130();
    void func_ov100_02142264();
    int  func_ov100_0214233c();
    void func_ov100_021424c0();
    void func_ov100_0214272c();
    void func_ov100_02142918();
    void func_ov100_02142b90();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daIbl_c_size_must_be_0x3fc[sizeof(daIbl_c) == 0x3fc ? 1 : -1];
#endif

#endif /* DAIBL_C_H */
