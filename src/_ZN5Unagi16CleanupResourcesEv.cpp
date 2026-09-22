//cpp
// @symbol _ZN5Unagi16CleanupResourcesEv

#include "SharedFilePtr.h"
#include "Unagi.h"

extern "C" {
extern SharedFilePtr data_ov016_02114d38;
extern SharedFilePtr data_ov016_02114d20;
extern SharedFilePtr data_ov016_02114d30;
extern SharedFilePtr data_ov016_02114d28;
}

s32 Unagi::CleanupResources()
{
    data_ov016_02114d38.Release();
    data_ov016_02114d20.Release();
    data_ov016_02114d30.Release();
    data_ov016_02114d28.Release();
    return 1;
}
