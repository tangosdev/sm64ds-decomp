//cpp
/* Genuine production translation unit for ov071/daEyBm_c.
 *
 * daEyBm_c_classInit and g_profile_EYEKUN_BEAM are reconstructed source-style
 * names. SM64DS proves the daEyBm_c RTTI identity, EYEKUN_BEAM registry ID,
 * descriptor/factory relationship, and object shape; later EAD lineage
 * supplies the spelling prior. Exact original SM64DS symbols are not
 * preserved. Historical project aliases: MrI_Projectile_Spawn and
 * MrI_Projectile_SpawnInfo. The private helper spellings are inferred; their
 * class ownership, bodies, calls and ordering are proven.
 *
 * mwccarm emits ordinary function sections in reverse source order. Keep the
 * ROM-high factory first and OnYoshiTryEat last. InitResources is the key
 * function; together with the inline destructor in the real header this
 * naturally emits retail D1 then D0 and the class RTTI/vtable, without D2 or a
 * forcing object.
 */

#include "daEyBm_c.h"
#include "dBgCh_Gnd.h"
#include "Particle__System.h"
#include "Player.h"

namespace Particle { struct Callback; }

/* Local three-word ABI value. Vector3 has a genuine non-trivial destructor;
 * these stack values are plain fixed-point words and must not instantiate its
 * otherwise unrelated vague-linkage D1 in this class TU. */
struct DaEyBmVector3Words { Fix12i x, y, z; };

/* Typed reconstruction of the actor-table descriptor at ov071:0x02122dc4. */
struct DaEyBmSpawnInfo {
    daEyBm_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char DaEyBmSpawnInfo_size_must_be_0x1c[
    sizeof(DaEyBmSpawnInfo) == 0x1c ? 1 : -1];

/* ROM ordinal 12 -- class initializer, 0x02121f9c, size 0x48. Natural new
 * targets the wrong allocator; retain the measured actor construction seam. */
extern "C" {
extern void *_ZN7fBase_cnwEj(u32 size);
extern void _ZN8dActor_cC2Ev(dActor_c *actor);
extern void _ZN11ShadowModelC1Ev(ShadowModel *shadow);
extern void _ZN10dCcAcPos_cC1Ev(dCcAcPos_c *collision);
extern void _ZN10dBgCh_ActrC1Ev(dBgCh_Actr *collision);

// @symbol daEyBm_c_classInit
daEyBm_c *daEyBm_c_classInit()
{
    daEyBm_c *actor = (daEyBm_c *)_ZN7fBase_cnwEj(sizeof(daEyBm_c));
    if (actor) {
        _ZN8dActor_cC2Ev(actor);
        *(int *)actor = (int)&_ZTV8daEyBm_c[2];
        _ZN11ShadowModelC1Ev(&actor->mShadowModel);
        _ZN10dCcAcPos_cC1Ev(&actor->mdCcAcPos_c);
        _ZN10dBgCh_ActrC1Ev(&actor->mWithMeshClsn);
    }
    return actor;
}
}

extern "C" DaEyBmSpawnInfo g_profile_EYEKUN_BEAM = {
    daEyBm_c_classInit,
    0x0108,
    0x007a,
    0x00000002,
    0,
    0x00019000,
    0x01000000,
    0x01000000
};

/* ROM ordinal 11 -- InitResources, 0x02121eb4, size 0xe8. */
struct DaEyBmMatrixWords { s32 words[12]; };

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
extern DaEyBmVector3Words data_ov071_021230b8;
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
    *(DaEyBmMatrixWords *)&mMatrix =
        *(DaEyBmMatrixWords *)&IDENTITY_MATRIX4X3;
    UpdateShadow();
    return 1;
}

/* ROM ordinal 10 -- Behavior, 0x02121d80, size 0x134. */
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
    DaEyBmVector3Words speed;
    int flags = mFlags;
    int destroying = (int)((flags & 0x40000) != 0);
    int beingEaten;
    if (destroying)
        return 1;

    beingEaten = (int)((flags & 0x20000) != 0);
    if (!beingEaten) {
        offset = data_ov071_021230b8;
        mdCcAcPos_c.SetPosRelativeToActor(*(Vector3 *)&offset);

        speed.z = mHorzSpeed;
        speed.x = 0;
        speed.y = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mPrevAngleX);
        MulVec3Mat4x3(
            &speed, &data_020a0e68, (DaEyBmVector3Words *)&unk_0a4);

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

/* ROM ordinal 9 -- Render, 0x02121d14, size 0x6c. */
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

/* ROM ordinal 8 -- OnPendingDestroy, 0x02121d10, size 0x4. */
void daEyBm_c::OnPendingDestroy()
{
}

/* ROM ordinal 7 -- CleanupResources, 0x02121d08, size 0x8. */
int daEyBm_c::CleanupResources()
{
    return 1;
}

/* ROM ordinal 6 -- inferred daEyBm_c::UpdateShadow,
 * 0x02121c6c, size 0x9c. */
extern "C" void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    dActor_c *actor, ShadowModel *shadow, Matrix4x3 *matrix,
    Fix12i radius, Fix12i depth, u32 opacity);

void daEyBm_c::UpdateShadow()
{
    mMatrix.t.x = mPosX >> 3;
    mMatrix.t.y = mPosY >> 3;
    mMatrix.t.z = mPosZ >> 3;

    dBgCh_Gnd ground;
    ground.SetObjAndPos(*(Vector3 *)&mPosX, this);
    Fix12i depth;
    if (ground.DetectClsn())
        depth = (mPosY - ground.clsnY) + 0x1e000;
    else
        depth = 0x1f4000;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mMatrix, 0x50000, depth, 0xf);
}

/* ROM ordinal 5 -- inferred daEyBm_c::HurtPlayer,
 * 0x02121ba4, size 0xc8. Player::Hurt retains the measured by-value Fix12
 * ABI seam; Player itself and all accessed fields are the real type. */
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
    int isPlayer = (int)(found->actorID == 0xbf);
    if (!isPlayer)
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

/* ROM ordinal 4 -- inferred daEyBm_c::UpdateCollision,
 * 0x02121b50, size 0x54. The reference parameter is retained because the
 * retail caller passes the owned collision subobject explicitly in r1. */
extern "C" void dBgCh_Actr_UpdateDiscreteNoLava_veneer(dBgCh_Actr *collision);

void daEyBm_c::UpdateCollision(dBgCh_Actr &collision)
{
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&collision);
    if (collision.IsOnGround() || collision.IsOnWall())
        SpawnDestroyEffect();
}

/* ROM ordinal 3 -- inferred daEyBm_c::SpawnDestroyEffect,
 * 0x02121b08, size 0x48. Particle::System::New is not yet shared-header
 * declared, so this exact typed ABI import remains local. */
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

/* ROM ordinal 2 -- OnYoshiTryEat, 0x02121b00, size 0x8. */
int daEyBm_c::OnYoshiTryEat()
{
    return 4;
}

/* ROM ordinals 0/1 -- D1 0x02121a6c and D0 0x02121aac. The inline class
 * destructor and InitResources vtable instantiation emit both naturally. */
