// @symbol func_ov036_021112f0
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjRcBuranko_c.h"
// @emits daObjRcBuranko_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjRcBuranko_c::Behavior - recovered from vtable slot identity */
extern int func_ov036_0211123c(char *t);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char *c, int a, int b);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(char *c);

int daObjRcBuranko_c_Behavior(char *c)
{
    struct daObjRcBuranko_c *self = (struct daObjRcBuranko_c *)(void *)c;
    if (self->unk_090 < 0) {
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
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
