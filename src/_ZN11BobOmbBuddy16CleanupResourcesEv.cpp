//cpp
// @symbol _ZN11BobOmbBuddy16CleanupResourcesEv

#include "BobOmbBuddy.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov084_02130da4;
extern SharedFilePtr data_ov084_02130d9c;

int BobOmbBuddy::CleanupResources()
{
    data_ov084_02130da4.Release();
    data_ov084_02130d9c.Release();
    return 1;
}
