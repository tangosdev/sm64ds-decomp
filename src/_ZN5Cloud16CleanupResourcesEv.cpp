//cpp
// @symbol _ZN5Cloud16CleanupResourcesEv

#include "Cloud.h"
#include "SharedFilePtr.h"

extern int data_ov039_021118e4[];

int Cloud::CleanupResources()
{
    ((SharedFilePtr *)data_ov039_021118e4)->Release();
    return 1;
}
