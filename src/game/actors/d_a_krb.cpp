//cpp
/* daKrb_c -- Goomba (KURIBO 200 / KURIBO_S 201 / KURIBO_L 202), ov084.
 *
 * RTTI ov084 0x021308d4 is 7daKrb_c; _ZTI7daKrb_c at 0x021308e0 names
 * dCapEnemy_c as its base. The tree used to call this class Goomba.
 * One class, three factories. Text is 0x02129020..0x0212c10c.
 * defer_codegen off is load-bearing: the out-of-line destructor
 * emits D1, then D0, then a D2 the cartridge does not keep.
 *
 * mGoombaType picks the per-type tables (small, normal, large, and
 * the minions the Goomba King spawns). mState picks walk, hop,
 * airborne, tumble, or pause. Behavior runs the Yoshi-eat update,
 * the state handler, the hit reaction, the position update, the
 * ledge check, collision, and the stuck check.
 *
 * leftovers:
 * - dBgCh_Actr::Init stays mangled. fx32 is a plain s32, so the
 *   header method mangles as int.
 * - daPly_c::Hurt, daPly_c::Bounce and dActor_c::IsTooFarAwayFromPlayer
 *   are not declared on those classes.
 * - dBgCh_Actr::GetFloorResult has no header member.
 * - dExtFrameCtrl_c::Advance is called on the subobject at +0x3c0.
 *   mAnmModel.Advance() this-adjusts.
 * - UpdateChaseRadius recomputes the chase radius at each store.
 *   A named temporary DIFFs, and spelling mUnstickTimer on both
 *   sides of the decrement folds the halfword.
 * - OnTurnIntoEgg, the reward helper and InitResources mask param1
 *   in place through GetParam1(). A plain `param1 &= mask` CSEs the
 *   +8 address across the read-modify-write and DIFFs.
 * - mInitAngleY is the spawn heading, but ChooseTarget
 *   overwrites it with the chase heading. mWanderRerollTimer is s16
 *   in the header and this TU loads it unsigned.
 * - The flag test is lsl/lsrs. `&= ~n` on the u8 compiles to and;
 *   the ROM has bic.
 * - Several direct member spellings shrink the function and DIFF:
 *   UpdatePose testing `(mFlags & 0x40000) == 0` in place of
 *   its inYoshiMouth int (3 words),
 *   Render without the `locked` materialization (3 words), Behavior
 *   with `(mFlags & 8) == 0`, `mStateTimer`/`mStuckTimer` named on
 *   both sides (3 words).
 * - data_ov084_02130cf8 is the BMD. 02130278 is the BCA table.
 *   02130ce0/ce8/cf0/cc0/cc8/cd0 are BCA handles (SetAnim reads [1]).
 *   0213089c / 0213088c are BMA. The 021302xx words are the per-type
 *   tables.
 * - GetCapState is the dCapEnemy_c.h member.
 * - What dCcAc_c bits 0x4 and 0x20000 mean, the king's mState 4 that
 *   mFreeRoamTimer watches, sounds 0xd0 / 0xe0 / 0xd6 / 0x110 / 0x111 / 0x118 /
 *   0x13a / 0x13b, mFlags bits 0x8 and 0x10000000, and gCurrentSublevel
 *   values 6 and 0x1b are not named here.
 *   Nothing in this TU enters STATE_PAUSE except through the table.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daKrb_c_AssetId {
    ASSET_KURIBO_MODEL_BMD = 0x0386,
    ASSET_KURIBO_RECOVER_BCA = 0x0387,
    ASSET_KURIBO_ROLLING_BCA = 0x0388,
    ASSET_KURIBO_RUN_BCA = 0x0389,
    ASSET_KURIBO_STRETCH_BCA = 0x038a,
    ASSET_KURIBO_UNBALANCE_BCA = 0x038b,
    ASSET_KURIBO_WAIT_BCA = 0x038c,
    ASSET_KURIBO_WALK_BCA = 0x038d,
    ASSET_KURIBO_LOW_MODEL_BMD = 0x038e
};

#pragma defer_codegen off

#include "daKrb_c.h"
#include "nitro/fx/fx.h"
#include "SpawnInfo.h"
#include "common.h"
#include "dBgCh_Gnd.h"
#include "types.h"
#include "dBgCh_Actr.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "decl_dCapEnemy_c.h"
#include "LevelRng.h"
#include "dExtAnmMaterial_c.h"
#include "daPly_c.h"
#include "daKuriKing_c.h"
#include "daStarBase_c.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/fx/fx_trig.h"
#include "daStar_c.h"

typedef struct {
    unsigned char b0 : 1;
    unsigned char flag : 1;
} Flags;

/* Resource handles: the files the Goombas load are read through filePtr.
   dExtSimpleModel_c handles construct through
   func_02017acc and destroy through func_02017ab4. Animation handles
   construct through SharedFilePtr::Construct and destroy through
   SharedFilePtr_Destruct_Anim. The manifest aliases those undefined members
   onto the ROM symbols. */
struct KrbModelFilePtr : SharedFilePtr {
    KrbModelFilePtr(u32 fileID);
    ~KrbModelFilePtr();
};

/* A state handler: data_ov084_02130d74 holds one pointer-to-member per
   daKrb_c::State. */
typedef void (daKrb_c::*StateHandler)();

/* Actor ids (symbols/actor_debug_names.tsv). */
enum {
    ACTOR_SILVER_STAR = 179,
    ACTOR_STARBASE = 180,
    ACTOR_PLAYER = 191,
    ACTOR_KURIKING = 198,
    ACTOR_KURIBO = 200,
    ACTOR_KURIBO_S = 201,
    ACTOR_KURIBO_L = 202
};

/* dCcAc_c hitFlags bits, with the names include/dCc_c.h gives its table. The
   egg and explosion bits are proven by matched code; punch, kick, breakdance,
   slide kick, dive, ground pound, explosion and the player bit were read on
   the cartridge from this class's hit reaction (the table's SEEN rows).
   0x20000 is not in it. */
enum {
    HIT_MEGA = 0x10,
    HIT_SPIN_OR_POUND = 0x20,
    HIT_PUNCH = 0x40,
    HIT_KICK = 0x80,
    HIT_BREAKDANCE = 0x100,
    HIT_SLIDE_KICK = 0x200,
    HIT_DIVE = 0x400,
    HIT_EGG = 0x2000,
    HIT_EXPLOSION = 0x4000,
    HIT_YOSHI_TONGUE = 0x8000,
    HIT_UNLISTED_20000 = 0x20000,
    HIT_FIRE = 0x40000,
    HIT_PLAYER = 0x400000
};

/* Sound ids for func_02012694(id, &camSpacePos), which plays an id at the
   actor's camera-space position. Named by where they are played; the sounds
   themselves are not identified. */
enum {
    SND_FOOTSTEP = 0xd0,           /* on the step frames of the walk / run animation */
    SND_STOMPED = 0xe0,            /* jumped on, or spin / ground-pound hit */
    SND_DEATH_SMALL = 0x110,       /* data_ov084_02130218[0]; also when a KURIBO_S touches the player */
    SND_DEATH_NORMAL = 0xd6,       /* data_ov084_02130218[1] and [3] */
    SND_DEATH_LARGE = 0x111,       /* data_ov084_02130218[2] */
    SND_HOP = 0x118,               /* start of a hop */
    SND_TUMBLE_BOUNCE_FIRST = 0x13a,  /* tumble start and first bounce */
    SND_TUMBLE_BOUNCE_LATER = 0x13b   /* second and third bounce */
};

/* data_ov084_02130cf8 is the BMD this TU LoadFile / Release. The other
   six handles are never passed to a SharedFilePtr method; this TU reads
   filePtr as the loaded BCA. Selected by role (see SetAnim calls):
   02130ce8 is the walk animation (target speed data_ov084_02130228[type],
   also what the Goomba returns to after a pause or tumble), 02130cf0 the
   faster one chosen when it speeds up to data_ov084_02130268[type],
   02130cc8 the standing one a king minion plays while its target speed is
   0, 02130cc0 the tumble animation, 02130ce0 the one played on deaths 2, 5
   and 7, and 02130cd0 the one played on deaths 3, 4 and 6. */
extern KrbAnimationFileHandle data_ov084_02130cc0;
extern KrbAnimationFileHandle data_ov084_02130cc8;
extern KrbAnimationFileHandle data_ov084_02130cd0;
extern KrbAnimationFileHandle gKuriboRecoverAnimationFile;
extern KrbAnimationFileHandle data_ov084_02130ce0;
extern KrbAnimationFileHandle data_ov084_02130ce8;
extern KrbAnimationFileHandle data_ov084_02130cf0;
extern KrbModelFilePtr data_ov084_02130cf8;
extern KrbModelFilePtr gKuriboLowModelModelFile;
extern StateHandler data_ov084_02130d74[];
extern "C" int func_02037e20(int *p);
void func_02012694(unsigned int id, const Vector3 *pos);
int SurfaceInfo_TestFlag0x20(int *p);
extern u8 gNumVsStarsCollected;
extern u8 *gVsStarSpawnOrder;
extern const u8 data_ov084_02130204[4];
extern const int data_ov084_02130208[4];
extern const int data_ov084_02130218[4];
extern const int data_ov084_02130228[4];
extern const int data_ov084_02130238[4];
extern const int data_ov084_02130248[4];
extern const int data_ov084_02130258[4];
extern const int data_ov084_02130268[4];
extern BMA_File data_ov084_0213088c;
extern BMA_File data_ov084_0213089c;
extern "C" int Math_Function_0203b14c(int *v, int target, int a, int b, int c);
extern s8 gCurrentSublevel;
void LoadSilverStarAndNumber(void);
extern SharedFilePtr *const sKuriboAnimationFiles[7];

daKrb_c::~daKrb_c() {}

static inline int IsActorID(dActor_c *actor, int id) {
    return actor->actorID == id;
}

static inline int IsOffScreen(dActor_c *actor) {
    return (actor->mFlags & 8) != 0;
}

static inline int IsInYoshiMouth(dActor_c *actor) {
    return (actor->mFlags & 0x40000) != 0;
}

/* Per-frame upkeep of mChaseRadius (how close the player must be to be
 * noticed) and of mUnstickTimer.
 *
 * While mUnstickTimer is running it counts down one a frame (or is cleared
 * outright in STATE_ATTACKED_MARIO) and the radius is left alone. Otherwise the
 * radius is 500 units, except that a Goomba with mCapId >= 6 that has been
 * stuck for more than 10 frames loses 20 units per extra frame, down to a
 * floor of 10 units (0xa000).
 *
 * The address has to be recomputed at each store: a named temporary, or
 * mUnstickTimer spelled the same way on both sides of the decrement, lets mwcc
 * CSE the field and the function stops matching. */
inline s32 *chaseRadius(daKrb_c *goomba)
{
    return &goomba->mChaseRadius;
}

