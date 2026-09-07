//cpp
// @symbol _ZN8CccArena13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
/* Model.h comes FIRST on purpose, now that CccArena derives from
   dBgActor_c (which reaches common.h's FLAT Matrix4x3 first for its own
   reasons -- see include/dBgActor_c.h). Model.h reaches math/Matrix.h,
   whose Matrix4x3 is the structured one, and it wins the include guard
   race so common.h's flat spelling stands down for this TU. mat4x3.t is
   only nameable this way round; the two spellings are the same 0x30
   bytes. */
#include "Model.h"
#include "CccArena.h"
/* recovered: declarations from a shared header */
#include "decl_common.h"
#include "dBgW.h"
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

int CccArena::InitResources()
{
    unsigned char idx;
    int f;

    switch (actorID) {
        case 0xaa:
            unk_330 = 3;
            mVariant = 0;
            break;
        case 0xab:
            unk_330 = 3;
            mSpawnIndex = data_ov073_02123424[0];
            data_ov073_02123424[0]++;
            mVariant = 1;
            break;
        case 0xac:
            unk_330 = 3;
            mSpawnIndex = data_ov073_02123420[0];
            data_ov073_02123420[0]++;
            mVariant = 2;
            break;
    }

    idx = mVariant;
    f = _ZN5Model8LoadFileER13SharedFilePtr(*(int*)(data_ov073_021231bc + idx * 0xc));
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, f, 1, -1);
    Matrix4x3_FromRotationXYZExt(((char *)this) + 0xf0, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char *)this));

    {
        unsigned char i = mVariant;
        f = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*(int*)(data_ov073_021231c0 + i * 0xc));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            ((char *)this) + 0x124, f, ((char *)this) + 0x2ec, 0x1000, mAngleY, *(int*)(data_ov073_021231c4 + i * 0xc));
    }

    func_020393d4(((char *)this) + 0x124, _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4(((char *)this) + 0x124, func_ov073_021227d0);
    ((dBgW *)(((char *)this) + 0x124))->Enable((dActor_c *)(((char *)this)));

    unk_338 = 0;
    unk_334 = 0;
    func_ov073_021223a4(((char *)this), &data_ov073_021234b0);

    return 1;
}
