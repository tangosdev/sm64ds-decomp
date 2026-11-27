//cpp
/* daSanbo_c -- Pokey (ov096).
 *
 * Head (actor id 0xf0) and body segments (0xf1) are the same class.
 * mPrevSegment / mNextSegment link the pieces. Both factories are
 * return new daSanbo_c(). There is no out-of-line constructor.
 *
 * Source order is ROM-descending. mwccarm emits .text in reverse
 * source order, and the inline destructor in include/daSanbo_c.h
 * emits D1 then D0 with no D2. Do not reorder, and do not write
 * the destructor out of line.
 *
 * All helpers are daSanbo_c members; the func_ov096_<addr> names are
 * ours, not recovered. The state pairs they serve live in
 * data_ov096_02137b48, filled by this translation unit's native
 * static initializer. CalcShyAwayAngle's ROM signature carried a
 * dead first argument (r0) that was never read -- as a member its
 * real argument lands in r1 either way.
 *
 * deslop leftovers:
 *   - func_02038414 is the veneer onto
 *   dBgCh_Actr::UpdateDiscreteNoLava_2 (UpdateKnockback,
 *   UpdateMegaKill, UpdateFloorAndWalls).
 *   - UpdateFloorAndWalls copies the floor and wall collision normals;
 *   the wall normal is unused.
 *   - gSanboHeadModelFile and gSanboBodyModelFile are the head
 *   and segment file handles (InitResources LoadFile, CleanupResources
 *   Release). Both actor profiles are defined with their factories below.
 *   - HandleAttacks keeps five int[3] positions. One
 *   Vector3 in their place changes that function.
 *   - gScratchMtx is the scratch matrix UpdateModelAndShadow fills
 *   before copying it onto the model. Actor id 0x135 is only the id that function shoves
 *   the segment away from.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daSanbo_c_AssetId {
    ASSET_SANBO_BODY_BMD = 0x041a,
    ASSET_SANBO_HEAD_BMD = 0x041b
};

#include "common.h"
#include "math/CalcLib.h"
#include "daSanbo_c.h"
#include "dBgPi.h"
#include "types.h"
#include "SharedFilePtr.h"
#include "SpawnInfo.h"
#include "decl_common.h"
#include "daPly_c.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "math/Approach.h"
#include "LevelRng.h"
#include "nitro/fx/fx_cp.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

enum { SANBO_HEAD = 0xf0, SANBO_BODY = 0xf1 };

extern "C" {

}

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * pair (ModelFile::ModelFile / ModelFile::~ModelFile), spelled through a declared-only
 * subclass so the static initializer names the real entry points. */
struct SanboModelFilePtr : SharedFilePtr {
    SanboModelFilePtr(u32 fileID);
    ~SanboModelFilePtr();
};
extern SanboModelFilePtr gSanboHeadModelFile;
extern SanboModelFilePtr gSanboBodyModelFile;
extern daSanbo_c::StateFunc data_ov096_02137b48[];

/* Both profiles construct this class. The synthesized ctor stores
 * `_ZTV9daSanbo_c + 2`; do not restate a file-local `extern _ZTV`. */

daSanbo_c *daSanbo_c_classInit_SANBO(void) {
    return new daSanbo_c();
}

/* Same class, SANBO_BODY profile (actor id 0xf1). */
daSanbo_c *daSanbo_c_classInit_SANBO_BODY(void) {
    return new daSanbo_c();
}

static inline int IsActorID(dActor_c *actor, int id) {
    return actor->actorID == id;
}

/* Only the head pays out a coin. Every piece then destroys itself. */
void daSanbo_c::OnTurnIntoEgg(daPly_c &player) {
    if (IsActorID(this, SANBO_HEAD))
        GivePlayerCoins(player, 1, 2);
    MarkForDestruction();
}

