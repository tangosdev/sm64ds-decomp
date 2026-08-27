//cpp
// @symbol _ZN16SwingingPlatform8BehaviorEv
#include "SwingingPlatform.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjRcBuranko_c.h"
// recovered name: daObjRcBuranko_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjRcBuranko_c::Behavior - recovered from vtable slot identity */
extern "C" {
extern int func_ov036_0211123c(char *t);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(char *c, int a, int b);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(char *c);
}

s32 SwingingPlatform::Behavior() {
    char * c = (char *)this;
    struct daObjRcBuranko_c *self = (struct daObjRcBuranko_c *)(void *)c;
    if (self->mAngleZ < 0) {
        short *p = (short *)(((int)c + 0x31e));
        *p = *p + 4;
    } else {
        short *p = (short *)(((int)c + 0x31e));
        *p = *p - 4;
    }
    {
        short *q = (short *)(((int)c + 0x90));
        *q = *q + ((short *)(c + 0x300))[15];
    }
    func_ov036_0211123c(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    return 1;
}
