#ifndef DAYEGG_C_H
#define DAYEGG_C_H

#include "types.h"
#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

/* The Yoshi egg. The cartridge's RTTI names the class: _ZTS8daYegg_c at 0x0210ad78,
 * _ZTI8daYegg_c at 0x0210ad84, and the vtable _ZTV8daYegg_c at 0x0210adb4 that the
 * factory and destructor store. Derives from dEnemyBase_c, and both witnesses agree:
 * daYegg_c_classInit allocates 0x42c, calls _ZN12dEnemyBase_cC2Ev, stores _ZTV8daYegg_c and
 * constructs the four members below in order; _ZN8daYegg_cD1Ev destroys the same four
 * in reverse and chains to _ZN12dEnemyBase_cD2Ev.
 *
 * SIZE 0x42c, the literal in the factory's fBase_c::operator new. dExtShadowModel_c ends
 * at 0x38c, so everything below that is this class's own.
 *
 * The old flat header also carried a marker at 0x350 called mAnimation. That is not a
 * member of this class at all -- ModelAnim derives from BOTH Model and dExtFrameCtrl_c, and
 * the dExtFrameCtrl_c base sits at +0x50, so 0x300 + 0x50 is mModelAnim's dExtFrameCtrl_c
 * subobject. It disappears here because the type expresses it.
 */
/* The egg's state number (mState, +0x3f0) indexes a table of eight member-function
 * pointers built at startup -- two per state, enter then execute. InitResources
 * seeds it from the low two bits of param1, so a freshly spawned egg starts in
 * whichever state param1 names. These names describe what each state's handlers
 * do, not labels recovered from the cartridge.
 *
 *   0  enter func_ov002_020ed5b0, execute func_ov002_020ed0d4: sits on the
 *      followed Player (mPlayer) and eases toward a point offset from it.
 *   1  enter func_ov002_020ecfc8, execute func_ov002_020ecf94: picks the nearest
 *      actor not yet targeted and flies at it, speed 100 units/frame.
 *   2  enter func_ov002_020ecad4, execute func_ov002_020ec9c4: speed zero; sets
 *      the X/Z tilt from a pair func_ov002_020d5f98 reads off the Player, then
 *      damps it back to level, and destroys the egg once func_ov002_020d6048
 *      says no for the Player.
 *   3  enter func_ov002_020ec978, execute func_ov002_020ec938: speed zero with
 *      gravity on; bursts as soon as it is on the ground.
 */
enum daYegg_State {
    daYegg_STATE_FOLLOW_PLAYER = 0,
    daYegg_STATE_SEEK          = 1,
    daYegg_STATE_WOBBLE        = 2,
    daYegg_STATE_DROP          = 3
};

/* The egg's own bits in dActor_c::mFlags (+0x0b0). Nothing in this file sets
 * them; the names say what the egg does while each is set. */
enum daYegg_Flag {
    daYegg_FLAG_ATTACHED = 0x100,   /* rides the Player's matrix (func_ov002_020ed998) */
    daYegg_FLAG_FLYING   = 0x400,   /* runs the flight logic in state 1 */
    daYegg_FLAG_DROPPED  = 0x2000   /* state 1 gives way to state 3 */
};

