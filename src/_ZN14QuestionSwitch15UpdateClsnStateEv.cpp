//cpp
// @symbol _ZN19daObjHatenaSwitch_c15UpdateClsnStateEv
/* Inferred descriptive name. The routine is called only by
 * daObjHatenaSwitch_c::Behavior and operates only on this object's active
 * moving-mesh collider. */
#include "QuestionSwitch.h"

int daObjHatenaSwitch_c::UpdateClsnState()
{
    int shouldDisable = (int)((mFlags & 8) != 0);

    if (shouldDisable) {
        if (mActiveMeshCollider->IsEnabled())
            mActiveMeshCollider->Disable();
        return 1;
    }

    if (!mActiveMeshCollider->IsEnabled())
        mActiveMeshCollider->Enable(this);
    return 0;
}