void daKrb_c::UpdateChaseRadius() {
    daKrb_c *self2;
    int state;
    if (mUnstickTimer != 0) {
        /* Comparison stays in the `if`: assigning `== 1` to an int is a bool
           in C++ and materialises moveq/movne. */
        state = mState;
        if (state == daKrb_c::STATE_ATTACKED_MARIO) {
            mUnstickTimer = 0;
        } else {
            mUnstickTimer--;
        }
        return;
    }
    if ((self2 = this)->mCapId < 6) {
        *chaseRadius(self2) = 0x1f4000;     /* 500.0 */
        return;
    }
    unsigned short stuck = mStuckTimer;
    if (stuck > 0xa)
    {
        *chaseRadius(self2) = 0x1f4000 - (((stuck - 0xa) * 0x14) << 12);
        if ((*chaseRadius(self2)) < 0xa000)     /* 10.0 */
        {
            *chaseRadius(self2) = 0xa000;
        }
        return;
    }
    *chaseRadius(self2) = 0x1f4000;
}

/* Enter STATE_TUMBLE: the Goomba is thrown clear. Called when the cap comes
 * off (UpdateYoshiEatAndHit) and, with a null `actor`, when a Yoshi-held Goomba
 * lands. `actor` is the eater
 * (mEatingPlayer) when there is one; the Goomba is turned toward it.
 *
 * Starts the 60-frame bounce countdown (mBounceCountdown) and the tumble
 * animation, gives it a vertical speed of (13.0 - mVertAccel) / mFloorNormal.y
 * and a backward horizontal speed equal to the length of the horizontal part
 * of the velocity vector at 0xa4, and releases it from Yoshi. */
void daKrb_c::EnterTumble(dActor_c *actor) {
    mBounceCountdown = 60;
    mSpeed.y = FX_Div(0xd000 - mVertAccel, mFloorNormal.y);
    mHorzSpeed = Vec3_HorzLen(&mSpeed) * -1;
    if (actor != 0)
        mPrevAngle.y = Vec3_HorzAngle(&mPos, &actor->mPos);
    mAnmModel.SetAnim((BCA_File *)data_ov084_02130cc0.filePtr, 0, 0x1000, 0);
    mState = daKrb_c::STATE_TUMBLE;
    mEatenByYoshi = 0;
    mWithMeshClsn.SetLimMovFlag();
    mWithMeshClsn.Unk_0203589c();
    mWithMeshClsn.ClearJustHitGroundFlag();
    mWithMeshClsn.ClearGroundFlag();
    func_02012694(SND_TUMBLE_BOUNCE_FIRST, &mCamSpacePos);
    mBounceCount = 0;
}

/* Water / toxic-floor check, run every frame while the Goomba is off the
 * ground. A probe is cast from 400.0 units above it that looks only for water
 * and toxic surfaces (ordinary ground is switched off). If it finds a surface
 * for which func_02037e20 (CLPS bit 0x2000000, see CLPS.h) is nonzero, and
 * the Goomba is below the height of the hit, the Goomba is lost: it drops its
 * coin, poofs, is removed (RemoveSelf), has its cap released and is
 * put back at mHomePos (RespawnIfHasCap). */
void daKrb_c::CheckHazardBelow() {
    if (mWithMeshClsn.IsOnGround() != 0)
        return;
    {
        Vector3 pos;
        {
            /* z before y: mwcc keeps that load order. */
            int vx = mPos.x;
            int vz = mPos.z;
            int vy = mPos.y + 0x190000; /* 400.0 above */
            pos.x = vx;
            pos.y = vy;
            pos.z = vz;
        }
        dBgCh_Gnd rg;
        rg.StartDetectingWater();
        rg.StartDetectingToxic();
        rg.StopDetectingOrdinary();
        rg.SetObjAndPos(pos, this);
        if (rg.DetectClsn() != 0) {
            if (func_02037e20((int *)&rg.surface) != 0) {
                if (rg.clsnY != 0x80000000) {
                    if (mPos.y < rg.clsnY) {
                        SpawnCoin();
                        PoofDust();
                        RemoveSelf();
                        {
                            Vector3 cap;
                            cap.x = 0;
                            cap.y = 0x6c000;     /* 108.0 */
                            cap.z = 0;
                            ReleaseCap(cap);
                        }
                        mPos.x = mHomePos.x;
                        mPos.y = mHomePos.y;
                        mPos.z = mHomePos.z;
                        RespawnIfHasCap();
                    }
                }
            }
        }
    }
}

/* Footstep sound. Only in STATE_WALK and on the ground. The animation frame
 * is currFrame >> 12 (the whole-frame part of the 20.12 value). With the walk
 * animation (data_ov084_02130ce8) the step windows are frames 0..4 and 12..16;
 * with the faster one (data_ov084_02130cf0) 0..3 and 16..19. On entering a
 * window the sound plays once and MOVE_STEP_SOUND_LATCH is set; the latch is
 * cleared when the frame leaves the window. */
void daKrb_c::PlayFootstepSound() {
    unsigned int kind;
    int frame;
    int type;

    if (mState != daKrb_c::STATE_WALK)
        return;

    if (!mWithMeshClsn.IsOnGround())
        return;

    /* Frame before the file pointer: that order is what colors the short
       extract and the file load into the registers the ROM uses. */
    frame = mAnmModel.currFrame;
    kind = (unsigned short)((unsigned)frame >> 12);
    type = (int)mAnmModel.file;

    if (type == (int)data_ov084_02130ce8.filePtr) {
        if (kind <= 4 || (kind >= 0xc && kind <= 0x10)) {
            if (((Flags *)&mMoveFlags)->flag)
                return;
            func_02012694(SND_FOOTSTEP, &mCamSpacePos);
            mMoveFlags |= daKrb_c::MOVE_STEP_SOUND_LATCH;
            return;
        }
        mMoveFlags &= ~daKrb_c::MOVE_STEP_SOUND_LATCH;
        return;
    }

    if (type == (int)data_ov084_02130cf0.filePtr) {
        if (kind <= 3 || (kind >= 0x10 && kind <= 0x13)) {
            if (((Flags *)&mMoveFlags)->flag)
                return;
            func_02012694(SND_FOOTSTEP, &mCamSpacePos);
            mMoveFlags |= daKrb_c::MOVE_STEP_SOUND_LATCH;
            return;
        }
        mMoveFlags &= ~daKrb_c::MOVE_STEP_SOUND_LATCH;
    }
}

/* Remove the Goomba. With mCapId below 6 (the cap table's "has a cap" range)
 * it is only marked for destruction (MarkForDestruction); otherwise it goes
 * through KillAndTrackInDeathTable. */
void daKrb_c::RemoveSelf() {
    if ((mCapId & 0xf) < 6)
        MarkForDestruction();
    else
        KillAndTrackInDeathTable();
}

/* Ground check, run every frame while the Goomba stands on something.
 * If the floor's CLPS record has flag 0x20 set (SurfaceInfo_TestFlag0x20;
 * CLPS.h reads it as water) the Goomba is lost: reward (PayOutReward),
 * coin, removal, cap released, back to mHomePos (RespawnIfHasCap).
 *
 * Otherwise the floor record is copied into a local dBgPi and its surface
 * type (CLPS bits 19..23, CLPS::GetSurfaceType) is read. The Goomba is treated as
 * lost (coin and removal, and a respawn at mHomePos when it has a cap or is
 * REWARD_SILVER_STAR_IF_CURRENT) if the type is 1, 4, 5 or 0x13, or if it is
 * 6..9 (daPly_c.h calls those the quicksand tiers) while the sound type
 * (CLPS bits 0..4, CLPS::GetSoundType) is 8. */
void daKrb_c::CheckGroundSurface() {
    if (!mWithMeshClsn.IsOnGround())
        return;

    if (SurfaceInfo_TestFlag0x20((int *)&mWithMeshClsn.GetFloorResult()->surface)) {
        PayOutReward();
        SpawnCoin();
        RemoveSelf();
        ReleaseCap(Vector3(0, 0x6c000, 0)); /* 108.0 */
        mPos = mHomePos;
        RespawnIfHasCap();
        return;
    }

    dBgPi floor(*mWithMeshClsn.GetFloorResult());
    int surfaceType = floor.surface.clps.GetSurfaceType();
    if ((floor.surface.clps.GetSoundType() == 8 &&
         (surfaceType == 6 || surfaceType == 7 || surfaceType == 8 || surfaceType == 9)) ||
        surfaceType == 0x13 || surfaceType == 1 || surfaceType == 4 || surfaceType == 5) {
        SpawnCoin();
        RemoveSelf();
        if ((mCapId & 0xf) < 6 || mRewardType == REWARD_SILVER_STAR_IF_CURRENT) {
            mPos = mHomePos;
            ReleaseCap(Vector3(0, 0x6c000, 0)); /* 108.0 */
            RespawnIfHasCap();
        }
    }
}

/* Set the home position (mHomePos) from a three-word position. daKuriKing_c
 * calls this on each minion it spawns, after the Spawn. */

void daKrb_c::SetHomePos(Vector3 *pos) {
    daKrb_c *goomba = (daKrb_c *)this;
    goomba->mHomePos.x = pos->x;
    goomba->mHomePos.y = pos->y;
    goomba->mHomePos.z = pos->z;
}

/* The reward helper, OnTurnIntoEgg and InitResources mask param1 in place
   with a read through this accessor. A plain `param1 &= mask` lets mwccarm
   CSE the field address across the read-modify-write; the cartridge
   re-issues [rN,#8]. */
inline u32 daKrb_c::GetParam1() const {
    return param1;
}

/* Pay out the reward the Goomba carries (mRewardType), then clear bits 4..7
 * and the upper halfword of param1 (mask 0xff0f), presumably so a respawned
 * copy does not read the reward again.
 *
 * REWARD_SILVER_STAR: untrack the star slot, spawn a STARBASE (180) at the
 * home position with parameter 0x50 and a SILVER_STAR (179) at the Goomba's
 * own position with parameter 0x10, give the silver star the marker's
 * uniqueID, link the two and play sound object 1.
 * REWARD_SILVER_STAR_IF_CURRENT: only when mStarID equals
 * gVsStarSpawnOrder[gNumVsStarsCollected]; spawns both actors with parameter
 * mStarID | 0x30 at the Goomba's position, marks the reward spent, and
 * plays sound object 1. */
void daKrb_c::PayOutReward() {
    if (mRewardType == daKrb_c::REWARD_SILVER_STAR) {
        char *starMarker;
        char *silverStar;
        UntrackStar(mStarTracked);
        starMarker = (char *)dActor_c::Spawn(ACTOR_STARBASE, 0x50, mHomePos, 0, mAreaId, -1);
        silverStar = (char *)dActor_c::Spawn(ACTOR_SILVER_STAR, 0x10, mPos, 0, mAreaId, -1);
        if (starMarker != 0 && silverStar != 0) {
            ((daStar_c *)silverStar)->mMarkerID = ((dActor_c *)starMarker)->uniqueID;
            ((daStarBase_c *)starMarker)->LinkSilverStarAndStarMarker((daStar_c *)silverStar);
            SpawnSoundObj(1);
        }
        param1 = GetParam1() & 0xff0f;
        return;
    }
    if (mRewardType != daKrb_c::REWARD_SILVER_STAR_IF_CURRENT)
        return;
    if (mStarID != gVsStarSpawnOrder[gNumVsStarsCollected])
        return;
    UntrackStar(mStarTracked);
    dActor_c::Spawn(ACTOR_STARBASE, mStarID | 0x30, mPos, 0, mAreaId, -1);
    dActor_c::Spawn(ACTOR_SILVER_STAR, mStarID | 0x30, mPos, 0, mAreaId, -1);
    mRewardType = daKrb_c::REWARD_SPENT;
    param1 = GetParam1() & 0xff0f;
    SpawnSoundObj(1);
}

