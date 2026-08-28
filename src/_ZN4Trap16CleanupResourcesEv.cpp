//cpp
// @symbol _ZN4Trap16CleanupResourcesEv
/* Trap::CleanupResources -- vtable slot 3, ov010 0x02111554. Attributed by
 * the vtable: _ZTV4Trap carries this address at slot 3, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043bf0. */
#include "Trap.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov010_02112d08;
extern SharedFilePtr data_ov010_02112d00;
}

int Trap::CleanupResources()
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
