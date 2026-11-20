//cpp
/* daEyBm_c -- Mr. I's projectile (EYEKUN_BEAM 264), ov071.
 *
 * It flies straight at 10.0 along the pitch and yaw it spawned with
 * (mPrevAngle.x / mPrevAngle.y) and bursts (particle 0x45) when it touches
 * ground or a wall, when it hurts a Mario (daPly_c::Hurt, not while he is
 * vanished), or after 150 frames.
 *
 * ov071 is mixed (COFFIN / SCUTTLEBUG / MR_I / BIG_MR_I / MR_I_PROJECTILE).
 * RTTI at ov071:0x02122db8 names this class daEyBm_c. Historical project
 * aliases: MrI_Projectile_Spawn and MrI_Projectile_SpawnInfo. Private helper
 * spellings are inferred; class ownership, bodies, calls and ordering are
 * proven.
 *
 * leftovers:
 * - ROM-descending source order: the factory stays first and
 *   OnYoshiTryEat last. InitResources is the key function; the inline
 *   destructor emits D1 then D0, the RTTI/vtable, and no D2.
 * - The UpdateDiscreteNoLava veneer stays because
 *   the named method is WRONG-DEST (ROM 0x02038420).
 * - data_ov071_021230b8 is this TU's sinit-constructed bss;
 *   g_profile_EYEKUN_BEAM is overlay data outside its .text.
 * - UpdateCollision takes R10dBgCh_Actr: a pointer would emit identical
 *   ARM.
 */

#include "common.h"
#include "daEyBm_c.h"
#include "SpawnInfo.h"
#include "dBgCh_Gnd.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "daPly_c.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx.h"

/* The collider offset at 0x021230b8: Vector3's constructor keeps the three
 * stores in the sinit (bss, not .data). */
extern Vector3 data_ov071_021230b8;

/* dActor_c::mFlags bits 0x20000 and 0x40000 are the two yoshi-mouth states:
 * Yoshi is eating the actor, then has it in his mouth. Actor ID 0xbf is
 * PLAYER. These int-returning predicates are what make the ROM materialize
 * 0/1 before testing it. */
static inline int IsBeingEaten(dActor_c *actor) {
    return (actor->mFlags & 0x20000) != 0;
}

static inline int IsInYoshiMouth(dActor_c *actor) {
    return (actor->mFlags & 0x40000) != 0;
}

static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

/* `new` calls the inherited fBase_c::operator new; the implicit constructor
 * emits the measured dActor_c C2, vptr store, and three member C1s. */
daEyBm_c *daEyBm_c_classInit() {
    return new daEyBm_c();
}

/* The actor-table descriptor, at ov071:0x02122dc4. */
extern "C" SpawnInfo<daEyBm_c> g_profile_EYEKUN_BEAM = {
    daEyBm_c_classInit, 0x0108, 0x007a, 0x00000002, 0, 0x00019000, 0x01000000, 0x01000000};

int daEyBm_c::InitResources() {
    if (!mShadowModel.InitCylinder())
        return 0;

    mdCcAcPos_c.Init(this, data_ov071_021230b8, 0x23000, 0x3c000, 0x200002, 0x8000);
    mScale.x = 0x1000;
    mScale.y = 0x1000;
    mScale.z = 0x1000;
    mWithMeshClsn.Init(this, 0x19000, 0x19000, 0, 0);
    mVertAccel = 0;
    mTerminalVelocity = 0;
    mHorzSpeed = 0xa000;
    mLifeTimer = 150;
    mMatrix = IDENTITY_MATRIX4X3;
    UpdateShadow();
    return 1;
}

int daEyBm_c::Behavior() {
    Vector3 offset;
    Vector3 velocity;
    if (IsInYoshiMouth(this))
        return 1;

    if (!IsBeingEaten(this)) {
        offset.x = data_ov071_021230b8.x;
        offset.y = data_ov071_021230b8.y;
        offset.z = data_ov071_021230b8.z;
        mdCcAcPos_c.SetPosRelativeToActor(offset);

        velocity.z = mHorzSpeed;
        velocity.x = 0;
        velocity.y = 0;
        Matrix4x3_FromRotationY(&gScratchMtx, mPrevAngle.y);
        Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx, mPrevAngle.x);
        MTX_MultVec43(&velocity, &gScratchMtx, &mSpeed);

        /* Retail retains a typed whole-vector writeback after the transform.
           Kept (#821 r82): dropping it -> Behavior 0x11c of 0x134. */
        mSpeed = mSpeed;

        UpdatePosWithOnlySpeed(&mdCcAcPos_c);
        UpdateCollision(mWithMeshClsn);
        HurtPlayer();
        UpdateShadow();
        mdCcAcPos_c.Clear();
        mdCcAcPos_c.Update();
        if (DecIfAbove0_Short(&mLifeTimer) == 0)
            SpawnDestroyEffect();
    } else {
        UpdateShadow();
    }
    return 1;
}

int daEyBm_c::Render() {
    mParticle1 =
        Particle::System::NewCleanParticle(mParticle1, kPtclMrIBeamA, mPos.x, mPos.y, mPos.z, 0);
    mParticle2 =
        Particle::System::NewCleanParticle(mParticle2, kPtclMrIBeamB, mPos.x, mPos.y, mPos.z, 0);
    return 1;
}
void daEyBm_c::OnPendingDestroy() {}

int daEyBm_c::CleanupResources() {
    return 1;
}

void daEyBm_c::UpdateShadow() {
    mMatrix.t.x = mPos.x >> 3;
    mMatrix.t.y = mPos.y >> 3;
    mMatrix.t.z = mPos.z >> 3;

    dBgCh_Gnd ground;
    ground.SetObjAndPos(mPos, this);
    fx32 depth;
    if (ground.DetectClsn())
        depth = (mPos.y - ground.clsnY) + 0x1e000;
    else
        depth = 0x1f4000;
    DropShadowRadHeight(mShadowModel, mMatrix, 0x50000, depth, 0xf);
}

/*
 * 0x02121ba4, size 0xc8. */

void daEyBm_c::HurtPlayer() {
    u32 ownerID = mdCcAcPos_c.otherOwner;
    if (!ownerID)
        return;
    dActor_c *found = dActor_c::FindWithID(ownerID);
    if (!found)
        return;
    if (!IsPlayerActor(found))
        return;
    if (mdCcAcPos_c.hitFlags & 0x8000)
        return;

    daPly_c *player = (daPly_c *)found;
    if (player->mIsVanish)
        return;

    Vector3 position;
    position.x = mPos.x;
    position.y = mPos.y;
    position.z = mPos.z;
    player->Hurt(position, 1, 0xc000, 1, 0, 1);
    SpawnDestroyEffect();
}

/*
 * 0x02121b50, size 0x54. The coined mangling uses a reference; a pointer
 * would generate identical ARM. The retail caller passes the owned
 * collision subobject explicitly in r1. */
void daEyBm_c::UpdateCollision(dBgCh_Actr &collision) {
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&collision);
    if (collision.IsOnGround() || collision.IsOnWall())
        SpawnDestroyEffect();
}

/* 0x02121b08, size 0x48. */
void daEyBm_c::SpawnDestroyEffect() {
    mParticle1 = Particle::System::New(0, kPtclMrIBeamBurst, mPos.x, mPos.y, mPos.z, 0, 0);
    MarkForDestruction();
}

int daEyBm_c::OnYoshiTryEat() {
    return 4;
}

Vector3 data_ov071_021230b8(0, -0x19000, 0);
/* The inline class destructor and InitResources vtable instantiation emit both naturally. */