/* For REWARD_SILVER_STAR_IF_CURRENT: while no star slot is tracked yet
 * (mStarTracked negative) and mStarID matches gVsStarSpawnOrder[gNumVsStarsCollected],
 * claim a slot (TrackStar(mStarID, 1)). */
void daKrb_c::TrackSilverStar() {
    if (mRewardType != daKrb_c::REWARD_SILVER_STAR_IF_CURRENT)
        return;
    if (mStarTracked >= 0)
        return;
    unsigned int current = gVsStarSpawnOrder[gNumVsStarsCollected];
    if (mStarID != current)
        return;
    mStarTracked = TrackStar(mStarID, 1);
}

/* Per-frame death handling (Behavior calls it while mDeathState != 0).
 * UpdateDeath runs the shared death sequence and returns nonzero on the frame
 * it finishes.
 *
 * For deaths 2..6 (the knocked-away ones) the animation is advanced at
 * normal speed. A king minion (type 3) that was hit by a KURIKING actor
 * drops a coin and is removed, and for every minion in that phase
 * dCcAc_c flags bit 0x20000 is set (its meaning is not recovered) and the
 * cylinder is cleared and updated.
 *
 * When the death finishes: play the per-type death sound
 * (data_ov084_02130218), pay out the reward, and for a Goomba with a cap
 * (mCapId < 6) or REWARD_SILVER_STAR_IF_CURRENT send it back to mHomePos and
 * respawn; one with a cap is also dropped from the death table. */
int daKrb_c::UpdateDeathState() {
    int deathState = UpdateDeath(mWithMeshClsn);
    if ((unsigned int)(mDeathState - 2) <= 4) {
        mAnmModel.speed = 0x1000;
        (&mAnmModel)->Advance();
        if (mGoombaType == daKrb_c::GOOMBA_KING_MINION) {
            /* SpawnCoin only when the actor we hit is a KURIKING. The flag and the
               cylinder update always run for a king minion in this death phase. */
            unsigned int id = mdCcAc_c.otherOwner;
            if (id != 0) {
                dActor_c *r = dActor_c::FindWithID(id);
                if (r != 0) {
                    if (IsActorID(r, ACTOR_KURIKING)) {
                        SpawnCoin();
                        RemoveSelf();
                    }
                }
            }
            mdCcAc_c.flags |= 0x20000;
            mdCcAc_c.Clear();
            mdCcAc_c.Update();
        }
    }
    if (deathState != 0) {
        func_02012694(data_ov084_02130218[mGoombaType], &mCamSpacePos);
        PayOutReward();
        if ((mCapId & 0xf) < 6 || mRewardType == daKrb_c::REWARD_SILVER_STAR_IF_CURRENT) {
            mPos.x = mHomePos.x;
            mPos.y = mHomePos.y;
            mPos.z = mHomePos.z;
            RespawnIfHasCap();
        }
        if ((mCapId & 0xf) < 6)
            UntrackInDeathTable();
    }
    return deathState;
}

/* Yoshi-eat and hit front end, called near the start of Behavior. Returns 0
 * when Behavior should carry on with the normal state update (nobody is
 * eating it, or the cap has just come off); every other path returns 1 and
 * Behavior ends the frame.
 *
 * eatState is UpdateYoshiEat's result (0 = not being eaten):
 *   1: GetCapEatenOffIt; if the cap came off, the Goomba is thrown clear
 *      (EnterTumble, then horizontal speed -15.0 and vertical speed
 *      20.0), its material animation is restarted and its cylinder cleared.
 *   3: halve the horizontal speed while on the ground.
 * A hit reported by SpawnParticlesIfHitOtherObj sets mDeathState to
 * DEATH_HIT_20000, hands the hit to KillByAttack, and sets bit 0 of
 * the dCcAc_c flags (disabled).
 * Otherwise the model and cap are placed (UpdatePose) and, while
 * Yoshi holds it (mEatenByYoshi != 0), the ground check runs
 * (CheckGroundSurface), then by mSpitOutTimer: 0 updates the cylinder; 5 replays
 * the data_ov084_02130ce0 animation, turns the Goomba half a turn, reverses
 * its horizontal speed and restarts the material animation. The animation is
 * advanced, and on touchdown the Goomba is thrown clear again
 * (EnterTumble with no eater).
 * A king minion with eatState >= 3 that has dropped more than 1000.0 below
 * mHomePos is removed. */
int daKrb_c::UpdateYoshiEatAndHit() {
    int eatState = UpdateYoshiEat(mWithMeshClsn);
    if (eatState != 0) {
        if (eatState == 1) {
            Vector3 v;
            dActor_c *actor = mEatingPlayer;
            v.x = 0;
            v.y = 0x6c000; /* 108.0 */
            v.z = 0;
            if (GetCapEatenOffIt(v) != 0) {
                EnterTumble(actor);
                mHorzSpeed = -0xf000; /* -15.0 */
                mSpeed.y = 0x14000;   /* 20.0 */
                dExtAnmMaterial_c::Prepare(*(BMD_File *)(data_ov084_02130cf8.filePtr),
                                           data_ov084_0213088c);
                mMaterialChanger.SetFile(data_ov084_0213088c, 0x40000000, 0x1000, 0);
                mMaterialChanger.currFrame = 0;
                (&mdCcAc_c)->Clear();
                return 0;
            }
        } else if (eatState == 3) {
            if (mWithMeshClsn.IsOnGround())
                mHorzSpeed >>= 1;
        }

        if (SpawnParticlesIfHitOtherObj(mdCcAc_c) != 0) {
            void *actor = dActor_c::FindWithID(mdCcAc_c.otherOwner);
            mDeathState = daKrb_c::DEATH_HIT_20000;
            KillByAttack((dActor_c *)actor, &mWithMeshClsn);
            mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce0.filePtr, 0x40000000, 0x1000, 0);
            mdCcAc_c.flags |= 1;
            return 1;
        }

        UpdatePose();
        (&mdCcAc_c)->Clear();
        if (mEatenByYoshi != 0) {
            CheckGroundSurface();
            {
                u16 s = mSpitOutTimer;
                if (s == 0) {
                    (&mdCcAc_c)->Update();
                } else if (s == 5) {
                    mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce0.filePtr, 0x40000000, 0x1000,
                                      0);
                    mPrevAngle.y += 0x8000; /* half a turn */
                    mHorzSpeed = -mHorzSpeed;
                    dExtAnmMaterial_c::Prepare(*(BMD_File *)(data_ov084_02130cf8.filePtr),
                                               data_ov084_0213088c);
                    mMaterialChanger.SetFile(data_ov084_0213088c, 0x40000000, 0x1000, 0);
                    mMaterialChanger.currFrame = 0;
                }
            }
            (&mAnmModel)->Advance();
            if (mWithMeshClsn.JustHitGround())
                EnterTumble(0);
        }

        if (mGoombaType == daKrb_c::GOOMBA_KING_MINION && eatState >= 3 &&
            mPos.y < mHomePos.y - 0x3e8000) { /* 1000.0 below home */
            MarkForDestruction();
            return 1;
        }
        return 1;
    }
    return 0;
}

/* Start a hop: play SND_HOP, enter STATE_JUMP with no horizontal speed and
 * the per-type launch speed data_ov084_02130248[type] as vertical speed, and
 * set the 0x4 bit of the dCcAc_c flags (meaning not recovered).
 * N64: goomba_begin_jump -- the same sound, action JUMP, forward speed 0 and
 * a size-scaled upward speed (50/3 * oGoombaScale there, a per-type table
 * here). */
void daKrb_c::BeginJump() {
    func_02012694(SND_HOP, &mCamSpacePos);
    mState = daKrb_c::STATE_JUMP;
    mHorzSpeed = 0;
    mSpeed.y = data_ov084_02130248[mGoombaType];
    mWithMeshClsn.ClearGroundFlag();
    mdCcAc_c.flags |= 4;
}

/* Choose what the Goomba is heading for. Sets mClosestPlayer, mDistToPlayer
 * (the distance to the target, or 0x61a8000 = 25000.0 for "nothing to
 * react to"; 0x7fffffff until the first call) and mInitAngleY (the heading
 * the Goomba should move on: toward home, toward the player, or away from the
 * player, as listed below).
 *
 * distThresh is how far from mHomePos the Goomba may stray before it heads
 * back (the caller passes 0x3e8000 = 1000.0).
 *  - No player, or (mCapId >= 6 and farther than distThresh from home): head
 *    for home, distance "none".
 *  - Capped Goomba (mCapId < 6): same if too far from home and not against a
 *    wall; otherwise, if the player is inside mChaseRadius, it flees (the
 *    heading is away from the player, or toward it while mUnstickTimer is
 *    running); else "none".
 *  - Capless Goomba: ignore the player if the player is farther than
 *    distThresh from home, else chase the player. */
void daKrb_c::ChooseTarget(fx32 distThresh) {
    struct Vector3 ppos;

    mClosestPlayer = ClosestPlayer();

    if (mClosestPlayer == 0 || (Vec3_Dist(&mPos, &mHomePos) > distThresh && mCapId >= 6)) {
        mInitAngleY = Vec3_HorzAngle(&mPos, &mHomePos);
        mDistToPlayer = 0x61a8000;
        return;
    }

    {
        int *ppos_src = (&mClosestPlayer->mPos.x);
        ppos.x = ppos_src[0];
        ppos.y = ppos_src[1];
        ppos.z = ppos_src[2];
    }

    if (mCapId < 6) {
        if (Vec3_Dist(&mPos, &mHomePos) > distThresh && !mWithMeshClsn.IsOnWall()) {
            mDistToPlayer = 0x61a8000;
            mInitAngleY = Vec3_HorzAngle(&mPos, &mHomePos);
            return;
        }

        if (Vec3_Dist(&mPos, &ppos) < mChaseRadius) {
            mDistToPlayer = Vec3_Dist(&mPos, &ppos);
            if (mUnstickTimer != 0) {
                mInitAngleY = Vec3_HorzAngle(&mPos, &ppos);
                return;
            }
            mInitAngleY = Vec3_HorzAngle(&ppos, &mPos);
            return;
        }
        mDistToPlayer = 0x61a8000;
        return;
    }

    if (Vec3_Dist(&mHomePos, &ppos) > distThresh) {
        mDistToPlayer = 0x61a8000;
        return;
    }
    mDistToPlayer = Vec3_Dist(&mPos, &ppos);
    mInitAngleY = Vec3_HorzAngle(&mPos, &ppos);
}

