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
 * - dBgCh_Actr::Init stays mangled. Fix12i is a plain s32, so the
 *   header method mangles as int.
 * - daPly_c::Hurt, daPly_c::Bounce and dActor_c::IsTooFarAwayFromPlayer
 *   are not declared on those classes.
 * - dBgCh_Actr::GetFloorResult has no header member.
 * - dExtFrameCtrl_c::Advance is called on the subobject at +0x3c0.
 *   mAnmModel.Advance() this-adjusts.
 * - func_ov084_021290d4 recomputes the chase radius at each store.
 *   A named temporary DIFFs, and spelling mUnstickTimer on both
 *   sides of the decrement folds the halfword.
 * - OnTurnIntoEgg, the reward helper and InitResources mask param1
 *   in place as `param1 = (int)param1 & mask`. Without the cast the
 *   read-modify-write CSEs the +8 address and DIFFs.
 * - mInitAngleY is the spawn heading, but func_ov084_02129cf4
 *   overwrites it with the chase heading. mWanderRerollTimer is s16
 *   in the header and this TU loads it unsigned.
 * - The flag test is lsl/lsrs. `&= ~n` on the u8 compiles to and;
 *   the ROM has bic.
 * - Several direct member spellings shrink the function and DIFF:
 *   func_ov084_0212a580 testing `(mFlags & 0x40000) == 0` in place of
 *   its inYoshiMouth int (3 words),
 *   Render without the `locked` materialization (3 words), Behavior
 *   with `(mFlags & 8) == 0`, `mStateTimer`/`mStuckTimer` named on
 *   both sides (3 words), 0212abd4 with `mHeadingHoldTimer` named on
 *   both sides of the decrement (2 words).
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

/* daStar_c::mMarkerID (daStar_c.h, 0x434), seen through a local view:
   including daStar_c.h renumbers this TU's @NNN labels. */
struct SilverStarView {
    u8 head[0x434];
    u32 mMarkerID;
};
typedef char SilverStarView_size_must_be_0x438[sizeof(SilverStarView) == 0x438 ? 1 : -1];

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

/* dCcAc_c hitFlags bits, with the names include/dCc_c.h gives its table. That
   table is a best-effort reading; only the egg and explosion bits are proven
   there. 0x20000 is not in it. */
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
int func_02037e20(int* p);
void func_02012694(unsigned int id, const Vector3 *pos);
int SurfaceInfo_TestFlag0x20(int *p);
extern u8 data_0209f208[];
extern u8 *data_0209f344;
/* Three-register reconstructed interface; death-state stores also use r3. */
extern "C" struct Aea30C;
void func_ov002_020aea30(Aea30C *self, dActor_c *actor, dBgCh_Actr *collision);
extern int *data_ov084_0213088c;
extern int data_ov084_02130248[];
extern u8 data_ov084_02130204[];
extern char data_ov084_0213089c;
extern int data_ov084_02130228[];
extern int data_ov084_02130268[];
void func_ov074_0212087c(Vector3 *out, void *player, u8 flag);
extern "C" int Math_Function_0203b14c(int *v, int target, int a, int b, int c);
extern s8 gCurrentSublevel;
void LoadSilverStarAndNumber(void);
/* Parameter names keep this TU's @NNN. */
extern "C" {
typedef void BgchInitAt_(void *self, void *a, Fix12i b, Fix12i cc, void *d, Fix12i e);
}
extern SharedFilePtr *const sKuriboAnimationFiles[7];
extern int data_ov084_02130258[];
extern int data_ov084_02130208[];
extern int data_ov084_02130238[];
extern int data_ov084_02130218[];

