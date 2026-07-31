// @symbol func_ov002_020f07dc
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daSCoin_c.h"
// @emits daSCoin_c_InitResources
/* recovered: renamed to Class_Method */
/* daSCoin_c::InitResources - recovered from vtable slot identity */
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, void* actor, int radius, int height, unsigned int flags, unsigned int vulnFlags);
extern void _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern int data_ov002_0210d9a8;
int daSCoin_c_InitResources(char* c){
    struct daSCoin_c *self = (struct daSCoin_c *)(void *)c;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0xd4, c, 0x64000, 0x40000, 0x800002, 0);
    self->unk_10d = *(unsigned int*)(c + 8) & 0xf;
    self->unk_10e = (*(unsigned int*)(c + 8) >> 8) & 0xf;
    self->unk_10f = 0;
    self->unk_108 = 0;
    self->unk_110 = 0;
    self->unk_113 = 0;
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9a8);
    return 1;
}
