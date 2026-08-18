//cpp
// @symbol _ZN14UnchainedChomp16CleanupResourcesEv

#include "SharedFilePtr.h"
#include "UnchainedChomp.h"

extern "C" {
void UnloadSilverStarAndNumber();
extern SharedFilePtr data_ov002_0211092c;
extern SharedFilePtr data_ov100_021486bc;
extern SharedFilePtr data_ov100_021486a4;
extern SharedFilePtr data_ov100_021486ac;
extern SharedFilePtr data_ov100_021486b4;
}

int UnchainedChomp::CleanupResources()
{
    data_ov002_0211092c.Release();
    data_ov100_021486bc.Release();
    data_ov100_021486a4.Release();
    data_ov100_021486ac.Release();
    data_ov100_021486b4.Release();
    UnloadSilverStarAndNumber();
    return 1;
}