int daSanbo_c::InitResources()
{
    int t;

    t = (actorID == SANBO_HEAD);
    if (t != false) {
        BMD_File *m = dExtSimpleModel_c::LoadFile(gSanboHeadModelFile);
        mModel.SetFile(m, 1, 1);
        dExtSimpleModel_c::LoadFile(gSanboBodyModelFile);
        LoadBlueCoinModel();
        mInstantSprout = 1;
    } else {
        t = (actorID == SANBO_BODY);
        if (t != false) {
            BMD_File *m = dExtSimpleModel_c::LoadFile(gSanboBodyModelFile);
            if (mModel.SetFile(m, 1, 1) == 0)
                return 0;
        }
    }

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    mdCcAc_c.Init(this, 0x3c000, 0x78000, 0x200004, 0x6eff0);
    mWithMeshClsn.Init(this, 0x3c000, 0x3c000, 0, 0);

    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0x3000;

    t = (actorID == SANBO_HEAD);
    if (t != false) {
        mRootPos = mPos;
        mScale.x = FX32_ONE;
        mScale.y = FX32_ONE;
        mScale.z = FX32_ONE;
        mPrevSegment = 0;
        mNextSegment = 0;
    } else {
        t = (actorID == SANBO_BODY);
        if (t != false) {
            mScale.x = 0;
            mScale.y = 0;
            mScale.z = 0;
            mPrevSegment = (daSanbo_c *)dActor_c::FindWithID(param1);
            mNextSegment = 0;
            mRootPos = mPrevSegment->mRootPos;
        }
    }

    ChangeState(1);
    mMatrix = IDENTITY_MATRIX4X3;
    UpdateModelAndShadow();
    return 1;
}

int daSanbo_c::Behavior()
{
    int s = mState;
    if (s != 2 && s != 5) {
        if (IsTooFarAwayFromPlayer(0x5dc000))
            return 1;
    }
    CallStateUpdate();
    MakeVanishLuigiWork(mdCcAc_c);
    UpdateModelAndShadow();
    return 1;
}

static inline int IsInYoshiMouth(dActor_c *actor) {
    return (actor->mFlags & 0x40000) != 0;
}

int daSanbo_c::Render()
{
    if (IsInYoshiMouth(this))
        return 1;
    mModel.Render(&mScale);
    return 1;
}

void daSanbo_c::OnPendingDestroy()
{
    int isBody = actorID;
    isBody = (isBody == SANBO_BODY);
    if (isBody) return;
    daSanbo_c *p = mNextSegment;
    if (!p) return;
    do {
        p->PoofAndDestroy();
        p = p->mNextSegment;
    } while (p);
}

int daSanbo_c::CleanupResources() {
    if (IsActorID(this, SANBO_HEAD)) {
        UnloadBlueCoinModel();
        gSanboHeadModelFile.Release();
        gSanboBodyModelFile.Release();
    } else {
        if (IsActorID(this, SANBO_BODY)) {
            gSanboBodyModelFile.Release();
        }
    }
    return 1;
}

void daSanbo_c::ChangeState(int a) {
    mStateFunctions = data_ov096_02137b48 + (a << 1);
    CallStateEnter();
}

void daSanbo_c::CallStateEnter() {
    daSanbo_c::StateFunc *p = mStateFunctions;
    (this->**p)();
}

void daSanbo_c::CallStateUpdate() {
    daSanbo_c::StateFunc *p = mStateFunctions + 1;
    (this->**p)();
}

int daSanbo_c::EnterStacked() {
    mState = 0;
    return 1;
}

namespace Sound { void PlayBank0(unsigned int id, const Vector3 &pos); }


extern "C" {
}

