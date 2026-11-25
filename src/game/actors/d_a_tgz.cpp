//cpp
/* daTgz_c -- the Spiny on the castle roof, ov077.
 *
 * Six PMF state records (SetState picks one): 0 Spin, 1 Walk, 2 Knockback,
 * 3 YoshiMouth, 4 Spit, 5 KnockedAway. State 0 is the
 * spinning bounce that settles back into the walk (state 1); state 4 spins
 * the same way after its carrier lets go. State 5 is the knock-away: it springs
 * up at vertical speed 40.0, flips backward 0x1000 a frame, and on landing
 * (or 45 frames later) drops a coin, poofs and is destroyed. It caches the
 * water surface under it once, splashes on entry, and poofs away if it sinks
 * too far or drops below course 0x1c's floor. Outside states 4 and 5, while
 * it is more than 1500.0 from the player it stops updating and counts
 * mDespawnTimer down from 44 frames, then is destroyed with no poof.
 *
 * Cartridge names: _ZTS7daTgz_c at 0x02127948, _ZTI7daTgz_c at 0x02127954,
 * vtable address point _ZTV7daTgz_c at 0x02127984. Thirty-four functions,
 * 0x02124b64..0x0212624c, between daJgm_c and daPopoi_c.
 *
 * Load-bearing:
 *   Function order is ROM-ascending. `#pragma defer_codegen off` makes
 *   CodeWarrior emit each definition where it stands, and the destructor pair
 *   comes out D1 then D0. One ~daTgz_c() emits both; a hand-mangled D0 next
 *   to it is the mwccarm ICE (ELFgen.c:483).
 *
 * deslop leftovers:
 * - St_Spit_Init and St_YoshiMouth_Main: carrier->mPos.x/.y/.z is -4
 *   (0x17c->0x178, 0xac->0xa8). The ROM keeps add #0x5c then [0]/[4]/[8].
 */
#include "decl_common.h"
#include "ParticleIds.h"
#include "Particle__System.h"
#include "nitro/fx/fx.h"
#include "daTgz_c.h"
#include "daPly_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "dBgCh_Gnd.h"
#include "decl_Particle.h"
#include "dBgCh_Actr.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "SpawnInfo.h"
#include "math/MtxStack.h"
#include "LevelRng.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/fx/fx_trig.h"

#pragma defer_codegen off

/* The BCA gTgzWalkAnimFile has loaded. */
#define TGZ_BCA (gTgzWalkAnimFile.filePtr)

/* Lakitu's model-file handles, owned and constructed by d_a_jgm.cpp's static
 * init. Same TU-local wrapper spelling as the owning TU. */
struct DaJgmModelFile : SharedFilePtr {
    DaJgmModelFile(u32 fileID);
    ~DaJgmModelFile();
};

/* Runtime asset handle, distinct from the NitroFS file ID. */
enum daTgz_c_AssetId { ASSET_TOGEZO_WALK_BCA = 0x042b };

/* The handle reaches its file through this wrapper's constructor and
 * destructor veneers; the manifest maps each to the retail destination.
 * Descriptive local type, not a recovered spelling. */
struct TgzAnimFile : SharedFilePtr {
    TgzAnimFile(u32 fileID);
    ~TgzAnimFile();
};

TgzAnimFile gTgzWalkAnimFile(ASSET_TOGEZO_WALK_BCA);

/* One {enter, update} record per state; SetState selects one and
 * CallStateEnter and CallStateUpdate call through it. State 1's entry returns
 * nothing, so its pointer is cast to the shared record type. */
typedef daTgz_c::StateFunc TgzState;
typedef daTgz_c::State TgzStateRecord;

TgzStateRecord gTgzStates[6] = {
    {&daTgz_c::St_Spin_Init, &daTgz_c::St_Spin_Main},
    {(TgzState)&daTgz_c::St_Walk_Init, &daTgz_c::St_Walk_Main},
    {&daTgz_c::St_Knockback_Init, &daTgz_c::St_Knockback_Main},
    {&daTgz_c::St_YoshiMouth_Init, &daTgz_c::St_YoshiMouth_Main},
    {&daTgz_c::St_Spit_Init, &daTgz_c::St_Spit_Main},
    {&daTgz_c::St_KnockedAway_Init, &daTgz_c::St_KnockedAway_Main},
};

