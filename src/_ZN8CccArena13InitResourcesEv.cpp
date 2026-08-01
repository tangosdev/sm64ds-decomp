//cpp
// @symbol _ZN8CccArena13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "CccArena.h"
extern int _ZN5Model8LoadFileER13SharedFilePtr(int);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(int);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,short,int);
extern void func_020393d4(void*,void*);
extern void func_020393c4(void*,void*);
extern int _ZN16MeshColliderBase6EnableEP5Actor(void*,void*);

extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_[];

int CccArena::InitResources()
{
    unsigned char idx;
    int f;

    switch (mActorID) {
        case 0xaa:
            unk_330 = 3;
            ((char *)this)[0x32c] = 0;
            break;
        case 0xab:
            unk_330 = 3;
            ((char *)this)[0x32d] = data_ov073_02123424[0];
            data_ov073_02123424[0]++;
            ((char *)this)[0x32c] = 1;
            break;
        case 0xac:
            unk_330 = 3;
            ((char *)this)[0x32d] = data_ov073_02123420[0];
            data_ov073_02123420[0]++;
            ((char *)this)[0x32c] = 2;
            break;
    }

    idx = (unsigned char)((char *)this)[0x32c];
    f = _ZN5Model8LoadFileER13SharedFilePtr(*(int*)(data_ov073_021231bc + idx * 0xc));
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, f, 1, -1);
    Matrix4x3_FromRotationXYZExt(((char *)this) + 0xf0, mAngleX, mAngleY, mAngleZ);
    unk_114 = mPosX >> 3;
    unk_118 = mPosY >> 3;
    unk_11c = mPosZ >> 3;
    _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));

    {
        unsigned char i = (unsigned char)((char *)this)[0x32c];
        f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(int*)(data_ov073_021231c0 + i * 0xc));
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            ((char *)this) + 0x124, f, ((char *)this) + 0x2ec, 0x1000, mAngleY, *(int*)(data_ov073_021231c4 + i * 0xc));
    }

    func_020393d4(((char *)this) + 0x124, _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    func_020393c4(((char *)this) + 0x124, func_ov073_021227d0);
    _ZN16MeshColliderBase6EnableEP5Actor(((char *)this) + 0x124, ((char *)this));

    unk_338 = 0;
    unk_334 = 0;
    func_ov073_021223a4(((char *)this), &data_ov073_021234b0);

    return 1;
}