static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == ACTOR_PLAYER;
}

/* Collision reaction: something touched this Goomba, decide what it did.
 *
 * `flags` is hitFlags (the collision word at 0x1a0) and `other` the actor found
 * through otherOwner (0x1a4). Type ids are the debug-table profile numbers this
 * class is registered under -- 200 KURIBO, 201 KURIBO_S, 202 KURIBO_L, and 191
 * for a daPly_c -- so the IsActorID(this, ...) tests ask which of the three
 * Goomba sizes WE are, not what hit us. mDeathState is set to the DEATH_* value for the hit, and
 * Behavior's mDeathState != 0 path plays it out. Hit-bit names are dCc_c.h's
 * best-effort table (HIT_*). In the order tested:
 *   HIT_MEGA (not KURIBO_S): cap released, then KillByInvincibleChar with
 *      speed 65.0 (KURIBO) or 150.0 (KURIBO_L) and a direction pitched back
 *      0x2000 (45 degrees) or 0x1800 (33.75 degrees).
 *   HIT_SPIN_OR_POUND: DEATH_STOMPED, scale reset to 1.0, SND_STOMPED.
 *   HIT_FIRE: DEATH_FIRE.
 *   For KURIBO_L the chain stops here and goes to its own daPly_c test (below,
 *   without the metal and shell tests); KURIBO and KURIBO_S go on:
 *   0x20000: DEATH_HIT_20000, dCcAc_c flag bit 0 set (disabled).
 *   HIT_EGG | HIT_DIVE: DEATH_DIVE_OR_EGG.   HIT_EXPLOSION: DEATH_EXPLOSION.
 *   HIT_KICK | HIT_BREAKDANCE | HIT_SLIDE_KICK: DEATH_KICKED.
 *   HIT_PUNCH: DEATH_PUNCHED.
 *   Anything but HIT_YOSHI_TONGUE then reaches the daPly_c test. For a daPly_c
 *   (actor 191): if it is metal (mIsMetal) the Goomba is killed (speed 65.0);
 *   if it is on a shell (IsOnShell) DEATH_DIVE_OR_EGG; if JumpedOnByPlayer, the
 *   daPly_c bounces (40.0) and DEATH_STOMPED; if it is vanished (mIsVanish)
 *   nothing happens; otherwise, in STATE_WALK, the daPly_c is hurt (knockback
 *   12.0, or 5.0 for a king minion) and the Goomba either removes itself
 *   (KURIBO_S) or, when the hit carries HIT_PLAYER, goes to STATE_ATTACKED_MARIO
 *   (the others).
 *   KURIBO_L's own daPly_c test: JumpedOnByPlayer gives DEATH_STOMPED; a
 *   vanished daPly_c does nothing; in STATE_WALK it goes to STATE_ATTACKED_MARIO
 *   first and then hurts the daPly_c only if the hit carries HIT_PLAYER.
 * If a death state was chosen the cap is released. In every case the hit is
 * handed to KillByAttack, and the Goomba is turned half a turn when the
 * hit asked for it (turnAround). A king minion hit by a punch, kick, breakdance or
 * slide kick adds the attacker's horizontal speed to its own; one hit by a
 * dive adds 0x20.
 *
 * The actor-ID tests go through IsActorID and IsPlayerActor: the
 * int-returning predicates leave the compare materialized and re-tested, which
 * is the shape the cartridge has. Testing actorID directly in the `if`
 * collapses the pair. */
void daKrb_c::ReactToCollision() {
    Vector3_16 killDirNormal;
    Vector3_16 killDirVariant;
    Vector3_16 killDirPlayer;
    Vector3 playerPos;
    Vector3 playerPosJump;
    Vector3 capReleaseOnHit;
    Vector3 capReleaseOnKill;
    Vector3 hurtOriginFirstHit;
    Vector3 hurtOriginRepeat;
    Vector3 hurtOriginJumped;
    Vector3 capReleaseOnExit;
    daPly_c *other;
    u32 flags;
    s32 turnAround;
    s32 hurtKnockback;
    u32 id;

    id = mdCcAc_c.otherOwner;
    if (id == 0)
        return;
    other = static_cast<daPly_c *>(dActor_c::FindWithID(id));
    if (other == 0)
        return;

    flags = mdCcAc_c.hitFlags;
    hurtKnockback = 0xc000; /* 12.0 */
    mLastHitFlags = flags;
    turnAround = 0;
    if (mGoombaType == daKrb_c::GOOMBA_KING_MINION)
        hurtKnockback = 0x5000; /* 5.0 */
    if (!IsActorID(this, ACTOR_KURIBO_S) && (flags & HIT_MEGA)) {
        capReleaseOnHit.x = 0;
        capReleaseOnHit.y = 0x6c000;
        capReleaseOnHit.z = 0; /* 108.0 */
        ReleaseCap(capReleaseOnHit);
        if (IsActorID(this, ACTOR_KURIBO)) {
            killDirNormal.x = -0x2000;
            killDirNormal.y = 0;
            killDirNormal.z = 0;
            KillByInvincibleChar(killDirNormal, *other, 0x41000); /* 65.0 */
            return;
        }
        killDirVariant.x = -0x1800;
        killDirVariant.y = 0;
        killDirVariant.z = 0;
        KillByInvincibleChar(killDirVariant, *other, 0x96000); /* 150.0 */
        return;
    }

    if (flags & HIT_SPIN_OR_POUND) {
        mDeathState = daKrb_c::DEATH_STOMPED;
        if (mGoombaType == daKrb_c::GOOMBA_LARGE)
            mCoinType = daKrb_c::COIN_BLUE;
        mScale.x = FX32_ONE;
        mScale.y = FX32_ONE;
        mScale.z = FX32_ONE;
        func_02012694(SND_STOMPED, &mCamSpacePos);
    } else if (flags & HIT_FIRE) {
        turnAround = 1;
        mAnmModel.SetAnim((BCA_File *)data_ov084_02130cd0.filePtr, 0x40000000, 0x1000, 0);
        mDeathState = daKrb_c::DEATH_FIRE;
    } else {
        if (!IsActorID(this, ACTOR_KURIBO_L)) {
            if (flags & HIT_UNLISTED_20000) {
                mDeathState = daKrb_c::DEATH_HIT_20000;
                mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce0.filePtr, 0x40000000, 0x1000, 0);
                mdCcAc_c.flags |= 1;
            } else if (flags & (HIT_EGG | HIT_DIVE)) {
                mDeathState = daKrb_c::DEATH_DIVE_OR_EGG;
                mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce0.filePtr, 0x40000000, 0x1000, 0);
            } else if (flags & HIT_EXPLOSION) {
                mDeathState = daKrb_c::DEATH_EXPLOSION;
                mAnmModel.SetAnim((BCA_File *)data_ov084_02130cd0.filePtr, 0x40000000, 0x1000, 0);
            } else if (flags & (HIT_KICK | HIT_BREAKDANCE | HIT_SLIDE_KICK)) {
                turnAround = 1;
                mAnmModel.SetAnim((BCA_File *)data_ov084_02130cd0.filePtr, 0x40000000, 0x1000, 0);
                mDeathState = daKrb_c::DEATH_KICKED;
            } else if (flags & HIT_PUNCH) {
                mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce0.filePtr, 0x40000000, 0x1000, 0);
                mDeathState = daKrb_c::DEATH_PUNCHED;
                turnAround = 1;
            } else if (!(flags & HIT_YOSHI_TONGUE)) {
                if (IsPlayerActor(other)) {
                    if (other->mIsMetal != 0) {
                        capReleaseOnKill.x = 0;
                        capReleaseOnKill.y = 0x6c000;
                        capReleaseOnKill.z = 0; /* 108.0 */
                        ReleaseCap(capReleaseOnKill);
                        killDirPlayer.x = 0x2000;
                        killDirPlayer.y = 0;
                        killDirPlayer.z = 0;
                        KillByInvincibleChar(killDirPlayer, *other, 0x41000); /* 65.0 */
                        return;
                    }
                    {
                        const Vector3 &p = other->mPos;
                        playerPos = p;
                    }
                    if (other->IsOnShell() != 0) {
                        mDeathState = daKrb_c::DEATH_DIVE_OR_EGG;
                        turnAround = 1;
                        mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce0.filePtr, 0x40000000,
                                          0x1000, 0);
                    } else if (JumpedOnByPlayer(mdCcAc_c, *other) != 0) {
                        other->Bounce(0x28000); /* 40.0 */
                        func_02012694(SND_STOMPED, &mCamSpacePos);
                        mDeathState = daKrb_c::DEATH_STOMPED;
                        mScale.x = FX32_ONE;
                        mScale.y = FX32_ONE;
                        mScale.z = FX32_ONE;
                    } else {
                        if (other->mIsVanish != 0)
                            return;
                        if (mState == daKrb_c::STATE_WALK) {
                            if (mGoombaType == daKrb_c::GOOMBA_SMALL) {
                                SmallPoofDust();
                                hurtOriginFirstHit = mPos;
                                other->Hurt(hurtOriginFirstHit, 0, hurtKnockback, 1, 0, 1);
                                RemoveSelf();
                                func_02012694(SND_DEATH_SMALL, &mCamSpacePos);
                                return;
                            }
                            if ((mdCcAc_c.hitFlags & HIT_PLAYER) == 0)
                                return;
                            hurtOriginRepeat = mPos;
                            other->Hurt(hurtOriginRepeat, data_ov084_02130204[mGoombaType],
                                        hurtKnockback, 1, 0, 1);
                            mState = daKrb_c::STATE_ATTACKED_MARIO;
                            return;
                        }
                    }
                }
            }
        } else {
            if (IsPlayerActor(other)) {
                {
                    const Vector3 &p = other->mPos;
                    playerPosJump = p;
                }
                if (JumpedOnByPlayer(mdCcAc_c, *other) != 0) {
                    other->Bounce(0x28000); /* 40.0 */
                    func_02012694(SND_STOMPED, &mCamSpacePos);
                    mDeathState = daKrb_c::DEATH_STOMPED;
                    mScale.x = FX32_ONE;
                    mScale.y = FX32_ONE;
                    mScale.z = FX32_ONE;
                } else {
                    if (other->mIsVanish != 0)
                        return;
                    if (mState == daKrb_c::STATE_WALK) {
                        mState = daKrb_c::STATE_ATTACKED_MARIO;
                        if ((mdCcAc_c.hitFlags & HIT_PLAYER) == 0)
                            return;
                        hurtOriginJumped = mPos;
                        other->Hurt(hurtOriginJumped, data_ov084_02130204[mGoombaType],
                                    hurtKnockback, 1, 0, 1);
                        return;
                    }
                }
            }
        }
    }

    if (mDeathState != 0) {
        capReleaseOnExit.x = 0;
        capReleaseOnExit.y = 0x6c000;
        capReleaseOnExit.z = 0; /* 108.0 */
        ReleaseCap(capReleaseOnExit);
    }
    KillByAttack((dActor_c *)other, &mWithMeshClsn);
    if (turnAround != 0) {
        mAngle.y = (mPrevAngle.y + 0x8000); /* half a turn */
    }
    if (mGoombaType != daKrb_c::GOOMBA_KING_MINION)
        return;
    if ((mLastHitFlags & HIT_PUNCH) ||
        (mLastHitFlags & (HIT_KICK | HIT_BREAKDANCE | HIT_SLIDE_KICK))) {
        mHorzSpeed += (other)->mHorzSpeed;
    }
    if (mLastHitFlags & HIT_DIVE) {
        mHorzSpeed += 0x20;
    }
}

