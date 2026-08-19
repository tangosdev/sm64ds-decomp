//cpp
// @symbol _ZN10FlameChomp16CleanupResourcesEv

#include "FlameChomp.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov070_02123698;

int FlameChomp::CleanupResources()
{
    data_ov070_02123698.Release();
    return 1;
}
