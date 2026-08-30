//cpp
// @symbol _ZN8daKrpa_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daKrpa_c.h"
#include "dBgCh_Gnd.h"
/* was `typedef int Fix12;` -- collides with the real Fix12<> template, which
   daKrpa_c.h now reaches via Model.h. The typedef WAS int, so spelling it
   int below is byte-neutral. */
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void* self);
}
extern "C" {
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void* self, void* actor, const struct Vector3* v, int a, int b, unsigned int c, unsigned int d);
}
extern "C" {
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void* self, void* actor, int a, int b, void* v, int c);
}
extern "C" {
extern void func_ov070_02121310(void* c);
}

extern struct Matrix4x3 IDENTITY_MATRIX4X3;

int daKrpa_c::InitResources()
{
    struct Vector3 v;
    void* bmd;
    int t;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov070_02123698);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, bmd, 1, 1);
    if (!_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel))
        return 0;

    v.x = 0;
    v.y = -0x32000;
    v.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        ((char*)this) + 0x160, ((char*)this), &v, 0x32000, 0x64000, 0x200002, 0x8000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        ((char*)this) + 0x1a0, ((char*)this), 0x32000, 0x32000, 0, 0);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    func_ov070_02121880(((char*)this), 0);

    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    *(struct Matrix4x3*)((char*)&mMatrix) = IDENTITY_MATRIX4X3;

    dBgCh_Gnd ground;
    ground.SetObjAndPos(*(Vector3 *)&mPosX, this);
    if (ground.DetectClsn())
        t = (mPosY - ground.clsnY) + 0x1e000;
    else
        t = 0x1f4000;
    mHeightAboveGnd = t;
    func_ov070_02121310(((char*)this));
    return 1;
}
