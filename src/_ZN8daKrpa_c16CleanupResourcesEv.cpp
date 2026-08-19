//cpp
// @symbol _ZN8daKrpa_c16CleanupResourcesEv

#include "daKrpa_c.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov070_02123698;

int daKrpa_c::CleanupResources()
{
    data_ov070_02123698.Release();
    return 1;
}
