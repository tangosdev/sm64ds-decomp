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
 * - InitResources adds to mAngle.y through a pointer local (plain
 *   `mAngle.y += 0x8000` CSE/size-DIFFs)
 * - data_ov010_02112d08 / 02112d00 are this file's resource handles;
 *   data_ov010_02112d28 is the state record the compiler copies into .bss;
 *   data_ov010_021122f8 is CLPS
 * - StoodOnCallback three-argument C ABI collider adapter
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

/* The block the door's mesh collider takes. CLPS.h keeps CLPS_Block opaque:
   an eight-byte header whose halfword at +4 is the entry stride, then one
   eight-byte CLPS per collider class. */
struct CLPS_Block {
    u32 tag;
    u16 stride;
    u16 count;
    CLPS entries[2];
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
void daObjC1_Trap_c::StoodOnCallback(dBgW *collider, dActor_c *owner, dActor_c *other) {
    static_cast<daObjC1_Trap_c *>(owner)->OnCollision(*other);
}

void daObjC1_Trap_c::OnCollision(dActor_c &other) {
    daObjC1_Trap_c *spawner;
    u32 spawnerID;
    Vector3 otherPos;
    Vector3 spawnerPos;
    const Vector3 *otherPosition;
    const Vector3 *spawnerPosition;
    s16 angle;
    s16 relativeAngle;
    s32 distance;
    s16 projection;
    s32 projectedDistance;
    s32 isPlayer;

    if (mState != 0)
        return;
    if (mAngle.z < -0x3000)
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

    otherPosition = &other.mPos;
    spawnerPosition = &spawner->mPos;
    otherPos.x = otherPosition->x;
    otherPos.y = otherPosition->y;
    otherPos.z = otherPosition->z;
    spawnerPos.x = spawnerPosition->x;
    spawnerPos.y = spawnerPosition->y;
    spawnerPos.z = spawnerPosition->z;
    angle = Vec3_HorzAngle(&spawnerPos, &otherPos);
    relativeAngle = (angle + spawner->mAngle.y);
    distance = Vec3_HorzDist(&spawnerPos, &otherPos);
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

        cosAngle = FX_CosIdx((u16)mAngle.y >> 4);
        sinAngle = FX_SinIdx((u16)mAngle.y >> 4);
        z = mPos.z + sinAngle * 349;
        x = mPos.x - cosAngle * 349;
        y = mPos.y;
        position.x = x;
        position.y = y;
        position.z = z;
        spawned = (daObjC1_Trap_c *)dActor_c::Spawn(0x24, 0, position, &mAngle, mAreaId, -1);
        spawned->mSpawnerID = uniqueID;

        sinAngle = FX_SinIdx((u16)mAngle.y >> 4);
        cosAngle = FX_CosIdx((u16)mAngle.y >> 4);
        z = mPos.z - sinAngle * 349;
        x = cosAngle * 349 + mPos.x;
        y = mPos.y;
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
    mMeshCollider.SetFile(dBgW_Kc::LoadFile(data_ov010_02112d00), mDoorMat, 0x1000, mAngle.y,
                          data_ov010_021122f8);
    mMeshCollider.SetStoodOnCallback(daObjC1_Trap_c::StoodOnCallback);
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

    TrapState *states = &data_ov010_02112d28.state0;
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
    int sinZ = FX_SinIdx((u16)mAngle.z >> 4);
    int cosY = FX_CosIdx((u16)mAngle.y >> 4);
    int sinY = FX_SinIdx((u16)mAngle.y >> 4);
    int radius = sinZ * 5;
    int offsetX = FX_MUL(radius, cosY);
    int offsetZ = FX_MUL(radius, sinY);
    int x = (mPos.x - offsetX) >> 3;
    int y = mPos.y >> 3;
    int z = (mPos.z + offsetZ) >> 3;

    Matrix4x3_FromTranslation(&gScratchMtx, x, y, z);
    Matrix4x3_ApplyInPlaceToRotationY(&gScratchMtx, mAngle.y);
    Matrix4x3_ApplyInPlaceToRotationZ(&gScratchMtx, mAngle.z);
    mDoorModel.mat4x3 = gScratchMtx;
}


void daObjC1_Trap_c::UpdateCollisionTransform() {
    Matrix4x3_FromTranslation(&gScratchMtx, mPos.x, mPos.y, mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&gScratchMtx, mAngle.y);
    Matrix4x3_ApplyInPlaceToRotationZ(&gScratchMtx, mAngle.z);
    mDoorMat = gScratchMtx;
    mMeshCollider.Transform(mDoorMat, mAngle.y);
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
    mAngle.z += mOpenSpeed;
    if (mAngle.z < -0x3d00) {
        mAngle.z = -0x3d00;
        mState = 2;
    }
}


void daObjC1_Trap_c::StateOpen() {
    daObjC1_Trap_c *spawner = GetSpawner();
    if (spawner && !spawner->mTrapActive)
        mState = 3;
}


void daObjC1_Trap_c::StateClosing() {
    mAngle.z += 0x400;
    if (mAngle.z > 0) {
        mAngle.z = 0;
        mState = 0;
    }
}


void daObjC1_Trap_c::StateHeldOpen() {
    mAngle.z = -0x3c00;
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

/* Course words. data_ov010_021121c0 abuts the CLPS block below;
 * data_ov010_02112318 runs up to the state descriptors. */
extern u32 data_ov010_021121c0[];
extern u32 data_ov010_021121c4[];
extern u32 data_ov010_021121cc[];
extern u32 data_ov010_021121dc[];
extern u32 data_ov010_021121fc[];
extern u32 data_ov010_02112204[];
extern u32 data_ov010_0211220c[];
extern u32 data_ov010_0211221c[];
extern u32 data_ov010_0211222c[];
extern u32 data_ov010_0211223c[];
extern u32 data_ov010_02112248[];
extern u32 data_ov010_02112288[];
extern u32 data_ov010_02112580[];
extern u32 data_ov010_0211261c[];
extern u32 data_ov010_02112790[];
extern u32 data_ov010_02112864[];
extern u32 data_ov010_02112948[];

/* 0x02112318..0x02112a78 (1888 bytes, 472 words) */
extern u32 data_ov010_02112214[];
extern u32 data_ov010_02112268[];
extern u32 data_ov010_021122b8[];
extern u32 data_ov010_021121d4[];
extern u32 data_ov010_02112398[];
extern u32 data_ov010_021123d8[];
extern u32 data_ov010_02112338[];
extern u32 data_ov010_02112234[];
extern u32 data_ov010_021121e4[];
extern u32 data_ov010_02112278[];
extern u32 data_ov010_02112420[];
extern u32 data_ov010_021121f4[];
extern u32 data_ov010_02112258[];
extern u32 data_ov010_02112224[];
extern u32 data_ov010_02112358[];
extern u32 data_ov010_021122d8[];
extern u32 data_ov010_02112520[];
extern u32 data_ov010_02112470[];
extern u32 data_ov010_021126d0[];
extern u32 data_ov010_021121ec[];
extern u32 data_ov010_021124c0[];
extern u32 data_ov010_02112318[];

u32 data_ov010_021121c0[] = {
    0x00000004,
    0x00000001,
    (u32)data_ov010_02112214,
    0x00000002,
    (u32)data_ov010_02112268,
    0x00000200,
    (u32)data_ov010_021122b8,
    0x00000001,
    (u32)data_ov010_021121d4,
    0x00000400,
    (u32)data_ov010_02112398,
    0x00000009,
    (u32)data_ov010_021123d8,
    0x00000200,
    (u32)data_ov010_02112338,
    0x00000001,
    (u32)data_ov010_02112234,
    0x00000001,
    (u32)data_ov010_021121e4,
    0x00000002,
    (u32)data_ov010_02112278,
    0x00000500,
    (u32)data_ov010_02112420,
    0x00000001,
    (u32)data_ov010_021121f4,
    0x00000100,
    (u32)data_ov010_02112258,
    0x00000001,
    (u32)data_ov010_02112224,
    0x00000400,
    (u32)data_ov010_02112358,
    0x000006e9,
    0x000006eb,
    0x000006ed,
    0x08980898,
    0x08980898,
    0x08980898,
    0x00000898,
    0x047f004b,
    0xf6b702d5,
    0x4000ffff,
    0x070d0000,
    0x00000200,
    (u32)data_ov010_021122d8,
    0x00000405,
    (u32)data_ov010_02112318,
    0x00000600,
    (u32)data_ov010_02112520,
    0x00000a05,
    (u32)data_ov010_02112470,
    0xff0e0000,
    0x000effff,
    0xffffff0c,
    0xff0c001a,
    0x0000ffff,
    (u32)data_ov010_021126d0,
    (u32)data_ov010_021121ec,
    0x06e806f0,
    0x06f206f1,
    (u32)data_ov010_021124c0,
    0x06400008,
    0x0000000f,
    0xfb820043,
    0xf8940000,
    0x00000000,
    0xff040000,
    0xfb820043,
    0xf8940000,
    0x00000000,
    0xff0b0000,
    0xf9a60050,
    0x04de0000,
    0x600001ff,
    0x0af00000,
    0x00000112,
    0x00000000,
    0x00000000,
    0xffff0000,
};

u32 data_ov010_02112318[] = {
    0xfa88fe25,
    0xfed40177,
    0xfaecfe25,
    0xfed40177,
    0xfd44fe25,
    0xfed40177,
    0xfda8fe25,
    0xfed40177,
    0xeaf90111,
    0xef8202bc,
    0x20000000,
    0xffff0000,
    0xe4a90113,
    0xe932044c,
    0x20000000,
    0xffff0000,
    0xf736002a,
    0xef66fed4,
    0x00000000,
    0x03550000,
    0xf501002a,
    0xf0c4fed4,
    0x2aaa0000,
    0x43550000,
    0xf966002a,
    0xf0c4fed4,
    0xd5560000,
    0x43550000,
    0xf74f004b,
    0xf48cfeed,
    0x8000ffff,
    0x070e0000,
    0x00fa002a,
    0xeed0ff38,
    0x00000000,
    0x01550000,
    0x02580050,
    0xf5d8fed4,
    0x8000ffff,
    0x0af20000,
    0x01a9004b,
    0xfbaa007d,
    0x8000ffff,
    0x070c0000,
    0xfed400ed,
    0xf830ff9c,
    0x80000000,
    0x21020000,
    0x00002602,
    (u32)data_ov010_02112864,
    0x00000303,
    (u32)data_ov010_02112288,
    0x00000b04,
    (u32)data_ov010_02112580,
    0x0000050b,
    (u32)data_ov010_0211223c,
    0x00001301,
    (u32)data_ov010_02112948,
    0x00000f09,
    (u32)data_ov010_0211261c,
    0x0000070c,
    (u32)data_ov010_02112248,
    0x0000010e,
    (u32)data_ov010_021121c0,
    0x00000f0a,
    (u32)data_ov010_02112790,
    0xec14002a,
    0xfe3e0190,
    0x40000000,
    0x00550000,
    0xf3da004b,
    0xfe700145,
    0xc000ffff,
    0x070a0000,
    0xf3da004b,
    0xfffb0145,
    0xc000ffff,
    0x070b0000,
    0xf3760050,
    0xfc0200c8,
    0xc000ffff,
    0x0b120000,
    0xf25400ed,
    0x019000c8,
    0x80000000,
    0x13010000,
    0x09f66a36,
    0xf7cc0226,
    0x0f3caa36,
    0xf9f20226,
    0x10cc8a36,
    0x02ee0226,
    0x09c46a36,
    0x06720226,
    0x0bea6a36,
    0xf7cc0226,
    0x0ddeaa36,
    0xf8940226,
    0x109aaa36,
    0xfb500226,
    0x0f3c8a36,
    0x044c0226,
    0x0dde8a36,
    0x05aa0226,
    0x0bea6a36,
    0x06720226,
    (u32)data_ov010_021121cc,
    0x00000000,
    0x06a40300,
    (u32)data_ov010_021121c4,
    0x00000000,
    0x05780200,
    (u32)data_ov010_0211221c,
    0x00000000,
    0x05dc0102,
    (u32)data_ov010_021121fc,
    0x00000000,
    0x01900200,
    (u32)data_ov010_021121dc,
    0x00000000,
    0x01900104,
    (u32)data_ov010_02112204,
    0x00000000,
    0x028a0200,
    (u32)data_ov010_0211222c,
    0x00000000,
    0x04b00300,
    (u32)data_ov010_0211220c,
    0x00000000,
    0x04b00300,
    0x07d00031,
    0xf9c00320,
    0x20000000,
    0x00170000,
    0x1068002a,
    0xff1f0190,
    0xc0000000,
    0x02550000,
    0x06400050,
    0x01f4012c,
    0x6000ffff,
    0x0af10000,
    0x065f004b,
    0xfba301a9,
    0x2000ffff,
    0x07110000,
    0x07d00031,
    0xf9c00320,
    0x20000000,
    0x00f80000,
    0x064000ed,
    0xfce0012c,
    0x20000000,
    0x01000000,
    0xfddaff04,
    0x05dc0190,
    0x80000000,
    0xff020000,
    0x0064fc18,
    0x0000ecaa,
    0x00000000,
    0xfc18ff02,
    0x0320fcae,
    0x80000000,
    0xff020000,
    0x0226f092,
    0x0000ff6a,
    0x0000b1c8,
    0xfd76ff02,
    0x012c0320,
    0x8e390000,
    0xff020000,
    0x02580c1c,
    0x0000fe0c,
    0x00003555,
    0x0d48ff02,
    0xff380258,
    0xc0000000,
    0xff020000,
    0xff9cf5d8,
    0x0000f3b2,
    0x000071c7,
    0xff6aff02,
    0xf34e0000,
    0x71c70000,
    0xff020000,
    0x0384ee3a,
    0x0000f3e4,
    0x00009556,
    0xfd44ff04,
    0x04b0012c,
    0x00000000,
    0x00000000,
    0x000000af,
    0x0000fbe6,
    0x00030500,
    0x0258023f,
    0xe000fa2a,
    0x000d0000,
    0x0000f6eb,
    0x0000fbe6,
    0x000e0300,
    0x00c8041a,
    0xc000fed4,
    0x00040700,
    0x01f4f5e2,
    0x2000fa6a,
    0x00090200,
    0x00c8f416,
    0x4000fd44,
    0x00020100,
    0x0000f905,
    0xe000fca6,
    0x000f0400,
    0x0000fec2,
    0x2000fd10,
    0x000f0400,
    0xff9cfbcd,
    0x0000ec46,
    0x000f4400,
    0x0000fb82,
    0x00000866,
    0x00010000,
    0x0000fcae,
    0x80000866,
    0x00010000,
    0xfbe6fcae,
    0x80000384,
    0x00064400,
    0xfbe6fb82,
    0x00000384,
    0x00064400,
    0x01f4fb82,
    0x0000fbe6,
    0x00070000,
    0x01f4fcae,
    0x8000fbe6,
    0x00070000,
    0x53504c43,
    0x00150008,
    0x04000fc0,
    0x000000ff,
    0x00000fc0,
    0x000000ff,
    0x00040fc0,
    0x000000ff,
    0x00040000,
    0x000000ff,
    0x00040fc1,
    0x000000ff,
    0x00040fc4,
    0x000000ff,
    0x00042fc1,
    0x000000ff,
    0x01042fc1,
    0x000000ff,
    0x01040fc4,
    0x000000ff,
    0x01040fc5,
    0x000000ff,
    0x01043fc5,
    0x000000ff,
    0x00042fc4,
    0x000000ff,
    0x01040005,
    0x000000ff,
    0x006c0004,
    0x000000ff,
    0x00040001,
    0x000000ff,
    0x00040004,
    0x000000ff,
    0x00042001,
    0x000000ff,
    0x00043004,
    0x000000ff,
    0x01043005,
    0x000000ff,
    0x00040284,
    0x000000ff,
    0x00040281,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0x0190ec14,
    0x0000fe3e,
    0x00064000,
    0x10685508,
    0xff1f0190,
    0xc0000000,
    0x55090008,
    0xfed4f736,
    0x0000ef66,
    0x000a0000,
    0x00fa5506,
    0xeed0ff38,
    0x00000000,
    0x55070007,
    0x0000fbb4,
    0x000008ca,
    0x04018000,
    0xfc1820ff,
    0xec46ff9c,
    0x00000000,
    0x22ff0503,
    0xfbe6fbb4,
    0x000003e8,
    0x00048000,
    0xfbb420ff,
    0xfb8201f4,
    0x00000000,
    0x10ff0005,
    0x0190ec5a,
    0xc000f0e3,
    0x00232000,
    0x07449b0a,
    0xf7ea02ee,
    0xa0000000,
    0x210e001d,
    0x032007c6,
    0x0000049c,
    0x001e6000,
    0x0275110d,
    0xfa5e0258,
    0xe0000000,
    0x11ff0032,
    0x01f4fc7c,
    0x0000fb82,
    0x0d050000,
    0xfc7c10ff,
    0x08ca0000,
    0x80000000,
    0x20ff0c01,
    0xfbe6fc7c,
    0x000003e8,
    0x0a048000,
    0x000020ff,
    0x012c08fc,
    0x0640fa24,
    0xfce0012c,
    0x012c0960,
    0x0960fda8,
    0x0064012c,
    0x012c05dc,
    0x0898012c,
    0x03e8012c,
    0x012c0a8c,
    0x0c1cff38,
    0x03e8012c,
    0x012c08fc,
    0x0f3c03e8,
    0x00c8012c,
    0x012c0f3c,
    0x0960fd44,
    0xfa88012c,
    0x012c0c1c,
    0x0a8cfa88,
    0xfed4012c,
    0x00c8ed40,
    0xf2540190,
    0x019000c8,
    0x00c8f254,
    0xed40faec,
    0xfaec00c8,
    0x00c8ed40,
    0xf18cfc18,
    0xfc1800c8,
    0x00c8f18c,
    0xecdcfe0c,
    0xfe0c012c,
    0x012cecdc,
    0xf18cfe70,
    0xfe7000c8,
    0x00c8f18c,
    0xed400064,
    0x006400c8,
    0xff9cfed4,
    0xfe70f830,
    0xeffcfed4,
    0xfed4ff9c,
    0x0064effc,
    0xf510fed4,
    0xfed4012c,
    0x01f4f510,
    0xf060fed4,
    0xfed40064,
    0x00c8f0c4,
    0xf4acfed5,
    0xfed50190,
    0x0258f448,
    0xeffcfed4,
    0xfed40384,
    0x0320effc,
    0xf830ff9c,
    0xfc180000,
    0x02b4fff9,
    0x80000000,
    0x00000000,
    0xfb500000,
    0x02bcffce,
    0x80000000,
    0x00000000,
    0xfd120000,
    0x02bcffce,
    0x80000000,
    0x00000000,
    0xfe700000,
    0x01f40000,
    0x80000000,
    0x00000000,
    0xfbb40000,
    0x076c0000,
    0x80000000,
    0x07000000,
    0xfc180000,
    0xedd6ff9c,
    0x00000000,
    0x070c0000,
    0xf7360000,
    0xef660000,
    0x00000000,
    0x05bb0000,
    0x00fa0000,
    0xeed00064,
    0x00000000,
    0x05c50000,
    0xec140000,
    0xfe3e02bc,
    0x40000000,
    0x05990000,
    0x10680000,
    0xff3802bc,
    0xc0000000,
    0x05af0000,
    0xeaf90000,
    0xef8201f4,
    0x20000000,
    0x044a0000,
    0xfbe60000,
    0xfc7c01f4,
    0x00000000,
    0x07200000,
    0xfbb40000,
    0x01f4fbe6,
    0x80000000,
    0x07140000,
    0x0a8c0000,
    0xff380514,
    0xc0000000,
    0x02370000,
    0xfc180000,
    0x02bc03e8,
    0x00000000,
    0x02000000,
    0x01f40000,
    0xfaba0258,
    0xe0000000,
    0x07200000,
    0xfc4a0000,
    0xfc7c01f4,
    0x00000000,
    0x07200000,
    0xfc7c0000,
    0x076c0000,
    0x80000000,
    0x07000000,
    0xfc7c0000,
    0x01f4fbe6,
    0x80000000,
    0x07140000,
};

/* Door shut, then door held open. The all-ones words the cartridge pads the
   block out to 32 bytes with are not part of the object. */
CLPS_Block data_ov010_021122f8 = {
    0x53504c43, 8, 2, {{0x04000fc1, 0x000000ff}, {0x00000fc1, 0x000000ff}}};
