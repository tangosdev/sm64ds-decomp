//cpp
// @symbol _ZN12daBgSnmHed_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "daBgSnmHed_c.h"

int daBgSnmHed_c::Behavior()
{
    CallStateBehavior();
    mCylinder.Clear();
    mCylinder.Update();
    UpdateModel();
    return 1;
}
