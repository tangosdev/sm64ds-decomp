//cpp
// @symbol _ZN11daObjTbox_c8BehaviorEv
#include "daObjTbox_c.h"

int daObjTbox_c::Behavior()
{
    CallStateBehavior();
    mCylinder.Clear();
    mCylinder.Update();
    return 1;
}
