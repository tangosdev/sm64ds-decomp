//cpp
#include "types.h"
// @symbol _ZN9WDW_Water13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WDW_Water.h"
#include "dBgW.h"
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *m, void *f, int a, int b);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void *bmd, void *bta);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *tt, void *bta, int a, int fix, unsigned int b);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *c);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *c);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *sfp);
}
extern "C" {
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *mc, void *kcl, void *mtx, int fix, short s, void *clps);
}

extern u8 data_0209f2c0[];
extern int data_0209f32c;

int WDW_Water::InitResources()
{
    void *m;
    void *k;

    m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov029_021142fc);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, m, 1, -1);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(data_ov029_021142fc[1], &data_ov029_02112fb8);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(((char *)this) + 0x320, &data_ov029_02112fb8, 0, 0x1000, 0);

    mUseSpawnPosY = (u8)((*(s32 *)&param1) & 1);
    if (mUseSpawnPosY == 0) {
        unsigned int idx = data_0209f2c0[0];
        if (idx >= 3) idx = 2;
        mPosY = data_ov029_02112b2c[idx];
    }
    mTargetPosY = mPosY;
    data_0209f32c = mPosY;

    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char *)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char *)this));

    k = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov029_02114304);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x1000, mAngleY, &data_ov029_02112fec);
    ((dBgW *)(&mMeshCollider))->Enable((dActor_c *)(((char *)this)));

    mTrueAreaID = (u8)mAreaId;
    mAreaId = -1;
    *(s16*)(((char *)this) + 0x300 + 0x42) = 0;

    return 1;
}
