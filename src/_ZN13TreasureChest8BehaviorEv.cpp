//cpp
// @symbol _ZN13TreasureChest8BehaviorEv
#include "TreasureChest.h"

int TreasureChest::Behavior()
{
    CallStateBehavior();
    mCylinder.Clear();
    mCylinder.Update();
    return 1;
}
