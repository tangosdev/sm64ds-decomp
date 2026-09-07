//cpp
// @symbol _ZN13TreasureChest8SetStateEi
#include "TreasureChest.h"

extern "C" TreasureChest::StateFunc data_ov064_0211c98c[][2];

void TreasureChest::SetState(s32 state)
{
    mState = state;
    s32 index = mState;
    (this->*data_ov064_0211c98c[index][0])();
}
