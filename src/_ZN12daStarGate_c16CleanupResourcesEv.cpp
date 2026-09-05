//cpp
// @symbol _ZN8StarDoor16CleanupResourcesEv

#include "SharedFilePtr.h"
#include "StarDoor.h"

extern SharedFilePtr data_ov100_02148934;

int StarDoor::CleanupResources()
{
    data_ov100_02148934.Release();
    return 1;
}
