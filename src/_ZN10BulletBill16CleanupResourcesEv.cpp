//cpp
// @symbol _ZN10BulletBill16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the two model files InitResources claimed -- one per Model member.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "BulletBill.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov079_02128300;
extern SharedFilePtr data_ov079_021282f0;
}

int BulletBill::CleanupResources()
{
    data_ov079_02128300.Release();
    data_ov079_021282f0.Release();
    return 1;
}
