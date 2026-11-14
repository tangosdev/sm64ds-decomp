//cpp
/* Production translation unit for ov010/daObjC1_Trap_c.
 *
 * Castle trap doors (profile C1_TRAP / TRAP(36)). ov010 also LIGHT_BEAM /
 * PEACH_PAINTING / ROTATING_COG_BIG. RTTI ov010:0x02112aa0 names
 * daObjC1_Trap_c as a direct dBgActor_c subclass; ugly RTTI name is final.
 *
 * The seventeen functions are defined in reverse ROM order because mwccarm
 * emits ordinary function sections in reverse source order. InitResources is
 * the class's out-of-line key function; its vtable references the inline
 * destructor in daObjC1_Trap_c.h, which makes the compiler emit retail's D1
 * then D0 pair without a D2 or a forcing helper.
 *
 * What stays, and why:
 * - VecFx32 POD locals; Vector3's empty destructor would add an
 *   unrelated helper to this TU
 * - InitResources adds to mAngleY through a pointer local (plain
 *   `mAngleY += 0x8000` CSE/size-DIFFs)
 * - data_ov010_02112d08 / 02112d00 are this file's resource handles;
 *   data_ov010_02112d28 is the state record the compiler copies into .bss;
 *   data_ov010_021122f8 is CLPS
 * - common.h first via dBgActor_c.h (Matrix4x3 copies in UpdateModel/Collision)
 * - func_ov010_02111984 three-argument C ABI collider adapter
 * - VecFx32 POD locals stay trivial; math/CalcLib.h's vector helpers operate
 *   on the same three-word buffers through their Vector3 pointer interface
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_c1_trap_AssetId { ASSET_C1_TRAP_BMD = 0x05af, ASSET_C1_TRAP_KCL = 0x05b0 };

#include "daObjC1_Trap_c.h"
#include "SpawnInfo.h"
#include "daPly_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "nitro/fx/fx.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx_trig.h"

/* The two copied position locals must stay POD. The shared Vector3 has an
 * empty destructor, which would add an unrelated compiler helper to this TU. */

typedef void (daObjC1_Trap_c::*TrapState)();

struct TrapStateRec {
    TrapState state0;
    TrapState state1;
    TrapState state2;
    TrapState state3;
    TrapState state4;
};

/* 8-byte resource handles. The ctor/dtor bodies are the ROM functions the
 * initializer calls; this TU only names them. */
struct TrapModelFilePtr : SharedFilePtr {
    TrapModelFilePtr(u32 fileID);
    ~TrapModelFilePtr();
};

struct TrapCollisionFilePtr : SharedFilePtr {
    TrapCollisionFilePtr(u32 fileID);
    ~TrapCollisionFilePtr();
};

extern CLPS_Block data_ov010_021122f8;
extern TrapModelFilePtr data_ov010_02112d08;
extern TrapCollisionFilePtr data_ov010_02112d00;
extern TrapStateRec data_ov010_02112d28;

/* Reconstructed source-style name: SM64DS proves daObjC1_Trap_c through RTTI,
 * allocation size, vtable identity, and the C1_TRAP registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Trap_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * The header's inline operator new keeps the allocation on fBase_c::operator
 * new; without it the call relocates to the unavailable global `_Znwm`. */
daObjC1_Trap_c *daObjC1_Trap_c_classInit() {
    return new daObjC1_Trap_c();
}

extern "C" SpawnInfo<daObjC1_Trap_c> g_profile_C1_TRAP = {
    daObjC1_Trap_c_classInit, 0x0024, 0x0143, 0, 0, 0x001f4000, 0, 0};

/*
 * registered directly in InitResources. */
void func_ov010_02111984(dBgW *collider, dActor_c *owner, dActor_c *other) {
    static_cast<daObjC1_Trap_c *>(owner)->OnCollision(*other);
}

