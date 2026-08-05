//cpp
// @symbol _ZN15RotatingFirebar13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "RotatingFirebar.h"
extern void* data_ov064_0211adbc[];
extern "C" {
extern void _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);
}

extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, int a, int b);
extern void _ZN19CylinderClsnWithPos4InitERK7Vector35Fix12IiES4_jj(void* thiz, void* v, int f1, int f2, unsigned int a, unsigned int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* thiz);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* thiz);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
extern void func_020393d4(int* p, int v);
}

int RotatingFirebar::InitResources()
{
    int i;
    char* p;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4,
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov064_0211adbc[0]), 1, -1);
    p = ((char*)this) + 0x360;
    for (i = 0; i < 8; i++) {
        _ZN19CylinderClsnWithPos4InitERK7Vector35Fix12IiES4_jj(p, ((char*)this) + 0x5c, 0x4b000, 0x96000, 0x200002, 0);
        p += 0x3c;
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
    _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char*)this) + 0x124,
        _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov064_0211adbc[1]),
        ((char*)this) + 0x2ec, 0x199, unk_08e, data_ov064_0211adbc[2]);
    func_020393d4((int*)((char*)&mMeshCollider),
        (int)&_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    return 1;
}
