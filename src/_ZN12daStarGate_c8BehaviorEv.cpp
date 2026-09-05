//cpp
// @symbol _ZN12daStarGate_c8BehaviorEv
#include "daStarGate_c.h"

int daStarGate_c::Behavior()
{
    Player *player = CalculateRelativePlayerPos();
    if (mState->main != 0)
        (this->*(mState->main))(player);
    return 1;
}
