//cpp
// @symbol _ZN14QuestionSwitch15UpdateClsnStateEv
/* Inferred descriptive name. The routine is called only by
 * QuestionSwitch::Behavior and operates only on this object's active
 * moving-mesh collider. */
#include "QuestionSwitch.h"

int QuestionSwitch::UpdateClsnState()
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
