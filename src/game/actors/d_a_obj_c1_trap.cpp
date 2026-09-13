//cpp
/* Production translation unit for ov010/daObjC1_Trap_c.
 * deslop
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
 * Leftover:
 * - dBgW_KcMbg::SetFile stays mangled (Fix12<int> by value, wall 6az)
 * - TrapVector3 POD locals; Vector3's empty destructor would add an
 *   unrelated helper to this TU
 * - InitResources mAngleY uses (int)this+0x8e (named member CSE/size-DIFF)
 * - func_020393c4 stores dBgW+0x1c (no setter)
 * - data_ov010_02112d08 / 02112d00 SharedFilePtr handles; data_ov010_021122f8
 *   CLPS; data_ov010_02112d28 PMF table (sinit-owned)
 * - common.h first via dBgActor_c.h (Matrix4x3 copies in UpdateModel/Collision)
 * - func_ov010_02111984 three-argument C ABI collider adapter
 */

#include "daObjC1_Trap_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Sound.h"

/* The two copied position locals must stay POD. The shared Vector3 has an
 * empty destructor, which would add an unrelated compiler helper to this TU. */
struct TrapVector3 {
    s32 x;
    s32 y;
    s32 z;
};

struct TrapSpawnInfo {
    daObjC1_Trap_c *(*spawn)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 flags;
    Fix12i rangeOffsetY;
    Fix12i range;
    Fix12i drawDistance;
    u32 unk_18;
};

typedef char TrapSpawnInfo_size_must_be_0x1c[
    sizeof(TrapSpawnInfo) == 0x1c ? 1 : -1];

typedef void (daObjC1_Trap_c::*TrapState)();

/* Fix12-by-value SetFile and the TrapVector3 POD seam are measured ABI walls.
 * The remaining declarations are genuine free functions or ROM-address
 * globals. */
extern "C" {
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *collider, void *file, const Matrix4x3 *matrix,
    int scale, s16 angleY, CLPS_Block *clps);
extern void func_020393c4(dBgW *collider, void *callback);

extern void Matrix4x3_FromTranslation(
    Matrix4x3 *matrix, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(
    Matrix4x3 *matrix, s16 angle);
extern void Matrix4x3_ApplyInPlaceToRotationZ(
    Matrix4x3 *matrix, s16 angle);
extern s16 Vec3_HorzAngle(const TrapVector3 *from, const TrapVector3 *to);
extern s32 Vec3_HorzDist(const TrapVector3 *from, const TrapVector3 *to);

extern Matrix4x3 data_020a0e68;
extern s16 data_02082214[];
extern SharedFilePtr data_ov010_02112d08;
extern SharedFilePtr data_ov010_02112d00;
extern CLPS_Block data_ov010_021122f8;
extern TrapState data_ov010_02112d28[];

void func_ov010_02111984(
    int unused, daObjC1_Trap_c *trap, dActor_c *other);
}


/* Reconstructed source-style name: SM64DS proves daObjC1_Trap_c through RTTI,
 * allocation size, vtable identity, and the C1_TRAP registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Trap_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * The header's inline operator new keeps the allocation on fBase_c::operator
 * new; without it the call relocates to the unavailable global `_Znwm`. */
extern "C" daObjC1_Trap_c *daObjC1_Trap_c_classInit()
{
    return new daObjC1_Trap_c();
}

extern "C" TrapSpawnInfo g_profile_C1_TRAP = {
    daObjC1_Trap_c_classInit,
    0x0024,
    0x0143,
    0,
    0,
    0x001f4000,
    0,
    0
};

/*
 * registered directly in InitResources. */
extern "C" void func_ov010_02111984(
    int unused, daObjC1_Trap_c *trap, dActor_c *other)
{
    trap->OnCollision(*other);
}


void daObjC1_Trap_c::OnCollision(dActor_c &other)
{
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
    projectedDistance =
        (s32)(((s64)distance * projection + 0x800) >> 12);
    if (projectedDistance < 0x10e000)
        spawner->mTrapActive = 1;
}


