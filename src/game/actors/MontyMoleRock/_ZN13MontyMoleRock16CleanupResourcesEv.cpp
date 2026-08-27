//cpp
// @symbol _ZN13MontyMoleRock16CleanupResourcesEv

#include "MontyMoleRock.h"
#include "SharedFilePtr.h"

extern "C" SharedFilePtr data_ov080_021283c8;

s32 MontyMoleRock::CleanupResources()
{
    data_ov080_021283c8.Release();
    return 1;
}