/* Per-frame model, shadow and cap placement. Rebuilds the model matrix from
 * mAngle.y, sets its translation to the position >> 3, and draws the drop
 * shadow (radius 80 * mScale.x; depth 30.0 on the ground, 150.0 in the air)
 * unless the actor is in Yoshi's mouth (mFlags 0x40000). Then the cap is attached (UpdateCapPos)
 * 108.0 above the origin and 10 * (sin, cos) of mAngle.y out along the
 * heading. UpdateCapPos is handed temporary copies of both, the rotation
 * built first (mwcc evaluates arguments right to left); passing pos and
 * rotation themselves drops the copies. */
void daKrb_c::UpdatePose() {
    Vector3_16 rotation;
    Vector3 pos;

    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngle.y);
    mAnmModel.mat4x3.t.x = mPos.x >> 3;
    mAnmModel.mat4x3.t.y = mPos.y >> 3;
    mAnmModel.mat4x3.t.z = mPos.z >> 3;
    rotation = mAngle;
    if (!IsInYoshiMouth(this)) {
        if (mWithMeshClsn.IsOnGround())
            DropShadowRadHeight(mShadowModel, mAnmModel.mat4x3, mScale.x * 0x50, 0x1e000, 0xf);
        else
            DropShadowRadHeight(mShadowModel, mAnmModel.mat4x3, mScale.x * 0x50, 0x96000, 0xf);
    }
    pos.x = 0;
    pos.z = 0;
    pos.y = 0x6c000;     /* 108.0 */
    pos.x += FX_SinIdx((u16)mAngle.y >> 4) * 10;
    pos.z += FX_CosIdx((u16)mAngle.y >> 4) * 10;
    UpdateCapPos(Vector3(pos), Vector3_16(rotation));
}

/* STATE_PAUSE: stand still (horizontal speed 0) until mWanderRerollTimer runs
 * out, then go back to STATE_WALK with the walk animation. */
void daKrb_c::ActPause() {
    mHorzSpeed = 0;
    /* Header spells this s16; the body loads it unsigned, and the two
       address forms stop mwcc from CSEing the halfword. */
    if (*(unsigned short *)&mWanderRerollTimer) {
        *(unsigned short *)(&mWanderRerollTimer) -= 1;
    }
    if (*(unsigned short *)&mWanderRerollTimer)
        return;
    mState = daKrb_c::STATE_WALK;
    mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce8.filePtr, 0, 0x1000, 0);
}

/* STATE_TUMBLE: the Goomba has been thrown clear (EnterTumble),
 * after its cap came off or when a Yoshi-held Goomba landed, and bounces while
 * mBounceCountdown runs.
 *
 * Countdown 0: the material animation advances and, once the body animation
 * has finished, the Goomba recovers: mFlags is set to mSavedParam (the
 * original param1 word; why is unrecovered),
 * mState returns to STATE_WALK with the walk animation and walk speed, and
 * the 0x20000 bit of the dCcAc_c flags is cleared.
 * Countdown 1..60: it counts down one a frame; at 0 the Goomba is lost
 * (reward, coin, removal, cap released, back to mHomePos).
 * On each landing: a countdown above 60 is clamped to 30; the vertical speed
 * becomes (countdown << 10) - mVertAccel when the old speed is no more than
 * countdown * 0x500, else 80 percent of the old one reversed; the horizontal
 * speed is halved but kept at 5.0 or more in magnitude; and a sound plays,
 * SND_TUMBLE_BOUNCE_FIRST when mBounceCount is 0, SND_TUMBLE_BOUNCE_LATER for 1
 * and 2. mBounceCount counts the landings. Staying on the ground clamps the
 * countdown the same way.
 * Last, mEatenByYoshi is 1 around a SpawnParticlesIfHitOtherObj call (which
 * reports hits differently while it is set, per dEnemyBase_c.h); a hit sets
 * DEATH_HIT_20000 and bit 0 of the dCcAc_c flags, as in UpdateYoshiEatAndHit. */
void daKrb_c::ActTumble() {
    Vector3 v;
    u16 h = mBounceCountdown;

    if (h == 0) {
        (&mMaterialChanger)->Advance();
        if ((&mAnmModel)->Finished() == 0)
            return;
        mFlags = mSavedParam;
        mState = daKrb_c::STATE_WALK;
        mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce8.filePtr, 0, 0x1000, 0);
        mTargetHorzSpeed = data_ov084_02130228[mGoombaType];
        mWithMeshClsn.ClearLimMovFlag();
        {
            mPrevAngle.y = mAngle.y;
            mdCcAc_c.flags &= ~0x20000;
        }
        dExtAnmMaterial_c::Prepare(*(BMD_File *)(data_ov084_02130cf8.filePtr), data_ov084_0213089c);
        mMaterialChanger.SetFile(data_ov084_0213089c, 0x40000000, 0x1000, 0);
        mMaterialChanger.currFrame = 0;
        return;
    }
    if (h <= 60) {
        mBounceCountdown -= 1;
        if (mBounceCountdown == 0) {
            PayOutReward();
            SpawnCoin();
            RemoveSelf();
            v.x = 0;
            v.y = 0x6c000;     /* 108.0 */
            v.z = 0;
            ReleaseCap(v);
            mPos.x = mHomePos.x;
            mPos.y = mHomePos.y;
            mPos.z = mHomePos.z;
            RespawnIfHasCap();
        }
    }
    if (mWithMeshClsn.JustHitGround() != 0) {
        int a8;
        int cnt;
        if (mBounceCountdown > 60)
            mBounceCountdown = 30;
        a8 = mSpeed.y;
        cnt = mBounceCountdown;
        {
            int aa = a8 < 0 ? -a8 : a8;
            if (aa <= cnt * 0x500) {
                mSpeed.y = (cnt << 0xa) - mVertAccel;
            } else {
                mSpeed.y = FX_Div((a8 * -0x50) / 100, mFloorNormal.y);
            }
        }
        {
            mHorzSpeed >>= 1;
        }
        {
            int v98 = mHorzSpeed;
            int av = v98 < 0 ? -v98 : v98;
            if (av < 0x5000) {
                if (v98 < 0)
                    mHorzSpeed = -0x5000;
                else
                    mHorzSpeed = 0x5000;
            }
        }

        if (mBounceCount == 0) {
            func_02012694(SND_TUMBLE_BOUNCE_FIRST, &mCamSpacePos);
        } else if (mBounceCount <= 2) {
            func_02012694(SND_TUMBLE_BOUNCE_LATER, &mCamSpacePos);
        }
        mBounceCount += 1;
    } else {
        if (mWithMeshClsn.IsOnGround() != 0) {
            mWithMeshClsn.ClearLimMovFlag();
            mHorzSpeed = 0;
            mSpeed.y = 0;
            if (mBounceCountdown > 60)
                mBounceCountdown = 30;
        }
    }
    mEatenByYoshi = 1;
    if (SpawnParticlesIfHitOtherObj(mdCcAc_c) != 0) {
        void *a = dActor_c::FindWithID(mdCcAc_c.otherOwner);
        mDeathState = daKrb_c::DEATH_HIT_20000;
        KillByAttack((dActor_c *)a, &mWithMeshClsn);
        mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce0.filePtr, 0x40000000, 0x1000, 0);
        mdCcAc_c.flags |= 1;
    }
    mEatenByYoshi = 0;
}

/* STATE_JUMP: after a hop begins. On the frame it touches down a large
 * Goomba kicks up HugeLandingDust and a normal one LandingDust. Once on the
 * ground it returns to STATE_WALK and clears dCcAc_c flag bit 0x4 (set by
 * BeginJump); in the air it turns mPrevAngle.y toward mTargetAngleY
 * by at most 0x800 (11.25 degrees) a frame.
 * N64: goomba_act_jump -- back to GOOMBA_ACT_WALK on the ground, otherwise
 * cur_obj_rotate_yaw_toward(oGoombaTargetYaw, 0x800). The landing dust is
 * DS-only. */
void daKrb_c::ActJump() {
    if (mWithMeshClsn.JustHitGround()) {
        switch (mGoombaType) {
        case GOOMBA_LARGE: HugeLandingDust(true); break;
        case GOOMBA_NORMAL: LandingDust(true); break;
        }
    }
    if (mWithMeshClsn.IsOnGround()) {
        mState = STATE_WALK;
        mdCcAc_c.flags &= ~4;
    } else {
        ApproachLinear(mPrevAngle.y, mTargetAngleY, 0x800);
    }
    mAngle.y = mPrevAngle.y;
}

/* STATE_ATTACKED_MARIO: begin a hop (BeginJump, which enters
 * STATE_JUMP). A KURIBO_L then scales its vertical speed by
 * 0x1800 / 0x1000 = 1.5. The target heading becomes the spawn heading
 * (mInitAngleY), MOVE_AVOIDING is cleared, and mAngle.y follows mPrevAngle.y.
 * N64: goomba_act_attacked_mario's non-tiny branch -- goomba_begin_jump, a new
 * oGoombaTargetYaw, oGoombaTurningAwayFromWall = FALSE. Its tiny branch (the
 * Goomba dies) is in ReactToCollision here, where KURIBO_S removes itself
 * instead of entering this state; KURIBO_L entering it from a player hit is
 * N64 huge_goomba_weakly_attacked. */
void daKrb_c::ActAttackedMario() {
    int large;
    BeginJump();
    large = (actorID == ACTOR_KURIBO_L);
    if (large != 0) {
        mSpeed.y = FX_MUL(mSpeed.y, 0x1800);
    }
    mTargetAngleY = mInitAngleY;
    mMoveFlags &= ~daKrb_c::MOVE_AVOIDING;
    mAngle.y = mPrevAngle.y;
}

