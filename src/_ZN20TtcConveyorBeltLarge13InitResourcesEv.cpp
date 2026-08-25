//cpp
// @symbol _ZN20TtcConveyorBeltLarge13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TtcConveyorBeltLarge.h"
struct BMD_File;
struct BTA_File;
struct KCL_File;
struct dActor_c;
struct Vector3;
struct Matrix4x3;
struct CLPS_Block;

extern "C" void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);

/* ModelBase is the real class now, through TtcConveyorBeltLarge.h. */

/* ShadowModel is the real class now, through this actor's header, and it
   declares InitCuboid itself. */

extern "C" void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(
    BMD_File &f, BTA_File &b);

/* TextureTransformer is the real class now, through this actor's header, which
   types mTextureTransformer. Redefining it ICEs mwccarm (CClass.c:3328); SetFile
   is still reached by its mangled symbol below, because the real one takes
   Fix12<int> BY VALUE and mwccarm passes that differently at the call site. */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *, BTA_File &f, int a, int fix, unsigned int u);


struct dBgActor_c {
    void UpdateModelPosAndRotY();
    void UpdateClsnPosAndRot();
};

extern "C" void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);

/* dBgW_KcMbg is the real class now, through this actor's header. Only the
   cast below needs the name; SetFile is already reached by its mangled
   symbol because the real one takes Fix12<int> by value. */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, KCL_File *f, const Matrix4x3 &m, int fix, short sh, CLPS_Block &b);


extern "C" void func_020393c4(void *p, void *v);

struct dBgCh_Gnd {
    int pad[0x11];
    int result;
    int pad2[2];

    dBgCh_Gnd();
    void SetObjAndPos(const Vector3 &v, dActor_c *a);
    int DetectClsn();
    ~dBgCh_Gnd();
};

struct Entry3 {
    void *a;
    void *b;
    void *c;
};

extern "C" Entry3 data_ov065_0211d194[];
extern "C" Entry3 data_ov065_0211d198[];
extern "C" Entry3 data_ov065_0211d19c[];
extern "C" void func_ov065_0211aacc();
extern "C" unsigned char data_0209f2c0;
extern "C" void *data_ov065_0211c0b8[];

struct V3 {
    int x;
    int y;
    int z;
};

int TtcConveyorBeltLarge::InitResources()
{
    void *locbuf[2];
    V3 v;
    unsigned char e;
    void *mf;
    void *kf;

    locbuf[0] = *(void **)&data_ov065_0211d16c[0];
    locbuf[1] = *(void **)&data_ov065_0211d16c[4];

    if (mActorID != 0x6f) {
        if (mActorID == 0x70)
            mVariant = 1;
    } else {
        mVariant = 0;
    }

    e = mVariant;
    mf = _ZN5Model8LoadFileER13SharedFilePtr(
        data_ov065_0211d194[e].a);
    ((ModelBase *)((char *)&mModel))->SetFile(
        (BMD_File *)mf, 1, -1);

    ((ShadowModel *)((char *)&mShadowModel))->InitCuboid();

    e = mVariant;
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(
        *(BMD_File *)*(void **)((char *)data_ov065_0211d194[e].a + 4),
        *(BTA_File *)locbuf[e]);

    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj((TextureTransformer *)((char *)&mTextureTransformer), *(BTA_File *)locbuf[mVariant], 0, 0x1000, 0);

    ((dBgActor_c *)((char *)this))->UpdateModelPosAndRotY();
    ((dBgActor_c *)((char *)this))->UpdateClsnPosAndRot();

    e = mVariant;
    kf = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(
        data_ov065_0211d198[e].a);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((dBgW_KcMbg *)((char *)&mMeshCollider), (KCL_File *)kf, *(Matrix4x3 *)((char *)&mClsnMat), 0x199, mAngleY, *(CLPS_Block *)data_ov065_0211d19c[e].a);

    func_020393c4(
        ((char *)this) + 0x124,
        (void *)&func_ov065_0211aacc);

    *(void **)((char *)&mTargetBeltSpeed) =
        data_ov065_0211c0b8[data_0209f2c0];
    *(void **)((char *)&mBeltSpeed) =
        *(void **)((char *)&mTargetBeltSpeed);
    /* 0x32c is +0xc inside the TextureTransformer at 0x320 -- its Animation base's
       speed. The cartridge's own ~TtcConveyorBeltLarge proves the extent; see
       tools/dtor_members.py. */
    *(void **)((char *)&mTextureTransformer.speed) =
        *(void **)((char *)&mBeltSpeed);

    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y = v.y - 0xa000;

    {
        dBgCh_Gnd rg;

        rg.SetObjAndPos(*(Vector3 *)&v, (dActor_c *)0);
        mGroundY = v.y;

        if (rg.DetectClsn() != 0)
            mGroundY = rg.result;
    }

    return 1;
}
