//cpp
// @symbol _ZN12daObjAbuku_c13InitResourcesEv
/* daObjAbuku_c::InitResources -- vtable slot 0. Real C++ method over the
 * shared header, named members. */
#include "daObjAbuku_c.h"
extern "C" {
int _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void*, void*, int, int, unsigned int, unsigned int);
}
s32 daObjAbuku_c::InitResources(){
    char* c = (char*)this;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(c+0xd4, c, 0x96000, 0x96000, 0x100002, 0);
    mLifeTimer = 0x12c;
    return 1;
}
