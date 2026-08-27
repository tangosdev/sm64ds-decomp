//cpp
#include "types.h"
// @symbol _ZN15TtcRotatingCube13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
#include "TtcRotatingCube.h"
struct SharedFilePtr;
struct BMD_File;
struct KCL_File;
struct Matrix4x3;
struct CLPS_Block;
struct Vector3;
struct dActor_c;

extern "C" {
    extern void*data_ov065_0211c0a8[];
    extern s16 data_ov065_0211cfa8[];
    extern void*data_ov065_0211cfd8[];
    extern u8 data_ov065_0211cfa4[];
    void *_ZN5Model8LoadFileER13SharedFilePtr(void *shared);
    void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *mb, void *bmd, int a, int b);
    void _ZN11ShadowModel10InitCuboidEv(void *self);
    void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *shared);
    void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        void *mc, void *kcl, void *mtx, int fix, s16 s, void *clps);
    void func_020393d4(void *p, void *v);
    void Matrix4x3_FromRotationY(void *m, int angle);
    void MulVec3Mat4x3(void *dst, void *mtx, void *src);
    void AddVec3(void *a, void *b, void *c);
    void _ZN9dBgCh_GndC1Ev(void *self);
    void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *self, void *pos, void *actor);
    int _ZN9dBgCh_Gnd10DetectClsnEv(void *self);
    void _ZN9dBgCh_GndD1Ev(void *self);
}

extern void *data_ov065_0211cfd0[];
extern void *data_ov065_0211cfd4[];
extern "C" void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
extern u8 data_0209f2c0;
extern s32 data_020a0e68[];

struct dBgCh_Gnd {
    char pad[0x44];
    s32 hitY;
    char pad2[0xc];
};

int TtcRotatingCube::InitResources()
{
    u8 *c = (u8 *)((void *)this);
    u16 id = actorID;

    if (id != 0x6c) {
        if (id == 0x6d) {
            mVariant = 1;
        }
    } else {
        mVariant = 0;
    }

    u8 idx = mVariant;
    {
        void *bmd = _ZN5Model8LoadFileER13SharedFilePtr(*(void **)((char *)data_ov065_0211cfd0 + idx * 0xc));
        _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, bmd, 1, -1);
    }

    {
        void *bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov065_0211c0a8[mVariant]);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(&mRotatingModel, bmd, 1, -1);
    }

    _ZN11ShadowModel10InitCuboidEv(&mShadowModel);

    mTargetAngleZ = data_ov065_0211cfa8[mVariant];

    UpdateModel();
    UpdateClsn();

    idx = mVariant;
    if (idx == 0) {
        s32 oi = idx * 0xc;
        void *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*(void **)((char *)data_ov065_0211cfd4 + oi));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY,
            *(void **)((char *)data_ov065_0211cfd8 + oi));
    } else {
        s32 oi = idx * 0xc;
        void *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*(void **)((char *)data_ov065_0211cfd4 + oi));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
            *(void **)((char *)data_ov065_0211cfd8 + oi));
    }

    func_020393d4(&mMeshCollider, (void *)_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);

    {
        s32 va[3];
        s32 vb[3];
        mWaitTimer = data_ov065_0211cfa4[data_0209f2c0];
        va[0] = 0;
        va[2] = 0;
        vb[0] = 0;
        vb[1] = 0;
        vb[2] = 0;
        va[1] = 0;
        va[0] = 0x64000;
        va[2] = 0x64000;

        Matrix4x3_FromRotationY(data_020a0e68, mAngleY);
        MulVec3Mat4x3(va, data_020a0e68, vb);
        AddVec3(vb, &mPosX, vb);
        vb[1] -= 0xd2000;

        dBgCh_Gnd rg;
        _ZN9dBgCh_GndC1Ev(&rg);
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, vb, 0);
        mFloorY = vb[1];
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) != 0) {
            mFloorY = rg.hitY;
        }

        vb[0] = mPosX;
        vb[1] = mPosY;
        vb[2] = mPosZ;
        vb[1] -= 0xd2000;

        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, vb, 0);
        s32 r5 = vb[1];
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) != 0) {
            r5 = rg.hitY;
        }
        if (r5 != mFloorY) {
            mUnevenGround = 1;
        }
        _ZN9dBgCh_GndD1Ev(&rg);
    }

    return 1;
}
