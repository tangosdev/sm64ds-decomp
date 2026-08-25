//cpp
// @symbol _ZN8daKpFr_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daKpFr_c.h"
extern "C" {
extern int _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* thiz, void* actor, int fix12, int t, unsigned int a, unsigned int b);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, int fix12, int t, void* vec, int last);
extern int IDENTITY_MATRIX4X3[];
}

int daKpFr_c::InitResources()
{
    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this) + 0xfc, ((char*)this), 0x37000, 0x78000, 0x200002, 0x8000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x130, ((char*)this), 0x32000, 0x32000, 0, 0);
    mVertAccel = -0x400;
    mTerminalVelocity = -0x5000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    func_ov070_02122044(((char*)this), 0);
    *(struct Matrix4x3*)((char*)&mMatrix) = *(struct Matrix4x3*)IDENTITY_MATRIX4X3;
    return 1;
}