extern "C" {
extern DaJgmModelFile data_ov077_02127b48;
extern DaJgmModelFile data_ov077_02127b38;

extern int gWaterHeight;
extern signed char gCurrentSublevel;
}
int SurfaceInfo_TestFlag0x20(int *p);
void func_02012694(unsigned int id, const Vector3 *pos);

/* Vtable slot 16. The compiler writes the whole body: one vtable store, the
 * members in reverse, then ~dActor_c. Slot 17 (D0) is the deleting variant of
 * this same definition. */
daTgz_c::~daTgz_c()
{
}

/* Vtable slot 18. */
int daTgz_c::OnYoshiTryEat()
{
    return 6;
}

/* Vtable slot 29: the height an egg aims at (30.0). */
int daTgz_c::OnAimedAtWithEgg()
{
    return 122880;
}

/* Until one is found, probe for a water surface below the Spiny and cache
 * its height in mWaterY. Returns how far above it the Spiny is (0 while
 * nothing has been found). */
int daTgz_c::GetHeightAboveWater() {
    if (this->mWaterY == 0) {
        dBgCh_Gnd rg;
        Vector3 pos;
        {
            int y = this->mPos.y;
            int z = this->mPos.z;
            int y2 = y + 0xc8000;
            int x = this->mPos.x;
            pos.x = x;
            pos.y = y2;
            pos.z = z;
        }
        rg.SetObjAndPos(pos, this);
        rg.StartDetectingWater();
        if (rg.DetectClsn() != 0 && SurfaceInfo_TestFlag0x20((int *)&rg.surface) != 0)
            this->mWaterY = rg.clsnY;
        else
            return 0;
        rg.StopDetectingWater();
    }
    return this->mPos.y - this->mWaterY;
}

/* Is the Spiny below the cached water surface? */
int daTgz_c::IsUnderwater() {
    int waterY = this->mWaterY;
    if (waterY == 0)
        return 0;
    return waterY > this->mPos.y;
}

/* Toxic floor destroys the Spiny. A sloped floor rewrites vertical speed
 * from mSpeed.x/mSpeed.z and pitches mAngle.x/mAngle.z to the normal. */
void daTgz_c::UpdateGroundCollision(dBgCh_Actr *clsn) {
    Vector3 pos;
    Vector3 normal;
    Vector3 wallnormal;

    dBgCh_Actr_UpdateDiscreteNoLava_veneer(clsn);
    if (clsn->IsOnGround()) {
        dBgCh_Gnd rc;
        {
            int actorY = this->mPos.y;
            int pz = this->mPos.z;
            int py = actorY + 0xc8000;
            pos.x = this->mPos.x;
            pos.y = py;
            pos.z = pz;
        }
        rc.StartDetectingToxic();
        rc.SetObjAndPos(pos, this);
        if (rc.DetectClsn()) {
            if (func_02037e20((int *)&rc.surface) != 0 && this->mPos.y < rc.clsnY) {
                this->PoofDust();
                func_02012694(0xc4, &this->mCamSpacePos);
                this->MarkForDestruction();
                return;
            }
            {
                dBgPi *fr = clsn->GetFloorResult();
                fr->surface.CopyNormalTo(normal);
            }
            if (normal.y != 0) {
                this->mSpeed.y =
                    -(FX_Div(FX_MUL(normal.x, this->mSpeed.x) + FX_MUL(normal.z, this->mSpeed.z),
                             normal.y) +
                      0x8000);
            }
            this->mAngle.x = this->func_02010844(&normal, this->mAngle.y);
            this->mAngle.z = this->func_02010844(&normal, (this->mAngle.y - 0x4000));
        }
    }
    if (clsn->IsOnWall()) {
        dBgPi *wr = clsn->GetWallResult();
        wr->surface.CopyNormalTo(wallnormal);
    }
}

/* Hit response while walking or spinning. Egg and the knock-up destroy or
 * launch; a shell / metal / attack hit in state 1 starts the chase. */
