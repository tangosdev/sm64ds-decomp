//cpp
// @symbol _ZN9daSCoin_c13InitResourcesEv
/* recovered: renamed to Class_Method, RTTI class fields named. Was
 * func_ov002_020f07dc, vtable slot 0. */
#include "daSCoin_c.h"
/* daSCoin_c::InitResources - recovered from vtable slot identity */
extern "C" {
void _ZN18MovingCylinderClsn4InitEP8dActor_c5Fix12IiES3_jj(void* self, void* actor, int radius, int height, unsigned int flags, unsigned int vulnFlags);
void _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern int data_ov002_0210d9a8;

int _ZN9daSCoin_c13InitResourcesEv(char* c){
    struct daSCoin_c *self = (struct daSCoin_c *)(void *)c;
    _ZN18MovingCylinderClsn4InitEP8dActor_c5Fix12IiES3_jj(c + 0xd4, c, 0x64000, 0x40000, 0x800002, 0);
    self->unk_10d = *(unsigned int*)(c + 8) & 0xf;
    self->unk_10e = (*(unsigned int*)(c + 8) >> 8) & 0xf;
    self->unk_10f = 0;
    self->unk_108 = 0;
    self->unk_110 = 0;
    self->unk_113 = 0;
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9a8);
    return 1;
}
}
