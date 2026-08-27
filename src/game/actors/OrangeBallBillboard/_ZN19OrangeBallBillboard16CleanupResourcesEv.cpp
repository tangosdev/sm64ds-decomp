//cpp
// @symbol _ZN19OrangeBallBillboard16CleanupResourcesEv

#include "OrangeBallBillboard.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov044_02111680;

int OrangeBallBillboard::CleanupResources()
{
    data_ov044_02111680.Release();
    return 1;
}
