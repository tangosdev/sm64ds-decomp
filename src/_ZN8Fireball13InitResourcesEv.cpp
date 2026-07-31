//cpp
// @symbol _ZN8Fireball13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Fireball.h"
extern int _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* thiz, void* actor, int fix12, int t, unsigned int a, unsigned int b);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, int fix12, int t, void* vec, int last);
extern void _ZN12WithMeshClsn19StartDetectingWaterEv(void* thiz);

int Fireball::InitResources()
{
    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char*)this) + 0x110, ((char*)this), 0x28000, 0x50000, 0x200002, 0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x144, ((char*)this), 0x32000, 0x32000, 0, 0);
    _ZN12WithMeshClsn19StartDetectingWaterEv((char*)&mWithMeshClsn);
    unk_100 = 0;
    unk_36a = 0;
    unk_360 = 0;
    unk_364 = 0x5dc000;
    unk_36d = unk_008 & 7;
    unk_370 = 0;
    unk_374 = 0;
    {
        unsigned char v = unk_36d;
        if (v != 0 && v != 4) {
            *(unsigned int*)(((int)((char*)&unk_12c))) |= 0x8000;
        }
    }
    return 1;
}