/* A segment tracks the one in front of it while that piece is still small. */
int daSanbo_c::UpdateStackedBody() {
    daSanbo_c *next = mNextSegment;

    if (!next) {
        ChangeState(1);
    } else {
        int vy;
        int clamped;

        mPos.x = next->mPos.x;
        mPos.z = mNextSegment->mPos.z;

        vy = mSpeed.y - 0x2000;
        clamped = mTerminalVelocity;
        if (vy >= clamped)
            clamped = vy;
        mSpeed.y = clamped;
        mPos.y += mSpeed.y;

        if (mPos.y < mNextSegment->mPos.y + 0x6e000) {
            mPos.y = mNextSegment->mPos.y + 0x6e000;
            mSpeed.y = 0;
        }

        mAngle.y = mNextSegment->mAngle.y;
        mSegAngY = mNextSegment->mSegAngY + 0x13000;
        mOffset.x = FX_MUL(FX_SinIdx((u16)mSegAngY >> 4), 0xe000);
        mOffset.y = 0;
        mOffset.z = FX_MUL(FX_CosIdx((u16)mSegAngY >> 4), 0xe000);
    }
    HandleAttacks();
    mdCcAc_c.Clear();
    mdCcAc_c.dCc_c::Update();
    return 1;
}

/* mInstantSprout on the head picks a zero delay or a 90-frame one. */
int daSanbo_c::EnterLeader() {
    daSanbo_c *head = GetHead();
    u8 timer = head->mInstantSprout;

    if (timer != 0)
        timer = 0;
    else
        timer = 90;
    mTimer = timer;
    timer = mTimer;
    if (timer == 0) {
        mScale.x = FX32_ONE;
        mScale.y = FX32_ONE;
        mScale.z = FX32_ONE;
    }
    mState = 1;
    return 1;
}

/* Grow this segment, then sprout the next one when the timer expires. */
int daSanbo_c::UpdateLeader() {
    int scale = mScale.x;
    int copied;

    ApproachLinear(scale, 0x1000, 0x12c);
    copied = scale;
    mScale.x = copied;
    mScale.y = copied;
    mScale.z = copied;
    mSegAngY = (mSegAngY + 0x500);
    mOffset.x = 0;
    mOffset.y = 0;
    mOffset.z = 0;
    UpdateWanderYaw();
    if (GetSegmentIndex() < 4) {
        if (DecIfAbove0_Byte(&mTimer) == 0) {
            dActor_c *spawned = dActor_c::Spawn(SANBO_BODY, uniqueID, mPos, &mAngle, mAreaId, -1);
            if (spawned != 0) {
                mNextSegment = (daSanbo_c *)spawned;
                ChangeState(4);
                if (GetSegmentIndex() >= 3)
                    GetHead()->mInstantSprout = 0;
            }
        }
    }
    UpdatePos(&mdCcAc_c);
    UpdateFloorAndWalls(&mWithMeshClsn);
    HandleAttacks();
    mdCcAc_c.Clear();
    mdCcAc_c.dCc_c::Update();
    if (mWithMeshClsn.IsOnGround() != 0) {
        Particle::RunningSlidingDustAt(mPos.x, mPos.y, mPos.z);
    }
    return 1;
}

int daSanbo_c::EnterKnockback() {

    if (IsActorID(this, SANBO_HEAD)) {
        dActor_c::Spawn(0x122, 2, mPos, (Vector3_16 *)0, mAreaId, -1);
    }
    UnlinkSegment();
    mPrevAngle.y = Vec3_HorzAngle(&mHitActor->mPos, &mPos);
    mSpeed.y = 0x14000;
    if (mHitActor->param1 == 2)
        mHorzSpeed = 0x28000;
    else
        mHorzSpeed = 0x14000;
    mState = 2;
    return 1;
}

