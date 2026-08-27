//cpp
// @symbol _ZN11daBgSnwmn_c16CleanupResourcesEv
/* recovered: real C++ method */
/* daBgSnwmn_c::CleanupResources() -- vtable slot 3. Drops the three shared
 * file handles InitResources took: two models and the texture sequence. */
#include "daBgSnwmn_c.h"
#include "decl_common.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov072_02122c48[];
}

s32 daBgSnwmn_c::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov072_02122c48);
    _ZN13SharedFilePtr7ReleaseEv(data_ov072_02122c40);
    _ZN13SharedFilePtr7ReleaseEv(data_ov072_02122c50);
    return 1;
}
