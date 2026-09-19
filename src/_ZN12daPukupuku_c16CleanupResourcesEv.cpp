//cpp
// @symbol _ZN12daPukupuku_c16CleanupResourcesEv

#include "daPukupuku_c.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov090_02134564;
extern SharedFilePtr data_ov090_0213455c;
}

int daPukupuku_c::CleanupResources()
{
    data_ov090_02134564.Release();
    data_ov090_0213455c.Release();
    return 1;
}
