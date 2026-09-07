//cpp
// @symbol _ZN10CheepCheep16CleanupResourcesEv

#include "CheepCheep.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov090_02134564;
extern SharedFilePtr data_ov090_0213455c;
}

int CheepCheep::CleanupResources()
{
    data_ov090_02134564.Release();
    data_ov090_0213455c.Release();
    return 1;
}
