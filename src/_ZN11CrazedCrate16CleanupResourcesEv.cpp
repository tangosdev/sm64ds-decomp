//cpp
// @symbol _ZN11CrazedCrate16CleanupResourcesEv
/* recovered: real C++ method */
/* CrazedCrate::CleanupResources() -- vtable slot 3. One shared file handle to
 * give back. */
#include "CrazedCrate.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov080_02128468[];
}

s32 CrazedCrate::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov080_02128468);
    return 1;
}
