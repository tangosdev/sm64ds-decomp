//cpp
// @symbol _ZN13TreasureChest10InitState1Ev
#include "TreasureChest.h"

void TreasureChest::InitState1()
{
    if (mIsLastChest != 0)
        mStateTimer = 0x2d;

    mFlags &= ~1;
}
