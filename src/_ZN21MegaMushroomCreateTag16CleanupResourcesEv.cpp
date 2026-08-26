//cpp
// @symbol _ZN21MegaMushroomCreateTag16CleanupResourcesEv

#include "MegaMushroomCreateTag.h"
#include "SharedFilePtr.h"

extern "C" int data_ov002_0210da30[];

s32 MegaMushroomCreateTag::CleanupResources()
{
    ((SharedFilePtr *)data_ov002_0210da30)->Release();
    return 1;
}
