//cpp
// @symbol func_ov002_020b8fe0
// recovered name: daObjPushblock_c_InitResources
/* recovered: shared common types, renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method */
/* daObjPushblock_c::InitResources - recovered from vtable slot identity */
struct SharedFilePtr;
struct BMD_File;
struct KCL_File;

extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int a, int b);
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern "C" KCL_File *_ZN12MeshCollider8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, KCL_File *k, void *m, int fix, short s, void *clps);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *a, int b, int c, void *d, int e);

struct V3 { int x, y, z; };
struct RaycastGround { char buf[0x44]; int f44; char rest[8]; };
extern "C" void _ZN13RaycastGroundC1Ev(RaycastGround *self);
extern "C" void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround *self, V3 *v, void *a);
extern "C" int _ZN13RaycastGround10DetectClsnEv(RaycastGround *self);
extern "C" void _ZN13RaycastGroundD1Ev(RaycastGround *self);

extern SharedFilePtr data_ov002_0210df9c;
extern SharedFilePtr data_ov002_0210df94;

extern "C" int func_ov002_020b8fe0(char *self)
{
    RaycastGround rg;
    V3 v;
    BMD_File *bmd;
    KCL_File *kcl;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210df9c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, bmd, 1, -1);
    _ZN8Platform21UpdateModelPosAndRotYEv(self);
    _ZN8Platform19UpdateClsnPosAndRotEv(self);
    kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov002_0210df94);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        self + 0x124, kcl, self + 0x2ec, 0x1000, *(short *)(self + 0x8e), &data_ov002_0210d7b4);

    *(int *)(self + 0x9c) = -0x2000;
    *(int *)(self + 0xa0) = -0x3c000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(self + 0x320, self, 0x96000, 0x96000, 0, 0);

    v.x = *(int *)(self + 0x5c);
    v.y = *(int *)(self + 0x60);
    v.z = *(int *)(self + 0x64);
    v.y = v.y + 0x14000;
    _ZN13RaycastGroundC1Ev(&rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &v, 0);
    *(int *)(self + 0x4e8) = v.y;
    if (_ZN13RaycastGround10DetectClsnEv(&rg))
        *(int *)(self + 0x4e8) = rg.f44;
    *(int *)(self + 0x4dc) = *(int *)(self + 0x5c);
    *(int *)(self + 0x4e0) = *(int *)(self + 0x60);
    *(int *)(self + 0x4e4) = *(int *)(self + 0x64);
    _ZN13RaycastGroundD1Ev(&rg);
    return 1;
}