void daObjC1_Trap_c::OnCollision(dActor_c &other) {
    daObjC1_Trap_c *spawner;
    u32 spawnerID;
    VecFx32 otherPos;
    VecFx32 spawnerPos;
    VecFx32 *otherPosition;
    VecFx32 *spawnerPosition;
    s16 angle;
    s16 relativeAngle;
    s32 distance;
    s16 projection;
    s32 projectedDistance;
    s32 isPlayer;

    if (mState != 0)
        return;
    if (mAngleZ < -0x3000)
        return;
    spawnerID = mSpawnerID;
    if (spawnerID == 0)
        return;
    spawner = (daObjC1_Trap_c *)dActor_c::FindWithID(spawnerID);
    if (!spawner)
        return;
    isPlayer = (other.actorID == 0xbf);
    if (!isPlayer)
        return;

    otherPosition = (VecFx32 *)&other.mPosX;
    spawnerPosition = (VecFx32 *)&spawner->mPosX;
    otherPos.x = otherPosition->x;
    otherPos.y = otherPosition->y;
    otherPos.z = otherPosition->z;
    spawnerPos.x = spawnerPosition->x;
    spawnerPos.y = spawnerPosition->y;
    spawnerPos.z = spawnerPosition->z;
    angle = Vec3_HorzAngle((const Vector3 *)&spawnerPos, (const Vector3 *)&otherPos);
    relativeAngle = (angle + spawner->mAngleY);
    distance = Vec3_HorzDist((const Vector3 *)&spawnerPos, (const Vector3 *)&otherPos);
    projection = FX_SinIdx((u16)relativeAngle >> 4);
    projectedDistance = FX_MUL(distance, projection);
    if (projectedDistance < 0x10e000)
        spawner->mTrapActive = 1;
}


int daObjC1_Trap_c::InitResources() {
    mTrapActive = 0;
    mSpawnerID = 0;

    if ((param1 & 0xff) == 0xff) {
        Vector3 position;
        int sinAngle;
        int cosAngle;
        int x;
        int y;
        int z;
        daObjC1_Trap_c *spawned;

        mIsSpawner = 1;
        mPlayerDist = 0;

        cosAngle = FX_CosIdx((u16)mAngleY >> 4);
        sinAngle = FX_SinIdx((u16)mAngleY >> 4);
        z = mPosZ + sinAngle * 0x15d;
        x = mPosX - cosAngle * 0x15d;
        y = mPosY;
        position.x = x;
        position.y = y;
        position.z = z;
        spawned = (daObjC1_Trap_c *)dActor_c::Spawn(0x24, 0, position, &mAngle, mAreaId, -1);
        spawned->mSpawnerID = uniqueID;

        sinAngle = FX_SinIdx((u16)mAngleY >> 4);
        cosAngle = FX_CosIdx((u16)mAngleY >> 4);
        z = mPosZ - sinAngle * 0x15d;
        x = cosAngle * 0x15d + mPosX;
        y = mPosY;
        position.x = x;
        position.y = y;
        position.z = z;
        spawned = (daObjC1_Trap_c *)dActor_c::Spawn(0x24, 1, position, &mAngle, mAreaId, -1);
        spawned->mSpawnerID = uniqueID;
        return 1;
    }

    mIsSpawner = 0;
    mDoorModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov010_02112d08), 1, -1);
    UpdateModelTransform();
    UpdateCollisionTransform();
    mMeshCollider.SetFile(dBgW_Kc::LoadFile(data_ov010_02112d00), mDoorMat, 0x1000, mAngleY,
                          data_ov010_021122f8);
    mMeshCollider.SetStoodOnCallback(func_ov010_02111984);
    mMeshCollider.Enable(this);
    mOpenSpeed = 0;
    mState = 0;

    if ((param1 & 0xff) == 1) {
        mAngle.y += 0x8000;
    }
    return 1;
}


