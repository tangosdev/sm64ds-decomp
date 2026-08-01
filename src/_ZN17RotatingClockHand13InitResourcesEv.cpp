//cpp
// @symbol _ZN17RotatingClockHand13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingClockHand.h"
struct BMD_File; struct KCL_File; struct Actor; struct Vector3; struct Matrix4x3;
struct CLPS_Block; struct SharedFilePtr;
struct ModelBase { void SetFile(BMD_File *f, int b, int c); };
struct ShadowModel { void InitCuboid(); };
struct Platform { void UpdateClsnPosAndRot(); };
struct MovingMeshCollider {
    void SetFile(KCL_File *f, const Matrix4x3 &m, int fix, short sh, CLPS_Block &b);
};
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

int RotatingClockHand::InitResources()
{
    void *mf = ModelLoadFile(&data_ov035_02112cb0);
    ((ModelBase*)((char *)&mModel))->SetFile((BMD_File*)mf, 1, -1);
    ((ShadowModel*)((char *)&mShadowModel))->InitCuboid();
    func_ov035_021118a8(((char *)this));
    ((Platform*)((char *)this))->UpdateClsnPosAndRot();
    void *kf = MeshColliderLoadFile(&data_ov035_02112cb8);
    ((MovingMeshCollider*)((char *)&mMeshCollider))->SetFile((KCL_File*)kf,
        *(Matrix4x3*)((char *)&unk_2ec), 0x1000, unk_08e, data_ov035_02112238);
    func_020393d4(((char *)this) + 0x124, (void*)&UpdatePosWithTransformSym);
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
