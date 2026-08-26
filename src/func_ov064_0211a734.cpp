//cpp
// @symbol _ZN13TreasureChest17CallStateBehaviorEv
#include "TreasureChest.h"

extern "C" TreasureChest::StateFunc data_ov064_0211c98c[][2];

void TreasureChest::CallStateBehavior()
{
    s32 index = mState;
    (this->*data_ov064_0211c98c[index][1])();
}
