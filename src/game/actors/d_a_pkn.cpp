//cpp
/* daPkn_c (PAKUN / PIRANHA_PLANT 250), ov084 0x0212eaf0..0x02130174.
 *
 * RTTI names daPkn_c. Typeinfo at 0x02130bf8 points to the string at
 * 0x02130bec; the vtable header at 0x02130c24 points to that typeinfo.
 * The base is dEnemyBase_c. This vtable's 31 slots end at 0x02130ca4; seven
 * zero words follow. The section boundary alone does not establish its extent.
 *
 * Factory is `return new daPkn_c()`, through the inherited
 * fBase_c::operator new. Definitions run in reverse address order for
 * mwccarm 2004/b56. The inline destructor in the header emits the required
 * D1 and D0 pair; `new` odr-uses the class so that pair is emitted here.
 *
 * deslop leftovers:
 * - daPly_c::Hurt and daPly_c::Bounce stay mangled extern "C" calls: each
 *   takes a Fix12<int> by value.
 * - UpdatePose still walks VecFx32 views: spelling those copies as
 *   Vector3 is emitted differently.
 * - The shared files keep their address names (d_a_f_pkn.cpp reads them
 *   too); the state table at 0x02130e80 also keeps its address name.
 *
 * Boundary, layout and compiler experiments are recorded in
 * notes/data/class-facts/daPkn_c.json and
 * notes/agents/handoffs/pr-2450-source-review-fixes.md.
 */

#include "common.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "daPkn_c.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "Sound.h"
#include "math/Approach.h"
#include "SpawnInfo.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"

/* Runtime asset handles, distinct from the NitroFS file IDs. */
enum daPkn_c_AssetId {
    ASSET_PAKKUN_ATTACK_BCA = 0x03cf,
    ASSET_PAKKUN_DEAD_BCA = 0x03d0,
    ASSET_PAKKUN_FIRE_BCA = 0x03d1,
    ASSET_PAKKUN_FIRE_ED_BCA = 0x03d2,
    ASSET_PAKKUN_MODEL_BMD = 0x03d3,
    ASSET_PAKKUN_SLEEP_BCA = 0x03d4,
    ASSET_PAKKUN_SLEEP_LOOP_BCA = 0x03d5
};

/* The handles reach their files through these wrappers' constructor and
   destructor veneers; the manifest maps each to the retail destination.
   Descriptive local types, not recovered spellings. */
struct PknModelFile : SharedFilePtr {
    PknModelFile(u32 fileID);
    ~PknModelFile();
};

struct PknAnimFile : SharedFilePtr {
    PknAnimFile(u32 fileID);
    ~PknAnimFile();
};

/* The piranha plants' shared files, in the order the retail initializer
   (0x02130654) builds them. d_a_f_pkn.cpp reads them too. */
PknModelFile data_ov084_02130dfc(ASSET_PAKKUN_MODEL_BMD);     /* the plant's own model */
PknAnimFile data_ov084_02130e14(ASSET_PAKKUN_ATTACK_BCA);     /* lunge animation */
PknAnimFile data_ov084_02130e24(ASSET_PAKKUN_DEAD_BCA);       /* death animation */
PknAnimFile data_ov084_02130e0c(ASSET_PAKKUN_SLEEP_BCA);      /* recoil animation */
PknAnimFile data_ov084_02130df4(ASSET_PAKKUN_SLEEP_LOOP_BCA); /* idle animation */
PknAnimFile data_ov084_02130e04(ASSET_PAKKUN_FIRE_BCA);       /* spit animation */
PknAnimFile data_ov084_02130e1c(ASSET_PAKKUN_FIRE_ED_BCA);    /* post-spit animation */

extern SharedFilePtr
    data_ov002_0210da38; /* BUBBLE_MODEL_PTR -- sleep bubble, shared out of ov002 */

/* Six animation handles, held indirectly: the table is pointers, not objects. */
extern SharedFilePtr *data_ov084_021302f4[];

/* Behavior calls the current state through this actor's member-pointer
   table, indexed by state. */
typedef void (daPkn_c::*PknStatePMF)();

PknStatePMF data_ov084_02130e80[9] = {
    &daPkn_c::StateWait,   &daPkn_c::StateSleep, &daPkn_c::StateBite,
    &daPkn_c::StateWake,   &daPkn_c::StateDoze,  &daPkn_c::StateDie,
    &daPkn_c::StateShrink, &daPkn_c::StateGone,  &daPkn_c::StateRegrow,
};

