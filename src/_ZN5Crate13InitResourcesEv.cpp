//cpp
// @symbol _ZN5Crate13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Crate.h"
extern int _ZN5Model8LoadFileER13SharedFilePtr(char* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(char* thiz, int f, int a, int b);
extern void _ZN11ShadowModel10InitCuboidEv(char* thiz);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(char* f);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(char* thiz, int f, char* m, int fix, short s, int blk);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(char* thiz, char* actor, int b, int d, void* v, int f);
extern void _ZN12WithMeshClsn19StartDetectingWaterEv(char* thiz);
extern void Crate_SetState(char* c, int i);
extern char data_ov098_0213c4c8[];

int Crate::InitResources()
{
    char* f = 0;
    if (mActorID == 0xc2) f = data_ov098_0213c4c8;
    if (f == 0) return 0;
    unk_4e8 = mPosX;
    unk_4ec = mPosY;
    unk_4f0 = mPosZ;
    unk_500 = mAngleX;
    unk_502 = mAngleY;
    unk_504 = mAngleZ;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr(*(char**)(f)), 1, -1);
    _ZN11ShadowModel10InitCuboidEv((char*)&mShadowModel);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char*)this) + 0x124, _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(char**)(f + 4)),
        ((char*)this) + 0x2ec, 0x199, mAngleY, *(int*)(f + 8));
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x320, ((char*)this), 0x28000, 0x28000, 0, 0);
    _ZN12WithMeshClsn19StartDetectingWaterEv((char*)&mWithMeshClsn);
    unk_09c = -0x2000;
    unk_0a0 = -0x3c000;
    Crate_SetState(((char*)this), 0);
    mEatingPlayer = 0;
    func_ov098_02138ce0(((char*)this));
    return 1;
}
