#ifndef DARNK_C_H
#define DARNK_C_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN7daRNk_cD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another, and dEnemyBase_c's own 0x110 closes
 * exactly on the first. Member NAMES are the ones this header already used --
 * a rebase should not also rename things its callers spell:
 *
 *     0x110 dCcAc_c       0x34   -> 0x144
 *     0x144 dBgCh_Actr             0x1bc  -> 0x300
 *     0x300 ModelAnim                0x64   -> 0x364
 *     0x364 dExtShadowModel_c              0x28   -> 0x38c
 *     0x3cc Vector3                  0xc    -> 0x3d8
 *     0x3d8 PathPtr                  0x8    -> 0x3e0
 *
 * SIZE IS THE ROM'S OWN: `daRNk_c_classInit` calls
 * `fBase_c::operator new(992)` -- 0x3e0 -- and the observed fields span
 * exactly to 0x3e0, so the literal and the layout agree.
 *
 * SM64DS RTTI names the implementation daRNk_c. The reconstructed factory
 * daRNk_c_classInit (historical alias KoopaTheQuick_Spawn)
 * installs this class's cartridge vtable; the reconstructed profile
 * global g_profile_RACE_NOKO (historical alias KoopaTheQuick_SpawnInfo)
 * is its registry descriptor.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "PathPtr.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

struct Player;

struct daRNk_c : dEnemyBase_c {
    dCcAc_c         mdCcAc_c;                        /* 0x110 */
    dBgCh_Actr      mWithMeshClsn;                   /* 0x144 */
    ModelAnim       mModelAnim;                      /* 0x300 */
    dExtShadowModel_c     mShadowModel;                    /* 0x364 */
    s32             mState;                          /* 0x38c -- State */
    /* Step within the current state. OFFER_RACE, RACE and POST_RACE_TALK
       switch on it; WAIT_FOR_PLAYER, PULL_UP and STOP only reset it. Every
       change of mState puts it back to 0. */
    u8              mStep;                           /* 0x390 */
    u8  pad_391[0x3];
    /* Unique ID of the RACE_FLAG actor (daRFlag_c): STATE_WAIT_FOR_PLAYER
       stores it, and STATE_RACE looks the flag up with it. */
    s32             mFlagID;                         /* 0x394 */
    /* The closest player, as of the last time STATE_WAIT_FOR_PLAYER looked. */
    Player *        mPlayer;                         /* 0x398 */
    /* Where InitResources found Koopa. Written once; nothing in this file
       reads it back. */
    Vector3         mSpawnPos;                       /* 0x39c */
    /* The angle Koopa is turning toward. Both the talk states and the path
       follower write it and ApproachLinear walks mPrevAngleY (the heading
       Behavior copies into mAngleY) to it, 0x800 a frame. */
    s16             mTargetAngleY;                   /* 0x3a8 */
    s16             unk_3aa;                         /* 0x3aa -- zeroed by InitResources and when the race is accepted; no reader */
    /* Latched once the player has been seen being shot out of a cannon while
       the race runs. A player who wins after that gets a different message
       and no star. */
    u8              mHasPlayerUsedCannon;            /* 0x3ac */
    /* Path node numbers where Koopa jumps (see func_ov062_02119954). Each is
       (a six-bit param1 field + 1) & 0x3f and is compared with mCurPathPt just
       after he reaches a node, i.e. after the increment, so it is the index of
       the NEXT node when he jumps. A field of 0 or 0x3f gives 1 or 0 and
       turns the jump off (0xff). */
    u8              mPathPtToJumpAt1;                /* 0x3ad */
    u8              mPathPtToJumpAt2;                /* 0x3ae */
    /* Set when Koopa reaches the end of the path: 1 if the flag had already
       been touched by then (the player got there first), else 0. Cleared again
       if the player used a cannon (IsBeingShotOutOfCannon latched) or
       param1 != 0 (not Mario). The post-race talk reads it to pick
       the message and to decide whether to spawn the star. */
    u8              mPlayerWon;                      /* 0x3af */
    /* Star-marker slot reserved by TrackStar (the s8& UntrackAndSpawnStar
       takes) and the star number it was reserved for, which InitResources
       takes from the low nibble of mAngleX. */
    s8              mTrackedStar;                    /* 0x3b0 */
    u8              mStarID;                         /* 0x3b1 */
    /* Latch for the stride effects in func_ov062_02119800: 1 once the
       footstep sound and dust for the current foot-down have been made, back
       to 0 when the animation leaves the foot-down frames. */
    u8              mFootstepDone;                   /* 0x3b2 */
    /* Set while the post-race message is the one for a player who is not
       Mario (message 0x9f); when it finishes, the talk restarts from step 0
       instead of going on to the step-4 chatter. */
    u8              mRestartTalk;                    /* 0x3b3 */
    u8              mHasFinished;                    /* 0x3b4 -- the race is over and the first post-race talk has begun */
    u8              mIsRacing;                       /* 0x3b5 -- set when the race starts; cleared when the post-race talk begins */
    u8              mIsTalkingToMario;               /* 0x3b6 -- the player Koopa is talking to is Mario (param1 == 0) */
    u8  pad_3b7[0x1];
    s32             mNumPathPts;                     /* 0x3b8 -- PathPtr::NumNodes() */
    /* Index of the node Koopa is running toward (mPathTarget). When it wraps
       to 0 the path is finished. */
    s32             mCurPathPt;                      /* 0x3bc */
    /* The node he just left (or where he started); the path follower uses it
       to tell when he has run past mPathTarget. */
    Vector3         mPrevPathPt;                     /* 0x3c0 */
    Vector3         mPathTarget;                     /* 0x3cc */
    PathPtr         mPathPtr;                        /* 0x3d8 */