int daObjC1_Trap_c::InitResources()
{
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
        spawned = dActor_c::Spawn(
            0x24, 0, *(const Vector3 *)&position,
            (const Vector3_16 *)&mAngleX, mAreaId, -1);
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
        spawned = dActor_c::Spawn(
            0x24, 1, *(const Vector3 *)&position,
            (const Vector3_16 *)&mAngleX, mAreaId, -1);
        ((daObjC1_Trap_c *)spawned)->mSpawnerID = uniqueID;
        return 1;
    }

    mIsSpawner = 0;
    mDoorModel.SetFile(
        (BMD_File *)Model::LoadFile(data_ov010_02112d08), 1, -1);
    UpdateModelTransform();
    UpdateCollisionTransform();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, dBgW_Kc::LoadFile(data_ov010_02112d00),
        &mDoorMat, 0x1000, mAngleY, &data_ov010_021122f8);
    func_020393c4(&mMeshCollider, (void *)func_ov010_02111984);
    mMeshCollider.Enable(this);
    mOpenSpeed = 0;
    mState = 0;

    if ((param1 & 0xff) == 1) {
        /* Named `mAngleY = mAngleY + 0x8000` size-DIFFs InitResources
         * (addressing shape / CSE). Keep the MATCH form. */
        s16 *angleY = (s16 *)((int)this + 0x8e);
        *angleY = *angleY + 0x8000;
    }
    return 1;
}


int daObjC1_Trap_c::Behavior()
{
    if (mIsSpawner) {
        mPlayerDist = DistToCPlayer();
        return 1;
    }

    TrapState *states = data_ov010_02112d28;
    (this->*states[mState])();
    UpdateModelTransform();
    UpdateCollisionTransform();
    return 1;
}


int daObjC1_Trap_c::Render()
{
    if (!mIsSpawner)
        mDoorModel.Render(0);
    return 1;
}


int daObjC1_Trap_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    if ((param1 & 0xff) != 0xff) {
        data_ov010_02112d08.Release();
        data_ov010_02112d00.Release();
    }
    return 1;
}


void daObjC1_Trap_c::UpdateModelTransform()
{
    int angleY = (int)(u16)mAngleY >> 4;
    int angleZ = (int)(u16)mAngleZ >> 4;
    int sinZ = data_02082214[angleZ * 2];
    int cosY = data_02082214[angleY * 2 + 1];
    int sinY = data_02082214[angleY * 2];
    int radius = sinZ * 5;
    int offsetX = (int)(((s64)radius * cosY + 0x800) >> 12);
    int offsetZ = (int)(((s64)radius * sinY + 0x800) >> 12);
    int x = (mPosX - offsetX) >> 3;
    int y = mPosY >> 3;
    int z = (mPosZ + offsetZ) >> 3;

    Matrix4x3_FromTranslation(&data_020a0e68, x, y, z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
    mDoorModel.mat4x3 = data_020a0e68;
}


void daObjC1_Trap_c::UpdateCollisionTransform()
{
    Matrix4x3_FromTranslation(
        &data_020a0e68, mPosX, mPosY, mPosZ);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
    mDoorMat = data_020a0e68;
    mMeshCollider.Transform(mDoorMat, mAngleY);
}


daObjC1_Trap_c *daObjC1_Trap_c::GetSpawner()
{
    if (mSpawnerID == 0) {
        MarkForDestruction();
        return 0;
    }

    daObjC1_Trap_c *spawner =
        (daObjC1_Trap_c *)dActor_c::FindWithID(mSpawnerID);
    if (!spawner)
        MarkForDestruction();
    return spawner;
}


void daObjC1_Trap_c::State0()
{
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
    Sound::PlayBank3(0xe, *(Vector3 *)&mCamSpacePosX);
}


void daObjC1_Trap_c::State1()
{
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


void daObjC1_Trap_c::State2()
{
    daObjC1_Trap_c *spawner = GetSpawner();
    if (spawner && !spawner->mTrapActive)
        mState = 3;
}


void daObjC1_Trap_c::State3()
{
    mAngleZ += 0x400;
    if (mAngleZ > 0) {
        mAngleZ = 0;
        mState = 0;
    }
}


void daObjC1_Trap_c::State4()
{
    mAngleZ = -0x3c00;
}

/*
 * InitResources-owned vtable: D1 at 0x021111a0, D0 at 0x021111ec. */
