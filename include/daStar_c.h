#ifndef DASTAR_C_H
#define DASTAR_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation(s) below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daStar_c -- daStar_c_classInit_STAR (was PowerStar_Spawn), g_profile_STAR (was PowerStar_SpawnInfo)
 */

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN8daStar_cD1Ev` destroys each member, and
 * `daStar_c_classInit_STAR` constructs the same types at the same offsets before
 * storing `_ZTV8daStar_c`. Everything this header used to restate below
 * 0x110 belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 dCcAcPos_c  0x40    -> 0x150
 *     0x150 dBgCh_Actr               0x1bc   -> 0x30c
 *     0x30c dExtAnmModel_c                  0x64    -> 0x370
 *     0x370 dExtAnmModel_c                  0x64    -> 0x3d4
 *     0x3d4 dExtShadowModel_c                0x28    -> 0x3fc
 *
 * SIZE IS THE ROM'S OWN: `daStar_c_classInit_STAR` calls
 * `fBase_c::operator new(1220)` -- 0x4c4 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 */

#include "dEnemyBase_c.h"
#include "dExtAnmModel_c.h"
#include "dCcAcPos_c.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

struct daPly_c;

/* The shadow's matrix (a bare 4x3: a real Matrix4x3 member would drag
   Vector3's destructor into ~daStar_c). tx/ty/tz are the position >> 3. */
struct daStarShadowMtx { s32 m[9]; s32 tx; s32 ty; s32 tz; };
#ifndef SM64DS_PLATFORM_PC
typedef char daStarShadowMtx_size_must_be_0x30[sizeof(struct daStarShadowMtx) == 0x30 ? 1 : -1];
#endif

struct daStar_c : dEnemyBase_c {
    /* ---- state, indexing data_ov002_021109d8 (the 14-entry pointer-to-member
       table built by __sinit_ov002_02106e40; Behavior calls entry mState) ----
       Each handler is named for its State; its ROM address is in d_a_star.cpp. */
    enum State {
        STATE_LAUNCH = 0, /* StateLaunch: one-shot pop-out start, then 1, 2 or 3 */
        /* StateRise: waits for the fall to reach -32.0/frame, then aims at the marker */
        STATE_RISE = 1,
        /* StateFlyToMarker: homes in on the marker, snaps 200.0 above it, then 3 */
        STATE_FLY_TO_MARKER = 2,
        STATE_LAND = 3, /* StateLand: settles, then 4 */
        STATE_IDLE = 4, /* StateIdle: the collectible star */
        /* StateCollectBegin: player touched it; starts the collection pose */
        STATE_COLLECT_BEGIN = 5,
        /* StateCollectHold: one frame on the player, then starts the pose */
        STATE_COLLECT_HOLD = 6,
        STATE_COLLECT_TALK = 7,  /* StateCollectTalk: pose, score popup, "save?" message */
        STATE_BOUNCE = 8,        /* StateBounce: bounces on the ground until picked up */
        STATE_WAIT_MARKER = 9,   /* StateWaitMarker: waits for its STARBASE marker, then 8 */
        STATE_TOUCHED = 10,      /* StateTouched: veneer to Collect (Collect) */
        STATE_SAVE_MESSAGE = 11, /* StateSaveMessage: the save-prompt message flow */
        STATE_PLAY_AND_END = 12, /* StatePlayAndEnd: plays its animation, then destroys itself */
        /* StateSpinAndEnd: spins, gives the VS star, then destroys itself */
        STATE_SPIN_AND_END = 13
    };

    /* param1 bits 4..7. Only what InitResources and its callees evidence:
       kinds 0, 5, 6 and 7 start in STATE_IDLE; kind 1 starts bouncing; kinds
       2 and 4 start in STATE_LAUNCH; anything else (3) starts in
       STATE_WAIT_MARKER. Init sets 8 and 9 itself when param1 & 0x7f is 0x7f
       (InitKind8) or 0x6f (InitKind9). */
    enum Kind {
        KIND_0 = 0, KIND_1 = 1, KIND_2 = 2, KIND_3 = 3, KIND_4 = 4,
        KIND_5 = 5,
        KIND_6 = 6,   /* its marker is spawned with param 0x40; STATE_IDLE
                         flickers it in and out against NumVsStarsObtained() == 5 */
        KIND_7 = 7,
        KIND_8 = 8,
        KIND_9 = 9
    };

    /* mStarFlags bits. */
    enum StarFlag {
        /* clear: UpdateModelMatrix spins it (+0xc00 on mAngleY per frame); set: faces mPrevAngleY, 50.0 up */
        FLAG_NO_SPIN = 0x0001,
        /* Render, the drop shadow and the touch volume all need it; InitResources disables the touch volume while clear */
        FLAG_VISIBLE = 0x0002,
        /* Render draws mAnmModel2 (the translucent star) instead of mAnmModel1;
           AddStarMarker turns marker type 2 into 3 */
        FLAG_COLLECTED_MODEL = 0x0004,
        FLAG_NO_PICKUP = 0x0008,  /* STATE_BOUNCE skips the pickup test (CheckPickup) while set */
        FLAG_WATER_MASK = 0x0030, /* bits 4..5, a WaterState */
        /* out of bounds -> re-link to a marker (RelinkMarker) instead of going home; set by SeekNewMarker */
        FLAG_RELINK_MARKER = 0x0040,
        /* set by daStarBase_c::InitResources on the star its kind-6 marker spawns */
        FLAG_SWITCH_STAR = 0x0080,
        FLAG_SWITCH_ON = 0x0100, /* set/cleared by SetSwitchState (daObjSwitch calls it) */
        /* STATE_IDLE kind 6: marker added and the appear sound already played */
        FLAG_APPEARED = 0x0200,
        /* bits 10..11: the answer to the message, from data_0209d684 & 3 (1 and 2 are acted on) */
        FLAG_ANSWER_MASK = 0x0c00,
        /* spawned as the reward for the red/silver coin set (SpawnRedCoinStarIfNecessary, daCoin_c); Collect then records the marker in the death table */
        FLAG_COIN_REWARD = 0x1000
    };

    /* mStarFlags again, as bit-fields (LSB first, as mwccarm lays a u16 out);
       the field order is the StarFlag order. */
    struct StarFlagBits {
        u16 noSpin : 1;
        u16 visible : 1;
        u16 collectedModel : 1;
        u16 noPickup : 1;
        u16 water : 2;
        u16 relink : 1;
        u16 switchStar : 1;
        u16 switchOn : 1;
        u16 appeared : 1;
        u16 answer : 2;
        u16 coinReward : 1;
        u16 pad_13 : 3;
    };

    /* Bits 4..5 of mStarFlags. */
    enum WaterState {
        WATER_NONE = 0,  /* dry */
        WATER_TOUCH = 1, /* touched water; UpdateWater is probing for the surface */
        /* surface at least 60.0 above it: weak gravity (-0.4375/frame^2), 16.0 terminal fall */
        WATER_IN = 2
    };

    dCcAcPos_c    mdCc_c;         /* 0x110 -- the touch volume: radius 100.0, height 150.0 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x150 */
    dExtAnmModel_c mAnmModel1;                        /* 0x30c -- the normal star */
    dExtAnmModel_c mAnmModel2; /* 0x370 -- the translucent "already collected" star */
    dExtShadowModel_c                  mShadowModel;          /* 0x3d4 */

    daStarShadowMtx              mShadowMtx;            /* 0x3fc */

    s32 mGroundY; /* 0x42c -- ProbeGround's downward probe from 50.0 above the star: the hit, or 0x7fffffff */
    u32 mSoundObjID; /* 0x430 -- uniqueID of the sound object SpawnCollectSoundObj spawned, 0 if none */
    u32  mMarkerID;               /* 0x434 -- uniqueID of the STARBASE marker (daStarBase_c) this star is linked to */
    /* Set by Collect, CollectNoPose and OnTurnIntoEgg. */
    daPly_c *mPlayer;             /* 0x438 -- the player that touched it */
    s32  mKind;                   /* 0x43c -- a Kind */
    union {
        s32  mState;              /* 0x440 -- a State */
        s32  unk_440;
    };
    s32 mHomeState; /* 0x444 -- the State chosen at init; ReturnToHome returns to it; 9 = placed by a marker */
    s32  mSafePosX;               /* 0x448 -- last position it rested at; the aim point when it lands on a bad floor */
    s32  mSafePosY;               /* 0x44c */
    s32  mSafePosZ;               /* 0x450 */
    s32  mHomePosX;               /* 0x454 -- spawn position (STATE_COLLECT_BEGIN overwrites it with the player's); out-of-bounds reset target, and the floor of a silver star */
    s32  mHomePosY;               /* 0x458 */
    s32  mHomePosZ;               /* 0x45c */
    s32  mCamLookAtX;             /* 0x460 -- camera look-at saved when the cutscene starts */
    s32  mCamLookAtY;             /* 0x464 */
    s32  mCamLookAtZ;             /* 0x468 */
    s32  mCamPosX;                /* 0x46c -- camera position saved when the cutscene starts */
    s32  mCamPosY;                /* 0x470 */
    s32  mCamPosZ;                /* 0x474 */
    s32  mInitPosX;               /* 0x478 -- a copy of the position at InitResources; nothing else reads it */
    s32  mInitPosY;               /* 0x47c */
    s32  mInitPosZ;               /* 0x480 */
    s32  mMinPosY;                /* 0x484 -- STAR_CAP_MIN_POS_Y copied at init: below it the star is out of bounds */
    s32 mWaterHeight;             /* 0x488 -- the water surface found by UpdateWater */
    s32 mSoundObjSoundID; /* 0x48c -- the sound id PickCollectSoundID picked; SpawnCollectSoundObj hands it to the sound object's mSoundID */
    u16  mSeqTimer;               /* 0x490 -- counts frames of the collection sequence */
    u16  mAppearTimer;            /* 0x492 -- KIND_6: frames spent appearing / disappearing */
    u16 mSparkleTimer; /* 0x494 -- frames left of the trail effect UpdateSparkleEffect spawns */
    u16 mCamSeq; /* 0x496 -- camera cutscene step (StepCameraCutscene): 0xffff = none; 0/1 and 0x64/0x65 start and hold, 0x1b6 and 0x1d6 count up to 0x1f4 (restore) and 0x1f5 (end) */
    s8   mMarkerSlot;             /* 0x498 -- index into STAR_MARKERS from AddStarMarker, -1 = none */
    s8   mSavedAreaId;            /* 0x499 -- mAreaId at init; the cutscene gives it back */
    u8   mMarkerType;             /* 0x49a -- type passed to SetStarMarker: 1 silver star, 2 power star, 3 power star already collected, 0 = decided by mStarFlags */
    u8   mTalkStep;               /* 0x49b -- STATE_SAVE_MESSAGE's step */
    u8 mSoundObjMode; /* 0x49c -- 0 none, 1 or 2; at the end StopCollectSoundObj zeroes the sound object's mCounterLimit (1) or destroys it (2) */
    union {
        u8   mStarID;             /* 0x49d -- param1 & 0xf */
        u8   unk_49d;
    };
    u8 mSoundObj6State; /* 0x49e -- 0xff until SpawnSoundObj6 spawns sound object 6, then 0x78 */
    union {
        u8   mInIceBlock;         /* 0x49f -- an ICE_BLOCK_LL was within 200.0 at spawn (daObjIceBlock clears it) */
        u8   unk_49f;
    };
    u8 mIceChecked;               /* 0x4a0 -- CheckIceBlock has already looked for that ice block */
    u8   mMusicTimer;             /* 0x4a1 -- frames of the KIND_9 music-volume hold */
    union {
        u16  mStarFlags;          /* 0x4a2 -- StarFlag bits */
        u16  unk_4a2;
        StarFlagBits mBits;       /* the same word as bit-fields, for the tests the ROM compiles as shifts */
    };
    u8   pad_4a4[0x4];
    s32  mCenterX;                /* 0x4a8 -- cache for func_ov002_020e8244; zeroed each Behavior */
    s32  mCenterY;                /* 0x4ac */
    s32  mCenterZ;                /* 0x4b0 */
    u32  mParticle[4];            /* 0x4b4 -- Particle::System handles: [0] shared by effects 0x113, 0x115 and 0x2f, [1] 0x116, [2] 0x114, [3] 0x10e (silver star) */

    /* --- vtable --- */
    virtual ~daStar_c();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual void OnTurnIntoEgg(daPly_c &player); /* slot 19 */

    void AddStarMarker();
    void SetSwitchState(int r1);
    int Behavior();
    int CleanupResources();
    s32 InitResources();
    int Render();

    /* Receivers. func_ov002_020e7eb4 (an empty stub) and func_ov002_020e9630 (a level
       test whose levels are unnamed) keep the address as their name. */
    void ResetAppearance();
    void InitKind9();
    void InitKind8();
    void SpawnCollectSoundObj(int arg);
    void CheckIceBlock();
    void CollectNoPose(daPly_c *arg);
    void SeekNewMarker(char *a, int gate);
    u8 PickTalkState();
    void RelinkMarker();
    void LinkRandomFreeMarker();
    void StepCameraCutscene();
    void PlaceCameraOnRing(void *cam);
    int PlaceCameraAtStarCamera(void *cam);
    void ProbeGround();
    int RearmSoundObj6();
    void SpawnSoundObj6();
    void StopCollectSoundObj();
    void func_ov002_020e7eb4();
    void UpdateSilverTrailEffect();
    void UpdateSparkleEffect();
    void SpawnPoseEffects();
    void SpawnCollectEffects();
    void UpdateTrailEffect();
    void DrawShadow();
    void UpdateModelMatrix();
    void FinishCollection();
    void UpdateWater();
    void OnBounceLand();
    void ReturnToHome();
    volatile unsigned int CheckBounds();
    int MaybeAddStarMarker();
    void CollectByEgg(int a);
    int Collect(void *p);
    void CheckPickup();
    void SetDefaultGravity();
    void StopMotion();
    void AimArcTo(Vector3 *p, int n);
    void FindMarker();
    int func_ov002_020e9630();
    void StateSpinAndEnd();
    void StatePlayAndEnd();
    void StateWaitMarker();
    void StateBounce();
    void StateSaveMessage();
    void StateCollectTalk();
    void StateCollectHold();
    void StateCollectBegin();
    int PickCollectSoundID();
    void StateTouched();
    void StateIdle();
    void StateLand();
    void StateFlyToMarker();
    void StateRise();
    void StateLaunch();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char PowerStar_size_must_be_0x4c4[sizeof(daStar_c) == 0x4c4 ? 1 : -1];
#endif

#endif /* DASTAR_C_H */
