//cpp
// @symbol _ZN16SpinningPlatform13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SpinningPlatform.h"
extern "C" void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern "C" void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();
struct BMD_File; struct KCL_File; struct Actor; struct Vector3; struct Matrix4x3;
struct CLPS_Block; struct SharedFilePtr;
/* ModelBase is the real class now, through this actor's header. */
struct ShadowModel { void InitCuboid(); };
struct dBgActor_c { void UpdateClsnPosAndRot(); };
/* Declared by final name, not as a member: the ROM's SetFile takes Fix12<int> where
   this call passes 0x1000, and Fix12<int> is an aggregate with no converting
   constructor from int. Declared `int fix` it mangles ...RK9Matrix4x3isR10CLPS_Block,
   which nothing defines. */
extern "C" void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, KCL_File *f, const Matrix4x3 &m, int fix, short sh, CLPS_Block &b);
extern "C" void func_020393d4(void *p, void *v);
struct RaycastGround {
    int pad[0x11];
    int result;       // offset 0x44
    int pad2[3];      // pad to 0x54 total
    RaycastGround();
    void SetObjAndPos(const Vector3 &v, Actor *a);
    int DetectClsn();
    ~RaycastGround();
};
extern "C" CLPS_Block data_ov035_02112238;

struct V3 { int x, y, z; };

int SpinningPlatform::InitResources()
{
    void *mf = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov035_02112cb0);
    ((ModelBase*)((char *)&mModel))->SetFile((BMD_File*)mf, 1, -1);
    ((ShadowModel*)((char *)&mShadowModel))->InitCuboid();
    func_ov035_021118a8(((char *)this));
    ((dBgActor_c*)((char *)this))->UpdateClsnPosAndRot();
    void *kf = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov035_02112cb8);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        (char *)&mMeshCollider, (KCL_File*)kf,
        *(Matrix4x3*)((char *)&unk_2ec), 0x1000, mAngleY, data_ov035_02112238);
    func_020393d4(((char *)this) + 0x124, (void*)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    func_020396c0(((char *)this) + 0x124, 0);
    V3 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y = v.y - 0xa000;
    RaycastGround rg;
    rg.SetObjAndPos(*(Vector3*)&v, (Actor*)0);
    unk_324 = v.y;
    if (rg.DetectClsn() != 0)
        unk_324 = rg.result;
    return 1;
}
