//cpp
// @symbol _ZN12daDossyCap_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "daDossyCap_c.h"
extern "C" {
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *m, void *f, int a, int b);
extern void func_ov001_020ab228(void *a, void *b, int c, int d, int e);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *clsn, void *actor, int a, int b, unsigned int c, unsigned int d);
}
struct G { int w[2]; };
extern struct G data_ov002_0210d9c0;

int daDossyCap_c::InitResources()
{
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xf0, (void *)data_ov002_0210d9c0.w[1], 1, -1);
    func_ov001_020ab228(((char *)this) + 0xd4, ((char *)this), 2, 2, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0x140, ((char *)this), 0x96000, 0x96000, 0x100002, 0x8000);
    mCarrier = 0;
    return 1;
}
