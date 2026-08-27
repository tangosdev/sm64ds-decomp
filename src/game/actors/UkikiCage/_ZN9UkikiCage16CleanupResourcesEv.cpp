//cpp
// @symbol _ZN9UkikiCage16CleanupResourcesEv
/* UkikiCage::CleanupResources -- vtable slot 3, ov030 0x0211130c. Attributed by
 * the vtable: _ZTV9UkikiCage carries this address at slot 3, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043bf0. Takes the inherited
 * mesh collider out of the collision world, then releases two shared files. */
#include "UkikiCage.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov030_02115c80[];
extern int data_ov030_02115c88[];
}

int UkikiCage::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov030_02115c88);
    _ZN13SharedFilePtr7ReleaseEv(data_ov030_02115c80);
    return 1;
}
