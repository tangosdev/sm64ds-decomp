//cpp
// @symbol _ZN15RollingIronBall13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_PathPtr.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RollingIronBall.h"
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN7PathPtr6FromIDEj(void *self, unsigned int id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, void *v, unsigned int idx);
extern int Vec3_Equal(void *a, void *b);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int a, int b, unsigned int c, unsigned int d);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v0, int v1);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *c);

extern char data_ov100_02148668;
extern int data_02092138;
extern signed char data_0209f2f8;

int RollingIronBall::InitResources()
{
    int kind;
    int d;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov100_02148668), 1, -1);
    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0)
        return 0;
    mVertAccel = -0x4000;
    mTerminalVelocity = -0x46000;
    mVariant = param1 & 0xf;
    param1 = param1 >> 4;
    kind = mVariant;

    if (kind == 2 || kind == 4) {
        _ZN7PathPtr6FromIDEj(&mPathPtr, param1 & 0xf);
        mNumPathNodes = _ZNK7PathPtr8NumNodesEv((char *)&mPathPtr);
        mPathNodeIndex = 0;
        mSpawnPosX = mPosX;
        mSpawnPosY = mPosY;
        mSpawnPosZ = mPosZ;
        _ZNK7PathPtr7GetNodeER7Vector3j(&mPathPtr, &mNextNodePosX, mPathNodeIndex);
        if (Vec3_Equal(&mPosX, &mNextNodePosX)) {
            *(int *)(((int)((char *)this) + 0x3d8) & 0xFFFFFFFFFFFFFFFFLL) += 1;
            _ZNK7PathPtr7GetNodeER7Vector3j(&mPathPtr, &mNextNodePosX, mPathNodeIndex);
        }
        func_ov100_0214233c(((char *)this));
        mPrevAngleY = unk_3ba;
        unk_3c8 = *(int *)&data_02092138;
        d = *(signed char *)&data_0209f2f8;
        if (d == 0x19) {
            mHorzSpeed = 0xa000;
            mDrawScaleX = 0x800;
            mDrawScaleY = 0x800;
            mDrawScaleZ = 0x800;
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, ((char *)this), 0x1e000, 0x1e000, 0x200004, 0x3c0);
            _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, ((char *)this), 0x1e000, 0x1e000, 0, 0);
            unk_3c8 = -0x640000;
        } else {
            mDrawScaleX = 0x1000;
            mDrawScaleY = 0x1000;
            mDrawScaleZ = 0x1000;
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, ((char *)this), 0x64000, 0x64000, 0x200004, 0x3c0);
            _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, ((char *)this), 0x64000, 0x64000, 0, 0);
            d = *(volatile signed char *)&data_0209f2f8;
            if (d == 0x18) {
                mHorzSpeed = 0x19000;
                unk_3c8 = 0xfec78000;
            } else if (d == 0x16) {
                mHorzSpeed = 0xa000;
                unk_3c8 = 0xff63c000;
            } else {
                if (d == 6)
                    mHorzSpeed = 0x14000;
                else
                    mHorzSpeed = 0xa000;
            }
        }
        _ZN8dActor_c9UpdatePosEP5dCc_c(((char *)this), 0);
        _ZN10dBgCh_Actr13SetLimMovFlagEv((char *)&mWithMeshClsn);
        goto end;
    }

    if (kind == 1) {
        *(unsigned int *)(((int)((char *)this) + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
        mDrawScaleX = 0x1000;
        mDrawScaleY = 0x1000;
        mDrawScaleZ = 0x1000;
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, ((char *)this), 0x64000, 0x64000, 0x200004, 0x3c0);
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, ((char *)this), 0x64000, 0x64000, 0, 0);
        _ZN10dBgCh_Actr13SetLimMovFlagEv((char *)&mWithMeshClsn);
        goto end;
    }

    if (kind == 0) {
        d = *(signed char *)&data_0209f2f8;
        if (d == 0x19) {
            unk_3c0 = 0x200000;
            unk_3c4 = 0x1770000;
        } else {
            unk_3c0 = 0x400000;
            if (d == 0x18)
                unk_3c4 = 0x1b58000;
            else if (d == 6)
                unk_3c4 = 0x11f8000;
            else if (d == 0x16)
                unk_3c4 = 0x1b58000;
            else
                unk_3c4 = 0x1964000;
        }
    }

end:
    unk_100 = 0;
    unk_108 = 0;
    unk_3cc = 0;
    unk_3a8 = 0;
    unk_3d2 = 0;
    return 1;
}
