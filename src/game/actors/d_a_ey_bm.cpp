//cpp
/* daEyBm_c -- Mr. I's projectile (EYEKUN_BEAM 264), ov071.
 *
 * It flies straight at 10.0 along the pitch and yaw it spawned with
 * (mPrevAngleX / mPrevAngleY) and bursts (particle 0x45) when it touches
 * ground or a wall, when it hurts a Mario (Player::Hurt, not while he is
 * vanished), or after 150 frames.
 *
 * ov071 is mixed (COFFIN / SCUTTLEBUG / MR_I / BIG_MR_I / MR_I_PROJECTILE).
 * RTTI at ov071:0x02122db8 names this class daEyBm_c. Historical project
 * aliases: MrI_Projectile_Spawn and MrI_Projectile_SpawnInfo. Private helper
 * spellings are inferred; class ownership, bodies, calls and ordering are
 * proven.
 *
 * leftovers:
 * - common.h first is load-bearing: InitResources assigns
 *   IDENTITY_MATRIX4X3 as three 4-word ldm/stm copies, which the flat
 *   s32 m[12] spelling reproduces and math/Matrix.h's nested {r,t}
 *   splits. UpdateShadow's m[9..11] writes come from the same layout.
 * - ROM-descending source order: the factory stays first and
 *   OnYoshiTryEat last. InitResources is the key function; the inline
 *   destructor emits D1 then D0, the RTTI/vtable, and no D2.
 * - DaEyBmVector3Words, not Vector3: a Vector3 stack local would
 *   instantiate its vague-linkage D1 into this TU. EyBmVec3's
 *   declared-not-defined destructor is that D1's alias.
 * - dCcAcPos_c::Init, dBgCh_Actr::Init, DropShadowRadHeight and
 *   Player::Hurt stay mangled: by-value Fix12<int> is wall 6az, and
 *   dBgCh_Actr::Init's header also mangles Fix12i as `i` where the ROM
 *   has 5Fix12IiE.
 * - Particle::System::New / NewUnkCallback818 stay mangled: no shared
 *   header declares them. The UpdateDiscreteNoLava veneer stays because
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
#include "Player.h"

namespace Particle { struct Callback; }

/* Local three-word ABI value. Vector3 has a genuine non-trivial destructor;
 * these stack values are plain fixed-point words and must not instantiate its
 * otherwise unrelated vague-linkage D1 in this class TU. */
struct DaEyBmVector3Words { Fix12i x, y, z; };

/* Dynamic offset at 0x021230b8. A user constructor keeps the three stores in
 * the sinit (bss, not .data). The undefined destructor is Vector3's D1. */
struct EyBmVec3 {
    int x, y, z;
    EyBmVec3(int a, int b, int c) {
        x = a;
        y = b;
        z = c;
    }
    ~EyBmVec3();
};

extern EyBmVec3 data_ov071_021230b8;

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

/* The leaf operator new forwards fBase_c::operator new; the implicit constructor
 * emits the measured dActor_c C2, vptr store, and three member C1s. */
// @symbol daEyBm_c_classInit
extern "C" daEyBm_c *daEyBm_c_classInit()
{
    return new daEyBm_c();
}

/* The actor-table descriptor, at ov071:0x02122dc4. */
extern "C" SpawnInfo<daEyBm_c> g_profile_EYEKUN_BEAM = {
    daEyBm_c_classInit,
    0x0108,
    0x007a,
    0x00000002,
    0,
    0x00019000,
    0x01000000,
    0x01000000
};

// @symbol _ZN8daEyBm_c13InitResourcesEv
extern "C" {
/* Known by-value Fix12 compiler walls: these exact ABI imports preserve the
 * retail register calling convention while the owned objects remain typed. */
extern int _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *collision, dActor_c *actor, const Vector3 *offset,
    Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags);
extern int _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *collision, dActor_c *actor, Fix12i radius, Fix12i height,
    void *a, void *b);
extern Matrix4x3 IDENTITY_MATRIX4X3;
}

int daEyBm_c::InitResources()
{
    if (!mShadowModel.InitCylinder())
        return 0;

    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mdCcAcPos_c, this, (const Vector3 *)&data_ov071_021230b8,
        0x23000, 0x3c000, 0x200002, 0x8000);
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x19000, 0x19000, 0, 0);
    mVertAccel = 0;
    mTerminalVelocity = 0;
    mHorzSpeed = 0xa000;
    mLifeTimer = 0x96;
    mMatrix = IDENTITY_MATRIX4X3;
    UpdateShadow();
    return 1;
}

// @symbol _ZN8daEyBm_c8BehaviorEv
extern "C" {
extern void Matrix4x3_FromRotationY(Matrix4x3 *matrix, s16 angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *matrix, s16 angle);
extern void MulVec3Mat4x3(
    const DaEyBmVector3Words *source, const Matrix4x3 *matrix,
    DaEyBmVector3Words *destination);
extern u16 DecIfAbove0_Short(u16 *value);
extern Matrix4x3 data_020a0e68;
}