/* STATE_WALK for an ordinary Goomba (not a king minion): wander and chase.
 *
 * ChooseTarget picks the target (mDistToPlayer and mInitAngleY), the
 * horizontal speed eases toward mTargetHorzSpeed (0.3125 a frame), and at the
 * end mPrevAngle.y is turned toward mTargetAngleY by `step` (0x200 = 2.8
 * degrees a frame unless a branch below changes it).
 *  - MOVE_AVOIDING set: turn toward mInitAngleY; the flag is cleared when
 *    ApproachLinear returns nonzero. Nothing else happens this frame.
 *  - mUnstickTimer running: a Goomba with a cap (mCapId < 6) hops if it is at
 *    or below walk speed, otherwise plays the faster animation, targets the
 *    run speed and mInitAngleY, step 0x800 (11.25 degrees). One without a cap
 *    targets the walk speed and the heading toward mHomePos, step 0x400.
 *  - No target (mDistToPlayer 0x61a8000 = 25000.0 or more): the heading is
 *    mInitAngleY and mHeadingHoldTimer is set to 25. AngleAwayFromWallOrCliff
 *    then sets MOVE_AVOIDING from its result.
 *  - Not avoiding: with a target inside mChaseRadius (or a capped Goomba more
 *    than 1000.0 from home) it runs: hop if at or below walk speed, else the
 *    faster animation, run speed, heading mInitAngleY (a capless Goomba chases
 *    the player; a capped one flees from the player or runs home; step 0x600
 *    for a capped Goomba whose unstick timer is 0). Otherwise it walks: mHeadingHoldTimer
 *    counts down, then a random heading is picked, three times in four as an
 *    offset from the current heading (held 100 frames) and otherwise with a hop.
 *  - A capless Goomba (mCapId >= 6) stuck for more than 30 frames starts
 *    mUnstickTimer at the value of mStuckTimer. */
void daKrb_c::UpdateWalk() {
    short step = 0x200;
    ChooseTarget(0x3e8000);
    ApproachLinear(mHorzSpeed, mTargetHorzSpeed, 0x500);
    if (((Flags *)&mMoveFlags)->b0) {
        if (ApproachLinear(mPrevAngle.y, mInitAngleY, step)) {
            mMoveFlags &= ~daKrb_c::MOVE_AVOIDING;
            return;
        }
        {
            unsigned char *p = (&mMoveFlags);
            *p = (*p & ~daKrb_c::MOVE_AVOIDING) | daKrb_c::MOVE_AVOIDING;
        }
        return;
    }
    if (mUnstickTimer != 0) {
        if (mCapId < 6) {
            if (mTargetHorzSpeed <= data_ov084_02130228[mGoombaType]) {
                BeginJump();
            } else {
                mAnmModel.SetAnim((BCA_File *)data_ov084_02130cf0.filePtr, 0, 0x1000, 0);
            }
            step = 0x800;
            mTargetHorzSpeed = data_ov084_02130268[mGoombaType];
            mTargetAngleY = mInitAngleY;
        } else {
            mTargetHorzSpeed = data_ov084_02130228[mGoombaType];
            mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce8.filePtr, 0, 0x1000, 0);
            mTargetAngleY = Vec3_HorzAngle(&mPos, &mHomePos);
            step = 0x400;
        }
        ApproachLinear(mPrevAngle.y, mTargetAngleY, step);
        return;
    }
    if (mDistToPlayer >= 0x61a8000) {
        mTargetAngleY = mInitAngleY;
        mHeadingHoldTimer = 25;
    }
    {
        int bit = AngleAwayFromWallOrCliff(mWithMeshClsn, mTargetAngleY);
        unsigned char *p = (&mMoveFlags);
        bit &= 1;
        *p = (*p & ~daKrb_c::MOVE_AVOIDING) | bit;
    }
    if (!((Flags *)&mMoveFlags)->b0) {
        if (mDistToPlayer < mChaseRadius ||
            (mCapId < 6 && Vec3_Dist(&mPos, &mHomePos) > 0x3e8000)) {
            if (mTargetHorzSpeed <= data_ov084_02130228[mGoombaType]) {
                BeginJump();
            } else {
                mAnmModel.SetAnim((BCA_File *)data_ov084_02130cf0.filePtr, 0, 0x1000, 0);
            }
            if (mCapId >= 6 || mUnstickTimer != 0) {
                mTargetAngleY = mInitAngleY;
            } else {
                step = 0x600;
                mTargetAngleY = mInitAngleY;
            }
            mTargetHorzSpeed = data_ov084_02130268[mGoombaType];
        } else {
            mTargetHorzSpeed = data_ov084_02130228[mGoombaType];
            mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce8.filePtr, 0, 0x1000, 0);

            if (mHeadingHoldTimer != 0) {
                mHeadingHoldTimer--;
            } else if (((unsigned)RandomIntInternal(&data_0209e650.value) >> 16) & 3) {
                mTargetAngleY =
                    mPrevAngle.y + (s16)((unsigned)RandomIntInternal(&data_0209e650.value) >> 16);
                mHeadingHoldTimer = 100;
            } else {
                mTargetAngleY = ((unsigned)RandomIntInternal(&data_0209e650.value) >> 16);
                BeginJump();
            }
        }
    }
    if (mCapId >= 6) {
        if (mStuckTimer > 30)
            mUnstickTimer = mStuckTimer;
    }
    ApproachLinear(mPrevAngle.y, mTargetAngleY, step);
}

/* STATE_WALK for a king minion (mGoombaType 3): follow the Goomba King
 * (mTargetUniqueID). If the king is gone it becomes an ordinary KURIBO
 * (mGoombaType = GOOMBA_NORMAL).
 *
 * The standing animation (data_ov084_02130cc8) plays while mTargetHorzSpeed
 * is 0, the walk animation otherwise. func_ov074_0212087c turns the king and
 * mMinionIndex into the spot (targetPos) this minion should stand at. When
 * ApproachAngle (mPrevAngle.y toward mInitAngleY) returns 0 and the Goomba is
 * on the ground:
 *  - If the king's mState is 4 with mWalkSpeed 0, or the mFreeRoamTimer countdown
 *    (DecIfAbove0_Byte) is nonzero, or the spot is more than 1000.0 away, the
 *    minion acts for itself: if the player is within 1000.0 and the angle
 *    difference to the player is below 0x3000 (67.5 degrees) it faces the
 *    player; if its target speed is walk speed it switches to run speed, and
 *    if it is anything else (for example 0 while standing) it hops toward the
 *    player (launch speed data_ov084_02130248[type], horizontal speed 0) and
 *    takes run speed as the target. Otherwise it stands still (target speed
 *    0) and counts mWanderRerollTimer down; when that reaches 0 it reloads it
 *    (30..61 frames) and picks a random heading.
 *  - Otherwise it walks toward the spot at a speed derived from the king's
 *    mGoombaTargetSpeed: a quarter, a half or all of it when the spot is under
 *    50.0, 100.0 or 150.0 away, and the run speed plus all of it beyond that.
 * A minion that falls more than 1000.0 below mHomePos is removed, and while
 * the king is in state 4 with mWalkSpeed 0 mFreeRoamTimer is reloaded with 30. */
void daKrb_c::UpdateMinionWalk() {
    daKuriKing_c *king;
    s32 dist;
    s32 flag;
    s16 ang;
    u32 rnd;
    s32 lvl;
    u32 id;

    id = mTargetUniqueID;
    if (id == 0) {
        mGoombaType = daKrb_c::GOOMBA_NORMAL;
        return;
    }
    king = (daKuriKing_c *)dActor_c::FindWithID(id);
    if (king == 0) {
        mGoombaType = daKrb_c::GOOMBA_NORMAL;
        return;
    }

    if (mTargetHorzSpeed == 0) {
        mAnmModel.SetAnim((BCA_File *)data_ov084_02130cc8.filePtr, 0, 0x1000, 0);
    } else {
        mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce8.filePtr, 0, 0x1000, 0);
    }

    Vector3 targetPos = king->func_ov074_0212087c(mMinionIndex);

    if (ApproachAngle(&mPrevAngle.y, mInitAngleY, 4, 0x1000, 0x400) == 0 &&
        mWithMeshClsn.IsOnGround() != 0) {
        Vec3_HorzDist(&mPos, &mHomePos);
        dist = Vec3_HorzDist(&mPos, &targetPos);

        if ((king->mState == 4 && king->mWalkSpeed == 0) ||
            DecIfAbove0_Byte(&mFreeRoamTimer) != 0 || dist > 0x3e8000) {
            ang = HorzAngleToCPlayer();
            flag = 1;
            if (DistToCPlayer() < 0x3e8000 && AngleDiff(ang, mAngle.y) < 0x3000) {
                mInitAngleY = HorzAngleToCPlayer();
                mTargetAngleY = mInitAngleY;
                lvl = mGoombaType;
                if (mTargetHorzSpeed == data_ov084_02130228[lvl]) {
                    mTargetHorzSpeed = data_ov084_02130268[lvl];
                }
                lvl = mGoombaType;
                if (mTargetHorzSpeed != data_ov084_02130268[lvl]) {
                    flag = 0;
                    mSpeed.y = data_ov084_02130248[lvl];
                    mHorzSpeed = 0;
                    mTargetHorzSpeed = data_ov084_02130268[mGoombaType];
                }
                if (mWithMeshClsn.JustHitGround() != 0) {
                    LandingDust(1);
                }
            } else {
                if (DecIfAbove0_Short(reinterpret_cast<unsigned short *>(&mWanderRerollTimer)) ==
                    0) {
                    rnd = RandomIntInternal(&data_0209e650.value);
                    mWanderRerollTimer = ((rnd >> 0x1b) + 30);
                    mInitAngleY = (rnd >> 0x10);
                }
                mTargetHorzSpeed = 0;
            }
            if (flag != 0) {
                ApproachLinear(mHorzSpeed, mTargetHorzSpeed, 0x500);
            }
        } else {
            mInitAngleY = Vec3_HorzAngle(&mPos, &targetPos);
            if (Vec3_HorzDist(&mPos, &targetPos) < 0x32000) {
                mTargetHorzSpeed = king->mGoombaTargetSpeed >> 2;
            } else if (Vec3_HorzDist(&mPos, &targetPos) < 0x64000) {
                mTargetHorzSpeed = king->mGoombaTargetSpeed >> 1;
            } else if (Vec3_HorzDist(&mPos, &targetPos) < 0x96000) {
                mTargetHorzSpeed = king->mGoombaTargetSpeed;
            } else {
                s32 idx = mGoombaType;
                s32 v = data_ov084_02130268[idx];
                mTargetHorzSpeed = v + king->mGoombaTargetSpeed;
            }
            Math_Function_0203b14c(&mHorzSpeed, mTargetHorzSpeed, 0x800, 0x10000, 4);
        }
    }

    if (mPos.y < mHomePos.y - 0x3e8000) { /* 1000.0 below home */
        MarkForDestruction();
    }

    if (king->mState != 4)
        return;
    if (king->mWalkSpeed == 0)
        mFreeRoamTimer = 30;
}

/* STATE_WALK handler: the king's minions follow the king, everything else
 * wanders / chases. mAngle.y then follows mPrevAngle.y.
 * N64: goomba_act_walk (UpdateWalk holds its body; its target picker
 * ChooseTarget takes treat_far_home_as_mario's 1000.0). The minion
 * branch is DS-only. */
void daKrb_c::ActWalk() {
    if (mGoombaType == daKrb_c::GOOMBA_KING_MINION)
        UpdateMinionWalk();
    else
        UpdateWalk();
    mAngle.y = mPrevAngle.y;
}

/* Returns an fx32 value by size: 65.0 for KURIBO, 150.0 for KURIBO_L and
 * 20.0 for KURIBO_S. */
int daKrb_c::OnAimedAtWithEgg()
{
    if (IsActorID(this, ACTOR_KURIBO))
        return 0x41000;
    if (IsActorID(this, ACTOR_KURIBO_L))
        return 0x96000;
    return 0x14000;
}

