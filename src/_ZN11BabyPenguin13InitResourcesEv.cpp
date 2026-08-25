//cpp
// @symbol _ZN11BabyPenguin13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BabyPenguin.h"
extern "C" void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *file, int a, int b);
extern "C" void _ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void *thiz);
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *thiz, void *actor, int r, int h, unsigned int a, unsigned int b);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *thiz, void *actor, int r, int h, void *v, int b);
extern "C" void func_ov072_02121d50(void *c);

int BabyPenguin::InitResources()
{
    void *f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov072_02122cb4);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, f, 1, -1);
    int i;
    for (i = 0; i < 5; i++) {
        _ZN9Animation8LoadFileER13SharedFilePtr((void *)data_ov072_02122004[i]);
    }
    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0) return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0x160, ((char *)this), 0x28000, 0x50000, 0x800004, 0x9000);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mScaleX = 0x400;
    mScaleY = 0x400;
    mScaleZ = 0x400;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x194, ((char *)this), 0x32000, 0x32000, 0, 0);
    mEatingPlayer = 0;
    unk_360 = 0;
    func_ov072_02121d50(((char *)this));
    func_ov072_021210c4(((char *)this));
    return 1;
}
