//cpp
// @symbol _ZN12daStarGate_c16CleanupResourcesEv

#include "SharedFilePtr.h"
#include "daStarGate_c.h"

extern SharedFilePtr data_ov100_02148934;

int daStarGate_c::CleanupResources()
{
    data_ov100_02148934.Release();
    return 1;
}
