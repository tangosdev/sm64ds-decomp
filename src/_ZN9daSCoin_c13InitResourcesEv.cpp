//cpp
// @symbol _ZN9daSCoin_c13InitResourcesEv
/* daSCoin_c::InitResources -- vtable slot 0. Real C++ method over the shared
 * header, named members. */
#include "daSCoin_c.h"
extern "C" {
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, void* actor, int radius, int height, unsigned int flags, unsigned int vulnFlags);
void _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern int data_ov002_0210d9a8;
}
s32 daSCoin_c::InitResources(){
    char* c = (char*)this;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(c + 0xd4, c, 0x64000, 0x40000, 0x800002, 0);
    unk_10d = *(unsigned int*)(c + 8) & 0xf;
    mGroupId = (*(unsigned int*)(c + 8) >> 8) & 0xf;
    mGroupRole = 0;
    mLeaderUniqueID = 0;
    mCollectedCount = 0;
    mDeathTimer = 0;
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9a8);
    return 1;
}
