//cpp
// @symbol func_ov036_0211137c
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjRcBuranko_c.h"
// recovered name: daObjRcBuranko_c_InitResources
/* recovered: renamed to Class_Method */
/* daObjRcBuranko_c::InitResources - recovered from vtable slot identity */
typedef int Fix12;
struct BMD_File;
struct KCL_File;
struct SharedFilePtr;
struct Matrix4x3;
struct CLPS_Block;
struct Model { static BMD_File *LoadFile(SharedFilePtr &p); };
struct ModelBase { void SetFile(BMD_File *f, int a, int b); };
struct Platform { void UpdateClsnPosAndRot(); };
struct MeshCollider { static KCL_File *LoadFile(SharedFilePtr &p); };
struct MovingMeshCollider {
    void SetFile(KCL_File *f, const Matrix4x3 &m, Fix12 a, short b, CLPS_Block &cb);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, KCL_File *f, const Matrix4x3 &m, Fix12 a, short b, CLPS_Block &cb);


extern SharedFilePtr data_ov036_02114028;
extern SharedFilePtr data_ov036_02114020;
extern CLPS_Block data_ov036_02112b68;
extern "C" void func_ov036_0211123c(char *t);
extern "C" void func_020393d4(int *p, int v);
extern "C" void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();

extern "C" int func_ov036_0211137c(char *c)
{
    struct daObjRcBuranko_c *self = (struct daObjRcBuranko_c *)(void *)c;
    BMD_File *f = Model::LoadFile(data_ov036_02114028);
    ((ModelBase *)(c + 0xd4))->SetFile(f, 1, -1);
    self->unk_090 = 0x2000;
    func_ov036_0211123c(c);
    ((Platform *)c)->UpdateClsnPosAndRot();
    KCL_File *k = MeshCollider::LoadFile(data_ov036_02114020);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((MovingMeshCollider *)(c + 0x124), k, *(Matrix4x3 *)(c + 0x2ec), 0x1000, self->unk_08e, data_ov036_02112b68);
    func_020393d4((int *)(c + 0x124), (int)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    return 1;
}
