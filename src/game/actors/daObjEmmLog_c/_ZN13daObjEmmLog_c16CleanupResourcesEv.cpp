//cpp
// @symbol _ZN13daObjEmmLog_c16CleanupResourcesEv
/* daObjEmmLog_c::CleanupResources -- vtable slot 3. Takes the inherited mesh
 * collider out of the collision world if it is still in it, then releases the
 * two shared files the log holds. */
#include "daObjEmmLog_c.h"

extern "C" {
int _ZN13SharedFilePtr7ReleaseEv(void *);
extern int *data_ov052_021124d4[];
}

int daObjEmmLog_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov052_021124d4[0]);
    _ZN13SharedFilePtr7ReleaseEv(data_ov052_021124d4[1]);
    return 1;
}
