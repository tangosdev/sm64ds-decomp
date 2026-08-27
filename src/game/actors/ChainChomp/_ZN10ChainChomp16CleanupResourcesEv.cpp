//cpp
// @symbol _ZN10ChainChomp16CleanupResourcesEv

#include "ChainChomp.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov014_02114968;
extern SharedFilePtr data_ov014_02114978;
extern SharedFilePtr data_ov014_02114980;
extern SharedFilePtr data_ov014_02114970;
}

int ChainChomp::CleanupResources()
{
    data_ov014_02114968.Release();
    data_ov014_02114978.Release();
    data_ov014_02114980.Release();
    data_ov014_02114970.Release();
    return 1;
}
