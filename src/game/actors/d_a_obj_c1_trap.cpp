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
 * - TrapVector3 POD locals; Vector3's empty destructor would add an
 *   unrelated helper to this TU
 * - InitResources adds to mAngleY through a pointer local (plain
 *   `mAngleY += 0x8000` CSE/size-DIFFs)
 * - func_020393c4 stores dBgW+0x1c (no setter)
 * - data_ov010_02112d08 / 02112d00 are this file's resource handles;
 *   data_ov010_02112d28 is the state record the compiler copies into .bss;
 *   data_ov010_021122f8 is CLPS
 * - common.h first via dBgActor_c.h (Matrix4x3 copies in UpdateModel/Collision)
 * - func_ov010_02111984 three-argument C ABI collider adapter
 * - Vec3_HorzAngle / Vec3_HorzDist stay local: they take the TrapVector3
 *   locals, and math/CalcLib.h's spelling takes const Vector3 *
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_c1_trap_AssetId { ASSET_C1_TRAP_BMD = 0x05af, ASSET_C1_TRAP_KCL = 0x05b0 };

#include "daObjC1_Trap_c.h"
#include "SpawnInfo.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "nitro/fx/fx.h"
#include "math/MtxStack.h"

/* The two copied position locals must stay POD. The shared Vector3 has an
 * empty destructor, which would add an unrelated compiler helper to this TU. */
struct TrapVector3 {
    s32 x;
    s32 y;
    s32 z;
};

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

extern s16 data_02082214[];
extern CLPS_Block data_ov010_021122f8;
extern TrapModelFilePtr data_ov010_02112d08;
extern TrapCollisionFilePtr data_ov010_02112d00;
extern TrapStateRec data_ov010_02112d28;

/* local extern: the dBgW callback setter and the two horizontal helpers, on
   the TrapVector3 POD seam (see the banner). */
extern "C" {
extern void func_020393c4(dBgW *collider, void *callback);
extern s16 Vec3_HorzAngle(const TrapVector3 *from, const TrapVector3 *to);
extern s32 Vec3_HorzDist(const TrapVector3 *from, const TrapVector3 *to);
}

/* Reconstructed source-style name: SM64DS proves daObjC1_Trap_c through RTTI,
 * allocation size, vtable identity, and the C1_TRAP registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Trap_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * The header's inline operator new keeps the allocation on fBase_c::operator
 * new; without it the call relocates to the unavailable global `_Znwm`. */
// @symbol daObjC1_Trap_c_classInit
extern "C" daObjC1_Trap_c *daObjC1_Trap_c_classInit() {
    return new daObjC1_Trap_c();
}

extern "C" SpawnInfo<daObjC1_Trap_c> g_profile_C1_TRAP = {
    daObjC1_Trap_c_classInit, 0x0024, 0x0143, 0, 0, 0x001f4000, 0, 0};

/*
 * registered directly in InitResources. */
// @symbol func_ov010_02111984
extern "C" void func_ov010_02111984(int unused, daObjC1_Trap_c *trap, dActor_c *other) {
    trap->OnCollision(*other);
}

// @symbol _ZN14daObjC1_Trap_c11OnCollisionER8dActor_c

void daObjC1_Trap_c::OnCollision(dActor_c &other) {
    daObjC1_Trap_c *spawner;
    u32 spawnerID;
    TrapVector3 otherPos;
    TrapVector3 spawnerPos;
    TrapVector3 *otherPosition;
    TrapVector3 *spawnerPosition;
    s16 angle;
    s16 relativeAngle;
    s32 distance;
    s32 index;
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
    isPlayer = (int)(other.actorID == 0xbf);
    if (!isPlayer)
        return;

    otherPosition = (TrapVector3 *)&other.mPosX;
    spawnerPosition = (TrapVector3 *)&spawner->mPosX;
    otherPos.x = otherPosition->x;
    otherPos.y = otherPosition->y;
    otherPos.z = otherPosition->z;
    spawnerPos.x = spawnerPosition->x;
    spawnerPos.y = spawnerPosition->y;
    spawnerPos.z = spawnerPosition->z;
    angle = Vec3_HorzAngle(&spawnerPos, &otherPos);
    relativeAngle = (s16)(angle + spawner->mAngleY);
    distance = Vec3_HorzDist(&spawnerPos, &otherPos);
    index = (u16)relativeAngle >> 4;
    projection = data_02082214[index * 2];
    projectedDistance = FX_MUL(distance, projection);
    if (projectedDistance < 0x10e000)
        spawner->mTrapActive = 1;
}

// @symbol _ZN14daObjC1_Trap_c13InitResourcesEv

