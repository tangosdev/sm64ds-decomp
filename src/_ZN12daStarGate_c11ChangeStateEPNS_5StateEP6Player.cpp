//cpp
// @symbol _ZN12daStarGate_c11ChangeStateEPNS_5StateEP6Player
#include "daStarGate_c.h"

bool daStarGate_c::ChangeState(State *nextState, Player *player)
{
    mState = nextState;
    if (mState->init == 0)
        return true;
    return (this->*(mState->init))(player);
}