daKrb_c::~daKrb_c()
{
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

void daKrb_c::func_ov084_021290d4() {
    daKrb_c *c = this;
    int timerOff;
    daKrb_c *self2;
    int state;
    timerOff = 0x458;
    if (c->mUnstickTimer != 0)
    {
        /* Comparison stays in the `if`: assigning `== 1` to an int is a bool
           in C++ and materialises moveq/movne. */
        state = c->mState;
        if (state == daKrb_c::STATE_ATTACKED_MARIO) {
            c->mUnstickTimer = 0;
        } else {
            /* timerOff is the offset of mUnstickTimer (0x458). The plain
               `c->mUnstickTimer--` builds the same .text but drops nine
               @NNN numbers that daKrb_c.json pins, so this stays raw. */
            *((unsigned short *)(((int)c) + timerOff)) =
                (*((unsigned short *)(((int)c) + 0x458))) - 1;
        }
        return;
    }
    if ((self2 = c)->mCapId < 6)
    {
        *chaseRadius(self2) = 0x1f4000;     /* 500.0 */
        return;
    }
    unsigned short stuck = c->mStuckTimer;
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
 * off (func_ov084_02129a00) and, with a null `actor`, when a Yoshi-held Goomba
 * lands. `actor` is the eater
 * (mEatingPlayer) when there is one; the Goomba is turned toward it.
 *
 * Starts the 60-frame bounce countdown (mBounceCountdown) and the tumble
 * animation, gives it a vertical speed of (13.0 - mVertAccel) / mFloorNormalY
 * and a backward horizontal speed equal to the length of the horizontal part
 * of the velocity vector at 0xa4, and releases it from Yoshi. */
void daKrb_c::func_ov084_02129168(dActor_c *actor) {
    daKrb_c *goomba = this;
    goomba->mBounceCountdown = 0x3c;
    goomba->mVertSpeed = FX_Div(0xd000 - goomba->mVertAccel, goomba->mFloorNormalY);
    goomba->mHorzSpeed = Vec3_HorzLen((const Vector3 *)&goomba->mSpeedX) * -1;
    if (actor != 0)
        goomba->mPrevAngleY = Vec3_HorzAngle(&goomba->mPos, &actor->mPos);
    goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130cc0.filePtr, 0, 0x1000, 0);
    goomba->mState = daKrb_c::STATE_TUMBLE;
    goomba->mEatenByYoshi = 0;
    goomba->mWithMeshClsn.SetLimMovFlag();
    goomba->mWithMeshClsn.Unk_0203589c();
    goomba->mWithMeshClsn.ClearJustHitGroundFlag();
    goomba->mWithMeshClsn.ClearGroundFlag();
    func_02012694(SND_TUMBLE_BOUNCE_FIRST, &goomba->mCamSpacePos);
    goomba->mBounceCount = 0;
}

/* Water / toxic-floor check, run every frame while the Goomba is off the
 * ground. A probe is cast from 400.0 units above it that looks only for water
 * and toxic surfaces (ordinary ground is switched off). If it finds a surface
 * for which func_02037e20 (CLPS bit 0x2000000, see CLPS.h) is nonzero, and
 * the Goomba is below the height of the hit, the Goomba is lost: it drops its
 * coin, poofs, is removed (func_ov084_02129498), has its cap released and is
 * put back at mHomePos (RespawnIfHasCap). */
void daKrb_c::func_ov084_02129238() {
    daKrb_c *goomba = this;
    if (goomba->mWithMeshClsn.IsOnGround() != 0)
        return;
    {
        Vector3 pos;
        {
            /* z before y: mwcc keeps that load order. */
            int vx = goomba->mPosX;
            int vz = goomba->mPosZ;
            int vy = goomba->mPosY + 0x190000;     /* 400.0 above */
            pos.x = vx;
            pos.y = vy;
            pos.z = vz;
        }
        dBgCh_Gnd rg;
        rg.StartDetectingWater();
        rg.StartDetectingToxic();
        rg.StopDetectingOrdinary();
        rg.SetObjAndPos(pos, goomba);
        if (rg.DetectClsn() != 0) {
            if (func_02037e20((int*)&rg.surface) != 0) {
                if (rg.clsnY != (int)0x80000000) {
                    if (goomba->mPosY < rg.clsnY) {
                        goomba->SpawnCoin();
                        goomba->PoofDust();
                        goomba->func_ov084_02129498();
                        {
                            Vector3 cap;
                            cap.x = 0;
                            cap.y = 0x6c000;     /* 108.0 */
                            cap.z = 0;
                            goomba->ReleaseCap(cap);
                        }
                        goomba->mPosX = goomba->mHomePos.x;
                        goomba->mPosY = goomba->mHomePos.y;
                        goomba->mPosZ = goomba->mHomePos.z;
                        goomba->RespawnIfHasCap();
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
void daKrb_c::func_ov084_0212934c() {
    daKrb_c *goomba = this;
    unsigned int kind;
    int frame;
    int type;

    if (goomba->mState != daKrb_c::STATE_WALK)
        return;

    if (!goomba->mWithMeshClsn.IsOnGround())
        return;

    /* Frame before the file pointer: that order is what colors the short
       extract and the file load into the registers the ROM uses. */
    frame = static_cast<dExtFrameCtrl_c &>(goomba->mAnmModel).currFrame;
    kind = (unsigned short)((unsigned)frame >> 12);
    type = (int)goomba->mAnmModel.file;

    if (type == (int)data_ov084_02130ce8.filePtr) {
        if (kind <= 4 || (kind >= 0xc && kind <= 0x10)) {
            if (((Flags *)&goomba->mMoveFlags)->flag)
                return;
            func_02012694(SND_FOOTSTEP, &goomba->mCamSpacePos);
            goomba->mMoveFlags |= daKrb_c::MOVE_STEP_SOUND_LATCH;
            return;
        }
        *(unsigned char *)&goomba->mMoveFlags &= ~daKrb_c::MOVE_STEP_SOUND_LATCH;
        return;
    }

    if (type == (int)data_ov084_02130cf0.filePtr) {
        if (kind <= 3 || (kind >= 0x10 && kind <= 0x13)) {
            if (((Flags *)&goomba->mMoveFlags)->flag)
                return;
            func_02012694(SND_FOOTSTEP, &goomba->mCamSpacePos);
            goomba->mMoveFlags |= daKrb_c::MOVE_STEP_SOUND_LATCH;
            return;
        }
        *(unsigned char *)&goomba->mMoveFlags &= ~daKrb_c::MOVE_STEP_SOUND_LATCH;
    }
}

/* Remove the Goomba. With mCapId below 6 (the cap table's "has a cap" range)
 * it is only marked for destruction (MarkForDestruction); otherwise it goes
 * through KillAndTrackInDeathTable. */
void daKrb_c::func_ov084_02129498() {
    daKrb_c *goomba = this;
    if ((goomba->mCapId & 0xf) < 6)
        goomba->MarkForDestruction();
    else
        goomba->KillAndTrackInDeathTable();
}

/* Ground check, run every frame while the Goomba stands on something.
 * If the floor's CLPS record has flag 0x20 set (SurfaceInfo_TestFlag0x20;
 * CLPS.h reads it as water) the Goomba is lost: reward (func_ov084_021296cc),
 * coin, removal, cap released, back to mHomePos (RespawnIfHasCap).
 *
 * Otherwise the floor record is copied into a local dBgPi and its surface
 * type (CLPS bits 19..23, CLPS::GetSurfaceType) is read. The Goomba is treated as
 * lost (coin and removal, and a respawn at mHomePos when it has a cap or is
 * REWARD_SILVER_STAR_IF_CURRENT) if the type is 1, 4, 5 or 0x13, or if it is
 * 6..9 (daPly_c.h calls those the quicksand tiers) while the sound type
 * (CLPS bits 0..4, CLPS::GetSoundType) is 8. */
void daKrb_c::func_ov084_021294d0() {
    if (!mWithMeshClsn.IsOnGround())
        return;

    if (SurfaceInfo_TestFlag0x20((int *)&mWithMeshClsn.GetFloorResult()->surface)) {
        func_ov084_021296cc();
        SpawnCoin();
        func_ov084_02129498();
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
        func_ov084_02129498();
        if ((mCapId & 0xf) < 6 || mRewardType == REWARD_SILVER_STAR_IF_CURRENT) {
            mPos = mHomePos;
            ReleaseCap(Vector3(0, 0x6c000, 0)); /* 108.0 */
            RespawnIfHasCap();
        }
    }
}

/* Set the home position (mHomePos) from a three-word position. daKuriKing_c
 * calls this on each minion it spawns, after the Spawn. */

void func_ov084_021296b0(void *a, void *b) {
    daKrb_c *goomba = (daKrb_c *)a;
    int *pos = (int *)b;
    goomba->mHomePos.x = pos[0];
    goomba->mHomePos.y = pos[1];
    goomba->mHomePos.z = pos[2];
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
 * data_0209f344[data_0209f208[0]]; spawns both actors with parameter
 * mStarID | 0x30 at the Goomba's position, marks the reward spent, and
 * plays sound object 1. */
void daKrb_c::func_ov084_021296cc() {
    daKrb_c *goomba = this;
    if (goomba->mRewardType == daKrb_c::REWARD_SILVER_STAR) {
        char *starMarker;
        char *silverStar;
        goomba->UntrackStar(goomba->mStarTracked);
        starMarker = (char *)dActor_c::Spawn(
            ACTOR_STARBASE, 0x50, goomba->mHomePos, 0, goomba->mAreaId, -1);
        silverStar =
            (char *)dActor_c::Spawn(ACTOR_SILVER_STAR, 0x10, goomba->mPos, 0, goomba->mAreaId, -1);
        if (starMarker != 0 && silverStar != 0) {
            ((SilverStarView *)silverStar)->mMarkerID = ((dActor_c *)starMarker)->uniqueID;
            ((daStarBase_c *)starMarker)->LinkSilverStarAndStarMarker(silverStar);
            goomba->SpawnSoundObj(1);
        }
        /* The (int) cast on the load is load-bearing: a plain
           `param1 & 0xff0f` CSEs the +8 address and builds one word longer. */
        goomba->param1 = (int)goomba->param1 & 0xff0f;
        return;
    }
    if (goomba->mRewardType != daKrb_c::REWARD_SILVER_STAR_IF_CURRENT)
        return;
    if (goomba->mStarID != data_0209f344[data_0209f208[0]])
        return;
    goomba->UntrackStar(goomba->mStarTracked);
    dActor_c::Spawn(ACTOR_STARBASE, goomba->mStarID | 0x30, goomba->mPos, 0, goomba->mAreaId, -1);
    dActor_c::Spawn(ACTOR_SILVER_STAR, goomba->mStarID | 0x30, goomba->mPos, 0, goomba->mAreaId,
                    -1);
    goomba->mRewardType = daKrb_c::REWARD_SPENT;
    goomba->param1 = (int)goomba->param1 & 0xff0f;
    goomba->SpawnSoundObj(1);
}

/* For REWARD_SILVER_STAR_IF_CURRENT: while no star slot is tracked yet
 * (mStarTracked negative) and mStarID matches data_0209f344[data_0209f208[0]],
 * claim a slot (TrackStar(mStarID, 1)). */
void daKrb_c::func_ov084_02129864() {
    daKrb_c *goomba = this;
    if (goomba->mRewardType != daKrb_c::REWARD_SILVER_STAR_IF_CURRENT)
        return;
    if (goomba->mStarTracked >= 0)
        return;
    unsigned int current = data_0209f344[data_0209f208[0]];
    if (goomba->mStarID != current)
        return;
    goomba->mStarTracked = (s8)goomba->TrackStar(goomba->mStarID, 1);
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
int daKrb_c::func_ov084_021298d0() {
    daKrb_c *goomba = this;
    int deathState = goomba->UpdateDeath(goomba->mWithMeshClsn);
    if ((unsigned int)(goomba->mDeathState - 2) <= 4) {
        static_cast<dExtFrameCtrl_c &>(goomba->mAnmModel).speed = 0x1000;
        ((dExtFrameCtrl_c *)&goomba->mAnmModel)->Advance();
        if (goomba->mGoombaType == daKrb_c::GOOMBA_KING_MINION) {
            /* SpawnCoin only when the actor we hit is a KURIKING. The flag and the
               cylinder update always run for a king minion in this death phase. */
            unsigned int id = goomba->mdCcAc_c.otherOwner;
            if (id != 0) {
                char *r = (char *)dActor_c::FindWithID(id);
                if (r != 0) {
                    int b = (((dActor_c *)r)->actorID == ACTOR_KURIKING);
                    if (b != 0) {
                        goomba->SpawnCoin();
                        goomba->func_ov084_02129498();
                    }
                }
            }
            *(int *)((int)&goomba->mdCcAc_c.flags) |= 0x20000;
            goomba->mdCcAc_c.Clear();
            goomba->mdCcAc_c.Update();
        }
    }
    if (deathState != 0) {
        func_02012694(data_ov084_02130218[goomba->mGoombaType], &goomba->mCamSpacePos);
        goomba->func_ov084_021296cc();
        if ((goomba->mCapId & 0xf) < 6 ||
            goomba->mRewardType == daKrb_c::REWARD_SILVER_STAR_IF_CURRENT) {
            goomba->mPosX = goomba->mHomePos.x;
            goomba->mPosY = goomba->mHomePos.y;
            goomba->mPosZ = goomba->mHomePos.z;
            goomba->RespawnIfHasCap();
        }
        if ((goomba->mCapId & 0xf) < 6)
            goomba->UntrackInDeathTable();
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
 *      (func_ov084_02129168, then horizontal speed -15.0 and vertical speed
 *      20.0), its material animation is restarted and its cylinder cleared.
 *   3: halve the horizontal speed while on the ground.
 * A hit reported by SpawnParticlesIfHitOtherObj sets mDeathState to
 * DEATH_HIT_20000, hands the hit to func_ov002_020aea30, and sets bit 0 of
 * the dCcAc_c flags (disabled).
 * Otherwise the model and cap are placed (func_ov084_0212a580) and, while
 * Yoshi holds it (mEatenByYoshi != 0), the ground check runs
 * (func_ov084_021294d0), then by mSpitOutTimer: 0 updates the cylinder; 5 replays
 * the data_ov084_02130ce0 animation, turns the Goomba half a turn, reverses
 * its horizontal speed and restarts the material animation. The animation is
 * advanced, and on touchdown the Goomba is thrown clear again
 * (func_ov084_02129168 with no eater).
 * A king minion with eatState >= 3 that has dropped more than 1000.0 below
 * mHomePos is removed. */
int daKrb_c::func_ov084_02129a00() {
    daKrb_c *goomba = this;
    int eatState = goomba->UpdateYoshiEat(goomba->mWithMeshClsn);
    if (eatState != 0) {
        if (eatState == 1) {
            Vector3 v;
            dActor_c *actor = (dActor_c *)goomba->mEatingPlayer;
            v.x = 0;
            v.y = 0x6c000; /* 108.0 */
            v.z = 0;
            if (goomba->GetCapEatenOffIt(v) != 0) {
                goomba->func_ov084_02129168(actor);
                goomba->mHorzSpeed = -0xf000; /* -15.0 */
                goomba->mVertSpeed = 0x14000; /* 20.0 */
                dExtAnmMaterial_c::Prepare(*(BMD_File *)(data_ov084_02130cf8.filePtr),
                                           *(BMA_File *)&data_ov084_0213088c);
                goomba->mMaterialChanger.SetFile(*(BMA_File *)&data_ov084_0213088c, 0x40000000,
                                                 0x1000, 0);
                goomba->mMaterialChanger.currFrame = 0;
                ((dCc_c *)&goomba->mdCcAc_c)->Clear();
                return 0;
            }
        } else if (eatState == 3) {
            if (goomba->mWithMeshClsn.IsOnGround())
                goomba->mHorzSpeed >>= 1;
        }

        if (goomba->SpawnParticlesIfHitOtherObj(*(dCc_c *)&goomba->mdCcAc_c) != 0) {
            void *actor = dActor_c::FindWithID(goomba->mdCcAc_c.otherOwner);
            goomba->mDeathState = daKrb_c::DEATH_HIT_20000;
            func_ov002_020aea30((Aea30C *)goomba, (dActor_c *)actor, &goomba->mWithMeshClsn);
            goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce0.filePtr, 0x40000000, 0x1000,
                                      0);
            *(int *)((int)&goomba->mdCcAc_c.flags) |= 1;
            return 1;
        }

        goomba->func_ov084_0212a580();
        ((dCc_c *)&goomba->mdCcAc_c)->Clear();
        if (goomba->mEatenByYoshi != 0) {
            goomba->func_ov084_021294d0();
            {
                u16 s = goomba->mSpitOutTimer;
                if (s == 0) {
                    ((dCc_c *)&goomba->mdCcAc_c)->Update();
                } else if (s == 5) {
                    goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce0.filePtr, 0x40000000,
                                              0x1000, 0);
                    goomba->mPrevAngleY += 0x8000; /* half a turn */
                    goomba->mHorzSpeed = -goomba->mHorzSpeed;
                    dExtAnmMaterial_c::Prepare(*(BMD_File *)(data_ov084_02130cf8.filePtr),
                                               *(BMA_File *)&data_ov084_0213088c);
                    goomba->mMaterialChanger.SetFile(*(BMA_File *)&data_ov084_0213088c, 0x40000000,
                                                     0x1000, 0);
                    goomba->mMaterialChanger.currFrame = 0;
                }
            }
            ((dExtFrameCtrl_c *)&goomba->mAnmModel)->Advance();
            if (goomba->mWithMeshClsn.JustHitGround())
                goomba->func_ov084_02129168(0);
        }

        if (goomba->mGoombaType == daKrb_c::GOOMBA_KING_MINION && eatState >= 3 &&
            goomba->mPosY < goomba->mHomePos.y - 0x3e8000) { /* 1000.0 below home */
            goomba->MarkForDestruction();
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
    daKrb_c *goomba = this;
    func_02012694(SND_HOP, &goomba->mCamSpacePos);
    goomba->mState = daKrb_c::STATE_JUMP;
    goomba->mHorzSpeed = 0;
    goomba->mVertSpeed = data_ov084_02130248[goomba->mGoombaType];
    goomba->mWithMeshClsn.ClearGroundFlag();
    goomba->mdCcAc_c.flags |= 4;
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
void daKrb_c::func_ov084_02129cf4(Fix12i distThresh) {
    struct Vector3 ppos;

    daKrb_c *goomba = this;
    goomba->mClosestPlayer = goomba->ClosestPlayer();

    if (goomba->mClosestPlayer == 0 ||
        (Vec3_Dist(&goomba->mPos, &goomba->mHomePos) > distThresh && goomba->mCapId >= 6)) {
        goomba->mInitAngleY = Vec3_HorzAngle(&goomba->mPos, &goomba->mHomePos);
        goomba->mDistToPlayer = 0x61a8000;
        return;
    }

    {
        int *ppos_src = (int *)(&goomba->mClosestPlayer->mPosX);
        ppos.x = ppos_src[0];
        ppos.y = ppos_src[1];
        ppos.z = ppos_src[2];
    }

    if (goomba->mCapId < 6) {
        if (Vec3_Dist(&goomba->mPos, &goomba->mHomePos) > distThresh &&
            !goomba->mWithMeshClsn.IsOnWall()) {
            goomba->mDistToPlayer = 0x61a8000;
            goomba->mInitAngleY = Vec3_HorzAngle(&goomba->mPos, &goomba->mHomePos);
            return;
        }

        if (Vec3_Dist(&goomba->mPos, &ppos) < goomba->mChaseRadius) {
            goomba->mDistToPlayer = Vec3_Dist(&goomba->mPos, &ppos);
            if (goomba->mUnstickTimer != 0) {
                goomba->mInitAngleY = Vec3_HorzAngle(&goomba->mPos, &ppos);
                return;
            }
            goomba->mInitAngleY = Vec3_HorzAngle(&ppos, &goomba->mPos);
            return;
        }
        goomba->mDistToPlayer = 0x61a8000;
        return;
    }

    if (Vec3_Dist(&goomba->mHomePos, &ppos) > distThresh) {
        goomba->mDistToPlayer = 0x61a8000;
        return;
    }
    goomba->mDistToPlayer = Vec3_Dist(&goomba->mPos, &ppos);
    goomba->mInitAngleY = Vec3_HorzAngle(&goomba->mPos, &ppos);
}

/* Collision reaction: something touched this Goomba, decide what it did.
 *
 * `flags` is hitFlags (the collision word at 0x1a0) and `other` the actor found
 * through otherOwner (0x1a4). Type ids are the debug-table profile numbers this
 * class is registered under -- 200 KURIBO, 201 KURIBO_S, 202 KURIBO_L, and 191
 * for a daPly_c -- so `myType` is which of the three Goomba sizes WE are, not
 * what hit us. mDeathState is set to the DEATH_* value for the hit, and
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
 * handed to func_ov002_020aea30, and the Goomba is turned half a turn when the
 * hit asked for it (turnAround). A king minion hit by a punch, kick, breakdance or
 * slide kick adds the attacker's horizontal speed to its own; one hit by a
 * dive adds 0x20.
 *
 * `variantMatch` and `typeMatch` are scratch booleans, and materializing them
 * instead of testing inline is deliberate: mwccarm emits the comparison into a
 * register and then re-tests it, which is the shape the cartridge has. Folding
 * either back into its `if` collapses the pair. See notes/matching-style.md 3. */
void daKrb_c::func_ov084_02129ed4() {
    daKrb_c *goomba = this;
    s16 killDirNormal[3];
    s16 killDirVariant[3];
    s16 killDirPlayer[3];
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
    u16 myType;
    s32 variantMatch;
    s32 typeMatch;
    u32 id;

    id = goomba->mdCcAc_c.otherOwner;
    if (id == 0)
        return;
    other = static_cast<daPly_c *>(dActor_c::FindWithID(id));
    if (other == 0)
        return;

    flags = goomba->mdCcAc_c.hitFlags;
    hurtKnockback = 0xc000; /* 12.0 */
    goomba->mLastHitFlags = flags;
    myType = goomba->actorID;
    turnAround = 0;
    if (goomba->mGoombaType == daKrb_c::GOOMBA_KING_MINION)
        hurtKnockback = 0x5000; /* 5.0 */
    variantMatch = (s32)(myType == ACTOR_KURIBO_S);

    if (variantMatch == 0 && (flags & HIT_MEGA)) {
        capReleaseOnHit.x = 0;
        capReleaseOnHit.y = 0x6c000;
        capReleaseOnHit.z = 0; /* 108.0 */
        goomba->ReleaseCap(capReleaseOnHit);
        typeMatch = (s32)(goomba->actorID == ACTOR_KURIBO);
        if (typeMatch != 0) {
            killDirNormal[0] = -0x2000;
            killDirNormal[1] = 0;
            killDirNormal[2] = 0;
            goomba->KillByInvincibleChar(*(Vector3_16 *)killDirNormal, *other, 0x41000); /* 65.0 */
            return;
        }
        killDirVariant[0] = -0x1800;
        killDirVariant[1] = 0;
        killDirVariant[2] = 0;
        goomba->KillByInvincibleChar(*(Vector3_16 *)killDirVariant, *other, 0x96000); /* 150.0 */
        return;
    }

    if (flags & HIT_SPIN_OR_POUND) {
        goomba->mDeathState = daKrb_c::DEATH_STOMPED;
        if (goomba->mGoombaType == daKrb_c::GOOMBA_LARGE)
            goomba->mCoinType = daKrb_c::COIN_BLUE;
        goomba->mScaleX = 0x1000; /* 1.0 */
        goomba->mScaleY = 0x1000;
        goomba->mScaleZ = 0x1000;
        func_02012694(SND_STOMPED, &goomba->mCamSpacePos);
    } else if (flags & HIT_FIRE) {
        turnAround = 1;
        goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130cd0.filePtr, 0x40000000, 0x1000, 0);
        goomba->mDeathState = daKrb_c::DEATH_FIRE;
    } else {
        variantMatch = (s32)(myType == ACTOR_KURIBO_L);
        if (variantMatch == 0) {
            if (flags & HIT_UNLISTED_20000) {
                goomba->mDeathState = daKrb_c::DEATH_HIT_20000;
                goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce0.filePtr, 0x40000000,
                                          0x1000, 0);
                *(s32 *)((int)&goomba->mdCcAc_c.flags) |= 1;
            } else if (flags & (HIT_EGG | HIT_DIVE)) {
                goomba->mDeathState = daKrb_c::DEATH_DIVE_OR_EGG;
                goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce0.filePtr, 0x40000000,
                                          0x1000, 0);
            } else if (flags & HIT_EXPLOSION) {
                goomba->mDeathState = daKrb_c::DEATH_EXPLOSION;
                goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130cd0.filePtr, 0x40000000,
                                          0x1000, 0);
            } else if (flags & (HIT_KICK | HIT_BREAKDANCE | HIT_SLIDE_KICK)) {
                turnAround = 1;
                goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130cd0.filePtr, 0x40000000,
                                          0x1000, 0);
                goomba->mDeathState = daKrb_c::DEATH_KICKED;
            } else if (flags & HIT_PUNCH) {
                goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce0.filePtr, 0x40000000,
                                          0x1000, 0);
                goomba->mDeathState = daKrb_c::DEATH_PUNCHED;
                turnAround = 1;
            } else if (!(flags & HIT_YOSHI_TONGUE)) {
                typeMatch = (s32)(((dActor_c *)other)->actorID == ACTOR_PLAYER);
                if (typeMatch != 0) {
                    if (other->mIsMetal != 0) {
                        capReleaseOnKill.x = 0;
                        capReleaseOnKill.y = 0x6c000;
                        capReleaseOnKill.z = 0; /* 108.0 */
                        goomba->ReleaseCap(capReleaseOnKill);
                        killDirPlayer[0] = 0x2000;
                        killDirPlayer[1] = 0;
                        killDirPlayer[2] = 0;
                        goomba->KillByInvincibleChar(*(Vector3_16 *)killDirPlayer, *other,
                                                     0x41000); /* 65.0 */
                        return;
                    }
                    {
                        const Vector3 &p = other->mPos;
                        playerPos = p;
                    }
                    if (other->IsOnShell() != 0) {
                        goomba->mDeathState = daKrb_c::DEATH_DIVE_OR_EGG;
                        turnAround = 1;
                        goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce0.filePtr,
                                                  0x40000000, 0x1000, 0);
                    } else if (((dActor_c *)goomba)
                                   ->JumpedOnByPlayer(*(dCc_c *)&goomba->mdCcAc_c, *other) != 0) {
                        other->Bounce(0x28000); /* 40.0 */
                        func_02012694(SND_STOMPED, &goomba->mCamSpacePos);
                        goomba->mDeathState = daKrb_c::DEATH_STOMPED;
                        goomba->mScaleX = 0x1000; /* 1.0 */
                        goomba->mScaleY = 0x1000;
                        goomba->mScaleZ = 0x1000;
                    } else {
                        if (other->mIsVanish != 0)
                            return;
                        if (goomba->mState == daKrb_c::STATE_WALK) {
                            if (goomba->mGoombaType == daKrb_c::GOOMBA_SMALL) {
                                ((dActor_c *)goomba)->SmallPoofDust();
                                hurtOriginFirstHit = goomba->mPos;
                                other->Hurt(hurtOriginFirstHit, 0, hurtKnockback, 1, 0, 1);
                                goomba->func_ov084_02129498();
                                func_02012694(SND_DEATH_SMALL, &goomba->mCamSpacePos);
                                return;
                            }
                            if ((goomba->mdCcAc_c.hitFlags & HIT_PLAYER) == 0)
                                return;
                            hurtOriginRepeat = goomba->mPos;
                            other->Hurt(hurtOriginRepeat, data_ov084_02130204[goomba->mGoombaType],
                                        hurtKnockback, 1, 0, 1);
                            goomba->mState = daKrb_c::STATE_ATTACKED_MARIO;
                            return;
                        }
                    }
                }
            }
        } else {
            typeMatch = (s32)(((dActor_c *)other)->actorID == ACTOR_PLAYER);
            if (typeMatch != 0) {
                {
                    const Vector3 &p = other->mPos;
                    playerPosJump = p;
                }
                if (((dActor_c *)goomba)->JumpedOnByPlayer(*(dCc_c *)&goomba->mdCcAc_c, *other) !=
                    0) {
                    other->Bounce(0x28000); /* 40.0 */
                    func_02012694(SND_STOMPED, &goomba->mCamSpacePos);
                    goomba->mDeathState = daKrb_c::DEATH_STOMPED;
                    goomba->mScaleX = 0x1000; /* 1.0 */
                    goomba->mScaleY = 0x1000;
                    goomba->mScaleZ = 0x1000;
                } else {
                    if (other->mIsVanish != 0)
                        return;
                    if (goomba->mState == daKrb_c::STATE_WALK) {
                        goomba->mState = daKrb_c::STATE_ATTACKED_MARIO;
                        if ((goomba->mdCcAc_c.hitFlags & HIT_PLAYER) == 0)
                            return;
                        hurtOriginJumped = goomba->mPos;
                        other->Hurt(hurtOriginJumped, data_ov084_02130204[goomba->mGoombaType],
                                    hurtKnockback, 1, 0, 1);
                        return;
                    }
                }
            }
        }
    }

    if (goomba->mDeathState != 0) {
        capReleaseOnExit.x = 0;
        capReleaseOnExit.y = 0x6c000;
        capReleaseOnExit.z = 0; /* 108.0 */
        goomba->ReleaseCap(capReleaseOnExit);
    }
    func_ov002_020aea30((Aea30C *)goomba, (dActor_c *)other, &goomba->mWithMeshClsn);
    if (turnAround != 0) {
        goomba->mAngleY = (s16)(goomba->mPrevAngleY + 0x8000); /* half a turn */
    }
    if (goomba->mGoombaType != daKrb_c::GOOMBA_KING_MINION)
        return;
    if ((goomba->mLastHitFlags & HIT_PUNCH) ||
        (goomba->mLastHitFlags & (HIT_KICK | HIT_BREAKDANCE | HIT_SLIDE_KICK))) {
        *(s32 *)((int)&goomba->mHorzSpeed) += ((dActor_c *)other)->mHorzSpeed;
    }
    if (goomba->mLastHitFlags & HIT_DIVE) {
        *(s32 *)((int)&goomba->mHorzSpeed) += 0x20;
    }
}

/* Per-frame model, shadow and cap placement. Rebuilds the model matrix from
 * mAngleY, sets its translation to the position >> 3, and draws the drop
 * shadow (radius 80 * mScaleX; depth 30.0 on the ground, 150.0 in the air)
 * unless the actor is in Yoshi's mouth (mFlags 0x40000). Then the cap is attached (UpdateCapPos)
 * 108.0 above the origin and 10 * (sin, cos) of mAngleY out along the
 * heading. UpdateCapPos is handed temporary copies of both, the rotation
 * built first (mwcc evaluates arguments right to left); passing pos and
 * rotation themselves drops the copies. */
void daKrb_c::func_ov084_0212a580() {
    Vector3_16 rotation;
    Vector3 pos;

    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.t.x = mPosX >> 3;
    mAnmModel.mat4x3.t.y = mPosY >> 3;
    mAnmModel.mat4x3.t.z = mPosZ >> 3;
    rotation = mAngle;
    int inYoshiMouth = (mFlags & 0x40000) ? 1 : 0;
    if (!inYoshiMouth) {
        if (mWithMeshClsn.IsOnGround())
            DropShadowRadHeight(mShadowModel, mAnmModel.mat4x3, mScaleX * 0x50, 0x1e000, 0xf);
        else
            DropShadowRadHeight(mShadowModel, mAnmModel.mat4x3, mScaleX * 0x50, 0x96000, 0xf);
    }
    pos.x = 0;
    pos.z = 0;
    pos.y = 0x6c000;     /* 108.0 */
    pos.x += FX_SinIdx((u16)mAngleY >> 4) * 10;
    pos.z += FX_CosIdx((u16)mAngleY >> 4) * 10;
    UpdateCapPos(Vector3(pos), Vector3_16(rotation));
}

/* STATE_PAUSE: stand still (horizontal speed 0) until mWanderRerollTimer runs
 * out, then go back to STATE_WALK with the walk animation. */
void daKrb_c::ActPause() {
    daKrb_c *goomba = this;
    goomba->mHorzSpeed = 0;
    /* Header spells this s16; the body loads it unsigned, and the two
       address forms stop mwcc from CSEing the halfword. */
    if (*(unsigned short *)&goomba->mWanderRerollTimer) {
        *(unsigned short *)((int)&goomba->mWanderRerollTimer) -= 1;
    }
    if (*(unsigned short *)&goomba->mWanderRerollTimer)
        return;
    goomba->mState = daKrb_c::STATE_WALK;
    goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce8.filePtr, 0, 0x1000, 0);
}

/* STATE_TUMBLE: the Goomba has been thrown clear (func_ov084_02129168),
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
 * DEATH_HIT_20000 and bit 0 of the dCcAc_c flags, as in func_ov084_02129a00. */
void daKrb_c::ActTumble() {
    daKrb_c *goomba = this;
    Vector3 v;
    u16 h = goomba->mBounceCountdown;

    if (h == 0) {
        ((dExtFrameCtrl_c *)&goomba->mMaterialChanger)->Advance();
        if (((dExtFrameCtrl_c *)&goomba->mAnmModel)->Finished() == 0)
            return;
        goomba->mFlags = goomba->mSavedParam;
        goomba->mState = daKrb_c::STATE_WALK;
        goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce8.filePtr, 0, 0x1000, 0);
        goomba->mTargetHorzSpeed = data_ov084_02130228[goomba->mGoombaType];
        goomba->mWithMeshClsn.ClearLimMovFlag();
        {
            goomba->mPrevAngleY = goomba->mAngleY;
            goomba->mdCcAc_c.flags &= ~0x20000;
        }
        dExtAnmMaterial_c::Prepare(*(BMD_File *)(data_ov084_02130cf8.filePtr),
                                   *(BMA_File *)&data_ov084_0213089c);
        goomba->mMaterialChanger.SetFile(*(BMA_File *)&data_ov084_0213089c, 0x40000000, 0x1000, 0);
        goomba->mMaterialChanger.currFrame = 0;
        return;
    }
    if (h <= 0x3c) {
        goomba->mBounceCountdown -= 1;
        if (goomba->mBounceCountdown == 0) {
            goomba->func_ov084_021296cc();
            goomba->SpawnCoin();
            goomba->func_ov084_02129498();
            v.x = 0;
            v.y = 0x6c000;     /* 108.0 */
            v.z = 0;
            goomba->ReleaseCap(v);
            goomba->mPosX = goomba->mHomePos.x;
            goomba->mPosY = goomba->mHomePos.y;
            goomba->mPosZ = goomba->mHomePos.z;
            goomba->RespawnIfHasCap();
        }
    }
    if (goomba->mWithMeshClsn.JustHitGround() != 0) {
        int a8;
        int cnt;
        if (goomba->mBounceCountdown > 0x3c)
            goomba->mBounceCountdown = 0x1e;
        a8 = goomba->mVertSpeed;
        cnt = goomba->mBounceCountdown;
        {
            int aa = a8 < 0 ? -a8 : a8;
            if (aa <= cnt * 0x500) {
                goomba->mVertSpeed = (cnt << 0xa) - goomba->mVertAccel;
            } else {
                goomba->mVertSpeed = FX_Div((a8 * -0x50) / 100, goomba->mFloorNormalY);
            }
        }
        {
            goomba->mHorzSpeed >>= 1;
        }
        {
            int v98 = goomba->mHorzSpeed;
            int av = v98 < 0 ? -v98 : v98;
            if (av < 0x5000) {
                if (v98 < 0)
                    goomba->mHorzSpeed = -0x5000;
                else
                    goomba->mHorzSpeed = 0x5000;
            }
        }

        if (goomba->mBounceCount == 0) {
            func_02012694(SND_TUMBLE_BOUNCE_FIRST, &goomba->mCamSpacePos);
        } else if (goomba->mBounceCount <= 2) {
            func_02012694(SND_TUMBLE_BOUNCE_LATER, &goomba->mCamSpacePos);
        }
        goomba->mBounceCount += 1;
    } else {
        if (goomba->mWithMeshClsn.IsOnGround() != 0) {
            goomba->mWithMeshClsn.ClearLimMovFlag();
            goomba->mHorzSpeed = 0;
            goomba->mVertSpeed = 0;
            if (goomba->mBounceCountdown > 0x3c)
                goomba->mBounceCountdown = 0x1e;
        }
    }
    goomba->mEatenByYoshi = 1;
    if (goomba->SpawnParticlesIfHitOtherObj(*(dCc_c *)&goomba->mdCcAc_c) != 0) {
        void *a = dActor_c::FindWithID(goomba->mdCcAc_c.otherOwner);
        goomba->mDeathState = daKrb_c::DEATH_HIT_20000;
        func_ov002_020aea30((Aea30C *)goomba, (dActor_c *)a, &goomba->mWithMeshClsn);
        goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce0.filePtr, 0x40000000, 0x1000, 0);
        goomba->mdCcAc_c.flags |= 1;
    }
    goomba->mEatenByYoshi = 0;
}

/* STATE_JUMP: after a hop begins. On the frame it touches down a large
 * Goomba kicks up HugeLandingDust and a normal one LandingDust. Once on the
 * ground it returns to STATE_WALK and clears dCcAc_c flag bit 0x4 (set by
 * BeginJump); in the air it turns mPrevAngleY toward mTargetAngleY
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
        ApproachLinear(mPrevAngleY, mTargetAngleY, 0x800);
    }
    mAngleY = mPrevAngleY;
}

/* STATE_ATTACKED_MARIO: begin a hop (BeginJump, which enters
 * STATE_JUMP). A KURIBO_L then scales its vertical speed by
 * 0x1800 / 0x1000 = 1.5. The target heading becomes the spawn heading
 * (mInitAngleY), MOVE_AVOIDING is cleared, and mAngleY follows mPrevAngleY.
 * N64: goomba_act_attacked_mario's non-tiny branch -- goomba_begin_jump, a new
 * oGoombaTargetYaw, oGoombaTurningAwayFromWall = FALSE. Its tiny branch (the
 * Goomba dies) is in func_ov084_02129ed4 here, where KURIBO_S removes itself
 * instead of entering this state; KURIBO_L entering it from a player hit is
 * N64 huge_goomba_weakly_attacked. */
void daKrb_c::ActAttackedMario() {
    daKrb_c *goomba = this;
    int large;
    goomba->BeginJump();
    large = (int)(goomba->actorID == (unsigned short)ACTOR_KURIBO_L);
    if (large != 0) {
        goomba->mVertSpeed = FX_MUL(goomba->mVertSpeed, 0x1800);
    }
    goomba->mTargetAngleY = goomba->mInitAngleY;
    *(unsigned char *)&goomba->mMoveFlags &= ~daKrb_c::MOVE_AVOIDING;
    goomba->mAngleY = goomba->mPrevAngleY;
}

/* STATE_WALK for an ordinary Goomba (not a king minion): wander and chase.
 *
 * func_ov084_02129cf4 picks the target (mDistToPlayer and mInitAngleY), the
 * horizontal speed eases toward mTargetHorzSpeed (0.3125 a frame), and at the
 * end mPrevAngleY is turned toward mTargetAngleY by `step` (0x200 = 2.8
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
void daKrb_c::func_ov084_0212abd4() {
    daKrb_c *goomba = this;
    short step = 0x200;
    func_ov084_02129cf4(0x3e8000);
    ApproachLinear(*(int *)&goomba->mHorzSpeed, goomba->mTargetHorzSpeed, 0x500);
    if (((Flags *)&goomba->mMoveFlags)->b0) {
        if (ApproachLinear(*(short *)&goomba->mPrevAngleY, goomba->mInitAngleY, step)) {
            *(unsigned char *)((int)&goomba->mMoveFlags) &= ~daKrb_c::MOVE_AVOIDING;
            return;
        }
        {
            unsigned char *p = (unsigned char *)((int)&goomba->mMoveFlags);
            *p = (*p & ~daKrb_c::MOVE_AVOIDING) | daKrb_c::MOVE_AVOIDING;
        }
        return;
    }
    if (goomba->mUnstickTimer != 0) {
        if (goomba->mCapId < 6) {
            if (goomba->mTargetHorzSpeed <= data_ov084_02130228[goomba->mGoombaType]) {
                goomba->BeginJump();
            } else {
                goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130cf0.filePtr, 0, 0x1000, 0);
            }
            step = 0x800;
            goomba->mTargetHorzSpeed = data_ov084_02130268[goomba->mGoombaType];
            goomba->mTargetAngleY = goomba->mInitAngleY;
        } else {
            goomba->mTargetHorzSpeed = data_ov084_02130228[goomba->mGoombaType];
            goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce8.filePtr, 0, 0x1000, 0);
            goomba->mTargetAngleY = Vec3_HorzAngle(&goomba->mPos, &goomba->mHomePos);
            step = 0x400;
        }
        ApproachLinear(*(short *)&goomba->mPrevAngleY, goomba->mTargetAngleY, step);
        return;
    }
    if (goomba->mDistToPlayer >= 0x61a8000) {
        goomba->mTargetAngleY = goomba->mInitAngleY;
        goomba->mHeadingHoldTimer = 0x19;
    }
    {
        int bit = goomba->AngleAwayFromWallOrCliff(goomba->mWithMeshClsn, goomba->mTargetAngleY);
        unsigned char *p = (unsigned char *)((int)&goomba->mMoveFlags);
        bit &= 1;
        *p = (*p & ~daKrb_c::MOVE_AVOIDING) | bit;
    }
    if (!((Flags *)&goomba->mMoveFlags)->b0) {
        if (goomba->mDistToPlayer < goomba->mChaseRadius ||
            (goomba->mCapId < 6 && Vec3_Dist(&goomba->mPos, &goomba->mHomePos) > 0x3e8000)) {
            if (goomba->mTargetHorzSpeed <= data_ov084_02130228[goomba->mGoombaType]) {
                goomba->BeginJump();
            } else {
                goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130cf0.filePtr, 0, 0x1000, 0);
            }
            if (goomba->mCapId >= 6 || goomba->mUnstickTimer != 0) {
                goomba->mTargetAngleY = goomba->mInitAngleY;
            } else {
                step = 0x600;
                goomba->mTargetAngleY = goomba->mInitAngleY;
            }
            goomba->mTargetHorzSpeed = data_ov084_02130268[goomba->mGoombaType];
        } else {
            goomba->mTargetHorzSpeed = data_ov084_02130228[goomba->mGoombaType];
            goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce8.filePtr, 0, 0x1000, 0);

            if (*(u16 *)&goomba->mHeadingHoldTimer != 0) {
                *(u16 *)((int)&goomba->mHeadingHoldTimer) =
                    *(u16 *)((int)&goomba->mHeadingHoldTimer) - 1;
            } else if (((unsigned)RandomIntInternal(&data_0209e650.value) >> 16) & 3) {
                goomba->mTargetAngleY =
                    goomba->mPrevAngleY +
                    (s16)((unsigned)RandomIntInternal(&data_0209e650.value) >> 16);
                goomba->mHeadingHoldTimer = 0x64;
            } else {
                goomba->mTargetAngleY =
                    (s16)((unsigned)RandomIntInternal(&data_0209e650.value) >> 16);
                goomba->BeginJump();
            }
        }
    }
    if (goomba->mCapId >= 6) {
        if (goomba->mStuckTimer > 0x1e)
            goomba->mUnstickTimer = goomba->mStuckTimer;
    }
    ApproachLinear(*(short *)&goomba->mPrevAngleY, goomba->mTargetAngleY, step);
}

/* STATE_WALK for a king minion (mGoombaType 3): follow the Goomba King
 * (mTargetUniqueID). If the king is gone it becomes an ordinary KURIBO
 * (mGoombaType = GOOMBA_NORMAL).
 *
 * The standing animation (data_ov084_02130cc8) plays while mTargetHorzSpeed
 * is 0, the walk animation otherwise. func_ov074_0212087c turns the king and
 * mMinionIndex into the spot (targetPos) this minion should stand at. When
 * ApproachAngle (mPrevAngleY toward mInitAngleY) returns 0 and the Goomba is
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
void daKrb_c::func_ov084_0212af74() {
    daKrb_c *goomba = this;
    Vector3 targetPos;
    daKuriKing_c *king;
    s32 dist;
    s32 flag;
    s16 ang;
    u32 rnd;
    s32 lvl;
    u32 id;

    id = goomba->mTargetUniqueID;
    if (id == 0) {
        goomba->mGoombaType = daKrb_c::GOOMBA_NORMAL;
        return;
    }
    king = (daKuriKing_c *)dActor_c::FindWithID(id);
    if (king == 0) {
        goomba->mGoombaType = daKrb_c::GOOMBA_NORMAL;
        return;
    }

    if (goomba->mTargetHorzSpeed == 0) {
        goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130cc8.filePtr, 0, 0x1000, 0);
    } else {
        goomba->mAnmModel.SetAnim((BCA_File *)data_ov084_02130ce8.filePtr, 0, 0x1000, 0);
    }

    func_ov074_0212087c(&targetPos, king, goomba->mMinionIndex);

    if (ApproachAngle(&goomba->mPrevAngleY, goomba->mInitAngleY, 4, 0x1000, 0x400) == 0 &&
        goomba->mWithMeshClsn.IsOnGround() != 0)
    {
        Vec3_HorzDist(&goomba->mPos, &goomba->mHomePos);
        dist = Vec3_HorzDist(&goomba->mPos, &targetPos);

        if ((king->mState == 4 && king->mWalkSpeed == 0) ||
            DecIfAbove0_Byte(&goomba->mFreeRoamTimer) != 0 || dist > 0x3e8000) {
            ang = goomba->HorzAngleToCPlayer();
            flag = 1;
            if (goomba->DistToCPlayer() < 0x3e8000 &&
                AngleDiff(ang, goomba->mAngleY) < 0x3000)
            {
                goomba->mInitAngleY = goomba->HorzAngleToCPlayer();
                goomba->mTargetAngleY = goomba->mInitAngleY;
                lvl = goomba->mGoombaType;
                if (goomba->mTargetHorzSpeed == data_ov084_02130228[lvl]) {
                    goomba->mTargetHorzSpeed = data_ov084_02130268[lvl];
                }
                lvl = goomba->mGoombaType;
                if (goomba->mTargetHorzSpeed != data_ov084_02130268[lvl]) {
                    flag = 0;
                    goomba->mVertSpeed = data_ov084_02130248[lvl];
                    goomba->mHorzSpeed = 0;
                    goomba->mTargetHorzSpeed = data_ov084_02130268[goomba->mGoombaType];
                }
                if (goomba->mWithMeshClsn.JustHitGround() != 0) {
                    goomba->LandingDust(1);
                }
            } else {
                if (DecIfAbove0_Short(
                        reinterpret_cast<unsigned short *>(&goomba->mWanderRerollTimer)) == 0) {
                    rnd = RandomIntInternal(&data_0209e650.value);
                    goomba->mWanderRerollTimer = (s16)((rnd >> 0x1b) + 0x1e);
                    goomba->mInitAngleY = (s16)(rnd >> 0x10);
                }
                goomba->mTargetHorzSpeed = 0;
            }
            if (flag != 0) {
                ApproachLinear(*(int *)&goomba->mHorzSpeed, goomba->mTargetHorzSpeed, 0x500);
            }
        } else {
            goomba->mInitAngleY = Vec3_HorzAngle(&goomba->mPos, &targetPos);
            if (Vec3_HorzDist(&goomba->mPos, &targetPos) < 0x32000) {
                goomba->mTargetHorzSpeed = king->mGoombaTargetSpeed >> 2;
            } else if (Vec3_HorzDist(&goomba->mPos, &targetPos) < 0x64000) {
                goomba->mTargetHorzSpeed = king->mGoombaTargetSpeed >> 1;
            } else if (Vec3_HorzDist(&goomba->mPos, &targetPos) < 0x96000) {
                goomba->mTargetHorzSpeed = king->mGoombaTargetSpeed;
            } else {
                s32 idx = goomba->mGoombaType;
                s32 v = data_ov084_02130268[idx];
                goomba->mTargetHorzSpeed = v + king->mGoombaTargetSpeed;
            }
            Math_Function_0203b14c((int *)&goomba->mHorzSpeed, goomba->mTargetHorzSpeed, 0x800, 0x10000, 4);
        }
    }

    if (goomba->mPosY < goomba->mHomePos.y - 0x3e8000) {     /* 1000.0 below home */
        goomba->MarkForDestruction();
    }

    if (king->mState != 4)
        return;
    if (king->mWalkSpeed == 0)
        goomba->mFreeRoamTimer = 0x1e;
}

