//cpp
// @symbol _ZN13TreasureChest10InitState2Ev
#include "TreasureChest.h"

void TreasureChest::InitState2()
{
    if (mIsLastChest == 0)
        mFlags |= 1;
}
