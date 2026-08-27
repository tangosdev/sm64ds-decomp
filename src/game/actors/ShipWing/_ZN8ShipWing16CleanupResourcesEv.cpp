//cpp
// @symbol _ZN8ShipWing16CleanupResourcesEv

#include "SharedFilePtr.h"
#include "ShipWing.h"

extern SharedFilePtr data_ov036_02114070;

int ShipWing::CleanupResources()
{
    data_ov036_02114070.Release();
    return 1;
}