/* C-linkage declarations for the remaining raw entry points and shared data. */


/* -- other modules -- */
void  func_02012694(u32 id, void *pos);
void  func_020105cc(void *thiz, u32 flags);
extern "C" {



/* -- shared tables -- */
extern int data_ov084_0213030c[];        /* per-frame particle X/Z offset (0212f460, 0xfb) */
extern int data_ov084_02130334[];        /* per-frame particle Y offset */
extern u8  data_ov084_021302ec[];        /* five indices {0,3,4,5,6}; 0212ec60 sums s16 at [i]*0x34+0x1c into ang[1] (dead -- later y-adjust reads ang[0]==0, so the offset is constant 0x32000). 0x34 is not BMD_Bone (0x40). */

int *Vec3_LslInPlace(int *v, int sh);

void *d_a_pknNewHold(u32 handle, u32 id, int x, int y, int z, const void *f, void *cb);
int d_a_pknPlaySubHold(u32 a, u32 b, u32 c, int t, int flag);
}


/* ==========================================================================
 *
 * Allocates sizeof(daPkn_c) (0x47c) through fBase_c::operator new and
 * inlines the implicit constructor: dEnemyBase_c C2, vptr store, then the
 * six member C1s in declaration order.
 * ======================================================================== */

extern "C" daPkn_c *daPkn_c_classInit()
{
    return new daPkn_c();
}

/* The actor profile, in SpawnInfo's layout (include/SpawnInfo.h). The
 * retail values are unchanged. */
SpawnInfo<daPkn_c> g_profile_PAKUN = {
    daPkn_c_classInit, /* classInit */
    0x00fa,            /* behaviorPriority */
    0x006c,            /* renderPriority */
    0x10000002,        /* actorFlags */
    0x64000,           /* clipOffsetY */
    0xc8000,           /* clipRadius */
    0x1000000,         /* clipDistance */
    0x1000000          /* farDistance */
};

/* -------------------------------------------------------------------------- */
/*
 * Unlike daFPkn_c's, this one checks its loads: a failed SetFile on either
 * model returns 0.
 *
 * The tail seeds the sleep-bubble position: 0xe0 along the facing angle
 * (FX_SinIdx / FX_CosIdx), and 0x37800 above mPosY.
 * UpdatePose rewrites mBubblePos every frame.
 */
