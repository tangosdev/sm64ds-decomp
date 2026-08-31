//cpp
// @symbol _ZN19daPropeller_Heyho_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "daPropeller_Heyho_c.h"
#include "SharedFilePtr.h"
typedef struct PMF PMF;
extern SharedFilePtr data_ov070_02123530;
extern SharedFilePtr data_ov070_02123520;
extern SharedFilePtr data_ov070_02123518;
extern SharedFilePtr data_ov070_02123510;
extern SharedFilePtr data_ov070_02123528;
extern SharedFilePtr data_ov070_02123508;
extern SharedFilePtr data_ov070_02123500;
extern PMF data_ov070_0212359c;
extern "C" {
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void* self);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, dActor_c* a, int r, int h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, int r, int h, Vector3_16* p, Vector3_16* q);
extern int FlyGuy_ChangeState(void* c, PMF* p);
}

int daPropeller_Heyho_c::InitResources()
{
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0x300, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov070_02123530), 1, -1);
    _ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov070_02123520);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov070_02123518);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov070_02123510);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov070_02123528);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov070_02123508);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov070_02123500);
    unk_3e0 = param1 & 0xff;
    if (unk_3e0 == 0xff) unk_3e0 = 0;
    mTerminalVelocity = -0x1e000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this)+0x110, (dActor_c*)((char*)this), 0x3c000, 0x32000, 0x200000, 0x7eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this)+0x144, (dActor_c*)((char*)this), 0x50000, 0x3c000, 0, 0);
    unk_108 = 1;
    unk_10a = 1;
    unk_3c0 = mPosX;
    unk_3c4 = mPosY;
    unk_3c8 = mPosZ;
    FlyGuy_ChangeState(((char*)this), &data_ov070_0212359c);
    return 1;
}
