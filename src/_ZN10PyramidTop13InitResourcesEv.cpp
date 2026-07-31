//cpp
// @symbol _ZN10PyramidTop13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PyramidTop.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* f, int a, int b);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
extern void func_020393d4(void* p, void* v);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void* thiz, void* act);
extern void _ZN5Event8ClearBitEj(unsigned int n);
extern void _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);
}

int PyramidTop::InitResources()
{
    void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov024_02113968);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x320, m, 1, -1);
    func_ov024_021114c4(((char*)this));
    func_ov024_02111480(((char*)this));
    void* mc = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov024_02113960);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char*)this) + 0x124, mc, ((char*)this) + 0x370, 0x199, unk_08e, data_ov024_021129f0);
    func_020393d4(((char*)this) + 0x124, (void*)&_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    _ZN16MeshColliderBase6EnableEP5Actor(((char*)this) + 0x124, ((char*)this));
    unk_3a0 = mPosX;
    unk_3a4 = mPosY;
    unk_3a8 = mPosZ;
    unk_3b0 = 0;
    unk_3b6 = 0;
    unk_3b7 = 0;
    unk_3ac = 0;
    unk_3b4 = 0;
    _ZN5Event8ClearBitEj(0xe);
    return 1;
}
