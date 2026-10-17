//cpp
/* leftovers:
 * - dBgW_KcMbg::SetFile stays a mangled call. It takes Fix12<int> by
 *   value, and the header method changes InitResources.
 * - func_020393a4 and func_020393d4 stay the arm9 dBgW hook names.
 *   data_02082214 is the sine table. data_0209f220 is the star index.
 * - data_ov016_021136dc, 021136e4 and 021149d4 are the overlay file
 *   and CLPS tables, indexed by mModelIndex. This TU does not own them.
 * - mBobAngle += 0xda is an s16 pun. The u16 member spelling fails.
 * - func_ov016_021126a8 is a member and keeps its address name.
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

/* Two model handles construct through func_02017acc and destroy through
 * func_02017ab4. Two collision handles construct through func_02017b4c and
 * destroy through SharedFilePtr_Destruct_Clsn. The file tables stay ROM data. */
struct KiFuneModelFilePtr : SharedFilePtr {
    unsigned int words[2];
    KiFuneModelFilePtr(unsigned int fileId);
    ~KiFuneModelFilePtr();
};
struct KiFuneClsnFileHandle : SharedFilePtr {
    unsigned int words[2];
    KiFuneClsnFileHandle(unsigned int fileId);
    ~KiFuneClsnFileHandle();
};
typedef char KiFuneModelFilePtr_size_must_be_8[
    sizeof(KiFuneModelFilePtr) == 8 ? 1 : -1];
typedef char KiFuneClsnFileHandle_size_must_be_8[
    sizeof(KiFuneClsnFileHandle) == 8 ? 1 : -1];

extern "C" {
extern void func_020393a4(int* p, int v);
extern void func_020393d4(int* p, int v);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
extern int IsStarCollected(int a, int b);
extern short data_02082214[];
extern unsigned char data_0209f220;
}

/* Actor 0x39 is the rocking hull, which uses model/collision index 0. */
static inline int IsRockingHull(daObjKi_Fune_c *ship) {
    return ship->actorID == 0x39;
}

// @symbol daObjKi_Fune_c_classInit_KI_FUNE
extern "C" daObjKi_Fune_c *daObjKi_Fune_c_classInit_KI_FUNE()
{
    return new daObjKi_Fune_c();
}

// @symbol daObjKi_Fune_c_classInit_KI_FUNE_UP
extern "C" daObjKi_Fune_c *daObjKi_Fune_c_classInit_KI_FUNE_UP()
{
    return new daObjKi_Fune_c();
}

// @symbol _ZN14daObjKi_Fune_c13InitResourcesEv
int daObjKi_Fune_c::InitResources()
{
    void* clpsBlocks[2];
    unsigned int idx;
    void* file;
    clpsBlocks[0] = data_ov016_021149d4[0];
    clpsBlocks[1] = data_ov016_021149d4[1];
    if (IsRockingHull(this))
        mModelIndex = 0;
    else mModelIndex = 1;
    idx = mModelIndex;
    file = Model::LoadFile(*(SharedFilePtr *)data_ov016_021136e4[idx]);
    mModel.SetFile((BMD_File *)file, 1, -1);
    func_ov016_021126a8();
    UpdateClsnPosAndRot();
    idx = mModelIndex;
    file = dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov016_021136dc[idx]);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider, file, &mClsnMat, 0x1000, mAngleY, clpsBlocks[idx]);
    if (mModelIndex == 0) {
        func_020393d4((int*)&mMeshCollider, (int)&dBgW::UpdatePosWithTransform);
    }
    mMeshCollider.Enable(this);
    mSoundHandle = 0;
    unk_328 = 0;
    if (data_0209f220 > 1 && IsStarCollected(SublevelToLevel(8), 1) != 0) {
        if (mModelIndex != 0)
            return 0;
    } else if (mModelIndex == 0) {
        return 0;
    }
    return 1;
}

// @symbol _ZN14daObjKi_Fune_c8BehaviorEv
int daObjKi_Fune_c::Behavior()
{
    if (mMeshCollider.IsEnabled() == 0) {
        mMeshCollider.Enable(this);
    }
    func_020393a4((int*)&mMeshCollider, 0x2000000);
    if (mModelIndex == 0) {
        *(s16 *)&mBobAngle += 0xda;
        mAngleX = (s16)((data_02082214[(mBobAngle >> 4) * 2] << 0xa) >> 0xc);
        if (DistToCPlayer() < 0xbb8000) {
            mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x8b, mCamSpacePos, 0);
        }
        func_ov016_021126a8();
        UpdateClsnPosAndRot();
    }
    return 1;
}

// @symbol _ZN14daObjKi_Fune_c6RenderEv
int daObjKi_Fune_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN14daObjKi_Fune_c16CleanupResourcesEv
int daObjKi_Fune_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    ((SharedFilePtr *)(data_ov016_021136e4[mModelIndex]))->Release();
    ((SharedFilePtr *)(data_ov016_021136dc[mModelIndex]))->Release();
    return 1;
}

// @symbol _ZN14daObjKi_Fune_c19func_ov016_021126a8Ev
void daObjKi_Fune_c::func_ov016_021126a8()
{
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9]  = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

/* Retail construction order. mwcc emits __sinit_d_a_obj_ki_fune.cpp. */
KiFuneModelFilePtr gKiFuneUpModelFile(ASSET_KI_FUNE_UP_BMD);
KiFuneModelFilePtr gKiFuneDownAModelFile(ASSET_KI_FUNE_DOWN_A_BMD);
KiFuneClsnFileHandle gKiFuneUpCollisionFile(ASSET_KI_FUNE_UP_KCL);
KiFuneClsnFileHandle gKiFuneDownACollisionFile(ASSET_KI_FUNE_DOWN_A_KCL);
