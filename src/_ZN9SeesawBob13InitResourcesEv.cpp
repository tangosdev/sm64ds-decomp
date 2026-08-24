//cpp
// @symbol _ZN9SeesawBob13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SeesawBob.h"
extern "C" {
extern int _ZN5Model8LoadFileER13SharedFilePtr(int);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern int _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(int);
extern int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,short,int);
extern void func_020393d4(void*,void*);
extern void func_020393c4(void*,void*);
}

extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_[];

int SeesawBob::InitResources()
{
    unsigned char idx;
    int f;
    switch (actorID) {
        case 0x1c: mVariant = 0; break;
        case 0x27: mVariant = 1; break;
        case 0x85: mVariant = 2; break;
        case 0x8f: mVariant = 3; break;
        case 0x95: mVariant = 4; break;
        case 0x96: mVariant = 5; break;
        case 0x80: mVariant = 6; break;
    }
    idx = mVariant;
    f = _ZN5Model8LoadFileER13SharedFilePtr(*(int*)(data_ov095_021374a0 + idx*0xc));
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, f, 1, -1);
    func_ov095_0213597c(((char *)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char *)this));
    {
        unsigned char i = mVariant;
        f = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*(int*)(data_ov095_021374a4 + i*0xc));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, f, &mClsnMat, 0x1000, mAngleY, *(int*)(data_ov095_021374a8 + i*0xc));
    }
    func_020393d4(&mMeshCollider, _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4(&mMeshCollider, func_ov095_02135e90);
    return 1;
}