void daTgz_c::HandlePlayerCollision() {
    daPly_c *player;
    int b;

    if (this->FindEgg(this->mdCcAc_c) != 0) {
        Vector3 v;
        v.x = this->mPos.x;
        v.y = this->mPos.y;
        v.z = this->mPos.z;
        this->SpawnCoins(v, 1, 0x2000, 0);
        this->PoofDust();
        func_02012694(0xc4, &this->mCamSpacePos);
        this->MarkForDestruction();
        return;
    }

    {
        unsigned int id = this->mdCcAc_c.otherOwner;
        if (id == 0)
            return;
        player = (daPly_c *)dActor_c::FindWithID(id);
    }
    if (player == 0)
        return;

    b = (player->actorID == 0xbf);
    if (b == 0)
        return;

    b = ((this->mFlags & 0x20000) != 0);
    if (b != 0) {
        this->SetState(3);
        return;
    }

    /* 0x403c0 = punch | kick | breakdance | slide kick | fire. */
    if ((this->mdCcAc_c.hitFlags & 0x403c0) || player->IsOnShell() != 0 || player->mIsMetal != 0) {
        if (this->mState != 1)
            return;
        Sound::PlayBank0(0xb5, this->mCamSpacePos);
        this->mChasePlayer = player;
        this->SetState(2);
        return;
    }

    if (this->mdCcAc_c.hitFlags & 0x10) {
        this->mPrevAngle.y = Vec3_HorzAngle(&player->mPos, &this->mPos);
        player->IncMegaKillCount();
        this->SetState(5);
        return;
    }

    if (this->mState == 4)
        return;

    {
        Vector3 v;
        v.x = this->mPos.x;
        v.y = this->mPos.y;
        v.z = this->mPos.z;
        player->Hurt(v, 2, 0xc000, 1, 0, 1);
    }
}

static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

static inline int IsBeingEaten(dActor_c *actor) {
    return (actor->mFlags & 0x20000) != 0;
}

/* Hit response during the spin. Mega sets the heading away from the player
 * (Vec3_HorzAngle takes the angle from its first point to its second) and
 * turns the model the other way to face him, then knocks it away; fire is
 * ignored; anything else hurts the player. */
void daTgz_c::HandlePlayerCollisionSpinning() {
    unsigned int id = this->mdCcAc_c.otherOwner;
    if (id == 0)
        return;
    daPly_c *player = (daPly_c *)dActor_c::FindWithID(id);
    if (player == 0)
        return;
    if (!IsPlayerActor(player))
        return;
    if (IsBeingEaten(this)) {
        this->SetState(3);
        return;
    }
    int flags = this->mdCcAc_c.hitFlags;
    if ((flags & 0x10) != 0) {
        this->mPrevAngle.y = Vec3_HorzAngle(&player->mPos, &this->mPos);
        this->mAngle.y = this->mPrevAngle.y + 0x8000;
        player->IncMegaKillCount();
        this->SetState(5);
        return;
    }
    if ((flags & 0x40000) != 0)
        return;
    Vector3 v;
    v.x = this->mPos.x;
    v.y = this->mPos.y;
    v.z = this->mPos.z;
    player->Hurt(v, 2, 0xc000, 1, 0, 1);
}

/* Tumble matrix: translate to pos>>3, up by the egg-aim height, rotate, then
 * back down by that height, and keep the result on the anim model's matrix. */
void daTgz_c::UpdateTumbleMatrix() {
    int v[3];
    Vec3_Asr(reinterpret_cast<Vector3 *>(v), &this->mPos, 3);
    Matrix4x3_FromTranslation(&gScratchMtx, v[0], v[1], v[2]);
    int aimHeight = this->OnAimedAtWithEgg();
    Matrix4x3_ApplyInPlaceToTranslation(&gScratchMtx, 0, aimHeight >> 3, 0);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&gScratchMtx, this->mAngle.x, this->mAngle.y,
                                           this->mAngle.z);
    int aimHeightAgain = this->OnAimedAtWithEgg();
    Matrix4x3_ApplyInPlaceToTranslation(&gScratchMtx, 0, (-aimHeightAgain) >> 3, 0);
    this->mAnmModel.mat4x3 = gScratchMtx;
}

