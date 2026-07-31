//cpp
// @symbol _ZN9PushBlock13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_SaveData.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PushBlock.h"
typedef int Fix12;

extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *act, Fix12 a, Fix12 b, unsigned int c2, unsigned int d);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *act, Fix12 a, Fix12 b, void *d, void *e);
extern void _ZN12WithMeshClsn19StartDetectingWaterEv(void *self);
extern void _ZN13RaycastGroundC1Ev(void *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, const struct Vector3 *pos, void *act);
extern int _ZN13RaycastGround10DetectClsnEv(void *self);
extern void *_ZN5Actor13ClosestPlayerEv(void *self);
extern void _ZN13RaycastGroundD1Ev(void *self);

extern void *data_ov002_0210d9d0[];
extern void *data_ov002_0210d9b0[];

int PushBlock::InitResources()
{
    struct Vector3 pos;
    char ray[0x54];
    short *angp;

    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9d0);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9b0);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x124, data_ov002_0210d9b0[1], 1, -1) == 0)
        return 0;
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, data_ov002_0210d9d0[1], 1, -1) == 0)
        return 0;
    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0)
        return 0;

    unk_09c = -0x668;
    unk_0a0 = -0xf000;
    func_ov002_020b9a1c(((char *)this));

    mScaleX = 0xfa0;
    mScaleY = 0xfa0;
    mScaleZ = 0xfa0;

    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char *)this) + 0x1cc, ((char *)this), 0x32000, 0x64000, 0x800002, 0x8000);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x200, ((char *)this), 0x3c000, 0x3c000, 0, 0);
    _ZN12WithMeshClsn19StartDetectingWaterEv((char *)&mWithMeshClsn);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    _ZN13RaycastGroundC1Ev(ray);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(ray, &pos, 0);
    unk_3bc = pos.y;
    if (_ZN13RaycastGround10DetectClsnEv(ray))
        unk_3bc = *(int *)(ray + 0x44);
    unk_3ca = 0xb4;

    if (mParam == 0xffff) {
        if (*(int *)((char *)_ZN5Actor13ClosestPlayerEv(((char *)this)) + 8) == 1 && _ZN8SaveData16HasPlayerLostCapEv() == 0) {
            func_ov002_020b9704(((char *)this), 2);
        } else {
            _ZN13RaycastGroundD1Ev(ray);
            return 0;
        }
    } else {
        func_ov002_020b9704(((char *)this), 0);
    }
    angp = (short *)(int)(((long long)(int)((char *)&unk_08e)));
    *angp = *angp - 0x4000;
    _ZN13RaycastGroundD1Ev(ray);
    return 1;
}
