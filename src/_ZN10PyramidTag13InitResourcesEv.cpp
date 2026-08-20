//cpp
// @symbol _ZN10PyramidTag13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "PyramidTag.h"
extern "C" {
int _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int, void*);
void _ZN7fBase_c18MarkForDestructionEv(void*);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void*, void*, int, int, unsigned int, unsigned int);
}

int PyramidTag::InitResources()
{
    int a = _ZN8dActor_c15FindWithActorIDEjPS_(0x55, 0);
    if(a == 0){ _ZN7fBase_c18MarkForDestructionEv(((char*)this)); return 1; }
    unk_108 = *(int*)(a+4);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this)+0xd4, ((char*)this), 0x7d000, 0x28000, 2, 0x400000);
    return 1;
}