/* STATE_WALK handler: the king's minions follow the king, everything else
 * wanders / chases. mAngleY then follows mPrevAngleY.
 * N64: goomba_act_walk (func_ov084_0212abd4 holds its body; its target picker
 * func_ov084_02129cf4 takes treat_far_home_as_mario's 1000.0). The minion
 * branch is DS-only. */
void daKrb_c::ActWalk() {
    daKrb_c *goomba = this;
    if (goomba->mGoombaType == daKrb_c::GOOMBA_KING_MINION)
        goomba->func_ov084_0212af74();
    else
        goomba->func_ov084_0212abd4();
    goomba->mAngleY = goomba->mPrevAngleY;
}

/* Returns a Fix12 value by size: 65.0 for KURIBO, 150.0 for KURIBO_L and
 * 20.0 for KURIBO_S. */
int daKrb_c::OnAimedAtWithEgg()
{
    int normal = (actorID == (unsigned short)ACTOR_KURIBO) ? 1 : 0;
    if (normal)
        return 0x41000;
    int large = (actorID == (unsigned short)ACTOR_KURIBO_L) ? 1 : 0;
    if (large)
        return 0x96000;
    return 0x14000;
}

/* Yoshi turns the Goomba into an egg. A Goomba with a cap (mCapId < 6) or
 * REWARD_SILVER_STAR_IF_CURRENT is first put back at mHomePos. If
 * OnYoshiTryEat() is 6 (KURIBO): a daPly_c that IsCollectingCap gets the coin
 * and the reward helper; otherwise the Goomba's coin and star state go to
 * RegisterEggCoinCount (b5 = it carries a plain coin, b4 = it carried a
 * silver star). If it is 4 (KURIBO_S) only the coin is given. Finally the
 * Goomba is removed (func_ov084_02129498). */