int daSanbo_c::UpdateKnockback() {
    struct Vector3 pos;
    Particle::System *p0;
    Particle::System *p1;
    int x;
    int y;
    int z;

    UpdatePos(&mdCcAc_c);
    if (mParticle0 != 0 && mParticle1 != 0) {
        x = mPos.x;
        z = mPos.z;
        y = mPos.y + 0x3c000;
        pos.x = x;
        pos.y = y;
        pos.z = z;
        mParticle0 = Particle::System::New(mParticle0, kPtclDeathA, pos.x, pos.y, pos.z, 0, 0);
        mParticle1 =
            Particle::System::NewCleanParticle(mParticle1, kPtclDeathB, pos.x, pos.y, pos.z, 0);
        p0 = Particle::System::FromUniqueID(mParticle0);
        p1 = Particle::System::FromUniqueID(mParticle1);
        if (p0)
            p0->callbackScale = 0x7fff;
        if (p1)
            p1->callbackScale = 0x7fff;
    }
    mWithMeshClsn.func_02038414();
    if (mWithMeshClsn.JustHitGround() != 0)
        PoofAndDestroy();
    return 1;
}

int daSanbo_c::EnterInMouth() {

    UnlinkSegment();
    mHorzSpeed = 0;
    mState = 3;
    return 1;
}

static inline int IsBeingEaten(dActor_c *actor) {
    return (actor->mFlags & 0x20000) != 0;
}

int daSanbo_c::UpdateInMouth() {
    if (!IsBeingEaten(this) && !IsInYoshiMouth(this))
        PoofAndDestroy();
    return 1;
}

int daSanbo_c::EnterStackedGrowing() {
    mState = 4;
    return 1;
}

/* Follow the segment ahead. Once its scale reaches full size, settle. */
int daSanbo_c::UpdateStackedOnGrowingBody() {
    daSanbo_c *next = mNextSegment;

    if (!next) {
        ChangeState(1);
    } else {
        int fullHeight = 0x6e000;
        int scale = next->mScale.x;
        int raised = FX_MUL(scale, fullHeight);

        mPos.x = next->mPos.x;
        mPos.y = raised + mNextSegment->mPos.y;
        mPos.z = mNextSegment->mPos.z;
        mAngle.y = mNextSegment->mAngle.y;
        mSegAngY = mNextSegment->mSegAngY + 0x13000;
        mOffset.x = FX_MUL(FX_SinIdx((u16)mSegAngY >> 4), 0xe000);
        mOffset.y = 0;
        mOffset.z = FX_MUL(FX_CosIdx((u16)mSegAngY >> 4), 0xe000);
        if (raised == fullHeight)
            ChangeState(0);
    }
    HandleAttacks();
    mdCcAc_c.Clear();
    mdCcAc_c.dCc_c::Update();
    return 1;
}

int daSanbo_c::EnterMegaKill() {
    int rnd;

    if (IsActorID(this, SANBO_HEAD)) {
        dActor_c::Spawn(0x122, 2, mPos, 0, mAreaId, -1);
    }
    Sound::PlayBank0(9, mCamSpacePos);
    UnlinkSegment();
    mPrevAngle.y = Vec3_HorzAngle(&mHitActor->mPos, &mPos);
    rnd = RandomIntInternal(&data_0209e650.value);
    mPrevAngle.y = (mPrevAngle.y + ((rnd & 0x7fff) - 0x4000));
    mAngle.y = (mPrevAngle.y + 0x8000);
    mSpeed.y = 0x28000;
    mHorzSpeed = 0xa000;
    mTimer = 45;
    Particle::System::NewSimple(kPtclKnockAwayA, mPos.x, mPos.y + OnAimedAtWithEgg(), mPos.z);
    Particle::System::NewSimple(kPtclKnockAwayB, mPos.x, mPos.y + OnAimedAtWithEgg(), mPos.z);
    mState = 5;
    return 1;
}

int daSanbo_c::UpdateMegaKill() {
    if (IsActorID(this, SANBO_HEAD))
        mAngle.x = (mAngle.x - 0x1000);
    UpdatePos(&mdCcAc_c);
    mWithMeshClsn.func_02038414();
    if (mWithMeshClsn.JustHitGround() || !DecIfAbove0_Byte(&mTimer))
        PoofAndDestroy();
    return 1;
}

