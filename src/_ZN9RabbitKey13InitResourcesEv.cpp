//cpp
// @symbol _ZN9RabbitKey13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RabbitKey.h"
extern "C" {
void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
void _ZN11ShadowModel12InitCylinderEv(void* self);
void func_ov085_0212d268(void* c, void* p);
}

int RabbitKey::InitResources()
{
    void* f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov085_021305d8);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x110, f, 1, -1) == 0)
        return 0;
    _ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel);
    unk_19c = unk_008 & 0xff;
    unk_0a0 = -0x3c000;
    unk_190 = 0;
    func_ov085_0212d268(((char*)this), &data_ov085_0213071c);
    return 1;
}