void daKrb_c::OnTurnIntoEgg(daPly_c &playerRef) {
    /* self is unused; deleting it drops @NNN numbers daKrb_c.json pins. */
    dActor_c *self = this;
    daPly_c *player = &playerRef;
    int b5;
    bool b4;

    if ((mCapId & 0xf) < 6 || mRewardType == REWARD_SILVER_STAR_IF_CURRENT) {
        mPosX = mHomePos.x;
        mPosY = mHomePos.y;
        mPosZ = mHomePos.z;
        RespawnIfHasCap();
    }

    if (((dActor_c *)this)->OnYoshiTryEat() == 6) {
        if (player->IsCollectingCap()) {
            if (mCoinType == COIN_PLAIN)
                GivePlayerCoins(*player, 1, 0);
            func_ov084_021296cc();
        } else {
            b5 = 0;
            b4 = b5;
            if (mCoinType == COIN_PLAIN)
                b5 = 1;
            if (mRewardType == REWARD_SILVER_STAR) {
                UntrackStar(mStarTracked);
                b4 = 1;
                dActor_c::Spawn(ACTOR_STARBASE, 0x50, mHomePos, 0, mAreaId, -1);
                /* The (int) cast keeps [rN,#8] on the load: a plain
                   `param1 & 0xff0f` lets mwccarm CSE the field address. */
                param1 = (int)param1 & 0xff0f;
            } else if (mRewardType == REWARD_SILVER_STAR_IF_CURRENT) {
                if (mStarID == data_0209f344[data_0209f208[0]]) {
                    UntrackStar(mStarTracked);
                    mRewardType = REWARD_SPENT;
                    b4 = 1;
                }
            }
            player->RegisterEggCoinCount(b5, b4, 0);
        }
    } else if (((dActor_c *)this)->OnYoshiTryEat() == 4) {
        if (mCoinType == COIN_PLAIN)
            GivePlayerCoins(*player, 1, 0);
    }

    func_ov084_02129498();
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
                unsigned char *p = (unsigned char *)((int)&a->mSpawnedCount);
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
        mScaleX = FX_MUL(mScaleX, data_ov084_02130258[mGoombaType]);
        mScaleY = FX_MUL(mScaleY, data_ov084_02130258[mGoombaType]);
        mScaleZ = FX_MUL(mScaleZ, data_ov084_02130258[mGoombaType]);
    }

    mAnmModel.Render(&mScale);

    mScale = backup;
    mMaterialChanger.Update(mAnmModel.data);
    RenderCapModel(0);
    return 1;
}

