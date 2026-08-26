//cpp
// @symbol _ZN8IceSheet8BehaviorEv
#include "IceSheet.h"

int IceSheet::Behavior()
{
    if (!mMeshCollider.IsEnabled())
        mMeshCollider.Enable(this);
    return 1;
}
