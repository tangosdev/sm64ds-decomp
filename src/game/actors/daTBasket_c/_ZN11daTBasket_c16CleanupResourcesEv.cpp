//cpp
// @symbol _ZN11daTBasket_c16CleanupResourcesEv

#include "daTBasket_c.h"
#include "SharedFilePtr.h"

extern "C" SharedFilePtr data_ov063_0211edec;

int daTBasket_c::CleanupResources()
{
    data_ov063_0211edec.Release();
    return 1;
}