    /* --- vtable --- */
    /* The inline destructor and factory emit D1 then D0 under 2004/b56.
       The complete emitted object is checked by the TU manifest. */
    virtual ~daRNk_c() {}

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    void func_ov062_02119800();      /* footstep sound and dust on the foot-down frames */
    void func_ov062_02119954();      /* start a jump */
    int  func_ov062_021199ac();      /* react to the rolling iron ball (BALL_*) */
    int  func_ov062_02119af0();      /* follow the path (PATH_*) */
    void func_ov062_02119be0();      /* STATE_POST_RACE_TALK */
    void func_ov062_0211a0f0();      /* STATE_STOP */
    void func_ov062_0211a168();      /* STATE_PULL_UP */
    void func_ov062_0211a1f4();      /* STATE_RACE */
    void func_ov062_0211a740();      /* STATE_OFFER_RACE */
    void func_ov062_0211a9c4();      /* STATE_WAIT_FOR_PLAYER */
    void func_ov062_0211aac0();      /* drop shadow, every frame */

    /* mState: which handler Behavior runs this frame. Behavior indexes a table
       of six pointers-to-member at data_ov062_0211e0a4 (filled at start-up by
       __sinit_ov062_0211d4a0, in this order) and calls the one at mState.
       Transitions: 0 -> 1; 1 -> 2 when the race is accepted, or back to 0 when
       it is declined (or the player is not Mario); 2 -> 3 -> 4 -> 5; 5 is
       never left. */
    enum State {
        STATE_WAIT_FOR_PLAYER = 0,  /* func_ov062_0211a9c4: idle until the player is close, then start the talk */
        STATE_OFFER_RACE = 1,       /* func_ov062_0211a740: turn to the player, ask the question, read the answer */
        STATE_RACE = 2,             /* func_ov062_0211a1f4: run the path to the flag */
        STATE_PULL_UP = 3,          /* func_ov062_0211a168: slow down after the flag until the animation ends */
        STATE_STOP = 4,             /* func_ov062_0211a0f0: brake to a halt and go back to the idle animation */
        STATE_POST_RACE_TALK = 5    /* func_ov062_02119be0: talk about who won; repeatable chat afterwards */
    };
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daRNk_c_size_must_be_0x3e0[sizeof(daRNk_c) == 0x3e0 ? 1 : -1];
#endif

#endif /* DARNK_C_H */
