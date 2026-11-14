//cpp
/**
 * Tall Tall Mountain's ukiki cage (profile HM_BASKET).
 *
 * Hangs from its mesh collider until something gives it downward
 * acceleration; from then on it falls, and the star actor it spawned at
 * InitResources rides 0x3c000 above it. When the falling cage lands,
 * CheckLanding destroys both it and the star and puffs two particle
 * effects where they were.
 *
 * daObjHmBskt_c_classInit is a reconstructed source-style name: SM64DS proves
 * the RTTI class, the HM_BASKET registry ID, the descriptor/factory
 * relationship, the allocation size and the vtable identity; later EAD lineage
 * supplies the spelling prior. The exact original SM64DS spelling is not
 * preserved. Historical alias: UkikiCage_Spawn. Coined C name; it makes no
 * parameter-type claim.
 *
 * Notes:
 * - Behavior's 16-byte frame is the Vector3 copy of mPos it raises
 *   before handing to the star; the copy itself is optimised away.
 * - data_ov030_02114ee4 is the CLPS block SetFile is handed, in overlay data
 *   this TU does not own.
 * - g_profile_HM_BASKET lives outside this TU.
 * - The destructor is inline: out of line emits D0 before D1.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_hm_bskt_AssetId { ASSET_HM_BASKET_BMD = 0x0618, ASSET_HM_BASKET_KCL = 0x0619 };

#include "daObjHmBskt_c.h"
#include "SharedFilePtr.h"
#include "Particle__System.h"
#include "ParticleIds.h"

struct KCL_File;
/* CLPS.h keeps the block opaque: an 8-byte header with the entry stride
   at +4, then the entries. This TU hands a two-entry block to SetFile. */
struct CLPS_Block {
    u32 tag;
    u32 hdr;
    u32 w0;
    u32 w1;
    u32 rest[4];
};

/* The cartridge's ctor/dtor imports are the functions the wrappers' aliases
 * name. */
struct HmBsktModelFilePtr : SharedFilePtr {
    HmBsktModelFilePtr(u32 fileID);
    ~HmBsktModelFilePtr();
};

struct HmBsktCollisionFilePtr : SharedFilePtr {
    HmBsktCollisionFilePtr(u32 fileID);
    ~HmBsktCollisionFilePtr();
};

extern HmBsktCollisionFilePtr data_ov030_02115c80; /* the KCL */
extern HmBsktModelFilePtr data_ov030_02115c88;     /* the BMD */
CLPS_Block data_ov030_02114ee4 = {
    0x53504c43,
    0x00010008,
    0x01000fc0,
    0x000000ff,
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff},
};

daObjHmBskt_c *daObjHmBskt_c_classInit() {
    return new daObjHmBskt_c();
}

s32 daObjHmBskt_c::InitResources() {
    BMD_File *modelFile = dExtSimpleModel_c::LoadFile(data_ov030_02115c88);
    mModel.SetFile(modelFile, 1, -1);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    mHorzSpeed = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;

    mWithMeshClsn.Init(this, 0x32000, 0x32000, 0, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *clsnFile = dBgW_Kc::LoadFile(data_ov030_02115c80);
    mMeshCollider.SetFile(clsnFile, mClsnMat, 0x199, mAngleY, data_ov030_02114ee4);

    Vector3 starPos;
    starPos.x = mPosX;
    starPos.y = mPosY;
    starPos.z = mPosZ;
    starPos.y += 0x3c000;
    mStarActor = dActor_c::Spawn(0xb2, (u8)(param1 & 0xf) | 0x50, starPos, 0, mAreaId, -1);

    return 1;
}

s32 daObjHmBskt_c::Behavior() {
    IsClsnInRangeOnScreen(0, 0);
    mAngleY = mAngleY + mHorzSpeed;

    if (mVertAccel != 0) {
        mFlags &= ~1;
        UpdatePos(0);
        CheckLanding(&mWithMeshClsn);

        Vector3 pos = mPos;
        pos.y += 0x3c000;
        dActor_c *star = mStarActor;
        star->mPos.x = pos.x;
        star->mPos.y = pos.y;
        star->mPos.z = pos.z;
    }

    return 1;
}

s32 daObjHmBskt_c::Render() {
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mModel.Render(0);
    return 1;
}

s32 daObjHmBskt_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov030_02115c88.Release();
    data_ov030_02115c80.Release();
    return 1;
}

/* Step the falling cage's collision. Once it is on the ground, destroy the
 * star it carried, respawn a free star 150.0 above the landing point, puff
 * two particle effects and destroy the cage. The landed path falls off the
 * end without a return value; the caller ignores it. */
int daObjHmBskt_c::CheckLanding(dBgCh_Actr *clsn) {
    dBgCh_Actr_UpdateContinuous_Veneer(clsn);
    int g = clsn->IsOnGround();
    if (!g)
        return g;
    mStarActor->MarkForDestruction();
    int px = mPosX;
    int pz = mPosZ;
    int py = mPosY + 0x96000;
    Vector3 p(px, py, pz);
    dActor_c::Spawn(0xb2, (unsigned char)(param1 & 0xf) | 0x20, p, 0, mAreaId, -1);
    Particle::System::NewSimple(kPtclBlockBreakA, mPosX, mPosY, mPosZ);
    Particle::System::NewSimple(kPtclBlockBreakB, mPosX, mPosY, mPosZ);
    MarkForDestruction();
}

/* _ZN13daObjHmBskt_cD1Ev and _ZN13daObjHmBskt_cD0Ev are NOT WRITTEN HERE ON
   PURPOSE. The inline destructor in
   include/daObjHmBskt_c.h emits D1 then D0 -- the cartridge's order -- and no
   D2, and tools/tiers.py scores both through that inline definition. */

/* The ROM's initializer constructs the model handle with file ID 1560 and
 * the collision handle with file ID 1561, registering a destructor node for
 * each. mwcc emits __sinit_d_a_obj_hm_bskt.cpp from these definitions. */
HmBsktModelFilePtr data_ov030_02115c88(ASSET_HM_BASKET_BMD);
HmBsktCollisionFilePtr data_ov030_02115c80(ASSET_HM_BASKET_KCL);
