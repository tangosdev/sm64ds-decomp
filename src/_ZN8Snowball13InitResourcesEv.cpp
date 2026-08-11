//cpp
// @symbol _ZN8Snowball13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Snowball.h"
extern "C" void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *file, int a, int b);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void *thiz);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *thiz, void *actor, int r, int h, unsigned int a, unsigned int b);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *thiz, void *actor, int r, int h, void *v, int b);
extern "C" char data_ov081_02128d90;

int Snowball::InitResources()
{
    void *f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov081_02128d90);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x300, f, 1, -1) == 0) return 0;

    _ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel);

    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char *)this) + 0x110, ((char *)this), 0x1e000, 0x1e000, 0x200004, 0x40010);

    unk_37c = mPosX;
    unk_380 = mPosY;
    unk_384 = mPosZ;
    *(int*)((char *)&mPosY) += 0x32000;
    mAngleY = mPrevAngleY;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x144, ((char *)this), 0x14000, 0x14000, 0, 0);

    func_ov081_021261d4(((char *)this), &data_ov081_02128eb4);
    return 1;
}