struct daYegg_c : dEnemyBase_c {
    dCcAc_c             mdCcAc_c;               /* 0x110 */
    dBgCh_Actr          mWithMeshClsn;          /* 0x144 */
    ModelAnim           mModelAnim;             /* 0x300 */
    dExtShadowModel_c         mShadowModel;           /* 0x364 */
    /* The ROM loads this WORD and passes it to _ZN6Player16IsInsideOfCannonEv as that
       function's `this`, which is an object address -- so the word is a Player *.
       It is the actor the egg follows and reads its pose from. */
    Player             *mPlayer;                /* 0x38c */
    /* A 4x3 matrix of twelve words. It is the Matrix4x3 argument of both
       dActor_c::DropShadow* calls, so it is the drop shadow's transform; the
       shadow code builds it from mAngleY and mPos >> 3 (words 9..11 are the
       translation). */
    s32 mShadowMtx[12];                         /* 0x390 */
    /* InitResources copies the actor's own position here once, and nothing
       migrated writes it again. */
    s32 mSpawnPosX;                             /* 0x3c0 */
    s32 mSpawnPosY;                             /* 0x3c4 */
    s32 mSpawnPosZ;                             /* 0x3c8 */
    /* Position sampled by func_ov002_020ed6cc each call, to measure how far the
       egg moved horizontally since the last one. */
    s32 mLastPosX;                              /* 0x3cc */
    s32 mLastPosY;                              /* 0x3d0 */
    s32 mLastPosZ;                              /* 0x3d4 */
    /* Where state 0 eases mPos toward: the followed Player's position plus an
       offset rotated by mEasedAngle. */
    s32 mHoldTargetX;                           /* 0x3d8 */
    s32 mHoldTargetY;                           /* 0x3dc */
    s32 mHoldTargetZ;                           /* 0x3e0 */
    /* TWO s16 triples. State 0 (func_ov002_020ed0d4) steps the second toward the
       first with ApproachAngle and copies its Y into mAngleY; both are seeded
       from mAngleX/Y/Z in InitResources and from the Player's angles in
       func_ov002_020ed5b0. */
    s16 mGoalAngleX;                            /* 0x3e4 */
    s16 mGoalAngleY;                            /* 0x3e6 */
    s16 mGoalAngleZ;                            /* 0x3e8 */
    s16 mEasedAngleX;                           /* 0x3ea */
    s16 mEasedAngleY;                           /* 0x3ec */
    s16 mEasedAngleZ;                           /* 0x3ee */
    /* The state number -- see daYegg_State. InitResources seeds it with
       param1 & 3 and that same value picks the collision size (2 is the big,
       2.0-scale egg); Behavior treats 1 as its own case. */
    s32 mState;                                 /* 0x3f0 */
    /* -0x68000 (-104 units) every time state 0 runs: the Z of the offset vector
       rotated by mEasedAngle and added to the Player's position. */
    s32 mHoldOffsetZ;                           /* 0x3f4 */
    u8  pad_3f8[0x4];
    /* uniqueIDs of the actors func_ov002_020edb3c has already chosen as a target,
       in the order chosen; mTargetedCount of the five are filled. It skips any
       candidate whose uniqueID is listed. */
    s32 mTargetedIds[5];                        /* 0x3fc */
    u32 mTargetId;                              /* 0x410 -- uniqueID of the actor being
                                                   sought now; 0 when none */
    s32 mParticleHandle;                        /* 0x414 -- Particle::System::New's
                                                   return, fed back on the next call */
    s32 mSoundHandle;                           /* 0x418 -- Sound::PlayLong's return,
                                                   fed back on the next call */
    u8  mTargetedCount;                         /* 0x41c */
    u8  mWobbling;                              /* 0x41d -- state 2: the tilt is being
                                                   damped back to level */
    u8  unk_41e;                                /* 0x41e -- DecIfAbove0_Byte countdown
                                                   in state 0; only ever cleared here */
    /* func_ov002_020ed6cc reloads this to 0xf whenever the egg moved 50 units or
       more since its last call, and counts it down otherwise. */
    u8  mStallTimer;                            /* 0x41f */
    /* Behavior indexes mPayoutDone with mPayoutIdx and stops at 5, which is what
       makes the array five wide and puts the byte at 0x426 outside it.
       func_ov002_020ec728 pays out once per slot. */
    u8  mPayoutIdx;                             /* 0x420 */
    u8  mPayoutDone[5];                         /* 0x421 */
    u8  mBurstDone;                             /* 0x426 -- set at the end of
                                                   func_ov002_020edca4, which returns at
                                                   once if it is already set */
    u8  mStarSlot;                              /* 0x427 -- dActor_c::TrackStar's answer,
                                                   the same shape as daIDonketu_c's */
    /* param1 >> 4. Bits 0-1: the variant func_ov002_020ec654 tests. Bits 2-5:
       how many coins func_ov002_020ec640 pays. Bit 6: pays a blue coin
       (func_ov002_020ec628). Bit 7: pays the tracked star (func_ov002_020ec610). */
    u8  mParamHigh;                             /* 0x428 */
    u8  pad_429[0x3];

    virtual ~daYegg_c();

    /* methods */
    int func_ov002_020ec610();
    int func_ov002_020ec628();
    unsigned char func_ov002_020ec640();
    int func_ov002_020ec654();
    void func_ov002_020ec670(int arg);
    void func_ov002_020ec728();
    void func_ov002_020ec80c(void *b, int count, int sl, short arg5);
    void func_ov002_020ec938();
    void func_ov002_020ec978();
    void func_ov002_020ec9c4();
    void func_ov002_020ecad4();
    void func_ov002_020ecb0c();
    void func_ov002_020ecd18();
    void func_ov002_020ecf94();
    void func_ov002_020ecfc8();
    void func_ov002_020ed0d4();
    void func_ov002_020ed5b0();
    void func_ov002_020ed684();
    int func_ov002_020ed6cc();
    void func_ov002_020ed738();
    void func_ov002_020ed7f8();
    void func_ov002_020ed998();
    int func_ov002_020edb3c(int a1, int best);
    int func_ov002_020eddc4();
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daYegg_c_size_must_be_0x42c[sizeof(daYegg_c) == 0x42c ? 1 : -1];
#endif

#endif /* DAYEGG_C_H */
