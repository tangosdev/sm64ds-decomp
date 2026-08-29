//cpp
// @symbol _ZN11PowerFlower13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_SaveData.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PowerFlower.h"
#include "dBgCh_Gnd.h"
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *act, Fix12i a, Fix12i b, unsigned int c2, unsigned int d);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *act, Fix12i a, Fix12i b, void *d, void *e);
extern void _ZN10dBgCh_Actr19StartDetectingWaterEv(void *self);
extern void *_ZN8dActor_c13ClosestPlayerEv(void *self);
}

extern void *data_ov002_0210d9d0[];
extern void *data_ov002_0210d9b0[];

int PowerFlower::InitResources()
{
    struct Vector3 pos;
    short *angp;

    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9d0);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9b0);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x124, data_ov002_0210d9b0[1], 1, -1) == 0)
        return 0;
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, data_ov002_0210d9d0[1], 1, -1) == 0)
        return 0;
    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0)
        return 0;

    mVertAccel = -0x668;
    mTerminalVelocity = -0xf000;
    func_ov002_020b9a1c(((char *)this));

    mScaleX = 0xfa0;
    mScaleY = 0xfa0;
    mScaleZ = 0xfa0;

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0x1cc, ((char *)this), 0x32000, 0x64000, 0x800002, 0x8000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x200, ((char *)this), 0x3c000, 0x3c000, 0, 0);
    _ZN10dBgCh_Actr19StartDetectingWaterEv((char *)&mWithMeshClsn);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    mGroundY = pos.y;
    if (ground.DetectClsn())
        mGroundY = ground.clsnY;
    mLifeTimer = 0xb4;

    if (param1 == 0xffff) {
        if (*(int *)((char *)_ZN8dActor_c13ClosestPlayerEv(((char *)this)) + 8) == 1 && _ZN8SaveData16HasPlayerLostCapEv() == 0) {
            func_ov002_020b9704(((char *)this), 2);
        } else {
            return 0;
        }
    } else {
        func_ov002_020b9704(((char *)this), 0);
    }
    angp = (short *)(int)((char *)&mAngleY);
    *angp = *angp - 0x4000;
    return 1;
}
