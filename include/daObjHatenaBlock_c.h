#ifndef DAOBJHATENABLOCK_C_H
#define DAOBJHATENABLOCK_C_H

#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "dExtAnmModel_c.h"
#include "dExtShadowModel_c.h"

/* Question / item / VS-item / cap blocks (HATENA_BLOCK 20, ITEM_BLOCK 21,
 * VS_ITEM_BLOCK 22, CAP_BLOCK_M/W/L 23-25). ROM RTTI daObjHatenaBlock_c;
 * direct base dBgActor_c. Factory allocates 0x3f8.
 *
 * daObjHatenaBlock_c_classInit_* are reconstructed (RTTI daObjHatenaBlock_c,
 * those six registry IDs). Retail does not store those spellings. */
struct daObjHatenaBlock_c : dBgActor_c {
    u8  pad_31e[0x2];
    dExtAnmModel_c mAnmModel;               /* 0x320 -- only HATENA_BLOCK (20) loads and draws it */
    dExtShadowModel_c mShadowModel;         /* 0x384 */
    /* Seeded from mModel.mat4x3 by InitResources, then rebuilt every frame
       by func_ov102_02149ea4. DropShadowScaleXYZ takes this pairing. 0x3ac + 0x30 = 0x3dc. */
    Matrix4x3 mShadowMat;             /* 0x3ac */
    /* Bounce squash Y added onto mPosY when writing the model/collider
       translation (func_ov102_02149ff0 / 02149e38 / 021498e0). Fix12:
       0x1000 = 1 unit. */
    s32 mBounceYOffs;                 /* 0x3dc */
    s32 mHomePosY;                    /* 0x3e0 -- mPosY at InitResources; Behavior clamps mPosY up to it after UpdatePos (not while POPPED) */
    /* Floor Y from the ground raycast (func_ov102_02149610); shadow height. */
    s32 mFloorY;                      /* 0x3e4 */
    s32 mState;                       /* 0x3e8 -- State */
    /* Bounce phase: starts at 0x4000 (a quarter turn) when the bounce begins
       and grows by 0x1000 (1/16 turn) per frame; its sine, from
       data_02082214, drives the squash scale and mBounceYOffs. */
    u16 mBounceAng;                   /* 0x3ec */
    /* Frame countdown, ticked by DecIfAbove0_Short: 7 when the bounce begins
       (func_ov102_02149c78); 300 (0x12c) when the block pops
       (func_ov102_021498c4), where it is the delay before the block can come
       back. */
    u16 mBounceTimer;                 /* 0x3ee */
    u8 mStarTracked;                  /* 0x3f0 -- TrackStar's result (CONTENT_STAR only); passed back to UntrackStar */
    u8 mStarId;                       /* 0x3f1 -- param1 bits 8..15 (CONTENT_STAR only) */
    /* Truncated from the hitter's param1 by every combat callback, then
       func_ov102_02149da8(this, STATE_BOUNCING). A prize column is picked
       with it, clamped to 0..3. */
    u8 mHitterParam;                  /* 0x3f2 */
    u8 mContentType;                  /* 0x3f3 -- Content */
    /* A SECRET_COIN that registered itself with this block: daSCoin_c::
       LinkToBlock stores itself here when it is within 200 units of
       an actor 20 / 21 block. Bounce-end (func_ov102_021498e0) collects it
       (actorID 0x149, SECRET_COIN) and clears this. Not written by this
       class. */
    dActor_c *mHeldActor;             /* 0x3f4 */

    /* Inline is load-bearing: out-of-line emits D0 before D1. */
    virtual ~daObjHatenaBlock_c() {}

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    void OnGroundPounded(dActor_c &other);     /* slot 21 */
    int OnAttacked1(dActor_c &other);          /* slot 22 */
    void OnKicked(dActor_c &other);            /* slot 24 */
    void OnHitByMegaChar(Player &player);      /* slot 27 */
    int OnHitFromUnderneath(dActor_c &other);  /* slot 28 */

    /* func_ov102_* helpers keep linker names -- no semantic names are known.
       All are ordinary members; func_ov102_02149684 is the one exception,
       staying free (the block is its second argument). */
    int func_ov102_02149078();
    void func_ov102_02149100(Vector3 *pos, int n, unsigned int speed, short baseAngle);
    void func_ov102_02149220();
    void func_ov102_02149288();
    void func_ov102_021492d4();
    void func_ov102_02149384();
    void func_ov102_021493dc();
    void func_ov102_02149428();
    void func_ov102_02149478();
    void func_ov102_021494cc();
    void func_ov102_0214953c(int p1, int p2);
    int func_ov102_02149610();
    void func_ov102_02149878();
    void func_ov102_021498c4();
    void func_ov102_021498e0();
    void func_ov102_02149c78();
    void func_ov102_02149ccc();
    void func_ov102_02149d80();
    void func_ov102_02149da8(int i);
    void func_ov102_02149df0();
    void func_ov102_02149e38();
    void func_ov102_02149ea4();
    void func_ov102_02149ff0();

    /* mState: an index into the {enter, update} pair table at
       data_ov102_0214e890. func_ov102_02149da8 stores the index and runs the
       row's enter routine; func_ov102_02149df0 (the top of Behavior) runs its
       update routine. Rows, read from the ROM:
         IDLE     enter 02149d80  update 02149ccc
         BOUNCING enter 02149c78  update 021498e0
         POPPED   enter 021498c4  update 02149878
       The names are descriptive, taken from what the routines do; the ROM
       stores none. */
    enum State {
        STATE_IDLE = 0,     /* waiting to be hit */
        STATE_BOUNCING = 1, /* hit: squash-and-stretch for a few frames, then spawn the prize */
        STATE_POPPED = 2    /* prize spawned: Render draws nothing; if the block survives the
                               prize (coins, star and 1-up kill it), back to IDLE after a
                               300-frame delay once the player is over 100 units away.
                               Behavior keeps the mesh collider off in every state but IDLE. */
    };

    /* mContentType, the low byte of param1 (0xff reads as 0; the VS block
       forces 0). Row index into the prize table data_ov102_0214e8c0
       (data_ov102_0214e870 for the VS block); each row has four columns,
       picked by mHitterParam clamped to 0..3 (ITEM_BLOCK always takes column
       0). The names say which actor the row spawns; the loaders in
       InitResources / CleanupResources key off the same numbers. */
    enum Content {
        CONTENT_COINS = 0,         /* a spray of COIN actors; count is param1 bits 8..15 (0xff = 1) */
        CONTENT_STAR = 1,          /* STAR; star id is param1 bits 8..15 (0xff = 0) */
        CONTENT_ONEUP = 2,         /* ONEUPKINOKO */
        CONTENT_SHELL = 3,         /* SHELL */
        CONTENT_SCALEUP_KINOKO = 4, /* SCALEUP_KINOKO */
        CONTENT_POWER_UP_5 = 5,    /* column 0 FEATHER, columns 1..3 POWER_UP_ITEM */
        CONTENT_POWER_UP_6 = 6,    /* POWER_UP_ITEM in every column */
        CONTENT_POWER_UP_7 = 7     /* column 0 BOMBHEI (func_ov102_02149220), 1..3 POWER_UP_ITEM */
    };
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjHatenaBlock_c_size_must_be_0x3f8[sizeof(daObjHatenaBlock_c) == 0x3f8 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJHATENABLOCK_C_H */
