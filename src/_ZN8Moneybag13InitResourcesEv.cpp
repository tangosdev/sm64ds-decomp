//cpp
// @symbol _ZN8Moneybag13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Moneybag.h"
#include "dBgCh_Gnd.h"

extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
extern "C" void* _ZN9Animation8LoadFileER13SharedFilePtr(void* fp);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void* self);
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, void* actor, int a, int b, unsigned int c, unsigned int d);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, void* actor, int a, int b, void* v, int c);
extern "C" void _ZN10dBgCh_Actr19StartDetectingWaterEv(void* self);


extern int data_ov002_0210d9b8[];
extern Matrix4x3 IDENTITY_MATRIX4X3;

int Moneybag::InitResources()
{
    Vector3 pos;
    void* m = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov081_02128ed4);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, m, 1, 1);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x138, (void*)data_ov002_0210d9b8[1], 1, 1) == 0)
        return 0;
    for (int i = 0; i < 4; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov081_021280d8[i]);
    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this) + 0x1b0, ((char*)this), 0x4b000, 0x73000, 0x200000, 0x6eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x1e4, ((char*)this), 0x4b000, 0x4b000, 0, 0);
    _ZN10dBgCh_Actr19StartDetectingWaterEv((char*)&mWithMeshClsn);
    func_ov081_0212777c(((char*)this), 0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    {
        int p60;
        pos.x = mPosX;
        p60 = mPosY;
        pos.y = p60;
        pos.z = mPosZ;
        pos.y = p60 + 0x14000;
    }
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mState = 1;
    *(Matrix4x3*)((char*)&mMatrix) = IDENTITY_MATRIX4X3;
    func_ov081_02126a20(((char*)this));
    return 1;
}