/* Upright matrix on the still model in states 0 and 4, otherwise on the anim. */
void daTgz_c::UpdateModelMatrix() {
    int v = this->mState;
    int b = 1;
    Matrix4x3 *m;
    if (v != 0)
        b = (v == 4);
    if (b)
        m = &this->mModel.mat4x3;
    else
        m = &this->mAnmModel.mat4x3;
    Matrix4x3_FromRotationZXYExt(m, this->mAngle.x, this->mAngle.y, this->mAngle.z);
    m->t.x = this->mPos.x >> 3;
    m->t.y = this->mPos.y >> 3;
    m->t.z = this->mPos.z >> 3;
}

static inline int IsInYoshiMouth(dActor_c *actor) {
    return (actor->mFlags & 0x40000) != 0;
}

void daTgz_c::UpdateMatrixAndShadow() {
    if (IsInYoshiMouth(this))
        return;
    if (this->mState == 5)
        this->UpdateTumbleMatrix();
    else
        this->UpdateModelMatrix();
    this->mShadowMatrix.t.x = this->mPos.x >> 3;
    this->mShadowMatrix.t.y = this->mPos.y >> 3;
    this->mShadowMatrix.t.z = this->mPos.z >> 3;
    this->DropShadowRadHeight(this->mShadowModel, this->mShadowMatrix, 0x50000, 0x320000, 0xf);
}

/* State 5 update: tumble backward until the ground or the 45-frame timer. */
int daTgz_c::St_KnockedAway_Main() {
    mAngle.x = mAngle.x - 0x1000;
    mAnmModel.dExtFrameCtrl_c::Advance();
    UpdatePos(&mdCcAc_c);

    if (mHorzSpeed >= mWithMeshClsn.mRadius || mSpeed.y >= mWithMeshClsn.mRadius)
        dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    else
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);

    if (mWithMeshClsn.JustHitGround() || DecIfAbove0_Byte(&mActionTimer) == 0) {
        Vector3 v;
        v.x = mPos.x;
        v.y = mPos.y;
        v.z = mPos.z;
        SpawnCoins(v, 1, 0x2000, 0);
        PoofDust();
        func_02012694(0xc4, &mCamSpacePos);
        MarkForDestruction();
    }
    return 1;
}

/* State 5 entry: launch upward at 40.0 and arm the 45-frame tumble. */
int daTgz_c::St_KnockedAway_Init() {
    Sound::PlayBank0(9, mCamSpacePos);
    mHorzSpeed = 0xa000;
    mSpeed.y = 0x28000;
    mAngle.x = 0;
    mAngle.z = 0;
    mActionTimer = 45;
    mAnmModel.SetAnim((BCA_File *)TGZ_BCA, 0, 0x1000, 0);
    mAnmModel.speed = 0x4000;
    int aimHeight = OnAimedAtWithEgg();
    Particle::System::NewSimple(kPtclKnockAwayA, mPos.x, mPos.y + aimHeight, mPos.z);
    int aimHeightAgain = OnAimedAtWithEgg();
    Particle::System::NewSimple(kPtclKnockAwayB, mPos.x, mPos.y + aimHeightAgain, mPos.z);
    mState = 5;
    return 1;
}

/* State 4 update: the same spin as state 0, after the carrier lets go. */
int daTgz_c::St_Spit_Main() {
    Vector3 vec;
    int x, y, z;
    int d;

    mAngle.x = mAngle.x + 0x4e20;

    if (mHorzSpeed >= mWithMeshClsn.mRadius || mSpeed.y >= mWithMeshClsn.mRadius)
        dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    else
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);

    if (mWithMeshClsn.JustHitGround()) {
        mSpeed.y = mSpeed.y * -0x3c / 100;
        if (mSpeed.y > 0x8000) {
            x = mPos.x;
            y = mPos.y + 0x28000;
            z = mPos.z;
            vec.x = x;
            vec.y = y;
            vec.z = z;
            Particle::System::NewSimple(kPtclLandingDust, vec.x, vec.y, vec.z);
            func_0201267c(0x109, &mCamSpacePos);
        } else {
            mSpeed.y = 0;
            mAngle.x = 0;
            mWithMeshClsn.ClearLimMovFlag();
            this->SetState(1);
        }
    }

    d = mWaterY ? mPos.y - mWaterY : 0;
    if (d < -0xc8000) {
        PoofDust();
        func_02012694(0x166, &mCamSpacePos);
        MarkForDestruction();
    }

    UpdatePos(&mdCcAc_c);
    this->HandlePlayerCollision();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* State 4 entry: leave the carrier, pop up in front of it, and start spinning. */
