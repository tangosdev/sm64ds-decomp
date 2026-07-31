// @symbol func_ov052_021112ac
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjEmmLog_c.h"
// @emits daObjEmmLog_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjEmmLog_c::Behavior - recovered from vtable slot identity */
typedef int Fix12;
extern void func_020393a4(int *p, int v);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, Fix12 a, int b);
extern short data_02082214[];
int daObjEmmLog_c_Behavior(char *c)
{
    struct daObjEmmLog_c *self = (struct daObjEmmLog_c *)(void *)c;
    func_020393a4((int*)(c + 0x124), 0x600000);
    short *p = (short*)(c + 0x300);
    int idx = (unsigned short)p[0xf] >> 4;
    int s = *(short*)((char*)data_02082214 + (idx << 2));
    int m = (int)(((long long)self->unk_324 * s + 0x800) >> 12);
    self->unk_060 = self->unk_320 + m;
    short *q = (short*)(((int)c + 0x31e) & 0xFFFFFFFFFFFFFFFF);
    *q = (short)(*q + 0x200);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x600000, 0);
    return 1;
}
