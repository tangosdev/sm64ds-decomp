//cpp
// @symbol _ZN5Shark16CleanupResourcesEv

#include "SharedFilePtr.h"
#include "Shark.h"

extern "C" {
extern SharedFilePtr data_ov090_021345a4;
extern SharedFilePtr data_ov090_021345ac;
}

int Shark::CleanupResources()
{
    data_ov090_021345a4.Release();
    data_ov090_021345ac.Release();
    return 1;
}