int daTgz_c::St_Spit_Init() {
    dActor_c *carrier;
    s16 sinv;
    s16 cosv;
    s16 carrierYaw;
    Vector3 v;
    int one;

    mFlags &= ~0x80000;
    mHorzSpeed = 0xa000;
    mSpeed.y = 0;

    carrier = mEatingPlayer;
    carrierYaw = carrier->mAngle.y;
    mAngle.x = 0;
    mAngle.y = carrierYaw;
    mAngle.z = 0;

    mPrevAngle.y = mAngle.y;

    carrier = mEatingPlayer;
    one = 1;
    int *src = &carrier->mPos.x;
    mPos.x = src[0];
    mPos.y = src[1];
    mPos.z = src[2];

    sinv = FX_SinIdx((u16)mAngle.y >> 4);
    mPos.x = mPos.x + FX_MUL(sinv, 0x3c000);
    mPos.y = mPos.y + 0x85000;
    cosv = FX_CosIdx((u16)mAngle.y >> 4);
    mPos.z = mPos.z + FX_MUL(cosv, 0x3c000);

    carrier = mEatingPlayer;
    {
        int ty = carrier->mPos.y;
        int tz = carrier->mPos.z;
        int ty2 = ty + 0x50000;
        int tx = carrier->mPos.x;
        v.x = tx;
        v.y = ty2;
        v.z = tz;
    }

    DetectRaycastClsn(v, mPos, one);

    mEatingPlayer = 0;
    mWithMeshClsn.SetLimMovFlag();

    mState = 4;
    return 1;
}

static inline int IsFlagSet(dActor_c *actor, u32 bit) {
    return (actor->mFlags & bit) != 0;
}

/* Held update: stick to the carrier, or on release enter state 4 / the walk. */
int daTgz_c::St_YoshiMouth_Main() {
    if (IsInYoshiMouth(this)) {
        dActor_c *carrier = mEatingPlayer;
        /* Address of the triple, then [0]/[4]/[8]. Three field loads fold the
           0x5c into the ldr and drop the add the ROM kept. */
        int *src = &carrier->mPos.x;
        mPos.x = src[0];
        mPos.y = src[1];
        mPos.z = src[2];
    }
    if (IsFlagSet(this, 0x80000))
        this->SetState(4);
    else if (!IsBeingEaten(this) && !IsInYoshiMouth(this)) {
        mEatingPlayer = 0;
        this->SetState(1);
    }
    return 1;
}

/* State 3 (held) entry. */
int daTgz_c::St_YoshiMouth_Init() {
    mHorzSpeed = 0;
    mdCcAc_c.Clear();
    mState = 3;
    return 1;
}

