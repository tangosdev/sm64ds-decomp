//cpp
// @symbol _ZN17BigMovingIceBlock16CleanupResourcesEv
/* BigMovingIceBlock::CleanupResources -- vtable slot 3. Takes the mesh collider
 * out of the collision world, then releases the block's two shared files. */
#include "BigMovingIceBlock.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr *data_ov056_02113314[];
}

int BigMovingIceBlock::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov056_02113314[0]->Release();
    data_ov056_02113314[1]->Release();
    return 1;
}
