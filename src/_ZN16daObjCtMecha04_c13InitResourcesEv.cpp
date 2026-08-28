//cpp
// @symbol _ZN16daObjCtMecha04_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjCtMecha04_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"
struct BMD_File;
struct BTA_File;
struct KCL_File;
struct dActor_c;
struct Matrix4x3;
struct CLPS_Block;

/* ModelBase is the real class now, through daObjCtMecha04_c.h. */

/* ShadowModel is the real class now, through this actor's header, and it
   declares InitCuboid itself. */

/* TextureTransformer is the real class now, through this actor's header, which
   types mTextureTransformer. Redefining it ICEs mwccarm (CClass.c:3328); SetFile
   is still reached by its mangled symbol below, because the real one takes
   Fix12<int> BY VALUE and mwccarm passes that differently at the call site. */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *, BTA_File &f, int a, int fix, unsigned int u);

/* dBgW_KcMbg is the real class now, through this actor's header. Only the
   cast below needs the name; SetFile is already reached by its mangled
   symbol because the real one takes Fix12<int> by value. */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, KCL_File *f, const Matrix4x3 &m, int fix, short sh, CLPS_Block &b);


extern "C" void func_020393c4(void *p, void *v);

struct Entry3 {
    void *a;
    void *b;
    void *c;
};

extern "C" Entry3 data_ov065_0211d194[];
extern "C" Entry3 data_ov065_0211d198[];
extern "C" Entry3 data_ov065_0211d19c[];
extern "C" unsigned char data_0209f2c0;
extern "C" void *data_ov065_0211c0b8[];

int daObjCtMecha04_c::InitResources()
{
    void *locbuf[2];
    Vector3 v;
    unsigned char e;
    void *mf;
    void *kf;

    locbuf[0] = *(void **)&data_ov065_0211d16c[0];
    locbuf[1] = *(void **)&data_ov065_0211d16c[4];

    if (actorID != 0x6f) {
        if (actorID == 0x70)
            mVariant = 1;
    } else {
        mVariant = 0;
    }

    e = mVariant;
    mf = Model::LoadFile(
        *(SharedFilePtr *)data_ov065_0211d194[e].a);
    mModel.SetFile(
        (BMD_File *)mf, 1, -1);

    mShadowModel.InitCuboid();

    e = mVariant;
    TextureTransformer::Prepare(
        *(BMD_File *)*(void **)((char *)data_ov065_0211d194[e].a + 4),
        *(BTA_File *)locbuf[e]);

    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj((TextureTransformer *)((char *)&mTextureTransformer), *(BTA_File *)locbuf[mVariant], 0, 0x1000, 0);

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    e = mVariant;
    kf = dBgW_Kc::LoadFile(
        *(SharedFilePtr *)data_ov065_0211d198[e].a);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider, (KCL_File *)kf, mClsnMat, 0x199, mAngleY, *(CLPS_Block *)data_ov065_0211d19c[e].a);

    func_020393c4(
        &mMeshCollider,
        (void *)&daObjCtMecha04_c::AfterClsnCallback);

    *(void **)((char *)&mTargetBeltSpeed) =
        data_ov065_0211c0b8[data_0209f2c0];
    *(void **)((char *)&mBeltSpeed) =
        *(void **)((char *)&mTargetBeltSpeed);
    /* 0x32c is +0xc inside the TextureTransformer at 0x320 -- its Animation base's
       speed. The cartridge's own ~daObjCtMecha04_c proves the extent; see
       tools/dtor_members.py. */
    mTextureTransformer.speed = mBeltSpeed;

    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y = v.y - 0xa000;

    {
        dBgCh_Gnd rg;

        rg.SetObjAndPos(v, (dActor_c *)0);
        mGroundY = v.y;

        if (rg.DetectClsn() != 0)
            mGroundY = rg.clsnY;
    }

    return 1;
}
