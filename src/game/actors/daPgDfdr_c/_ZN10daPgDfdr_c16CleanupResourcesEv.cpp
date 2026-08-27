//cpp
// @symbol _ZN10daPgDfdr_c16CleanupResourcesEv
/* daPgDfdr_c::CleanupResources -- vtable slot 3. Releases five shared files --
 * one, then the three-entry table, then another -- and takes the inherited mesh
 * collider out of the collision world in between, before the last release. The
 * order is the ROM's. */
#include "daPgDfdr_c.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern void *data_ov027_02112ca4[3];
extern void *data_ov027_02113c6c;
extern void *data_ov027_02113c7c;
extern void *data_ov027_02113c94;
}

s32 daPgDfdr_c::CleanupResources()
{
    int i;
    _ZN13SharedFilePtr7ReleaseEv(&data_ov027_02113c7c);
    for (i = 0; i < 3; i++) {
        _ZN13SharedFilePtr7ReleaseEv(data_ov027_02112ca4[i]);
    }
    _ZN13SharedFilePtr7ReleaseEv(&data_ov027_02113c94);
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    _ZN13SharedFilePtr7ReleaseEv(&data_ov027_02113c6c);
    return 1;
}