int daEyBm_c::Behavior()
{
    DaEyBmVector3Words offset;
    DaEyBmVector3Words velocity;
    if (IsInYoshiMouth(this))
        return 1;

    if (!IsBeingEaten(this)) {
        offset = *(DaEyBmVector3Words *)&data_ov071_021230b8;
        mdCcAcPos_c.SetPosRelativeToActor(*(Vector3 *)&offset);

        velocity.z = mHorzSpeed;
        velocity.x = 0;
        velocity.y = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mPrevAngleX);
        MulVec3Mat4x3(
            &velocity, &data_020a0e68, (DaEyBmVector3Words *)&unk_0a4);

        /* Retail retains a typed whole-vector writeback after the transform. */
        *(DaEyBmVector3Words *)&unk_0a4 =
            *(DaEyBmVector3Words *)&unk_0a4;

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

// @symbol _ZN8daEyBm_c6RenderEv
extern "C" u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    u32 handle, u32 effectID, Fix12i x, Fix12i y, Fix12i z,
    const Vector3_16f *rotation);

int daEyBm_c::Render()
{
    mParticle1 =
        _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            mParticle1, 0x46, mPosX, mPosY, mPosZ, 0);
    mParticle2 =
        _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            mParticle2, 0x47, mPosX, mPosY, mPosZ, 0);
    return 1;
}
// @symbol _ZN8daEyBm_c16OnPendingDestroyEv
void daEyBm_c::OnPendingDestroy()
{
}

// @symbol _ZN8daEyBm_c16CleanupResourcesEv
int daEyBm_c::CleanupResources()
{
    return 1;
}
// @symbol _ZN8daEyBm_c12UpdateShadowEv
extern "C" void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
    dActor_c *actor, dExtShadowModel_c *shadow, Matrix4x3 *matrix,
    Fix12i radius, Fix12i depth, u32 opacity);

void daEyBm_c::UpdateShadow()
{
    mMatrix.m[9] = mPosX >> 3;
    mMatrix.m[10] = mPosY >> 3;
    mMatrix.m[11] = mPosZ >> 3;

    dBgCh_Gnd ground;
    ground.SetObjAndPos(mPos, this);
    Fix12i depth;
    if (ground.DetectClsn())
        depth = (mPosY - ground.clsnY) + 0x1e000;
    else
        depth = 0x1f4000;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mMatrix, 0x50000, depth, 0xf);
}

/*
 * 0x02121ba4, size 0xc8. Player::Hurt retains the measured by-value Fix12
 * ABI seam; Player itself and all accessed fields are the real type. */
// @symbol _ZN8daEyBm_c10HurtPlayerEv
extern "C" void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    Player *player, const DaEyBmVector3Words *position, u32 source,
    Fix12i amount, u32 a, u32 b, u32 c);

void daEyBm_c::HurtPlayer()
{
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

    Player *player = (Player *)found;
    if (player->mIsVanish)
        return;

    DaEyBmVector3Words position;
    position.x = mPosX;
    position.y = mPosY;
    position.z = mPosZ;
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
        player, &position, 1, 0xc000, 1, 0, 1);
    SpawnDestroyEffect();
}

/*
 * 0x02121b50, size 0x54. The coined mangling uses a reference; a pointer
 * would generate identical ARM. The retail caller passes the owned
 * collision subobject explicitly in r1. */
// @symbol _ZN8daEyBm_c15UpdateCollisionER10dBgCh_Actr
extern "C" void dBgCh_Actr_UpdateDiscreteNoLava_veneer(dBgCh_Actr *collision);

void daEyBm_c::UpdateCollision(dBgCh_Actr &collision)
{
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&collision);
    if (collision.IsOnGround() || collision.IsOnWall())
        SpawnDestroyEffect();
}

/*
 * 0x02121b08, size 0x48. Particle::System::New is not yet shared-header
 * declared, so this exact typed ABI import remains local. */
// @symbol _ZN8daEyBm_c18SpawnDestroyEffectEv
extern "C" u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z,
    const Vector3_16f *rotation, Particle::Callback *callback);

void daEyBm_c::SpawnDestroyEffect()
{
    mParticle1 =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 0x45, mPosX, mPosY, mPosZ, 0, 0);
    MarkForDestruction();
}

// @symbol _ZN8daEyBm_c13OnYoshiTryEatEv
int daEyBm_c::OnYoshiTryEat()
{
    return 4;
}

EyBmVec3 data_ov071_021230b8(0, -0x19000, 0);
// @symbol _ZN8daEyBm_cD1Ev
// @symbol _ZN8daEyBm_cD0Ev
/* The inline class destructor and InitResources vtable instantiation emit both naturally. */