void daSanbo_c::UpdateModelAndShadow() {
    struct Vector3 cam;
    int head;

    if (mState == 5) {
        head = (actorID == SANBO_HEAD);
        if (head) {
            int y;

            Vec3_Asr(&cam, &mPos, 3);
            Matrix4x3_FromTranslation(&gScratchMtx, cam.x, cam.y, cam.z);

            y = OnAimedAtWithEgg() >> 3;
            Matrix4x3_ApplyInPlaceToTranslation(&gScratchMtx, 0, y, 0);
            Matrix4x3_ApplyInPlaceToRotationZXYExt(&gScratchMtx, mAngle.x, mAngle.y, mAngle.z);

            y = (-OnAimedAtWithEgg()) >> 3;
            Matrix4x3_ApplyInPlaceToTranslation(&gScratchMtx, 0, y, 0);
            mModel.mat4x3 = gScratchMtx;

            mMatrix.t.x = mPos.x >> 3;
            mMatrix.t.y = mPos.y >> 3;
            mMatrix.t.z = mPos.z >> 3;

            DropShadowRadHeight(mShadowModel, mMatrix, 0xa0000, 0x2bc000, 0xf);
            return;
        }
    }

    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngle.y);
    mModel.mat4x3.t.x = (mPos.x + mOffset.x) >> 3;
    mModel.mat4x3.t.y = (mPos.y + 0x3c000) >> 3;
    mModel.mat4x3.t.z = (mPos.z + mOffset.z) >> 3;

    if (mNextSegment == 0 || mState == 2 || mState == 5) {
        DropShadowRadHeight(mShadowModel, mModel.mat4x3, 0x82000, 0x2bc000, 0xf);
    }
}

void daSanbo_c::UpdateFloorAndWalls(dBgCh_Actr *clsn) {
    Vector3 n0;
    Vector3 n1;

    clsn->func_02038414();
    if (clsn->IsOnGround()) {
        clsn->GetFloorResult()->surface.CopyNormalTo(n0);
        if (n0.y != 0) {
            int x = FX_MUL(n0.x, mSpeed.x);
            int z = FX_MUL(n0.z, mSpeed.z);
            int dot = x + z;
            int slope = FX_Div(dot, n0.y);
            mSpeed.y = -(slope + 0x8000);
        }
    }
    if (clsn->IsOnWall()) {
        clsn->GetWallResult()->surface.CopyNormalTo(n1);
    }
}

#include "decl_Actor.h"
#include "decl_Player.h"

static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