int daObjC1_Trap_c::Behavior() {
    if (mIsSpawner) {
        mPlayerDist = DistToCPlayer();
        return 1;
    }

    TrapState *states = (TrapState *)&data_ov010_02112d28;
    (this->*states[mState])();
    UpdateModelTransform();
    UpdateCollisionTransform();
    return 1;
}


int daObjC1_Trap_c::Render() {
    if (!mIsSpawner)
        mDoorModel.Render(0);
    return 1;
}


int daObjC1_Trap_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    if ((param1 & 0xff) != 0xff) {
        data_ov010_02112d08.Release();
        data_ov010_02112d00.Release();
    }
    return 1;
}


void daObjC1_Trap_c::UpdateModelTransform() {
    int sinZ = FX_SinIdx((u16)mAngleZ >> 4);
    int cosY = FX_CosIdx((u16)mAngleY >> 4);
    int sinY = FX_SinIdx((u16)mAngleY >> 4);
    int radius = sinZ * 5;
    int offsetX = FX_MUL(radius, cosY);
    int offsetZ = FX_MUL(radius, sinY);
    int x = (mPosX - offsetX) >> 3;
    int y = mPosY >> 3;
    int z = (mPosZ + offsetZ) >> 3;

    Matrix4x3_FromTranslation(&data_020a0e68, x, y, z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
    mDoorModel.mat4x3 = data_020a0e68;
}


void daObjC1_Trap_c::UpdateCollisionTransform() {
    Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
    mDoorMat = data_020a0e68;
    mMeshCollider.Transform(mDoorMat, mAngleY);
}


daObjC1_Trap_c *daObjC1_Trap_c::GetSpawner() {
    if (mSpawnerID == 0) {
        MarkForDestruction();
        return 0;
    }

    daObjC1_Trap_c *spawner = (daObjC1_Trap_c *)dActor_c::FindWithID(mSpawnerID);
    if (!spawner)
        MarkForDestruction();
    return spawner;
}


void daObjC1_Trap_c::StateClosed() {
    daPly_c *player = ClosestPlayer();
    if (!player)
        return;
    if (player->IsEnteringLevel()) {
        mState = 4;
        return;
    }

    mOpenSpeed = 0x400;
    daObjC1_Trap_c *spawner = GetSpawner();
    if (!spawner || !spawner->mTrapActive)
        return;
    mState = 1;
    Sound::PlayBank3(0xe, mCamSpacePos);
}


void daObjC1_Trap_c::StateOpening() {
    daObjC1_Trap_c *spawner = GetSpawner();
    if (!spawner)
        return;

    spawner->mTrapActive = 0;
    mOpenSpeed -= 0x100;
    mAngleZ += mOpenSpeed;
    if (mAngleZ < -0x3d00) {
        mAngleZ = -0x3d00;
        mState = 2;
    }
}


void daObjC1_Trap_c::StateOpen() {
    daObjC1_Trap_c *spawner = GetSpawner();
    if (spawner && !spawner->mTrapActive)
        mState = 3;
}


void daObjC1_Trap_c::StateClosing() {
    mAngleZ += 0x400;
    if (mAngleZ > 0) {
        mAngleZ = 0;
        mState = 0;
    }
}


void daObjC1_Trap_c::StateHeldOpen() {
    mAngleZ = -0x3c00;
}

/* ROM ordinals 1 and 0 are emitted from the inline destructor and the
 * InitResources-owned vtable: D1 at 0x021111a0, D0 at 0x021111ec. */

/* mwcc copies these descriptors in __sinit_d_a_obj_c1_trap.cpp. */
TrapModelFilePtr data_ov010_02112d08(ASSET_C1_TRAP_BMD);
TrapCollisionFilePtr data_ov010_02112d00(ASSET_C1_TRAP_KCL);
TrapStateRec data_ov010_02112d28 = {
    &daObjC1_Trap_c::StateClosed,  &daObjC1_Trap_c::StateOpening,  &daObjC1_Trap_c::StateOpen,
    &daObjC1_Trap_c::StateClosing, &daObjC1_Trap_c::StateHeldOpen,
};
