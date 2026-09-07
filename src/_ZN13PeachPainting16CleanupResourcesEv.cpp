//cpp
// @symbol _ZN13PeachPainting16CleanupResourcesEv
#include "PeachPainting.h"
#include "SharedFilePtr.h"

extern "C" SharedFilePtr data_ov010_02112d64;

int PeachPainting::CleanupResources()
{
    data_ov010_02112d64.Release();
    return 1;
}
