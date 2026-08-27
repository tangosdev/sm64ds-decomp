//cpp
// @symbol _ZN8SignPost13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SignPost.h"
/* Model::LoadFile and dBgW_Kc::LoadFile by their real ROM symbols, carried
   forward from #1554. decl_common.h's ModelLoadFile / MeshColliderLoadFile are
   phantoms -- names no module defines -- and match.py compares relocated words as
   wildcards, so the byte gate never saw it. */
extern "C" void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
extern "C" void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *);
/* THREE OF THE SHADOWS ARE GONE, because SignPost.h now says `SignPost :
   dBgActor_c` and dBgActor_c.h brings in the real dBgActor_c, Model/ModelBase and
   dBgW_KcMbg. Each one is replaced by the thing it was standing in for:

     ModelBase          -> mModel.SetFile, whose real declaration in
                           include/ModelBase.h mangles identically
                           (_ZN9ModelBase7SetFileEP8BMD_Fileii) -- only the
                           return type differed, and that is not mangled.
     dBgActor_c           -> the two calls are unqualified members now.
     dBgW_KcMbg -> its real SetFile takes Fix12<int> BY VALUE, so it
                           cannot be declared as a callable method here without
                           changing how the caller passes the argument
                           (notes/mwccarm-codegen.md 6az). It keeps a scalar
                           extern "C" declaration under its exact ROM symbol,
                           which also FIXES A PHANTOM: the shadow spelled `int`
                           and so emitted a `bl` to
                           ..._Matrix4x3isR10CLPS_Block, which exists nowhere.
                           match.py compares relocated words as wildcards, so
                           nothing caught it.

   The remaining shadows -- ShadowModel, dCcAc_c, dBgCh_Actr,
   dBgCh_Gnd -- are untouched: dBgActor_c.h does not pull those headers in, so
   they still compile, and dCcAc_c::Init and dBgCh_Actr::Init have
   the same Fix12<int> problem with no collision forcing the issue yet. */
struct BMD_File; struct KCL_File; struct dActor_c; struct Vector3; struct Matrix4x3;
struct CLPS_Block; struct SharedFilePtr; struct Vector3_16;
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, KCL_File *f, const Matrix4x3 &m, s32 fix, s16 sh, CLPS_Block &b);
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *self, dActor_c *a, s32 radius, s32 height, u32 u0, u32 u1);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, dActor_c *a, s32 radius, s32 height, Vector3_16 *v, Vector3_16 *v2);
/* dBgCh_Actr is the real class now, through this actor's header, and it
   declares StartDetectingWater itself. */
struct dBgCh_Gnd {
    int pad[0x11];
    int result;       // offset 0x44
    int pad2[2];      // pad to 0x50 total
    dBgCh_Gnd();
    void SetObjAndPos(const Vector3 &v, dActor_c *a);
    int DetectClsn();
    ~dBgCh_Gnd();
};
extern "C" CLPS_Block data_ov002_0210d714;

struct V3 { int x, y, z; };

int SignPost::InitResources()
{
    void *mf = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210e064);
    mModel.SetFile((BMD_File*)mf, 1, -1);
    mShadowModel.InitCuboid();

    int py = mPosY;
    int pz = mPosZ;
    int px = mPosX;
    int py2 = py + 0x64000;
    V3 v = { px, py2, pz };
    dBgCh_Gnd rg;
    rg.SetObjAndPos(*(Vector3*)&v, (dActor_c*)0);
    if (rg.DetectClsn() != 0)
        mPosY = rg.result;

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    func_ov002_020baf80(((char *)this));

    void *kf = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov002_0210e05c);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File*)kf, mClsnMat, 0x199, mAngleY, data_ov002_0210d714);

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, (dActor_c*)((char *)this), 0x64000, 0x64000, 0x4800002, 0x41000);

    mPoundsLeft = 2;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mHomeAngleX = mAngleX;
    mHomeAngleY = mAngleY;
    mHomeAngleZ = mAngleZ;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, (dActor_c*)((char *)this), 0x28000, 0x28000, 0, 0);
    mWithMeshClsn.StartDetectingWater();

    return 1;
}
