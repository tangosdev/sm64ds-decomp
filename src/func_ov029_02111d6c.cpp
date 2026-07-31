//cpp
// @symbol func_ov029_02111d6c
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjWc_Obj05_c.h"
// @emits daObjWc_Obj05_c_InitResources
/* recovered: renamed to Class_Method */
/* daObjWc_Obj05_c::InitResources - recovered from vtable slot identity */
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* f, int a, int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* thiz);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* thiz);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
extern void func_020393d4(void* p, void* v);
extern void func_020393c4(void* p, void* v);
extern void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);

int daObjWc_Obj05_c_InitResources(char* c)
{
    struct daObjWc_Obj05_c *self = (struct daObjWc_Obj05_c *)(void *)c;
    void* f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov029_0211428c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, f, 1, -1);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    void* mc = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov029_02114284);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, mc, c + 0x2ec, 0x1000, self->unk_08e, data_ov029_0211306c);
    func_020393d4(c + 0x124, (void*)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    func_020393c4(c + 0x124, (void*)&func_ov029_02111e60);
    self->unk_32b = 0;
    self->unk_32a = self->unk_32b;
    self->unk_328 = 0;
    if (*(int*)(c + 8) & 1)
        self->unk_32c = 3;
    else
        self->unk_32c = 0;
    self->unk_320 = self->unk_060;
    return 1;
}
}
