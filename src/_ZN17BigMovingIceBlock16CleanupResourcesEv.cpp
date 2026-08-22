//cpp
// @symbol _ZN17BigMovingIceBlock16CleanupResourcesEv
/* BigMovingIceBlock::CleanupResources -- vtable slot 3. Takes the mesh collider
 * out of the collision world, then releases the block's two shared files. */
#include "BigMovingIceBlock.h"

extern "C" {
int _ZN13SharedFilePtr7ReleaseEv(void *);
extern int *data_ov056_02113314[];
}

int BigMovingIceBlock::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov056_02113314[0]);
    _ZN13SharedFilePtr7ReleaseEv(data_ov056_02113314[1]);
    return 1;
}