int daPkn_c::InitResources()
{
    int i;
    Vector3 v;
    for (i = 0; i < 6; i++)
        dExtFrameCtrl_c::LoadFile(*data_ov084_021302f4[i]);
    LoadBlueCoinModel();
    dExtSimpleModel_c::LoadFile(data_ov084_02130dfc);
    dExtSimpleModel_c::LoadFile(data_ov002_0210da38);
    if (mAnmModel.SetFile((BMD_File *)data_ov084_02130dfc.filePtr, 1, -1) == 0)
        return 0;
    if (mModel.SetFile((BMD_File *)data_ov002_0210da38.filePtr, 1, -1) == 0)
        return 0;
    mdCcAc_c1.Init(this, 0x32000, 0x64000, 0x200004, 0x423e0);
    mdCcAc_c2.Init(this, 0x82000, 0x64000, 2, 0x423e0);
    v.x = 0; v.y = 0; v.z = 0;
    mdCcAcPos_c.Init(this, v, 0x46000, 0x64000, 0x200002, 0);
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mState = 0;
    mTargetAngleY = mPrevAngleY;
    mPlayerDist = 0x7fffffff;
    mClosestPlayer = 0;
    mClsnEnabled = 0;
    mSubSoundFadedOut = 1;
    mStateTimer = 0;
    mCoinType = 3;
    mPlayerAirborne = 0;
    mWithMeshClsn.Init(this, 0x64000, 0x64000, 0, 0);
    mHeadClsnOffset.x = mPosX;
    mHeadClsnOffset.y = mPosY;
    mHeadClsnOffset.z = mPosZ;
    {
        /* Copied through a pointer: `mHomePos = mHeadClsnOffset` emits a
           different copy. */
        Vector3 *home = &mHeadClsnOffset;
        mHomePos = *home;
        /* The shift must be LOGICAL so the angle wraps -- on the signed s16 it
           would index the wrong table entry for negative angles. */
        int ang = (u16)mAngleY >> 4;
        /* Temporaries in the ROM's load order; folding them reorders it. */
        int y0 = mPosY;
        int z0 = mPosZ;
        int cosv = FX_CosIdx(ang);
        int sinv = FX_SinIdx(ang);
        int z = cosv * 0xe0 + z0;
        int y = y0 + 0x37800;
        int x = sinv * 0xe0 + mPosX;
        mBubblePos.x = x;
        mBubblePos.y = y;
        mBubblePos.z = z;
    }
    mParticleHandle2 = 0;
    mParticleHandle = mParticleHandle2;
    mLoopSoundHandle = 0;
    mAnmModel.SetAnim((BCA_File *)data_ov084_02130df4.filePtr, 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
int daPkn_c::Behavior()
{
    int r;
    int old;
    int cur;
    r = UpdateKillByInvincibleChar(mWithMeshClsn, mAnmModel, 1);
    if (r != 0) {
        if (r == 2) {
            mCoinType = 0;
            mState = 7;
            mScaleX = 0;
            mScaleY = 0;
            mScaleZ = 0;
            mPosX = mHomePos.x;
            mPosY = mHomePos.y;
            mPosZ = mHomePos.z;
        }
        return 1;
    }
    MakeVanishLuigiWork(mdCcAc_c1);
    mAnmModel.Advance();
    TrackClosestPlayer();
    old = mState;
    (this->*data_ov084_02130e80[old])();
    mStateTimer++;
    cur = mState;
    if (old != cur) {
        if (cur == 5) {
            mFlags &= ~0x10000000;
        }
        mStateTimer = 0;
        mLoopSoundHandle = 0;
    }
    UpdatePose();
    mdCcAc_c1.Clear();
    mdCcAc_c2.Clear();
    mdCcAcPos_c.Clear();
    if (mClsnEnabled != 0) {
        mdCcAc_c1.Update();
        mdCcAc_c2.Update();
        if (mState == 2) {
            mdCcAcPos_c.SetPosRelativeToActor(mHeadClsnOffset);
            mdCcAcPos_c.Update();
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/*
 * The bubble is drawn only while the idle animation (data_ov084_02130df4)
 * is playing.
 */
int daPkn_c::Render()
{
    if (mScaleX == 0)
        return 1;
    mAnmModel.Render(&mScale);
    if ((int)mAnmModel.file == (int)data_ov084_02130df4.filePtr)
        mModel.Render(&mBubbleScale);
    return 1;
}

/* -------------------------------------------------------------------------- */
/*
 * fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`.
 */
void daPkn_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
int daPkn_c::CleanupResources()
{
    int i;
    data_ov084_02130dfc.Release();
    for (i = 0; i < 6; i++)
        data_ov084_021302f4[i]->Release();
    UnloadBlueCoinModel();
    data_ov002_0210da38.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
void daPkn_c::StateWait()
{
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mClsnEnabled = 1;
    mFlags |= 0x10000000;
    mAnmModel.SetAnim((BCA_File *)data_ov084_02130df4.filePtr, 0, 0x1000, 0);
    if (mPlayerDist < 0x4b0000)
        mState = 1;
    CheckHits();
}

/* -------------------------------------------------------------------------- */
void daPkn_c::StateSleep()
{
    mClsnEnabled = 1;
    if (mAnmModel.Finished() || mAnmModel.WillHitFrame(0))
        mAnmModel.SetAnim((BCA_File *)data_ov084_02130df4.filePtr, 0, 0x1000, 0);
    int thr = (mPlayerAirborne != 0) ? 0x12c000 : 0x190000;
    if (mPlayerDist < thr && PlayerMovingFast()) {
        mAnmModel.SetAnim((BCA_File *)data_ov084_02130e0c.filePtr, 0x40000000, 0x1000, 0);
        mState = 3;
        func_02012694(0x175, &mCamSpacePosX);
        Particle::System::NewSimple(kPtclPiranhaWakeA, mBubblePos.x, mBubblePos.y, mBubblePos.z);
        Particle::System::NewSimple(kPtclPiranhaWakeB, mBubblePos.x, mBubblePos.y, mBubblePos.z);
        return;
    }
    mLoopSoundHandle = Sound::PlayLong(mLoopSoundHandle, 3, 0x174, mCamSpacePos, 0);
    if (mPlayerDist < 0x352000) {
        Sound::PlaySub(0x36, 0, 0x7f, 0x1451, 0);
        mSubSoundFadedOut = 0;
    } else if (mSubSoundFadedOut == 0) {
        if (Sound::PlaySub(0x36, 0x7f, 0, 0x1451, 0))
            mSubSoundFadedOut = 1;
    }
    CheckHits();
}

/* -------------------------------------------------------------------------- */
void daPkn_c::StateBite()
{
    dActor_c *actor;
    u16 type;
    int isPlayer;
    int isNine;
    u32 flags;
    Vector3 pos1;
    Vector3 pos2;

    /* isPlayer and isNine hold the compares as ints: testing
       `type == 0xbf` directly changes the code. */
    mClsnEnabled = 1;

    if (mAnmModel.WillHitFrame(0x10) || mAnmModel.WillHitFrame(0x20) ||
        mAnmModel.WillHitFrame(0x34) || mAnmModel.WillHitFrame(0x4b)) {
        func_0201267c(0xc0, &mCamSpacePos);
    }

    ApproachLinear(mPrevAngleY, mTargetAngleY, 0x800);
    mAngleY = mPrevAngleY;

    if (mAnmModel.Finished() != 0) {
        int thr;
        if (mPlayerAirborne != 0)
            thr = 0x12c000;
        else
            thr = 0x190000;
        thr = thr + 0x64000;
        if (mPlayerDist > thr)
            mState = 4;
        else
            mAnmModel.currFrame = 0;
    }

    /* Body cylinder. */
    if (mdCcAc_c1.otherOwner != 0) {
        actor = dActor_c::FindWithID(mdCcAc_c1.otherOwner);
        if (actor != 0) {
            type = actor->actorID;
            isPlayer = (int)(type == 0xbf);
            if (isPlayer != 0) {
                flags = mdCcAc_c1.hitFlags;
                if (flags & 0x10) {
                    ((daPly_c *)actor)->IncMegaKillCount();
                    func_02012694(0x1d, &mCamSpacePosX);
                    Die();
                    return;
                }
                if (((daPly_c *)actor)->mIsMetal != 0) {
                    Die();
                    return;
                }
                if (((daPly_c *)actor)->mIsVanish == 0) {
                    pos1.x = mPosX;
                    pos1.y = mPosY;
                    pos1.z = mPosZ;
                    ((daPly_c *)actor)->Hurt(pos1, 3, 0xc000, 1, 0, 1);
                    return;
                }
                if ((flags & 0x40000) == 0)
                    return;
                Die();
                return;
            }
            if ((mdCcAc_c1.hitFlags & 0x2000) != 0) {
                isNine = (int)(type == 9);
                if (isNine != 0) {
                    Die();
                    return;
                }
            }
        }
    }

    /* Second body cylinder. Its mega test reads mdCcAc_c1's hit flags. */
    if (mdCcAc_c2.otherOwner != 0) {
        actor = dActor_c::FindWithID(mdCcAc_c2.otherOwner);
        if (actor != 0) {
            type = actor->actorID;
            isPlayer = (int)(type == 0xbf);
            if (isPlayer != 0) {
                if ((mdCcAc_c1.hitFlags & 0x10) != 0) {
                    ((daPly_c *)actor)->IncMegaKillCount();
                    func_02012694(0x1d, &mCamSpacePosX);
                    Die();
                    return;
                }
                if (((daPly_c *)actor)->mIsMetal != 0) {
                    Die();
                    return;
                }
                if ((mdCcAc_c2.hitFlags & 0x40000) == 0)
                    return;
                Die();
                return;
            }
            if ((mdCcAc_c2.hitFlags & 0x2000) != 0) {
                isNine = (int)(type == 9);
                if (isNine != 0) {
                    Die();
                    return;
                }
            }
        }
    }

    /* Head cylinder, players only. */
    if (mdCcAcPos_c.otherOwner == 0)
        return;
    actor = dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
    if (actor == 0)
        return;
    type = actor->actorID;
    isPlayer = (int)(type == 0xbf);
    if (isPlayer == 0)
        return;
    if ((mdCcAcPos_c.hitFlags & 0x10) != 0) {
        ((daPly_c *)actor)->IncMegaKillCount();
        func_02012694(0x1d, &mCamSpacePosX);
        Die();
        return;
    }
    if (((daPly_c *)actor)->mIsMetal != 0)
        return;
    pos2.x = mPosX;
    pos2.y = mPosY;
    pos2.z = mPosZ;
    ((daPly_c *)actor)->Hurt(pos2, 3, 0xc000, 1, 0, 1);
}

/* -------------------------------------------------------------------------- */
void daPkn_c::StateWake()
{
    Sound::PlaySub(0x36, 0x7f, 0, 0xcb33, 0);
    if (CheckHits() != 0) return;
    if (mStateTimer <= 0xb)
        return;
    if (!mAnmModel.Finished())
        return;
    mAnmModel.SetAnim((BCA_File *)data_ov084_02130e14.filePtr, 0x40000000, 0x1000, 0);
    mState = 2;
}

/* -------------------------------------------------------------------------- */
void daPkn_c::StateDoze()
{
    mClsnEnabled = 1;
    if (mAnmModel.Finished()) {
        mAnmModel.SetAnim((BCA_File *)data_ov084_02130e0c.filePtr, 0x40000000, 0x1000, 0);
        mState = 1;
    } else {
        int thresh = (mPlayerAirborne != 0) ? 0x12c000 : 0x190000;
        if (mPlayerDist < thresh) {
            if (PlayerMovingFast() != 0) {
                mAnmModel.currFrame = 0;
                mState = 2;
            }
        }
    }
}

/* -------------------------------------------------------------------------- */
void daPkn_c::StateDie()
{
    Vector3 pos;

    Sound::PlaySub(0x36, 0x7f, 0, 0xcb33, 0);
    mClsnEnabled = 0;
    if (mStateTimer < 0xa) {
        pos.x = mPosX;
        pos.y = mPosY;
        pos.z = mPosZ;
        pos.x += data_ov084_0213030c[mStateTimer] * FX_SinIdx((u16)mAngleY >> 4);
        pos.z += data_ov084_0213030c[mStateTimer] * FX_CosIdx((u16)mAngleY >> 4);
        pos.y += data_ov084_02130334[mStateTimer] << 12;
        mParticleHandle = (s32)Particle::System::New(mParticleHandle, kPtclPiranhaDeathBurst, pos.x,
                                                     pos.y, pos.z, 0, 0);
    }
    if (mAnmModel.Finished() == 0)
        return;
    mState = 6;
    mParticleHandle2 = 0;
    mParticleHandle = mParticleHandle2;
}

/* -------------------------------------------------------------------------- */
void daPkn_c::StateShrink()
{
    int scale;

    mClsnEnabled = 0;
    scale = mScaleX;
    if (mStateTimer == 0) {
        func_02012694(0x11f, &mCamSpacePosX);
        scale = 0x1000;
    }

    mParticleHandle = (s32)Particle::System::New(mParticleHandle, kPtclPiranhaDeathSmokeA, mPosX,
                                                 mPosY + 0x1e000, mPosZ, 0, 0);
    mParticleHandle2 = (s32)Particle::System::New(mParticleHandle2, kPtclPiranhaDeathSmokeB, mPosX,
                                                  mPosY + 0x1e000, mPosZ, 0, 0);

    if (scale > 0) {
        scale = scale - 0xa3;
    } else {
        scale = 0;
        if (mCoinType != 0) {
            Vector3 v(mPosX, mPosY + 0x78000, mPosZ);
            dActor_c::Spawn(0x122, 0, v, 0, mAreaId, -1);
            mCoinType = 0;
        }
        mState = 7;
    }
    mScaleX = scale;
    mScaleY = scale;
    mScaleZ = scale;
}

/* -------------------------------------------------------------------------- */
void daPkn_c::StateGone()
{
    mClsnEnabled = 0;
    if (mPlayerDist <= 0x4b0000) return;
    mState = 8;
    mParticleHandle2 = 0;
    mParticleHandle = mParticleHandle2;
    mAnmModel.SetAnim((BCA_File *)data_ov084_02130e0c.filePtr, 0, 0x1000, 0);
}

/* -------------------------------------------------------------------------- */
void daPkn_c::StateRegrow()
{
    int v;
    mClsnEnabled = 0;
    v = mScaleX;
    if (mStateTimer == 0)
        v = 0x4cc;
    if (v < 0x1000) {
        v += 0x52;
    } else {
        mState = 0;
        v = 0x1000;
    }
    mScaleX = v;
    mScaleY = v;
    mScaleZ = v;
}

/* -------------------------------------------------------------------------- */
/* Tracks the closest player: its distance, the yaw toward it and whether it
   is airborne. */
void daPkn_c::TrackClosestPlayer()
{
    Vector3 v;
    mClosestPlayer = ClosestPlayer();
    daPly_c *p = mClosestPlayer;
    if (p != 0) {
        /* Member by member: `v = *pos` emits a different copy. */
        Vector3 *pos = &p->mPos;
        v.x = pos->x;
        v.y = pos->y;
        v.z = pos->z;
        mPlayerDist = Vec3_Dist(&mPos, &v);
        mTargetAngleY = Vec3_HorzAngle(&mPos, &v);
        mPlayerAirborne = mClosestPlayer->mIsAirborne;
    } else {
        mPlayerDist = 0x7fffffff;
        mTargetAngleY = mAngleY;
    }
}

/* -------------------------------------------------------------------------- */
/* Is the closest player moving fast enough to wake the plant? */
int daPkn_c::PlayerMovingFast()
{
    daPly_c *p = mClosestPlayer;
    if (p == 0) return 0;
    if (p->mVertSpeed > 0xa000) return 1;
    return p->mHorzSpeed > 0xa000;
}

/* -------------------------------------------------------------------------- */
/* The collision check the idle states share: returns 1 when a hit killed
   the plant. t and u hold the actor-type compares as ints; testing
   `type == 0xbf` directly changes the code. */
int daPkn_c::CheckHits()
{
    daPly_c *actor;
    u32 flags;
    u16 type;
    u32 id;

    id = mdCcAc_c1.otherOwner;
    if (id != 0) {
        actor = static_cast<daPly_c *>(dActor_c::FindWithID(id));
        if (actor != 0) {
            type = actor->actorID;
            int t = (int)(type == 0xbf);
            if (t != 0) {
                flags = mdCcAc_c1.hitFlags & 0x26ff0;
                if (flags != 0) {
                    if (flags & 0x10) {
                        actor->IncMegaKillCount();
                        func_02012694(0x1d, &mCamSpacePosX);
                    }
                    func_020105cc(this, flags);
                    Die();
                } else if (JumpedOnByPlayer(mdCcAc_c1, *actor) != 0) {
                    actor->Bounce(0x28000);
                    Die();
                } else if (mdCcAc_c1.hitFlags & 0x40000) {
                    Die();
                } else {
                    Vector3 v;
                    v.x = mPosX;
                    v.y = mPosY;
                    v.z = mPosZ;
                    actor->Hurt(v, 3, 0xc000, 1, 0, 1);
                    mAnmModel.SetAnim((BCA_File *)data_ov084_02130e0c.filePtr, 0, 0x1000, 0);
                    mAnmModel.SetFlags(0x40000000);
                    mState = 3;
                    func_02012694(0x175, &mCamSpacePosX);
                    return 0;
                }
                return 1;
            } else if (mdCcAc_c1.hitFlags & 0x2000) {
                int u = (int)(type == 9);
                if (u != 0) {
                    Die();
                    return 1;
                }
            }
        }
    }

    id = mdCcAc_c2.otherOwner;
    if (id != 0) {
        actor = static_cast<daPly_c *>(dActor_c::FindWithID(id));
        if (actor != 0) {
            type = actor->actorID;
            int t = (int)(type == 0xbf);
            if (t != 0) {
                flags = mdCcAc_c2.hitFlags & 0x26ff0;
                if (flags != 0) {
                    if (flags & 0x10) {
                        actor->IncMegaKillCount();
                        func_02012694(0x1d, &mCamSpacePosX);
                    }
                    func_020105cc(this, flags);
                    Die();
                    return 1;
                }
                if (JumpedOnByPlayer(mdCcAc_c1, *actor) != 0) {
                    actor->Bounce(0x28000);
                    Die();
                    return 1;
                }
                if (mdCcAc_c2.hitFlags & 0x40000) {
                    Die();
                    return 1;
                }
            } else if (mdCcAc_c2.hitFlags & 0x2000) {
                int u = (int)(type == 9);
                if (u != 0) {
                    Die();
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* Poses both models and places the head cylinder and the sleep bubble. The
   bone sum into ang[1] is never read. */
void daPkn_c::UpdatePose()
{
    volatile s16 ang[3];
    /* common.h comes first, so Matrix4x3 is the flat twelve words and
       assigning it is one block copy. */
    struct {
        Matrix4x3 saved;
        VecFx32 tv;
        VecFx32 v;
    } L;
    int lr;
    int scale;

    Vec3_Asr((Vector3 *)&L.v, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, L.v.x, L.v.y, L.v.z);
    L.saved = data_020a0e68;
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    mAnmModel.mat4x3 = data_020a0e68;

    ang[0] = 0;
    ang[1] = 0;
    ang[2] = 0;
    for (lr = 0; lr < 5; lr++) {
        ang[1] = (s16)(ang[1] + *(s16 *)((char *)mAnmModel.data.bones +
                                         data_ov084_021302ec[lr] * 0x34 + 0x1c));
    }

    mHeadClsnOffset.x = 0;
    mHeadClsnOffset.y = 0;
    mHeadClsnOffset.z = 0;
    data_020a0e68 = L.saved;
    MTX_Concat43(&mAnmModel.data.transforms[6], &data_020a0e68, &data_020a0e68);
    mHeadClsnOffset.x = data_020a0e68.m[9];
    mHeadClsnOffset.y = data_020a0e68.m[10];
    mHeadClsnOffset.z = data_020a0e68.m[11];
    Vec3_LslInPlace((int *)&mHeadClsnOffset, 3);
    VEC_Subtract(&mHeadClsnOffset, &mPos, &mHeadClsnOffset);

    /* Still a hand-indexed FX_SinIdx: through the inline the table base loads
       before mHeadClsnOffset.y and ang[0], and no spelling tried (local, index
       local, y read first, split subtraction) gets the ROM's order back. */
    mHeadClsnOffset.y -= FX_SinCosTable_[((u16)ang[0] >> 4) << 1] * (s16)0x14 + 0x32000;
    mHeadClsnOffset.z += 0x32000;

    L.tv.x = mPosX;
    L.tv.y = mPosY;
    L.tv.z = mPosZ;

    {
        int rr = (int)((u32)(mAnmModel.currFrame << 4) >> 0x10);
        int d = rr - 0x28;
        if (d < 0)
            d = -d;
        scale = ((d << 12) / 10) + 0x400;
        if (scale < 0x800)
            scale = 0x800;
        mBubbleScale.x = scale;
        mBubbleScale.y = scale;
        mBubbleScale.z = scale;
    }

    L.tv.x = FX_SinIdx((u16)mAngleY >> 4) * (s16)0xe0 + L.tv.x;
    scale = scale - 0xc00;
    L.tv.z = FX_CosIdx((u16)mAngleY >> 4) * (s16)0xe0 + L.tv.z;
    L.tv.y = L.tv.y + (scale * 0x18 + 0x38000);

    mBubblePos.x = L.tv.x;
    mBubblePos.y = L.tv.y;
    mBubblePos.z = L.tv.z;

    Matrix4x3_FromTranslation(&mModel.mat4x3, L.tv.x >> 3, L.tv.y >> 3, L.tv.z >> 3);
}

/* -------------------------------------------------------------------------- */
/* Vtable slot 29. */
s32 daPkn_c::OnAimedAtWithEgg()
{
    return 0x46000;
}

/* -------------------------------------------------------------------------- */
/* Enters state 2 with the lunge animation at `frame`. */
void daPkn_c::EnterLunge(int frame)
{
    mState = 2;
    mAnmModel.SetAnim((BCA_File *)data_ov084_02130e14.filePtr, 0x40000000, 0x1000, 0);
    mAnmModel.currFrame = (u16)frame << 12;
}

/* -------------------------------------------------------------------------- */
/* Kills the plant: death animation and state 5. */
void daPkn_c::Die()
{
    func_0201267c(0xc1, &mCamSpacePos);
    mState = 5;
    mAnmModel.SetAnim((BCA_File *)data_ov084_02130e24.filePtr, 0x40000000, 0x1000, 0);
}
