//cpp
// @symbol _ZN8Moneybag13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Moneybag.h"
struct RaycastGround { char buf0[0x14]; int floor[12]; char buf1[0x50-0x14-0x30]; };

extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
extern "C" void* _ZN9Animation8LoadFileER13SharedFilePtr(void* fp);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void* self);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, void* actor, int a, int b, unsigned int c, unsigned int d);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, void* actor, int a, int b, void* v, int c);
extern "C" void _ZN12WithMeshClsn19StartDetectingWaterEv(void* self);
extern "C" void _ZN13RaycastGroundC1Ev(RaycastGround* self);
extern "C" void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* self, const Vector3& v, void* actor);
extern "C" int _ZN13RaycastGround10DetectClsnEv(RaycastGround* self);
extern "C" void _ZN13RaycastGroundD1Ev(RaycastGround* self);

struct Block48 { int w[12]; };

extern int data_ov002_0210d9b8[];
extern Block48 data_02082128;

int Moneybag::InitResources()
{
    RaycastGround rc;
    Vector3 pos;
    void* m = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov081_02128ed4);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, m, 1, 1);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x138, (void*)data_ov002_0210d9b8[1], 1, 1) == 0)
        return 0;
    for (int i = 0; i < 4; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov081_021280d8[i]);
    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char*)this) + 0x1b0, ((char*)this), 0x4b000, 0x73000, 0x200000, 0x6eff0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x1e4, ((char*)this), 0x4b000, 0x4b000, 0, 0);
    _ZN12WithMeshClsn19StartDetectingWaterEv((char*)&mWithMeshClsn);
    func_ov081_0212777c(((char*)this), 0);
    unk_09c = -0x2000;
    unk_0a0 = -0x3c000;
    {
        int p60;
        pos.x = mPosX;
        p60 = mPosY;
        pos.y = p60;
        pos.z = mPosZ;
        pos.y = p60 + 0x14000;
    }
    _ZN13RaycastGroundC1Ev(&rc);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rc, pos, 0);
    if (_ZN13RaycastGround10DetectClsnEv(&rc))
        mPosY = rc.floor[(0x44 - 0x14) / 4];
    else
        mPosY = pos.y;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    unk_3d0 = mPosX;
    unk_3d4 = mPosY;
    unk_3d8 = mPosZ;
    unk_3f0 = 1;
    *(Block48*)((char*)&unk_3a0) = data_02082128;
    func_ov081_02126a20(((char*)this));
    _ZN13RaycastGroundD1Ev(&rc);
    return 1;
}
