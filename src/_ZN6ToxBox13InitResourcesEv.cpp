//cpp
// @symbol _ZN6ToxBox13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "ToxBox.h"
#include "SharedFilePtr.h"

typedef struct { int x, y, z; } Vec3;
typedef struct { int m[12]; } Mtx43;

#define LA(p) (p)

extern "C" {
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, dActor_c *actor, int radius, int height,
    void *a, int b);
void _ZN7PathPtr6FromIDEj(PathPtr *path, unsigned int id);
void func_ov092_021313b0(void *);
void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void func_ov092_02131a88(char *self);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    int scale, short angY, void *clps);
void func_020393d4(void *p, void *v);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *self, dActor_c *actor, const Vector3 *offset,
    int radius, int height, unsigned int flags, unsigned int vulnFlags);
}

extern SharedFilePtr data_ov092_02132540;
extern SharedFilePtr data_ov092_02132548;
extern char data_ov092_02132220;
extern char data_ov092_02132294;
extern Mtx43 data_020a0e68;
extern char _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;

int ToxBox::InitResources()
{
    void *f;
    Vec3 tmp;
    unsigned int idx;

    f = Model::LoadFile(data_ov092_02132540);
    mModel.SetFile((BMD_File *)f, 1, -1);

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0xfa000, 0, 0, 0);
    mWithMeshClsn.SetLimMovFlag();

    idx = param1 & 3;
    mMoveKind = (unsigned char)idx;
    idx = mMoveKind;
    if (idx != 3) {
        mMoveSeq = ((s32 **)&data_ov092_02132294)[idx];
        mMoveSeqIndex = 0;
        mMoveDir = *mMoveSeq;
    } else {
        _ZN7PathPtr6FromIDEj(&mPathPtr, (param1 >> 8) & 0xf);
        mPathNodeCount = mPathPtr.NumNodes();
        mPathNodeIndex = 0;
        mPathPtr.GetNode(mPathNode, mPathNodeIndex);
        func_ov092_021313b0(this);
    }

    {
        mPosY += 0xfa000;
        mRestPos.x = mPosX;
        mRestPos.y = mPosY;
        mRestPos.z = mPosZ;
        Vec3_Asr(&tmp, (Vec3 *)&mPosX, 3);
    }

    Matrix4x3_FromTranslation(&data_020a0e68, tmp.x, tmp.y, tmp.z);
    *(Mtx43 *)((char *)this + 0xf0) = data_020a0e68;
    *(Mtx43 *)&mBaseMtx = *(Mtx43 *)((char *)this + 0xf0);

    func_ov092_02131a88((char *)this);

    f = dBgW_Kc::LoadFile(data_ov092_02132548);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)f, &mClsnMat,
        0x1000, mAngleY, &data_ov092_02132220);
    func_020393d4((char *)&mMeshCollider, &_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);

    mOrientBits = (unsigned char)((mAngleX >> 0xe) & 3);
    {
        unsigned char *b = (unsigned char *)LA(&mOrientBits);
        *b = (unsigned char)(*b | ((mAngleY >> 0xc) & 0xc));
        *b = (unsigned char)(*b | ((mAngleZ >> 0xa) & 0x30));
    }

    {
        int stk[3];
        stk[0] = 0;
        stk[1] = -0xfa000;
        stk[2] = 0;
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            &mdCcAcPos_c, this, (const Vector3 *)&stk[0],
            0xc8000, 0x190000, 2, 0x6003c0);
    }

    mPlayerActor = 0;
    return 1;
}
