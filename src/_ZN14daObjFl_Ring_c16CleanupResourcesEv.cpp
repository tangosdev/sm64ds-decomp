//cpp
// @symbol _ZN14daObjFl_Ring_c16CleanupResourcesEv
#include "daObjFl_Ring_c.h"
#include "SharedFilePtr.h"

/* daObjFl_Ring_c::CleanupResources -- vtable slot 3, ov022 0x0211123c.
 *
 * mMeshCollider is dBgActor_c's own field. data_ov022_02113cc8 is the same
 * file-table InitResources (slot 0) fills; its first two words are the
 * model/kcl SharedFilePtr this function releases -- same as the
 * pre-migration recovery's `((int*)&data)[0]`/`[1]`. */
extern "C" void *data_ov022_02113cc8;

int daObjFl_Ring_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    ((SharedFilePtr *)((void **)&data_ov022_02113cc8)[0])->Release();
    ((SharedFilePtr *)((void **)&data_ov022_02113cc8)[1])->Release();
    return 1;
}
