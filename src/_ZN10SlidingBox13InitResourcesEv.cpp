//cpp
// @symbol _ZN10SlidingBox13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SlidingBox.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* f, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* thiz);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
extern void func_020393d4(void* p, void* v);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* act, int fix, int t, void* vr, int t2);
extern void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);
}

int SlidingBox::InitResources()
{
    void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov016_02114e74);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, m, 1, -1);
    func_ov016_021130a4(((char*)this));
    _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
    void* mc = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov016_02114e6c);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char*)this) + 0x124, mc, ((char*)this) + 0x2ec, 0x199, mAngleY, data_ov016_02113bac);
    func_020393d4(((char*)this) + 0x124, (void*)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x324, ((char*)this), 0x14000, 0x14000, 0, 0);
    unk_09c = -0x2000;
    unk_0a0 = -0x14000;
    unk_320 = 0;
    unk_4f4 = 0;
    unk_4f0 = 0;
    return 1;
}
