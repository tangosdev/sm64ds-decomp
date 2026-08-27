//cpp
// @symbol _ZN6ToxBox13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "ToxBox.h"

typedef struct { int x, y, z; } Vec3;
typedef struct { int m[12]; } Mtx43;

#define LA(p) (p)

extern "C" {
void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, void *, int, int);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *, void *, int, int, void *, int);
void _ZN10dBgCh_Actr13SetLimMovFlagEv(void *);
void _ZN7PathPtr6FromIDEj(void *, unsigned int);
int _ZNK7PathPtr8NumNodesEv(void *);
void _ZNK7PathPtr7GetNodeER7Vector3j(void *, void *, unsigned int);
void func_ov092_021313b0(void *);
void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void func_ov092_02131a88(char *self);
void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, void *, void *, int, short, void *);
void func_020393d4(void *p, void *v);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *, void *, void *, int, int, unsigned int, unsigned int);
}

extern char data_ov092_02132540;
extern char data_ov092_02132548;
extern char data_ov092_02132220;
extern char data_ov092_02132294;
extern Mtx43 data_020a0e68;
extern char _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;

int ToxBox::InitResources()
{
    void *f;
    Vec3 tmp;
    unsigned int idx;

    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov092_02132540);
    _ZN9ModelBase7SetFileEP8BMD_Fileii((char *)&mModel, f, 1, -1);

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_((char *)&mWithMeshClsn, (char *)this, 0xfa000, 0, 0, 0);
    _ZN10dBgCh_Actr13SetLimMovFlagEv((char *)&mWithMeshClsn);

    idx = mParam & 3;
    mMoveKind = (unsigned char)idx;
    idx = mMoveKind;
    if (idx != 3) {
        int v = ((int *)&data_ov092_02132294)[idx];
        mMoveSeq = v;
        mMoveSeqIndex = 0;
        mMoveDir = *(int *)mMoveSeq;
    } else {
        _ZN7PathPtr6FromIDEj((char *)this + 0x58c, ((unsigned int)mParam >> 8) & 0xf);
        mPathNodeCount = _ZNK7PathPtr8NumNodesEv((char *)this + 0x58c);
        mPathNodeIndex = 0;
        _ZNK7PathPtr7GetNodeER7Vector3j((char *)this + 0x58c, (char *)this + 0x580, mPathNodeIndex);
        func_ov092_021313b0((char *)this);
    }

    {
        mPosY += 0xfa000;
        mRestPosX = mPosX;
        mRestPosY = mPosY;
        mRestPosZ = mPosZ;
        Vec3_Asr(&tmp, (Vec3 *)&mPosX, 3);
    }

    Matrix4x3_FromTranslation(&data_020a0e68, tmp.x, tmp.y, tmp.z);
    *(Mtx43 *)((char *)this + 0xf0) = data_020a0e68;
    *(Mtx43 *)&mBaseMtx = *(Mtx43 *)((char *)this + 0xf0);

    func_ov092_02131a88((char *)this);

    f = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov092_02132548);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        (char *)&mMeshCollider, f, (char *)this + 0x2ec, 0x1000, mAngleY, &data_ov092_02132220);
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
            (char *)&mdCcAcPos_c, (char *)this, (void *)&stk[0], 0xc8000, 0x190000, 2, 0x6003c0);
    }

    mPlayerActor = 0;
    return 1;
}
