//cpp
// @symbol _ZN9Spindrift16CleanupResourcesEv

#include "SharedFilePtr.h"
#include "Spindrift.h"

extern "C" {
extern SharedFilePtr data_ov081_02128d60;
extern SharedFilePtr data_ov081_02128d68;
}

int Spindrift::CleanupResources()
{
    data_ov081_02128d60.Release();
    data_ov081_02128d68.Release();
    return 1;
}
