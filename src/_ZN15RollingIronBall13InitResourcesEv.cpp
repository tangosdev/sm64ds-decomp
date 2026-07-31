//cpp
// @symbol _ZN15RollingIronBall13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_PathPtr.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RollingIronBall.h"
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN7PathPtr6FromIDEj(void *self, unsigned int id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, void *v, unsigned int idx);
extern int Vec3_Equal(void *a, void *b);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *actor, int a, int b, unsigned int c, unsigned int d);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v0, int v1);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *c);

extern char data_ov100_02148668;
extern int data_02092138;
extern signed char data_0209f2f8;

int RollingIronBall::InitResources()
{
    int kind;
    int d;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x2cc,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov100_02148668), 1, -1);
    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0)
        return 0;
    unk_09c = -0x4000;
    unk_0a0 = -0x46000;
    unk_3d0 = mParam & 0xf;
    mParam = mParam >> 4;
    kind = unk_3d0;

    if (kind == 2 || kind == 4) {
        _ZN7PathPtr6FromIDEj(((char *)this) + 0x3f4, mParam & 0xf);
        unk_3d4 = _ZNK7PathPtr8NumNodesEv((char *)&mPathPtr);
        unk_3d8 = 0;
        unk_3dc = mPosX;
        unk_3e0 = mPosY;
        unk_3e4 = mPosZ;
        _ZNK7PathPtr7GetNodeER7Vector3j(((char *)this) + 0x3f4, ((char *)this) + 0x3e8, unk_3d8);
        if (Vec3_Equal(((char *)this) + 0x5c, ((char *)this) + 0x3e8)) {
            *(int *)(((int)((char *)this) + 0x3d8) & 0xFFFFFFFFFFFFFFFFLL) += 1;
            _ZNK7PathPtr7GetNodeER7Vector3j(((char *)this) + 0x3f4, ((char *)this) + 0x3e8, unk_3d8);
        }
        func_ov100_0214233c(((char *)this));
        unk_094 = unk_3ba;
        unk_3c8 = *(int *)&data_02092138;
        d = *(signed char *)&data_0209f2f8;
        if (d == 0x19) {
            mHorzSpeed = 0xa000;
            unk_3ac = 0x800;
            unk_3b0 = 0x800;
            unk_3b4 = 0x800;
            _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char *)this) + 0x374, ((char *)this), 0x1e000, 0x1e000, 0x200004, 0x3c0);
            _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x110, ((char *)this), 0x1e000, 0x1e000, 0, 0);
            unk_3c8 = -0x640000;
        } else {
            unk_3ac = 0x1000;
            unk_3b0 = 0x1000;
            unk_3b4 = 0x1000;
            _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char *)this) + 0x374, ((char *)this), 0x64000, 0x64000, 0x200004, 0x3c0);
            _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x110, ((char *)this), 0x64000, 0x64000, 0, 0);
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
        _ZN5Actor9UpdatePosEP12CylinderClsn(((char *)this), 0);
        _ZN12WithMeshClsn13SetLimMovFlagEv((char *)&mWithMeshClsn);
        goto end;
    }

    if (kind == 1) {
        *(unsigned int *)(((int)((char *)this) + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
        unk_3ac = 0x1000;
        unk_3b0 = 0x1000;
        unk_3b4 = 0x1000;
        _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char *)this) + 0x374, ((char *)this), 0x64000, 0x64000, 0x200004, 0x3c0);
        _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x110, ((char *)this), 0x64000, 0x64000, 0, 0);
        _ZN12WithMeshClsn13SetLimMovFlagEv((char *)&mWithMeshClsn);
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