int daObjC1_Trap_c::InitResources() {
    mTrapActive = 0;
    mSpawnerID = 0;

    if ((param1 & 0xff) == 0xff) {
        TrapVector3 position;
        int index;
        int sinAngle;
        int cosAngle;
        int x;
        int y;
        int z;
        void *spawned;

        mIsSpawner = 1;
        mPlayerDist = 0;

        index = ((int)(u16)mAngleY >> 4) * 2;
        /* data_02082214[2i] = sin, [2i+1] = cos (dScStarSel COS()). */
        cosAngle = data_02082214[index + 1];
        sinAngle = data_02082214[index];
        z = mPosZ + sinAngle * 0x15d;
        x = mPosX - cosAngle * 0x15d;
        y = mPosY;
        position.x = x;
        position.y = y;
        position.z = z;
        spawned = dActor_c::Spawn(0x24, 0, *(const Vector3 *)&position, &mAngle, mAreaId, -1);
        ((daObjC1_Trap_c *)spawned)->mSpawnerID = uniqueID;

        index = ((int)(u16)mAngleY >> 4) * 2;
        sinAngle = data_02082214[index];
        cosAngle = data_02082214[index + 1];
        z = mPosZ - sinAngle * 0x15d;
        x = cosAngle * 0x15d + mPosX;
        y = mPosY;
        position.x = x;
        position.y = y;
        position.z = z;
        spawned = dActor_c::Spawn(0x24, 1, *(const Vector3 *)&position, &mAngle, mAreaId, -1);
        ((daObjC1_Trap_c *)spawned)->mSpawnerID = uniqueID;
        return 1;
    }

    mIsSpawner = 0;
    mDoorModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov010_02112d08), 1, -1);
    UpdateModelTransform();
    UpdateCollisionTransform();
    mMeshCollider.SetFile((KCL_File *)dBgW_Kc::LoadFile(data_ov010_02112d00), mDoorMat, 0x1000,
                          mAngleY, data_ov010_021122f8);
    func_020393c4(&mMeshCollider, (void *)func_ov010_02111984);
    mMeshCollider.Enable(this);
    mOpenSpeed = 0;
    mState = 0;

    if ((param1 & 0xff) == 1) {
        /* Named `mAngleY += 0x8000` size-DIFFs InitResources (addressing
         * shape / CSE); the pointer local keeps the ROM form. */
        s16 *angleY = &mAngleY;
        *angleY = *angleY + 0x8000;
    }
    return 1;
}

// @symbol _ZN14daObjC1_Trap_c8BehaviorEv

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

// @symbol _ZN14daObjC1_Trap_c6RenderEv

int daObjC1_Trap_c::Render() {
    if (!mIsSpawner)
        mDoorModel.Render(0);
    return 1;
}

// @symbol _ZN14daObjC1_Trap_c16CleanupResourcesEv

int daObjC1_Trap_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    if ((param1 & 0xff) != 0xff) {
        data_ov010_02112d08.Release();
        data_ov010_02112d00.Release();
    }
    return 1;
}

// @symbol _ZN14daObjC1_Trap_c20UpdateModelTransformEv

void daObjC1_Trap_c::UpdateModelTransform() {
    int angleY = (int)(u16)mAngleY >> 4;
    int angleZ = (int)(u16)mAngleZ >> 4;
    int sinZ = data_02082214[angleZ * 2];
    int cosY = data_02082214[angleY * 2 + 1];
    int sinY = data_02082214[angleY * 2];
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

// @symbol _ZN14daObjC1_Trap_c24UpdateCollisionTransformEv

void daObjC1_Trap_c::UpdateCollisionTransform() {
    Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
    mDoorMat = data_020a0e68;
    mMeshCollider.Transform(mDoorMat, mAngleY);
}

// @symbol _ZN14daObjC1_Trap_c10GetSpawnerEv

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

// @symbol _ZN14daObjC1_Trap_c11StateClosedEv

void daObjC1_Trap_c::StateClosed() {
    Player *player = ClosestPlayer();
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

// @symbol _ZN14daObjC1_Trap_c12StateOpeningEv

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

// @symbol _ZN14daObjC1_Trap_c9StateOpenEv

void daObjC1_Trap_c::StateOpen() {
    daObjC1_Trap_c *spawner = GetSpawner();
    if (spawner && !spawner->mTrapActive)
        mState = 3;
}

// @symbol _ZN14daObjC1_Trap_c12StateClosingEv

void daObjC1_Trap_c::StateClosing() {
    mAngleZ += 0x400;
    if (mAngleZ > 0) {
        mAngleZ = 0;
        mState = 0;
    }
}

// @symbol _ZN14daObjC1_Trap_c13StateHeldOpenEv

void daObjC1_Trap_c::StateHeldOpen() {
    mAngleZ = -0x3c00;
}

// @symbol _ZN14daObjC1_Trap_cD1Ev
// @symbol _ZN14daObjC1_Trap_cD0Ev
/* ROM ordinals 1 and 0 are emitted from the inline destructor and the
 * InitResources-owned vtable: D1 at 0x021111a0, D0 at 0x021111ec. */

/* mwcc copies these descriptors in __sinit_d_a_obj_c1_trap.cpp. */
TrapModelFilePtr data_ov010_02112d08(ASSET_C1_TRAP_BMD);
TrapCollisionFilePtr data_ov010_02112d00(ASSET_C1_TRAP_KCL);
TrapStateRec data_ov010_02112d28 = {
    &daObjC1_Trap_c::StateClosed,  &daObjC1_Trap_c::StateOpening,  &daObjC1_Trap_c::StateOpen,
    &daObjC1_Trap_c::StateClosing, &daObjC1_Trap_c::StateHeldOpen,
};
