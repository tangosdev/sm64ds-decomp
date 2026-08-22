//cpp
// @symbol _ZN7BooCage13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_dBgCh_Actr.h"
/* recovered: named members + shared header, real C++ method */
#include "BooCage.h"
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int a, int b, unsigned int c, unsigned int d);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v, int c);
}
extern void *data_ov063_0211edec;

int BooCage::InitResources()
{
    void *m = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov063_0211edec);
    if (!_ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x300, m, 1, -1)) return 0;
    if (!_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel)) return 0;
    mVertAccel = -0x4000;
    mTerminalVelocity = -0x46000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0x110, ((char *)this), 0x64000, 0x64000, 0x200004, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x144, ((char *)this), 0x64000, 0x64000, 0, 0);
    _ZN10dBgCh_Actr13SetLimMovFlagEv((char *)&mWithMeshClsn);
    mParticleID = 0;
    mSoundTimer = 0;
    unk_37e = 0;
    return 1;
}
