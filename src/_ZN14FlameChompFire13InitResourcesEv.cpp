//cpp
// @symbol _ZN14FlameChompFire13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FlameChompFire.h"
struct M48 { int w[12]; };
extern "C" {
extern int _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* thiz, void* actor, int fix12, int t, unsigned int a, unsigned int b);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, int fix12, int t, void* vec, int last);
extern int data_02082128[];
}

int FlameChompFire::InitResources()
{
    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char*)this) + 0xfc, ((char*)this), 0x37000, 0x78000, 0x200002, 0x8000);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x130, ((char*)this), 0x32000, 0x32000, 0, 0);
    unk_09c = -0x400;
    unk_0a0 = -0x5000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    func_ov070_02122044(((char*)this), 0);
    *(struct M48*)((char*)&unk_2ec) = *(struct M48*)data_02082128;
    return 1;
}