void daSanbo_c::HandleAttacks() {
    unsigned int hitId;
    dActor_c *other;
    int radius;
    int angle;
    int index;
    s16 sine;
    int dist;
    s16 cosine;
    int dist2;
    unsigned int flags;
    int state;
    daSanbo_c *prev;
    struct Vector3 pos;
    /* Five vectors on purpose. One shared local changes this function. */
    Vector3 vv1;
    Vector3 vv2;
    Vector3 vv3;
    Vector3 vv4;
    Vector3 vv5;
    int x;
    int y;
    int z;

    if (FindEgg(mdCcAc_c) != 0 ||
        FindExplosionActor(mdCcAc_c) != 0) {
        Sound::PlayBank0(9, mCamSpacePos);
        UnlinkSegment();
        {
            if (IsActorID(this, SANBO_HEAD))
                dActor_c::Spawn(0x122, 2, mPos, 0, mAreaId, -1);
        }
        PoofAndDestroy();
        return;
    }

    hitId = mdCcAc_c.otherOwner;
    if (hitId == 0)
        return;

    other = dActor_c::FindWithID(hitId);
    if (other == 0)
        return;

    if (mState == 1) {
        if (IsActorID(other, 0x135)) {
            radius = mdCcAc_c.radius;
            if (Vec3_HorzDist(&mPos, &other->mPos) < radius + 0x1a9000) {
                angle = Vec3_HorzAngle(&other->mPos, &mPos);
                index = (unsigned short)angle >> 4;
                sine = FX_SinIdx(index);
                dist = mdCcAc_c.radius + 0x1a9000;
                mPos.x = other->mPos.x + FX_MUL(dist, sine);
                cosine = FX_CosIdx(index);
                dist2 = mdCcAc_c.radius + 0x1a9000;
                mPos.z = other->mPos.z + FX_MUL(dist2, cosine);
            }
        }
    }

    {
        if (!IsPlayerActor(other))
            return;
    }

    flags = mdCcAc_c.hitFlags;
    if (!(flags & 0x8000)) {
        daPly_c *player = (daPly_c *)other;

        if (flags & 0x40000) {
            x = mPos.x;
            z = mPos.z;
            y = mPos.y + 0x3c000;
            pos.x = x;
            pos.y = y;
            pos.z = z;
            mParticle0 = Particle::System::New(mParticle0, kPtclDeathA, pos.x, pos.y, pos.z, 0, 0);
            mParticle1 =
                Particle::System::NewCleanParticle(mParticle1, kPtclDeathB, pos.x, pos.y, pos.z, 0);
            Sound::PlayBank0(9, mCamSpacePos);
            mHitActor = player;
            ChangeState(2);
        } else if ((flags & 0x26fe0) || player->IsOnShell() != 0 ||
                   player->mIsMetal != 0) {
            Sound::PlayBank0(9, mCamSpacePos);
            mHitActor = player;
            ChangeState(2);
        } else if (mdCcAc_c.hitFlags & 0x10) {
            mHitActor = player;
            player->IncMegaKillCount();
            ChangeState(5);
        } else {
            state = mState;
            if (state == 0) {
                prev = mPrevSegment;
                if (prev == 0) {
                    if (mSpeed.y == 0) {
                        vv1.x = mPos.x;
                        vv1.y = mPos.y;
                        vv1.z = mPos.z;
                        player->Hurt(vv1, 2, 0xc000, 1, 0, 1);
                    }
                } else if (mSpeed.y == 0 && prev->mSpeed.y == 0) {
                    vv2.x = mPos.x;
                    vv2.y = mPos.y;
                    vv2.z = mPos.z;
                    player->Hurt(vv2, 2, 0xc000, 1, 0, 1);
                }
            } else if (state == 1) {
                if (mPrevSegment == 0) {
                    if (mWithMeshClsn.IsOnGround() != 0) {
                        vv3.x = mPos.x;
                        vv3.y = mPos.y;
                        vv3.z = mPos.z;
                        player->Hurt(vv3, 2, 0xc000, 1, 0, 1);
                    }
                } else if (mWithMeshClsn.IsOnGround() != 0 && mPrevSegment->mSpeed.y == 0) {
                    vv4.x = mPos.x;
                    vv4.y = mPos.y;
                    vv4.z = mPos.z;
                    player->Hurt(vv4, 2, 0xc000, 1, 0, 1);
                }
            } else {
                vv5.x = mPos.x;
                vv5.y = mPos.y;
                vv5.z = mPos.z;
                player->Hurt(vv5, 2, 0xc000, 1, 0, 1);
            }
        }
    }

    {
        int inMouth = (mFlags & 0x20000) != 0;
        if (inMouth)
            ChangeState(3);
    }
}

void daSanbo_c::UpdateWanderYaw() {
    daPly_c *player = ClosestNonVanishPlayer();
    const Vector3 *target;
    int dist;
    short ang;
    int turn;

    if (player) {
        dist = Vec3_HorzDist(&mRootPos, &player->mPos);
        if (dist < 0x3e8000)
            target = &player->mPos;
        else
            target = &mRootPos;
    } else {
        target = &mRootPos;
    }
    dist = Vec3_HorzDist(&mPos, target);
    ang = Vec3_HorzAngle(&mPos, target);
    ApproachLinear(mAngle.y, ang, 0x320);
    turn = CalcShyAwayAngle(dist);
    mPrevAngle.y = (mAngle.y + turn);
}

