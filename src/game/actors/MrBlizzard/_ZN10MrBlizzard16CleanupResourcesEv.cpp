//cpp
// @symbol _ZN10MrBlizzard16CleanupResourcesEv

#include "MrBlizzard.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov081_02128d90;
extern SharedFilePtr data_ov081_02128db0;
extern SharedFilePtr data_ov081_02128d98;
extern SharedFilePtr data_ov081_02128db8;
extern SharedFilePtr data_ov081_02128da8;
extern SharedFilePtr data_ov081_02128d88;
extern SharedFilePtr data_ov081_02128da0;
}

int MrBlizzard::CleanupResources()
{
    data_ov081_02128d90.Release();
    data_ov081_02128db0.Release();
    data_ov081_02128d98.Release();
    data_ov081_02128db8.Release();
    data_ov081_02128da8.Release();
    data_ov081_02128d88.Release();
    data_ov081_02128da0.Release();
    return 1;
}
