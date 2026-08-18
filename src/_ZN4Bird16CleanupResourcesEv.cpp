//cpp
// @symbol _ZN4Bird16CleanupResourcesEv

#include "Bird.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov009_02113c28;
extern SharedFilePtr data_ov009_02113c20;

int Bird::CleanupResources()
{
    data_ov009_02113c28.Release();
    data_ov009_02113c20.Release();
    return 1;
}