/* State 2 update: bounce, then stand up into the walk. */
int daTgz_c::St_Knockback_Main() {
    int v;

    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
    if (mWithMeshClsn.JustHitGround() != 0)
        mSpeed.y = mSpeed.y * -50 / 100;
    else if (mWithMeshClsn.IsOnGround() != 0) {
        mSpeed.y = 0;
        mWithMeshClsn.ClearLimMovFlag();
        mChasePlayer = 0;
        mPrevAngle.y = mAngle.y;
        this->SetState(1);
    }

    if (mWaterY != 0)
        v = mPos.y - mWaterY;
    else
        v = 0;

    if (v < -0xc8000) {
        PoofDust();
        func_02012694(0x166, &mCamSpacePos);
        MarkForDestruction();
    }

    mAnmModel.dExtFrameCtrl_c::Advance();
    UpdatePos(&mdCcAc_c);
    this->HandlePlayerCollision();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* State 2 entry: hop away from the player that just hit us. mPrevAngle.y is
 * the heading dActor_c::UpdatePos integrates mHorzSpeed along, and
 * Vec3_HorzAngle(hitter, self) points from the hitter to us. */
int daTgz_c::St_Knockback_Init() {
    mHorzSpeed = 0x5000;
    mSpeed.y = 0xd000;
    short ang = Vec3_HorzAngle(&mChasePlayer->mPos, &mPos);
    mPrevAngle.y = ang;
    mWithMeshClsn.SetLimMovFlag();
    mState = 2;
    return 1;
}

/* State 1 update: walk, turning toward mTurnTarget, until the timer expires. */
int daTgz_c::St_Walk_Main() {
    int d;
    ApproachLinear(mAngle.y, mTurnTarget, 0x64);
    mPrevAngle.y = mAngle.y;
    mAnmModel.dExtFrameCtrl_c::Advance();
    this->HandlePlayerCollision();
    if (mWaterY)
        d = mPos.y - mWaterY;
    else
        d = 0;
    if (d < -0xc8000) {
        PoofDust();
        func_02012694(0x166, &mCamSpacePos);
        MarkForDestruction();
    }
    UpdatePos(&mdCcAc_c);
    this->UpdateGroundCollision(&mWithMeshClsn);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    if (DecIfAbove0_Byte(&mActionTimer) == 0)
        this->SetState(1);
    return 1;
}

/* State 1 entry. */
void daTgz_c::St_Walk_Init() {
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mAnmModel.SetAnim((BCA_File *)TGZ_BCA, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mWithMeshClsn.SetHeight(0x28000);
    mHorzSpeed = 0x1b33;
    mTurnTarget = RandomIntInternal(&data_0209e650.value);
    mActionTimer = RandomIntInternal(&data_0209e650.value);
    mState = 1;
}

/* State 0 update: spin, splash, and settle into the walk. */
int daTgz_c::St_Spin_Main() {
    int underwater;
    int d;
    int x, y, z;
    Vector3 vec;

    mAngle.x = mAngle.x + 0x4e20;
    UpdatePos(&mdCcAc_c);
    this->HandlePlayerCollisionSpinning();
    mWithMeshClsn.SetLimMovFlag();
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);

    underwater = this->IsUnderwater();
    if (underwater) {
        if (mInWater == 0) {
            func_02012694(0xe2, &mCamSpacePos);
            Particle::System::NewBigSplash(mPos.x, gWaterHeight, mPos.z);
            mRippleId =
                Particle::System::New(mRippleId, kPtclRipple, mPos.x, gWaterHeight, mPos.z, 0, 0);
        }
        mVertAccel = -0x400;
        mTerminalVelocity = -0x5000;
        mHorzSpeed = 0x2000;
    } else {
        mVertAccel = -0x2000;
        mTerminalVelocity = -0x3c000;
        mHorzSpeed = 0x4000;
    }
    mInWater = underwater;

    if (mWithMeshClsn.JustHitGround()) {
        mSpeed.y = (mSpeed.y * -0x3c) / 100;
        d = mWaterY ? mPos.y - mWaterY : 0;
        if (d < -0xc8000) {
            PoofDust();
            func_02012694(0x166, &mCamSpacePos);
            MarkForDestruction();
        } else if (mSpeed.y < 0xa000) {
            mSpeed.y = 0;
            mAngle.x = mPrevAngle.x;
            mAngle.y = mPrevAngle.y;
            mAngle.z = mPrevAngle.z;
            mWithMeshClsn.ClearLimMovFlag();
            mFlags |= 0x10000000u;
            this->SetState(1);
        } else {
            x = mPos.x;
            y = mPos.y + 0x28000;
            z = mPos.z;
            vec.x = x;
            vec.y = y;
            vec.z = z;
            Particle::System::NewSimple(kPtclLandingDust, vec.x, vec.y, vec.z);
            func_0201267c(0x109, &mCamSpacePos);
        }
    }

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* State 0 entry. */
int daTgz_c::St_Spin_Init() {
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0x4000;
    mSpeed.y = 0x19000;
    mWithMeshClsn.SetLimMovFlag();
    mState = 0;
    return 1;
}

void daTgz_c::CallStateUpdate() {
    TgzState *p = &this->mStateDesc->update;
    (this->**p)();
}

void daTgz_c::CallStateEnter() {
    TgzState *p = &this->mStateDesc->enter;
    (this->**p)();
}

void daTgz_c::SetState(int i) {
    this->mStateDesc = &gTgzStates[i];
    this->CallStateEnter();
}

int daTgz_c::CleanupResources()
{
    data_ov077_02127b48.Release();
    data_ov077_02127b38.Release();
    gTgzWalkAnimFile.Release();
    return 1;
}

void daTgz_c::OnPendingDestroy()
{
}

int daTgz_c::Render()
{
    if (IsInYoshiMouth(this))
        return 1;
    int s = mState;
    if (s == 0 || s == 4)
        mModel.Render(0);
    else
        mAnmModel.Render(0);
    return 1;
}

int daTgz_c::Behavior()
{
    if ((mState != 1 || mWithMeshClsn.IsOnGround()) && mState != 4 && mState != 5 &&
        IsTooFarAwayFromPlayer(0x5dc000)) {
        if (DecIfAbove0_Byte(&mDespawnTimer) == 0) {
            MarkForDestruction();
            return 1;
        }
    } else {
        this->GetHeightAboveWater();
        this->CallStateUpdate();
        MakeVanishLuigiWork(mdCcAc_c);
        this->UpdateMatrixAndShadow();
        if (gCurrentSublevel == 0x1c && mPos.y <= -0x1600000) {
            PoofDust();
            func_02012694(0xc4, &mCamSpacePos);
            MarkForDestruction();
        }
    }
    return 1;
}

int daTgz_c::InitResources()
{
    BMD_File *bmd;
    bmd = dExtSimpleModel_c::LoadFile(data_ov077_02127b48);
    mModel.SetFile(bmd, 1, -1);
    bmd = dExtSimpleModel_c::LoadFile(data_ov077_02127b38);
    mAnmModel.SetFile(bmd, 1, -1);
    dExtFrameCtrl_c::LoadFile(gTgzWalkAnimFile);
    mAnmModel.SetAnim((BCA_File *)TGZ_BCA, 0, 0x1000, 0);
    if (!mShadowModel.InitCylinder())
        return 0;
    mdCcAc_c.Init(this, 0x2d000, 0x3c000, 0x200000, 0x4a3d0);
    mWithMeshClsn.Init(this, 0x2d000, 0, &mPrevAngle, &mAngle);
    mScale.x = 0x1000;
    mScale.y = 0x1000;
    mScale.z = 0x1000;
    mDespawnTimer = 44;
    this->SetState(0);
    mShadowMatrix = IDENTITY_MATRIX4X3;
    this->UpdateMatrixAndShadow();
    return 1;
}

/* Vtable slot 19. Yoshi's egg is worth one coin -- paid directly while the
 * player is collecting a cap, otherwise counted as an egg coin. */
void daTgz_c::OnTurnIntoEgg(daPly_c &player) {
    if (player.IsCollectingCap())
        GivePlayerCoins(player, 1, 0);
    else
        player.RegisterEggCoinCount(1, 0, 0);
    MarkForDestruction();
}

daTgz_c *daTgz_c_classInit() {
    return new daTgz_c;
}

/* The actor profile, in SpawnInfo's layout (include/SpawnInfo.h). The
 * retail values are unchanged. */
SpawnInfo<daTgz_c> g_profile_TOGEZO = {
    daTgz_c_classInit, /* classInit */
    0x0104,            /* behaviorPriority */
    0x0076,            /* renderPriority */
    0x10000002,        /* actorFlags */
    0,                 /* clipOffsetY */
    0x3c000,           /* clipRadius */
    0x01000000,        /* clipDistance */
    0x007d0000         /* farDistance */
};
