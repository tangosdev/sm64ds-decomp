#ifndef DAHANACHAN_C_H
#define DAHANACHAN_C_H

#include "types.h"

/* The Wiggler (daHanachan_c). Five body segments, so five of everything -- and EIGHT arrays,
 * which close on eight consecutive boundaries:
 *
 *     dEnemyBase_c                        ends 0x110
 *     ModelAnim[5]                 0x110 + 5*0x64 = 0x304  -> MaterialChanger
 *     MaterialChanger[5]           0x304 + 5*0x14 = 0x368  -> TextureSequence
 *     TextureSequence[5]           0x368 + 5*0x14 = 0x3cc  -> the first triple
 *     Vector3[5]                   0x3cc + 5*0x0c = 0x408  -> the second
 *     Vector3[5]                   0x408 + 5*0x0c = 0x444  -> the shorts
 *     Vector3s[5]                  0x444 + 5*0x06 = 0x462
 *     dCcAcPos_c[5] 0x478 + 5*0x40 = 0x5b8  -> the second set
 *     dCcAcPos_c[5] 0x5b8 + 5*0x40 = 0x6f8
 *
 * The Vector3s array is what named func_02011508 -- see the note on Vector3s in
 * include/types.h and src/_ZN8Vector3sD1Ev.cpp.
 *
 * SM64DS RTTI names the implementation daHanachan_c. The reconstructed factory
 * daHanachan_c_classInit (historical alias Wiggler_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_HANACHAN
 * (historical alias Wiggler_SpawnInfo) is its registry descriptor.
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "MaterialChanger.h"
#include "TextureSequence.h"
#include "dBgCh_Actr.h"
#include "dCcAcPos_c.h"

struct daHanachan_c : dEnemyBase_c {
    /* One state table row: Init runs on entry, Main every frame. */
    struct State {
        void (daHanachan_c::*mInit)();   /* 0x00 */
        void (daHanachan_c::*mMain)();   /* 0x08 */
        const char *mName;               /* 0x10 -- "DEMOWAIT", "MOVE", ... */
    };

    ModelAnim mModelAnims[5];                        /* 0x110 */
    MaterialChanger mMaterialChangers[5];            /* 0x304 */
    TextureSequence mTextureSequences[5];            /* 0x368 */
    /* One entry per body segment, head first; these segment names are coined. */
    Vector3 mSegmentPos[5];                          /* 0x3cc -- [0] follows mPos */
    Vector3 mSegmentScale[5];                        /* 0x408 -- Render's per-segment scale */
    Vector3s mSegmentRot[5];                         /* 0x444 */
    u8  pad_462[0x2];
    s32 mSegmentSpacing[5];                          /* 0x464 -- distance to the segment ahead */
    dCcAcPos_c mdCc_cs1[5];    /* 0x478 */
    dCcAcPos_c mdCc_cs2[5];    /* 0x5b8 */
    s16 mSegmentBouncePhase[5];                      /* 0x6f8 -- Behavior adds 0x1200 */
    u8  mSegmentBounceTimer[5];                      /* 0x702 -- set to 0x1e when that segment is stomped */
    u8  pad_707[0x1];
    dBgCh_Actr mWithMeshClsn;                      /* 0x708 */
    /* The fields below close on the ROM's `new daHanachan_c` size; see tools/opnew_sizes.py.
       Every name here is coined from how the state functions use the field. */
    s32 mState;                                      /* 0x8c4 -- index into the state table */
    Player *mPlayer;                                 /* 0x8c8 -- the player being talked to or hit */
    Vector3 mHomePos;                                /* 0x8cc -- spawn position */
    s16 mTargetAngleY;                               /* 0x8d8 -- mSegmentRot[0].y turns towards it */
    u8  mStateTimer;                                 /* 0x8da */
    u8  mHealth;                                     /* 0x8db -- 4 at spawn; 4 - mHealth picks the face frame */
    u8  mDeathDelay;                                 /* 0x8dc -- DEAD waits five frames after the last segment */
    u8  mDeathSegment;                               /* 0x8dd -- next segment DEAD poofs */
    u8  mIntroDone;                                  /* 0x8de -- set on entering MOVE_STOP */
    u8  mFalling;                                    /* 0x8df -- nonzero: falling out of the level */
    u8  mDefeated;                                   /* 0x8e0 -- set on entering DEAD */
    u8  mDriveCamera;                                /* 0x8e1 */
    u8  mStarID;                                     /* 0x8e2 -- low byte of the spawn parameter */
    u8  mStarTrackID;                                /* 0x8e3 -- TrackStar result */
    void *mParticle;                                 /* 0x8e4 -- Particle::System handle */

    virtual ~daHanachan_c();

    /* An override the cartridge proves and this header never declared. _ZTV12daHanachan_c
       slot 6 pointed at fBase_c::Behavior; the ROM has ov034:_ZN12daHanachan_c8BehaviorEv
       (0x02112b5c, 0x6e0 bytes), named in symbols.txt but not yet decompiled -- the
       slot needs the symbol, not a body, so declaring it is the whole fix.
       No `virtual` keyword, matching the overrides beside it: a derived declaration
       of a base virtual overrides whether or not it repeats the word.
       Measured by tools/romdata_check.py, the only gate that reads vtable bytes --
       objisolate drops every non-.text section, so 106/106 is blind here. */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* The state machine. Each state is an Init and Main pair reached through the
       table at data_ov034_02114538, whose entries also carry the ROM's state name
       strings; the state ids are the table order. The method names are coined from
       those strings, and the four helpers below them are coined from their bodies. */
    void StateDamageStartMain();   /* 10 DAMAGE_START */
    void StateDamageStartInit();
    void StateDamageWaitMain();    /*  9 DAMAGE_WAIT */
    void StateDamageWaitInit();
    void StateDamageStopMain();    /*  8 DAMAGE_STOP */
    void StateDamageStopInit();
    void StateDeadMain();          /*  7 DEAD */
    void StateDeadInit();
    void StateDamageTalkMain();    /*  6 DAMAGETALK */
    void StateDamageTalkInit();
    void StateDamageMain();        /*  5 DAMAGE */
    void StateDamageInit();
    void StateMoveMain();          /*  4 MOVE */
    void StateMoveInit();
    void StateMoveStartMain();     /*  3 MOVE_START */
    void StateMoveStartInit();
    void StateMoveStopMain();      /*  2 MOVE_STOP */
    void StateMoveStopInit();
    void StateDemoCallMain();      /*  1 DEMOCALL */
    void StateDemoCallInit();
    void StateDemoWaitMain();      /*  0 DEMOWAIT */
    void StateDemoWaitInit();
    void SetState(int state);
    void ExecState();
    int  IsStompedBy(dCcAcPos_c *cyl, dActor_c *actor);   /* coined */
    void HandlePlayerHits();                               /* coined */
    void UpdateSegments();                                 /* coined */
    void UpdateSegmentModels();                            /* coined */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daHanachan_c_size_must_be_0x8e8[sizeof(struct daHanachan_c) == 0x8e8 ? 1 : -1];
#endif

#else

/* The same object for a C translation unit, flat. */
struct daHanachan_c {
    u8  pad_000[0x708];
    u8  mWithMeshClsn[0x1bc];      /* 0x708 */
    /* trailing extent the ROM's `new daHanachan_c` literal proves; see tools/opnew_sizes.py */
    u8 pad_8c4[0x24];
};

#endif /* __cplusplus */

#endif /* DAHANACHAN_C_H */