/* Yoshi turns the Goomba into an egg. A Goomba with a cap (mCapId < 6) or
 * REWARD_SILVER_STAR_IF_CURRENT is first put back at mHomePos. If
 * OnYoshiTryEat() is 6 (KURIBO): a daPly_c that IsCollectingCap gets the coin
 * and the reward helper; otherwise the Goomba's coin and star state go to
 * RegisterEggCoinCount (b5 = it carries a plain coin, b4 = it carried a
 * silver star). If it is 4 (KURIBO_S) only the coin is given. Finally the
 * Goomba is removed (RemoveSelf). */
void daKrb_c::OnTurnIntoEgg(daPly_c &playerRef) {
    daPly_c *player = &playerRef;
    int b5;
    bool b4;

    if ((mCapId & 0xf) < 6 || mRewardType == REWARD_SILVER_STAR_IF_CURRENT) {
        mPos.x = mHomePos.x;
        mPos.y = mHomePos.y;
        mPos.z = mHomePos.z;
        RespawnIfHasCap();
    }

    if (OnYoshiTryEat() == 6) {
        if (player->IsCollectingCap()) {
            if (mCoinType == COIN_PLAIN)
                GivePlayerCoins(*player, 1, 0);
            PayOutReward();
        } else {
            b5 = 0;
            b4 = b5;
            if (mCoinType == COIN_PLAIN)
                b5 = 1;
            if (mRewardType == REWARD_SILVER_STAR) {
                UntrackStar(mStarTracked);
                b4 = 1;
                dActor_c::Spawn(ACTOR_STARBASE, 0x50, mHomePos, 0, mAreaId, -1);
                param1 = GetParam1() & 0xff0f;
            } else if (mRewardType == REWARD_SILVER_STAR_IF_CURRENT) {
                if (mStarID == gVsStarSpawnOrder[gNumVsStarsCollected]) {
                    UntrackStar(mStarTracked);
                    mRewardType = REWARD_SPENT;
                    b4 = 1;
                }
            }
            player->RegisterEggCoinCount(b5, b4, 0);
        }
    } else if (OnYoshiTryEat() == 4) {
        if (mCoinType == COIN_PLAIN)
            GivePlayerCoins(*player, 1, 0);
    }

    RemoveSelf();
}

/* Release the shared files: the blue-coin model for a KURIBO_L, the BMD and
 * the seven BCA handles, the silver-star model when mRewardType is 1 or 2,
 * and the cap model. A king minion also decrements the king's mSpawnedCount. */
int daKrb_c::CleanupResources() {
    int i;
    if (mGoombaType == GOOMBA_LARGE)
        UnloadBlueCoinModel();
    data_ov084_02130cf8.Release();
    for (i = 0; i < 7; ++i)
        sKuriboAnimationFiles[i]->Release();
    if ((unsigned char)(mRewardType + 0xff) <= 1)
        UnloadSilverStarAndNumber();
    UnloadCapModel();
    if (mGoombaType == GOOMBA_KING_MINION) {
        unsigned int id = mTargetUniqueID;
        if (id != 0) {
            daKuriKing_c *a = (daKuriKing_c *)dActor_c::FindWithID(id);
            if (a != 0) {
                unsigned char *p = (&a->mSpawnedCount);
                *p = *p - 1;
            }
        }
    }
    return 1;
}

void daKrb_c::OnPendingDestroy()
{
}

/* Draw the body and the cap. Nothing is drawn while mFlags bit 0x40000 (the
 * Yoshi-mouth state in dActor_c.h) is set or the Goomba is dormant (mIsDormant). While mDeathState is
 * DEATH_STOMPED the scale is multiplied by data_ov084_02130258[type] for
 * the draw and restored afterwards. */
int daKrb_c::Render()
{
    int locked = (mFlags & 0x40000) != 0;
    if (locked || mIsDormant != 0) return 1;

    Vector3 backup = mScale;

    if (mDeathState == DEATH_STOMPED) {
        mScale.x = FX_MUL(mScale.x, data_ov084_02130258[mGoombaType]);
        mScale.y = FX_MUL(mScale.y, data_ov084_02130258[mGoombaType]);
        mScale.z = FX_MUL(mScale.z, data_ov084_02130258[mGoombaType]);
    }

    mAnmModel.Render(&mScale);

    mScale = backup;
    mMaterialChanger.Update(mAnmModel.data);
    RenderCapModel(0);
    return 1;
}

/* Per-frame update; always returns 1.
 *
 * In order: func_ov084_02129864 (claim the star slot) and UpdateChaseRadius
 * (chase radius and unstick timer). GetCapState 0 ends the frame; 1 sets
 * mFlags bit 0x10000000 and poofs. A Goomba that is not a king minion,
 * tumbling, held by Yoshi or dying, and more than 1500.0 from the player, is
 * handed to Unk_02005d94 and the frame ends. While mDeathState != 0 the death
 * runs (UpdateKillByInvincibleChar, then UpdateDeathState, which ends it)
 * and the frame ends. UpdateYoshiEatAndHit (Yoshi / hit front end) may end the
 * frame. The animation speed is 1.0 in STATE_TUMBLE and STATE_PAUSE and for a
 * standing king minion, else horizontal speed / (2 * scale) capped at 3.0.
 * Footstep sound and animation advance run except when airborne. The state
 * handler is taken from data_ov084_02130d74[mState]; mStateTimer counts up and
 * is zeroed when the state changes. Then the hit reaction
 * (ReactToCollision), UpdatePos, the ledge check (a step that would carry
 * it off a ledge is undone by restoring mSafePos), UpdateWMClsn (mode 0 for
 * KURIBO_S; mode 2 for the others, or 3 when gCurrentSublevel is 6 or 0x1b, its
 * target speed is the walk speed and mDeathState is not DEATH_HIT_20000), the ground
 * check, the cylinder, model placement and the water check. In STATE_WALK
 * (unless mFlags bit 3, "off screen" in dActor_c.h, is set) a Goomba that has stayed within 10.0 units of
 * mStuckCheckPos counts up mStuckTimer: at 30 a capped one hops and starts
 * mUnstickTimer at 90, and at 300 with no unstick timer running it is lost
 * (reward, coin, removal, back to mHomePos). */
int daKrb_c::Behavior() {
    Vector3 v1;
    Vector3 v2;
    int r;
    int st;

    TrackSilverStar();
    UpdateChaseRadius();
    r = GetCapState();
    if (r == 0)
        return 1;
    if (r == 1) {
        mFlags |= 0x10000000;
        PoofDust();
    }
    if (mGoombaType != GOOMBA_KING_MINION && mState != STATE_TUMBLE && mEatenByYoshi == 0 &&
        mDeathState == DEATH_NONE && IsTooFarAwayFromPlayer(0x5dc000) != 0) /* 1500.0 */
    {
        Unk_02005d94();
        return 1;
    }

    if (mDeathState != DEATH_NONE) {
        r = UpdateKillByInvincibleChar(mWithMeshClsn, mAnmModel, 3);
        if (r != 0) {
            if (r == 2) {
                RemoveSelf();
                v1.x = 0;
                v1.y = 0x6c000; /* 108.0 */
                v1.z = 0;
                ReleaseCap(v1);
                mPos.x = mHomePos.x;
                mPos.y = mHomePos.y;
                mPos.z = mHomePos.z;
                mAngle.x = 0;
                mAngle.y = 0;
                mAngle.z = 0;
                RespawnIfHasCap();
                PayOutReward();
            }
            return 1;
        }
        if (UpdateDeathState() == 0)
            UpdatePose();
        return 1;
    }

    if (UpdateYoshiEatAndHit() != 0)
        return 1;

    if (mState >= STATE_TUMBLE || (mGoombaType == GOOMBA_KING_MINION &&
                                   (int)mAnmModel.file == (int)data_ov084_02130cc8.filePtr)) {
        mAnmModel.speed = 0x1000;
    } else {
        int v = FX_Div(mHorzSpeed, mScale.x * 2);
        if (v > 0x3000)
            v = 0x3000;
        mAnmModel.speed = v;
    }

    MakeVanishLuigiWork(mdCcAc_c);

    if (mState != STATE_JUMP) {
        PlayFootstepSound();
        (&mAnmModel)->Advance();
    }

    st = mState;
    (this->*data_ov084_02130d74[st])();

    {
        u16 *hp = (&mStateTimer);
        *hp += 1;
        if (st != mState)
            *hp = 0;
    }

    ReactToCollision();

    if (mCapId < 6)
        UpdatePos(0);
    else
        UpdatePos(&mdCcAc_c);

    if (mDeathState == DEATH_NONE && mState != STATE_JUMP && mState != STATE_TUMBLE) {
        /* Args: 50.0 reach, angle limit 0x1f49 (about 44 degrees), 0, 1, 50.0. */
        if (IsGoingOffCliff(mWithMeshClsn, 0x32000, 0x1f49, 0, 1, 0x32000) != 0) {
            mPos.x = mSafePos.x;
            mPos.y = mSafePos.y;
            mPos.z = mSafePos.z;
        } else {
            mSafePos.x = mPos.x;
            mSafePos.y = mPos.y;
            mSafePos.z = mPos.z;
        }
    }

    {
        int lvl = mGoombaType;
        if (lvl == GOOMBA_SMALL) {
            UpdateWMClsn(mWithMeshClsn, 0);
        } else if (gCurrentSublevel == 6 || gCurrentSublevel == 0x1b) {
            if (mTargetHorzSpeed == data_ov084_02130228[lvl] && mDeathState != DEATH_HIT_20000)
                UpdateWMClsn(mWithMeshClsn, 3);
            else
                UpdateWMClsn(mWithMeshClsn, 2);
        } else {
            UpdateWMClsn(mWithMeshClsn, 2);
        }
    }

    CheckGroundSurface();
    (&mdCcAc_c)->Clear();
    if (mDeathState == DEATH_NONE)
        (&mdCcAc_c)->Update();
    UpdatePose();
    CheckHazardBelow();

    if (mState == STATE_WALK && !IsOffScreen(this)) {
        if (Vec3_Dist(&mPos, &mStuckCheckPos) < 0xa000) { /* moved less than 10.0 */
            mStuckTimer += 1;
            if (mCapId < 6 && mStuckTimer == 30) {
                BeginJump();
                mUnstickTimer = 90;
            }
            if (mStuckTimer >= 300 && mUnstickTimer == 0) {
                PayOutReward();
                SpawnCoin();
                RemoveSelf();
                v2.x = 0;
                v2.y = 0x6c000; /* 108.0 */
                v2.z = 0;
                ReleaseCap(v2);
                mPos.x = mHomePos.x;
                mPos.y = mHomePos.y;
                mPos.z = mHomePos.z;
                RespawnIfHasCap();
                return 1;
            }
        } else {
            mStuckTimer = 0;
            mStuckCheckPos.x = mPos.x;
            mStuckCheckPos.y = mPos.y;
            mStuckCheckPos.z = mPos.z;
        }
    } else if (mUnstickTimer == 0) {
        mStuckTimer = 0;
    }
    return 1;
}

