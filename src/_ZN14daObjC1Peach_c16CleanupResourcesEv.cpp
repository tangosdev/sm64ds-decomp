//cpp
// @symbol _ZN14daObjC1Peach_c16CleanupResourcesEv
#include "daObjC1Peach_c.h"
#include "SharedFilePtr.h"

extern "C" SharedFilePtr data_ov010_02112d64;

int daObjC1Peach_c::CleanupResources()
{
    data_ov010_02112d64.Release();
    return 1;
}