/* Per-frame update; always returns 1.
 *
 * In order: func_ov084_02129864 (claim the star slot) and func_ov084_021290d4
 * (chase radius and unstick timer). GetCapState 0 ends the frame; 1 sets
 * mFlags bit 0x10000000 and poofs. A Goomba that is not a king minion,
 * tumbling, held by Yoshi or dying, and more than 1500.0 from the player, is
 * handed to Unk_02005d94 and the frame ends. While mDeathState != 0 the death
 * runs (UpdateKillByInvincibleChar, then func_ov084_021298d0, which ends it)
 * and the frame ends. func_ov084_02129a00 (Yoshi / hit front end) may end the
 * frame. The animation speed is 1.0 in STATE_TUMBLE and STATE_PAUSE and for a
 * standing king minion, else horizontal speed / (2 * scale) capped at 3.0.
 * Footstep sound and animation advance run except when airborne. The state
 * handler is taken from data_ov084_02130d74[mState]; mStateTimer counts up and
 * is zeroed when the state changes. Then the hit reaction
 * (func_ov084_02129ed4), UpdatePos, the ledge check (a step that would carry
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

    func_ov084_02129864();
    func_ov084_021290d4();
    r = GetCapState();
    if (r == 0)
        return 1;
    if (r == 1) {
        *(u32 *)((char *)&mFlags) |= 0x10000000;
        PoofDust();
    }
    if (mGoombaType != GOOMBA_KING_MINION && mState != STATE_TUMBLE && mEatenByYoshi == 0 &&
        mDeathState == DEATH_NONE &&
        ((dActor_c *)(char *)this)->IsTooFarAwayFromPlayer(0x5dc000) != 0) /* 1500.0 */
    {
        Unk_02005d94();
        return 1;
    }

    if (mDeathState != DEATH_NONE) {
        r = UpdateKillByInvincibleChar(mWithMeshClsn, mAnmModel, 3);
        if (r != 0) {
            if (r == 2) {
                func_ov084_02129498();
                v1.x = 0;
                v1.y = 0x6c000; /* 108.0 */
                v1.z = 0;
                ReleaseCap(v1);
                mPosX = mHomePos.x;
                mPosY = mHomePos.y;
                mPosZ = mHomePos.z;
                mAngleX = 0;
                mAngleY = 0;
                mAngleZ = 0;
                RespawnIfHasCap();
                func_ov084_021296cc();
            }
            return 1;
        }
        if (func_ov084_021298d0() == 0)
            func_ov084_0212a580();
        return 1;
    }

    if (func_ov084_02129a00() != 0)
        return 1;

    if (mState >= STATE_TUMBLE || (mGoombaType == GOOMBA_KING_MINION &&
                                   (int)mAnmModel.file == (int)data_ov084_02130cc8.filePtr)) {
        mAnmModel.speed = 0x1000;
    } else {
        int v = FX_Div(mHorzSpeed, mScaleX * 2);
        if (v > 0x3000)
            v = 0x3000;
        mAnmModel.speed = v;
    }

    MakeVanishLuigiWork(*(dCc_c *)&mdCcAc_c);

    if (mState != STATE_JUMP) {
        func_ov084_0212934c();
        ((dExtFrameCtrl_c *)&mAnmModel)->Advance();
    }

    st = mState;
    (this->*data_ov084_02130d74[st])();

    {
        u16 *hp = (u16 *)((char *)&mStateTimer);
        *hp += 1;
        if (st != mState)
            *hp = 0;
    }

    func_ov084_02129ed4();

    if (mCapId < 6)
        UpdatePos(0);
    else
        UpdatePos((dCc_c *)&mdCcAc_c);

    if (mDeathState == DEATH_NONE && mState != STATE_JUMP && mState != STATE_TUMBLE) {
        /* Args: 50.0 reach, angle limit 0x1f49 (about 44 degrees), 0, 1, 50.0. */
        if (IsGoingOffCliff(mWithMeshClsn, 0x32000, 0x1f49, 0, 1, 0x32000) != 0) {
            mPosX = mSafePos.x;
            mPosY = mSafePos.y;
            mPosZ = mSafePos.z;
        } else {
            mSafePos.x = mPosX;
            mSafePos.y = mPosY;
            mSafePos.z = mPosZ;
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

    func_ov084_021294d0();
    ((dCc_c *)&mdCcAc_c)->Clear();
    if (mDeathState == DEATH_NONE)
        ((dCc_c *)&mdCcAc_c)->Update();
    func_ov084_0212a580();
    func_ov084_02129238();

    do {
        if (mState == STATE_WALK) {
            int b = (mFlags & 8) ? 1 : 0;
            if (b == 0) {
                if (Vec3_Dist(&mPos, &mStuckCheckPos) < 0xa000) { /* moved less than 10.0 */
                    *(u16 *)((char *)&mStuckTimer) += 1;
                    if (mCapId < 6 && mStuckTimer == 0x1e) {
                        BeginJump();
                        mUnstickTimer = 0x5a;
                    }
                    if (mStuckTimer >= 0x12c && mUnstickTimer == 0) {
                        func_ov084_021296cc();
                        SpawnCoin();
                        func_ov084_02129498();
                        v2.x = 0;
                        v2.y = 0x6c000; /* 108.0 */
                        v2.z = 0;
                        ReleaseCap(v2);
                        mPosX = mHomePos.x;
                        mPosY = mHomePos.y;
                        mPosZ = mHomePos.z;
                        RespawnIfHasCap();
                        return 1;
                    }
                } else {
                    mStuckTimer = 0;
                    mStuckCheckPos.x = mPosX;
                    mStuckCheckPos.y = mPosY;
                    mStuckCheckPos.z = mPosZ;
                }
                break;
            }
        }
        if (mUnstickTimer == 0)
            mStuckTimer = 0;
    } while (0);
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
 * Fix12 size arguments. mSavedParam keeps the original param1 for the end of
 * STATE_TUMBLE. */
int daKrb_c::InitResources() {
    dActor_c *actor = this;
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
        /* The (int) cast is load-bearing: a plain `param1 & 0xf0ff` lets mwcc
           CSE the field address across the RMW, and the ROM re-issues it. See
           notes/mwccarm-codegen.md. */
        param1 = (int)param1 & 0xf0ff;

    if (DestroyIfCapNotNeeded() == 0)
        return 0;

    if (((dExtModel_c *)&mAnmModel)->SetFile((BMD_File *)(data_ov084_02130cf8.filePtr), 1, -1) == 0)
        return 0;

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    dExtAnmMaterial_c::Prepare(*(BMD_File *)data_ov084_02130cf8.filePtr,
                               *(BMA_File *)&data_ov084_0213089c);
    mMaterialChanger.SetFile(*(BMA_File *)&data_ov084_0213089c, 0x40000000, 0x1000, 0);

    mCoinType = COIN_PLAIN;

    {
        int id = actorID;
        int cond = (id == ACTOR_KURIBO_S);
        if (cond != false) {
            mGoombaType = GOOMBA_SMALL;
        } else {
            cond = (id == ACTOR_KURIBO);
            if (cond != false) {
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
        mScaleX = scale;
        mScaleY = scale;
        mScaleZ = scale;
    }
    mdCcAc_c.Init(actor, mScaleX * 0x3c, data_ov084_02130208[mGoombaType], 0x200000, 0xa6efe0);

    if (mGoombaType == GOOMBA_LARGE)
        *(int *)((int)&mdCcAc_c.vulnFlags) &= ~HIT_YOSHI_TONGUE;

    mWithMeshClsn.Init(actor, mScaleX * 0x3c, mScaleX * 0x3c, 0, 0);
    mWithMeshClsn.StartDetectingWater();

    mMoveFlags = 0;
    mState = STATE_WALK;
    mDeathState = DEATH_NONE;
    mClosestPlayer = 0;
    mDistToPlayer = 0x7fffffff;
    mInitAngleY = mPrevAngleY;
    mTargetHorzSpeed = data_ov084_02130228[mGoombaType];
    mHeadingHoldTimer = 0;
    mTargetUniqueID = 0;
    mWanderRerollTimer = 0;
    mStuckTimer = 0;
    mStuckCheckPos.x = mPosX;
    mStuckCheckPos.y = mPosY;
    mStuckCheckPos.z = mPosZ;
    mUnstickTimer = 0;
    func_ov084_021290d4();

    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;
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
    int normal = (actorID == (unsigned short)ACTOR_KURIBO) ? 1 : 0;
    if (normal)
        return 0x6;
    int small = (actorID == (unsigned short)ACTOR_KURIBO_S) ? 1 : 0;
    if (small)
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
