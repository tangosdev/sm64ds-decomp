//cpp
// @symbol _ZN4Flag16CleanupResourcesEv
#include "Flag.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov009_02113eb8;
extern SharedFilePtr data_ov009_02113eb0;

int Flag::CleanupResources()
{
    data_ov009_02113eb8.Release();
    data_ov009_02113eb0.Release();
    return 1;
}
