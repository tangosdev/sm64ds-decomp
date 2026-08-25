//cpp
// @symbol _ZN8daEyBm_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "common.h"
#include "daEyBm_c.h"
/* was `typedef int Fix12;` -- collides with the real Fix12<> template, which
   daEyBm_c.h now reaches via dActor_c.h. The typedef WAS int, so
   spelling it int below is byte-neutral. */
extern "C" {
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern int _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *actor, void *v, int r, int t1, unsigned int u1, unsigned int u2);
extern int _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v, int c);
extern void func_ov071_02121c6c(char *c);
}
extern void *data_ov071_021230b8;
extern struct Matrix4x3 IDENTITY_MATRIX4X3;

int daEyBm_c::InitResources()
{
    if (!_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel)) return 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(((char *)this) + 0xfc, ((char *)this), &data_ov071_021230b8, 0x23000, 0x3c000, 0x200002, 0x8000);
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x13c, ((char *)this), 0x19000, 0x19000, 0, 0);
    mVertAccel = 0;
    mTerminalVelocity = 0;
    mHorzSpeed = 0xa000;
    mLifeTimer = 0x96;
    *(struct Matrix4x3*)((char *)&mMatrix) = IDENTITY_MATRIX4X3;
    func_ov071_02121c6c(((char *)this));
    return 1;
}
