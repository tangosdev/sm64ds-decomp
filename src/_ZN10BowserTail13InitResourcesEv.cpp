//cpp
// @symbol _ZN10BowserTail13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "BowserTail.h"
extern "C" {
int _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int fix, int t, unsigned int e, unsigned int f);
}

int BowserTail::InitResources()
{
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0xd4, ((char *)this), 0x32000, 0x50000, 0x800000, 0x1000);
    return 1;
}
