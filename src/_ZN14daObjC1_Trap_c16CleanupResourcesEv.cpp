//cpp
// @symbol _ZN14daObjC1_Trap_c16CleanupResourcesEv
/* daObjC1_Trap_c::CleanupResources -- vtable slot 3, ov010 0x02111554. Attributed by
 * the vtable: _ZTV14daObjC1_Trap_c carries this address at slot 3, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043bf0. */
#include "daObjC1_Trap_c.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov010_02112d08;
extern SharedFilePtr data_ov010_02112d00;
}

int daObjC1_Trap_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    if ((param1 & 0xff) != 0xff) {
        data_ov010_02112d08.Release();
        data_ov010_02112d00.Release();
    }
    return 1;
}