int daSanbo_c::CalcShyAwayAngle(int x) {
    if (x > 0x190000)
        return 0;
    int rate = FX_Div(0x4000, 0xc8000);
    int drop = FX_MUL(rate, x);
    s16 r = 0x8000 - drop;
    return r;
}

void daSanbo_c::PoofAndDestroy() {

    PoofDust();
    MarkForDestruction();
}

int daSanbo_c::GetSegmentIndex() {
    daSanbo_c *seg = mPrevSegment;
    int count = 0;

    if (seg == 0)
        return count;
    do {
        seg = seg->mPrevSegment;
        count = count + 1;
    } while (seg != 0);
    return count;
}

void daSanbo_c::UnlinkSegment() {
    daSanbo_c *next;
    daSanbo_c *prev;

    if (IsActorID(this, SANBO_HEAD))
        return;
    next = mNextSegment;
    prev = mPrevSegment;
    prev->mNextSegment = next;
    next = mNextSegment;
    if (next)
        next->mPrevSegment = mPrevSegment;
}

daSanbo_c *daSanbo_c::GetHead() {
    daSanbo_c *prev = mPrevSegment;

    if (prev == 0)
        return this;
    while (prev) {
        unsigned int notHead = (prev->actorID != SANBO_HEAD) ? 1u : 0u;
        if (!notHead)
            return prev;
        prev = prev->mPrevSegment;
    }
    return 0;
}

/* Constant height used by the segment particles and the head's shadow. */
int daSanbo_c::OnAimedAtWithEgg()
{
    return 245760;
}

/* Returns a constant. The body ignores `this`. */
int daSanbo_c::OnYoshiTryEat()
{
    return 4;
}

/* D1 and D0 come from the inline destructor in include/daSanbo_c.h.
 * An out-of-line body here would emit D2 and put D0 below D1. */

SanboModelFilePtr gSanboHeadModelFile(ASSET_SANBO_HEAD_BMD);   /* head model */
SanboModelFilePtr gSanboBodyModelFile(ASSET_SANBO_BODY_BMD);   /* segment model */

/* Native member pointers emit twelve descriptors in .data. The initializer
 * copies them into this runtime table as six entry/update pairs. The state
 * numbers and address-named helpers are retained reconstruction identities. */

daSanbo_c::StateFunc data_ov096_02137b48[12] = {
    &daSanbo_c::EnterStacked,        &daSanbo_c::UpdateStackedBody,          /* state 0 */
    &daSanbo_c::EnterLeader,         &daSanbo_c::UpdateLeader,               /* state 1 */
    &daSanbo_c::EnterKnockback,      &daSanbo_c::UpdateKnockback,            /* state 2 */
    &daSanbo_c::EnterInMouth,        &daSanbo_c::UpdateInMouth,              /* state 3 */
    &daSanbo_c::EnterStackedGrowing, &daSanbo_c::UpdateStackedOnGrowingBody, /* state 4 */
    &daSanbo_c::EnterMegaKill,       &daSanbo_c::UpdateMegaKill,             /* state 5 */
};
SpawnInfo<daSanbo_c> g_profile_SANBO = {
    daSanbo_c_classInit_SANBO, 0xf0, 0x5f, 0x10000002, 0x1f4000, 0x1f4000, 0x1000000, 0x1000000};
SpawnInfo<daSanbo_c> g_profile_SANBO_BODY = {daSanbo_c_classInit_SANBO_BODY,
                                             0xf1,
                                             0x60,
                                             0x10000002,
                                             0x1f4000,
                                             0x1f4000,
                                             0x1000000,
                                             0x1000000};
