//cpp
// @symbol _ZN8SignPost13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SignPost.h"
extern "C" void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
struct BMD_File; struct KCL_File; struct Actor; struct Vector3; struct Matrix4x3;
struct CLPS_Block; struct SharedFilePtr; struct Vector3_16;
struct ModelBase { void SetFile(BMD_File *f, int b, int c); };
struct ShadowModel { void InitCuboid(); };
struct Platform { void UpdateModelPosAndRotY(); void UpdateClsnPosAndRot(); };
struct WithMeshClsn {
    void StartDetectingWater();
};

/* Three methods declared by their final names rather than as members, because each
   takes Fix12<int> where these call sites pass an int literal. Fix12<int> is an
   aggregate with no converting constructor, so a faithful member declaration cannot
   be called with `0x64000` -- and every way of building a temporary costs stack
   traffic the ROM does not have. extern "C" reaches the same symbol and records the
   real signature in the name.

   WithMeshClsn::Init's LAST parameter is a Vector3_16* too, not an int: the S5_ in
   ...P10Vector3_16S5_ is a back-reference to the pointer type two places earlier. */
extern "C" void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, KCL_File *f, const Matrix4x3 &m, int fix, short sh, CLPS_Block &b);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(
    void *self, Actor *a, int fix, int t, unsigned int u0, unsigned int u1);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
    void *self, Actor *a, int fix, int t, Vector3_16 *v, Vector3_16 *v2);
struct RaycastGround {
    int pad[0x11];
    int result;       // offset 0x44
    int pad2[2];      // pad to 0x50 total
    RaycastGround();
    void SetObjAndPos(const Vector3 &v, Actor *a);
    int DetectClsn();
    ~RaycastGround();
};
extern "C" CLPS_Block data_ov002_0210d714;

struct V3 { int x, y, z; };

int SignPost::InitResources()
{
    void *mf = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210e064);
    ((ModelBase*)((char *)&mModel))->SetFile((BMD_File*)mf, 1, -1);
    ((ShadowModel*)((char *)&mShadowModel))->InitCuboid();

    int py = mPosY;
    int pz = mPosZ;
    int px = mPosX;
    int py2 = py + 0x64000;
    V3 v = { px, py2, pz };
    RaycastGround rg;
    rg.SetObjAndPos(*(Vector3*)&v, (Actor*)0);
    if (rg.DetectClsn() != 0)
        mPosY = rg.result;

    ((Platform*)((char *)this))->UpdateModelPosAndRotY();
    ((Platform*)((char *)this))->UpdateClsnPosAndRot();
    func_ov002_020baf80(((char *)this));

    void *kf = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov002_0210e05c);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        (char *)&mMeshCollider, (KCL_File*)kf,
        *(Matrix4x3*)((char *)&unk_2ec), 0x199, mAngleY, data_ov002_0210d714);

    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(
        (char *)&mMovingCylinderClsn, (Actor*)((char *)this), 0x64000, 0x64000, 0x4800002, 0x41000);

    unk_58e = 2;
    unk_3b0 = mPosX;
    unk_3b4 = mPosY;
    unk_3b8 = mPosZ;
    unk_3bc = mAngleX;
    unk_3be = mAngleY;
    unk_3c0 = mAngleZ;
    unk_09c = -0x2000;
    unk_0a0 = -0x3c000;

    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
        (char *)&mWithMeshClsn, (Actor*)((char *)this), 0x28000, 0x28000, (Vector3_16*)0, (Vector3_16*)0);
    ((WithMeshClsn*)((char *)&mWithMeshClsn))->StartDetectingWater();

    return 1;
}
