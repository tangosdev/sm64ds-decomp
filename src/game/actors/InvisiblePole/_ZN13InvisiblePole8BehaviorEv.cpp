//cpp
// @symbol _ZN13InvisiblePole8BehaviorEv
/* Vtable slot 6. Rebuild the climbing cylinder every frame. */
#include "InvisiblePole.h"

s32 InvisiblePole::Behavior()
{
    mClsn.Clear();
    mClsn.Update();
    return 1;
}
