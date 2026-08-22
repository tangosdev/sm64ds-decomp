//cpp
// @symbol _ZN15daObjPathLift_c16CleanupResourcesEv
/* daObjPathLift_c::CleanupResources -- vtable slot 3. Takes the inherited mesh
 * collider out of the collision world if it is still in it, then releases the
 * three shared files the lift holds.
 *
 * THE FIRST FILE LIVES IN A DIFFERENT OVERLAY: the retail relocation for that
 * site resolves into ov002, the other two into ov100. They were all spelled
 * G0/G1/G2 in the legacy C form, which hid the crossing. */
#include "daObjPathLift_c.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210d9f0[];
extern int data_ov100_02148a54;
extern int data_ov100_02148a5c;
}

int daObjPathLift_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9f0);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov100_02148a54);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov100_02148a5c);
    return 1;
}
