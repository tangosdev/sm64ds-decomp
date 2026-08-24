//cpp
// @symbol _ZN11SnowmanBody13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SnowmanBody.h"
struct dBgCh_Gnd { char buf0[0x14]; int floor[12]; char buf1[0x50-0x14-0x30]; };

extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void* self);
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, void* actor, int a, int b, unsigned int c, unsigned int d);
extern "C" void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd* self);
extern "C" void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd* self, const Vector3& v, void* actor);
extern "C" int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd* self);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, void* actor, int a, int b, void* v, int c);
extern "C" void func_ov072_0211f3e4(char* self);
extern "C" void _ZN7PathPtr6FromIDEj(void* self, unsigned int id);
extern "C" void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd* self);


extern Matrix4x3 IDENTITY_MATRIX4X3;

int SnowmanBody::InitResources()
{
    dBgCh_Gnd rc;
    Vector3 pos;
    void* file = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov072_02122b20);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, file, 1, 1);
    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this) + 0x14c, ((char*)this), 0x82000, 0x104000, 0x800004, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x180, ((char*)this), 0x82000, 0x82000, 0, 0);
    {
        int p60;
        pos.x = mPosX;
        p60 = mPosY;
        pos.y = p60;
        pos.z = mPosZ;
        pos.y = p60 + 0x14000;
    }
    _ZN9dBgCh_GndC1Ev(&rc);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rc, pos, 0);
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rc))
        mPosY = rc.floor[(0x44 - 0x14) / 4];
    else
        mPosY = pos.y;
    unk_33c = mPosX;
    unk_340 = mPosY;
    unk_344 = mPosZ;
    unk_348 = mAngleX;
    unk_34a = mAngleY;
    unk_34c = mAngleZ;
    unk_3a4 = 0x5a;
    func_ov072_0211fcb0(((char*)this), 0);
    unk_390 = 0;
    *(Matrix4x3*)((char*)&unk_350) = IDENTITY_MATRIX4X3;
    func_ov072_0211f3e4(((char*)this));
    _ZN7PathPtr6FromIDEj(((char*)this) + 0x380, mParam & 0xff);
    _ZN9dBgCh_GndD1Ev(&rc);
    return 1;
}
