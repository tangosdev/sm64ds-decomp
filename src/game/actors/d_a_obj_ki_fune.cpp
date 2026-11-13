//cpp
/* What stays:
 * - FX_SinCosTable_ is the sine table. gCurrentStarId is the star index.
 * - data_ov016_021136dc, 021136e4 and 021149d4 are the overlay file
 *   and CLPS tables, indexed by mModelIndex. This TU does not own them.
 * - UpdateModelMatrix is a coined name; the ROM records none.
 *
 * daObjKi_Fune_c, the Jolly Roger Bay ship (KI_FUNE / KI_FUNE_UP).
 * ov016 .text 0x0211260c..0x02112a00. Actor 0x39 is the rocking hull;
 * the other index is the still hull, and the two never coexist.
 * D1 is 0x0211260c and D0 is 0x02112650, both from the inline
 * destructor in daObjKi_Fune_c.h. InitResources is the key function.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_ki_fune_AssetId {
    ASSET_KI_FUNE_DOWN_A_BMD = 0x0639,
    ASSET_KI_FUNE_DOWN_A_KCL = 0x063a,
    ASSET_KI_FUNE_UP_BMD = 0x063b,
    ASSET_KI_FUNE_UP_KCL = 0x063c
};

#include "daObjKi_Fune_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "dBgW.h"
#include "nitro/fx/fx_trig.h"

/* Two model handles construct through func_02017acc and destroy through
 * func_02017ab4. Two collision handles construct through func_02017b4c and
 * destroy through SharedFilePtr_Destruct_Clsn. The file tables stay ROM data. */
struct KiFuneModelFilePtr : SharedFilePtr {
    KiFuneModelFilePtr(unsigned int fileId);
    ~KiFuneModelFilePtr();
};
struct KiFuneClsnFileHandle : SharedFilePtr {
    KiFuneClsnFileHandle(unsigned int fileId);
    ~KiFuneClsnFileHandle();
};
typedef char KiFuneModelFilePtr_size_must_be_8[sizeof(KiFuneModelFilePtr) == 8 ? 1 : -1];
typedef char KiFuneClsnFileHandle_size_must_be_8[sizeof(KiFuneClsnFileHandle) == 8 ? 1 : -1];

/* local extern: no shared header declares these. */
extern int IsStarCollected(int a, int b);
extern "C" {
}

extern unsigned char gCurrentStarId;

/* Actor 0x39 is the rocking hull, which uses model/collision index 0. */
static inline int IsRockingHull(daObjKi_Fune_c *ship) {
    return ship->actorID == 0x39;
}

daObjKi_Fune_c *daObjKi_Fune_c_classInit_KI_FUNE() {
    return new daObjKi_Fune_c();
}

daObjKi_Fune_c *daObjKi_Fune_c_classInit_KI_FUNE_UP() {
    return new daObjKi_Fune_c();
}

int daObjKi_Fune_c::InitResources() {
    void *clpsBlocks[2];
    unsigned int idx;
    void *file;
    clpsBlocks[0] = data_ov016_021149d4[0];
    clpsBlocks[1] = data_ov016_021149d4[1];
    if (IsRockingHull(this))
        mModelIndex = 0;
    else
        mModelIndex = 1;
    idx = mModelIndex;
    file = dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)data_ov016_021136e4[idx]);
    mModel.SetFile((BMD_File *)file, 1, -1);
    UpdateModelMatrix();
    UpdateClsnPosAndRot();
    idx = mModelIndex;
    file = dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov016_021136dc[idx]);
    mMeshCollider.SetFile((KCL_File *)file, mClsnMat, 0x1000, mAngleY,
                          *(CLPS_Block *)clpsBlocks[idx]);
    if (mModelIndex == 0) {
        mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosWithTransform);
    }
    mMeshCollider.Enable(this);
    mSoundHandle = 0;
    unk_328 = 0;
    if (gCurrentStarId > 1 && IsStarCollected(SublevelToLevel(8), 1) != 0) {
        if (mModelIndex != 0)
            return 0;
    } else if (mModelIndex == 0) {
        return 0;
    }
    return 1;
}

int daObjKi_Fune_c::Behavior() {
    if (mMeshCollider.IsEnabled() == 0) {
        mMeshCollider.Enable(this);
    }
    mMeshCollider.SetClipRadius(0x2000000);
    if (mModelIndex == 0) {
        mBobAngle += 0xda;
        mAngleX = (s16)((FX_SinIdx((u16)mBobAngle >> 4) << 0xa) >> 0xc);
        if (DistToCPlayer() < 0xbb8000) {
            mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x8b, mCamSpacePos, 0);
        }
        UpdateModelMatrix();
        UpdateClsnPosAndRot();
    }
    return 1;
}

int daObjKi_Fune_c::Render() {
    mModel.Render(0);
    return 1;
}

int daObjKi_Fune_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    ((SharedFilePtr *)(data_ov016_021136e4[mModelIndex]))->Release();
    ((SharedFilePtr *)(data_ov016_021136dc[mModelIndex]))->Release();
    return 1;
}

void daObjKi_Fune_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

/* Retail construction order. mwcc emits __sinit_d_a_obj_ki_fune.cpp. */
KiFuneModelFilePtr gKiFuneUpModelFile(ASSET_KI_FUNE_UP_BMD);
KiFuneModelFilePtr gKiFuneDownAModelFile(ASSET_KI_FUNE_DOWN_A_BMD);
KiFuneClsnFileHandle gKiFuneUpCollisionFile(ASSET_KI_FUNE_UP_KCL);
KiFuneClsnFileHandle gKiFuneDownACollisionFile(ASSET_KI_FUNE_DOWN_A_KCL);
