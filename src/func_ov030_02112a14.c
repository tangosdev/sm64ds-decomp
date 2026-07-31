// @symbol func_ov030_02112a14
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjHmMaruta_c.h"
// @emits daObjHmMaruta_c_AfterClsn
/* recovered: renamed to Class_Method */
/* daObjHmMaruta_c::AfterClsn - recovered from vtable slot identity */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *m, void *f, int i, int fix, unsigned int j);
extern void _ZN7PathPtr6FromIDEj(void *p, unsigned int id);
struct G { void *a; void *b; };
extern struct G data_ov030_02115d18;
int daObjHmMaruta_c_AfterClsn(char *c) {
    struct daObjHmMaruta_c *self = (struct daObjHmMaruta_c *)(void *)c;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, data_ov030_02115d18.b, 0, 0x1000, 0);
    self->unk_130 = 0x1000;
    _ZN7PathPtr6FromIDEj(c+0x398, *(int*)(c+8) & 0xff);
    self->unk_3a0 = 1;
    self->unk_3c7 = 0;
    self->unk_098 = 0x6000;
    self->unk_3b4 = 8;
    return 1;
}