/* Load the files and set the Goomba up. Spawn parameter (param1) layout:
 * bits 0..3 the cap (AddCap), bits 4..7 mRewardType, bits 8..11 the
 * dCapEnemy_c byte at 0x112 (mHadBank1Cap), bits 12..15 mStarID. The size
 * class follows from the actor id (see GoombaType); a KURIBO spawned with
 * param1 0xeeee or 0xeeef is a Goomba King minion, with bit 1 of mFlags
 * cleared, and 0xeeee also drops no coin (mCoinType = COIN_NONE). The dCcAc_c
 * cylinder gets radius 60 * scale and height data_ov084_02130208[type]; a
 * KURIBO_L has bit 0x8000 cleared from its vulnFlags (the Yoshi-tongue bit in
 * dCc_c.h's best-effort table); dBgCh_Actr::Init gets 60 * scale for its two
 * fx32 size arguments. mSavedParam keeps the original param1 for the end of
 * STATE_TUMBLE. */
int daKrb_c::InitResources() {
    int i;

    mRewardType = (param1 >> 4) & 0xf;
    mStarTracked = -1;
    mHadBank1Cap = (param1 >> 8) & 0xf;
    mStarID = (param1 >> 0xc) & 0xf;

    if (mRewardType == REWARD_SILVER_STAR) {
        mStarTracked = TrackStar(mStarID, 1);
        LoadSilverStarAndNumber();
    } else if (mRewardType == REWARD_SILVER_STAR_IF_CURRENT) {
        LoadSilverStarAndNumber();
    }

    dExtSimpleModel_c::LoadFile(data_ov084_02130cf8);
    for (i = 0; i < 7; i++)
        dExtFrameCtrl_c::LoadFile(*sKuriboAnimationFiles[i]);

    AddCap((unsigned char)(param1 & 0xf));

    if ((mCapId & 0xf) < 6)
        param1 = GetParam1() & 0xf0ff;

    if (DestroyIfCapNotNeeded() == 0)
        return 0;

    if ((&mAnmModel)->SetFile((BMD_File *)(data_ov084_02130cf8.filePtr), 1, -1) == 0)
        return 0;

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    dExtAnmMaterial_c::Prepare(*(BMD_File *)data_ov084_02130cf8.filePtr, data_ov084_0213089c);
    mMaterialChanger.SetFile(data_ov084_0213089c, 0x40000000, 0x1000, 0);

    mCoinType = COIN_PLAIN;

    {
        if (IsActorID(this, ACTOR_KURIBO_S)) {
            mGoombaType = GOOMBA_SMALL;
        } else {
            if (IsActorID(this, ACTOR_KURIBO)) {
                if (param1 == 0xeeee || param1 == 0xeeef) {
                    mGoombaType = GOOMBA_KING_MINION;
                    mFlags &= ~2;
                    if (param1 == 0xeeee)
                        mCoinType = COIN_NONE;
                } else {
                    mGoombaType = GOOMBA_NORMAL;
                }
            } else {
                mGoombaType = GOOMBA_LARGE;
                LoadBlueCoinModel();
            }
        }
    }

    {
        int scale = data_ov084_02130258[mGoombaType];
        mScale.x = scale;
        mScale.y = scale;
        mScale.z = scale;
    }
    mdCcAc_c.Init(this, mScale.x * 0x3c, data_ov084_02130208[mGoombaType], 0x200000, 0xa6efe0);

    if (mGoombaType == GOOMBA_LARGE)
        mdCcAc_c.vulnFlags &= ~HIT_YOSHI_TONGUE;

    mWithMeshClsn.Init(this, mScale.x * 0x3c, mScale.x * 0x3c, 0, 0);
    mWithMeshClsn.StartDetectingWater();

    mMoveFlags = 0;
    mState = STATE_WALK;
    mDeathState = DEATH_NONE;
    mClosestPlayer = 0;
    mDistToPlayer = 0x7fffffff;
    mInitAngleY = mPrevAngle.y;
    mTargetHorzSpeed = data_ov084_02130228[mGoombaType];
    mHeadingHoldTimer = 0;
    mTargetUniqueID = 0;
    mWanderRerollTimer = 0;
    mStuckTimer = 0;
    mStuckCheckPos.x = mPos.x;
    mStuckCheckPos.y = mPos.y;
    mStuckCheckPos.z = mPos.z;
    mUnstickTimer = 0;
    UpdateChaseRadius();

    mHomePos.x = mPos.x;
    mHomePos.y = mPos.y;
    mHomePos.z = mPos.z;
    mVertAccel = data_ov084_02130238[mGoombaType];
    mTerminalVelocity = -0x32000; /* -50.0 */
    mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce8.filePtr, 0, 0x1000, 0);

    mBounceCount = 0;
    mSavedParam = param1;
    return 1;
}

/* Returns 6 for KURIBO, 4 for KURIBO_S and 0 for KURIBO_L; OnTurnIntoEgg
 * compares against 6 and 4. */
int daKrb_c::OnYoshiTryEat()
{
    if (IsActorID(this, ACTOR_KURIBO))
        return 0x6;
    if (IsActorID(this, ACTOR_KURIBO_S))
        return 0x4;
    return 0;
}

daKrb_c *daKrb_c_classInit_KURIBO_L() {
    return new daKrb_c();
}

daKrb_c *daKrb_c_classInit_KURIBO_S() {
    return new daKrb_c();
}

daKrb_c *daKrb_c_classInit_KURIBO() {
    return new daKrb_c();
}

SpawnInfo<daKrb_c> g_profile_KURIBO = {
    daKrb_c_classInit_KURIBO,
    0x00c8, /* KURIBO registry id 200 */
    0x0018,
    0x10000006,
    0x00032000,
    0x00046000,
    0x01000000,
    0x01000000,
};

SpawnInfo<daKrb_c> g_profile_KURIBO_S = {
    daKrb_c_classInit_KURIBO_S,
    0x00c9,
    0x0019,
    0x10000006,
    0x00032000,
    0x00046000,
    0x006a4000,
    0x00800000,
};

SpawnInfo<daKrb_c> g_profile_KURIBO_L = {
    daKrb_c_classInit_KURIBO_L,
    0x00ca,
    0x001a,
    0x10000006,
    0x00064000,
    0x000c8000,
    0x01000000,
    0x01000000,
};

/* The nine file-scope resource handles, in retail initializer order. Two
   model files (func_02017acc / func_02017ab4), then seven animation files
   (SharedFilePtr::Construct / SharedFilePtr_Destruct_Anim); the manifest
   aliases each wrapper ctor/dtor onto those ROM symbols. */
KrbModelFilePtr data_ov084_02130cf8(ASSET_KURIBO_MODEL_BMD);
KrbModelFilePtr gKuriboLowModelModelFile(ASSET_KURIBO_LOW_MODEL_BMD);
KrbAnimationFileHandle data_ov084_02130ce8(ASSET_KURIBO_WALK_BCA);
KrbAnimationFileHandle data_ov084_02130ce0(ASSET_KURIBO_ROLLING_BCA);
KrbAnimationFileHandle data_ov084_02130cd0(ASSET_KURIBO_STRETCH_BCA);
KrbAnimationFileHandle data_ov084_02130cc0(ASSET_KURIBO_UNBALANCE_BCA);
KrbAnimationFileHandle gKuriboRecoverAnimationFile(ASSET_KURIBO_RECOVER_BCA);
KrbAnimationFileHandle data_ov084_02130cc8(ASSET_KURIBO_WAIT_BCA);
KrbAnimationFileHandle data_ov084_02130cf0(ASSET_KURIBO_RUN_BCA);

/* One pointer-to-member per State, in state-index order. Each &daKrb_c::
   expression is an 8-byte {code address, 0} constant in .data, and
   __sinit_d_a_krb.cpp copies the five into this .bss table. */
StateHandler data_ov084_02130d74[5] = {
    &daKrb_c::ActWalk,          /* STATE_WALK */
    &daKrb_c::ActAttackedMario, /* STATE_ATTACKED_MARIO */
    &daKrb_c::ActJump,          /* STATE_JUMP */
    &daKrb_c::ActTumble,        /* STATE_TUMBLE */
    &daKrb_c::ActPause,         /* STATE_PAUSE */
};

SharedFilePtr *const sKuriboAnimationFiles[7] = {
    &data_ov084_02130ce8,         &data_ov084_02130ce0, &data_ov084_02130cd0, &data_ov084_02130cc0,
    &gKuriboRecoverAnimationFile, &data_ov084_02130cc8, &data_ov084_02130cf0,
};

/* The Goomba's material animation (BMA). The two files share the colour-key
 * debug string below; each carries its own key stream and material table. */
char data_ov084_02130880[12] = "kuribo_all";
extern const u8 data_ov084_02130174[4] = {0x1f, 0x10, 0x00, 0x00};
extern const u8 data_ov084_02130178[20] = {
    0x00, 0x03, 0x0c, 0x16, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
    0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x10, 0x00, 0x00,
};
extern const BMAMaterialRecord data_ov084_0213018c = {
    0xffff,
    0,
    (char *)data_ov084_02130880,
    {
        {1, 1, 0},
        {1, 0, 4},
        {1, 0, 4},
        {1, 0, 0x11},
        {1, 0, 0x11},
        {1, 0, 0x11},
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 4},
    },
};
extern const BMAMaterialRecord data_ov084_021301c8 = {
    0xffff,
    0,
    (char *)data_ov084_02130880,
    {
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 1},
        {1, 0, 1},
        {1, 0, 1},
        {1, 0, 2},
        {1, 0, 2},
        {1, 0, 2},
        {1, 0, 2},
        {1, 0, 2},
        {1, 0, 2},
        {1, 0, 0},
    },
};
BMA_File data_ov084_0213088c = {
    0x11, 0, (u8 *)data_ov084_02130178, 1, 0, (BMAMaterialRecord *)&data_ov084_0213018c,
};
BMA_File data_ov084_0213089c = {
    0x2, 0, (u8 *)data_ov084_02130174, 1, 0, (BMAMaterialRecord *)&data_ov084_021301c8,
};

/* Per-type tables: dive hit power, walk speed range, chase speed range,
 * jump speed, launch speed, and the two per-frame particle paths. */
extern const u8 data_ov084_02130204[4] = {0, 1, 2, 1};
extern const int data_ov084_02130208[4] = {0x28000, 0x64000, 0xe0000, 0x64000};
extern const int data_ov084_02130218[4] = {0x110, 0xd6, 0x111, 0xd6};
extern const int data_ov084_02130228[4] = {0xaaa, 0x2000, 0x4aaa, 0x6000};
extern const int data_ov084_02130238[4] = {-0xaaa, -0x2000, -0x4aaa, -0x2000};
extern const int data_ov084_02130248[4] = {0x8000, 0x10000, 0x20000, 0x10000};
extern const int data_ov084_02130258[4] = {0x555, 0x1000, 0x2555, 0x1000};
extern const int data_ov084_02130268[4] = {0xa800, 0x15000, 0x15000, 0x15000};
