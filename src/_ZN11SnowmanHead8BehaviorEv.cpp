//cpp
// @symbol _ZN11SnowmanHead8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "SnowmanHead.h"

int SnowmanHead::Behavior()
{
    CallStateBehavior();
    mCylinder.Clear();
    mCylinder.Update();
    UpdateModel();
    return 1;
}
