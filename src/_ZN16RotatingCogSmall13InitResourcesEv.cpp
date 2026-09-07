//cpp
#include "types.h"
// @symbol _ZN16RotatingCogSmall13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingCogSmall.h"
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *m, void *f, int a, int b);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *c);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *c);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *sfp);
}
extern "C" {
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *mc, void *kcl, void *mtx, int fix, short s, void *clps);
}
extern "C" {
extern void func_020393d4(void *p, void *v);
}
extern int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
extern int data_ov035_02112c78[];
extern int data_ov035_02112c70[];
extern int data_ov035_02112c60[];
extern u8 data_0209f2c0[];
extern s16 data_ov035_02111ef4[][4];
extern s16 data_ov035_02111ef0[];

int RotatingCogSmall::InitResources()
{
    int b;

    b = (int)(actorID == 0x77);
    if (b != 0) {
        _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel,
            _ZN5Model8LoadFileER13SharedFilePtr(data_ov035_02112c78), 1, -1);
        _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char *)this));
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char *)this));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider,
            _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov035_02112c68),
            &mClsnMat, 0x1000, mAngleY, data_ov035_021121d8);
        func_020396c0(&mMeshCollider, 0);
        func_020393d4(&mMeshCollider, &_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
        mRotationState = 0;
    } else {
        b = (int)(actorID == 0x79);
        if (b != 0) {
            _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel,
                _ZN5Model8LoadFileER13SharedFilePtr(data_ov035_02112c70), 1, -1);
        } else {
            _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel,
                _ZN5Model8LoadFileER13SharedFilePtr(data_ov035_02112c60), 1, -1);
        }
        _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char *)this));
        mRotationState = 1;
    }

    {
        int st = mRotationState;
        mStepTimer = data_ov035_02111ef4[st][data_0209f2c0[0]];
        mAngleYStep = data_ov035_02111ef0[mRotationState];
        unk_326 = data_ov035_02111ef0[mRotationState];
    }
    return 1;
}
