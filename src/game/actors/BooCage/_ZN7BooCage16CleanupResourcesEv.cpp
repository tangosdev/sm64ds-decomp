//cpp
// @symbol _ZN7BooCage16CleanupResourcesEv

#include "BooCage.h"
#include "SharedFilePtr.h"

extern "C" SharedFilePtr data_ov063_0211edec;

int BooCage::CleanupResources()
{
    data_ov063_0211edec.Release();
    return 1;
}
