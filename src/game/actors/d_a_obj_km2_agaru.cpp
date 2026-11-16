//cpp
/* What stays:
 * - data_ov045_02113180 and 02113188 are this TU's KCL and BMD handles.
 *   data_ov045_021125d0, the CLPS block, stays overlay data.
 *
 * daObjKm2_Agaru_c, the Fire Sea rising platform. It waits, pauses 20
 * frames, rises 10.0 a frame to 1500.0, pauses, then sinks back.
 * The mesh carries the rider through dBgW::UpdatePosWithTransform.
 * The aboard test is StoodOnCallback / OnStoodOn (coined names).
 * classInit is reconstructed. Retail does not store that spelling.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km2_agaru_AssetId { ASSET_KM2_AGARU_BMD = 0x065b, ASSET_KM2_AGARU_KCL = 0x065c };

#include "daObjKm2_Agaru_c.h"
#include "SharedFilePtr.h"

struct KCL_File;
/* CLPS.h keeps the block opaque: an 8-byte header whose halfword at +4 is
   the entry stride, then N entries. This TU's block is 0x20 bytes. */
struct CLPS_Block {
    u32 words[8];
};

/* Retail constructs these 8-byte handles in this order and registers
 * their destructors. The wrapper names are local; the ctor/dtor
 * addresses are the ROM resource-family functions. */
struct AgaruModelFilePtr : SharedFilePtr {
    AgaruModelFilePtr(u32 fileID);
    ~AgaruModelFilePtr();
};

struct AgaruCollisionFilePtr : SharedFilePtr {
    AgaruCollisionFilePtr(u32 fileID);
    ~AgaruCollisionFilePtr();
};

extern AgaruCollisionFilePtr data_ov045_02113180; /* the KCL */
extern AgaruModelFilePtr data_ov045_02113188;     /* the BMD */
CLPS_Block data_ov045_021125d0 = {
    0x53504c43, 0x00020008, 0x00000fc0, 0x000000ff, 0x00000fc4, 0x000000ff, 0xffffffff, 0xffffffff,
};

enum {
    kPlayerActorID = 0xbf,
    kWaitFrames = 20,
    kRideSpeed = 0xa000,    /* 10.0 */
    kRideHeight = 0x5dc000, /* 1500.0 */
};

daObjKm2_Agaru_c *daObjKm2_Agaru_c_classInit() {
    return new daObjKm2_Agaru_c();
}

/* The collider's second callback word: drop the collider, forward the pair. */
void daObjKm2_Agaru_c::StoodOnCallback(dBgW *collider, dActor_c *self, dActor_c *other) {
    static_cast<daObjKm2_Agaru_c *>(self)->OnStoodOn(other);
}

void daObjKm2_Agaru_c::OnStoodOn(dActor_c *other) {
    int isPlayer = other->actorID == kPlayerActorID;
    if (isPlayer)
        mStoodOn = true;
}

s32 daObjKm2_Agaru_c::InitResources() {
    mModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov045_02113188), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = dBgW_Kc::LoadFile(data_ov045_02113180);
    mMeshCollider.SetFile(kcl, mClsnMat, 0x199, mAngleY, data_ov045_021125d0);
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosWithTransform);
    mMeshCollider.SetStoodOnCallback(&StoodOnCallback);

    mWaitTimer = 0;
    mState = 0;
    mRestY = mPosY;
    return 1;
}

s32 daObjKm2_Agaru_c::Behavior() {
    switch (mState) {
    case 0:
        if (mStoodOn)
            mState++;
        break;
    case 1:
        if (mWaitTimer >= kWaitFrames) {
            mPosY += kRideSpeed;
            int top = mRestY + kRideHeight;
            if (mPosY >= top) {
                mPosY = top;
                mState++;
                mWaitTimer = 0;
            }
        } else {
            mWaitTimer++;
        }
        break;
    case 2:
        if (mWaitTimer >= kWaitFrames) {
            mPosY -= kRideSpeed;
            int bottom = mRestY;
            if (mPosY <= bottom) {
                mPosY = bottom;
                mState = 0;
                mWaitTimer = 0;
            }
        } else {
            mWaitTimer++;
        }
        break;
    }
    UpdateModelPosAndRotY();
    if (IsClsnInRange(0x1f4000, 0))
        UpdateClsnPosAndRot();
    mStoodOn = false;
    return 1;
}

s32 daObjKm2_Agaru_c::Render() {
    mModel.Render(0);
    return 1;
}

s32 daObjKm2_Agaru_c::CleanupResources() {
    mMeshCollider.Disable();
    data_ov045_02113188.Release();
    data_ov045_02113180.Release();
    return 1;
}

/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */

/* The static-init globals -- mwcc emits __sinit_d_a_obj_km2_agaru.cpp
   from these: one ctor veneer plus destructor registration per handle. */
AgaruModelFilePtr data_ov045_02113188(ASSET_KM2_AGARU_BMD);
AgaruCollisionFilePtr data_ov045_02113180(ASSET_KM2_AGARU_KCL);
