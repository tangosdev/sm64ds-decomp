//cpp
// @symbol _ZN4Coin13OnTurnIntoEggER6Player
/* recovered: real C++ method, vtable slot 19. Declared to return int (base's
 * dActor_c::OnTurnIntoEgg signature), but -- like the .c file this replaces --
 * every path is a tail call and none of them sets r0 explicitly; the ROM
 * bytes end right after the last bl. */
#include "decl_common.h"
#include "Coin.h"

int Coin::OnTurnIntoEgg(Player &player)
{
    char *c = (char *)this;
    char *p = (char *)&player;
    int state = mCoinType;

    if (state == 1) {
        mPosY += 0x50000;
        func_ov002_020b16c4(c, p);
    } else if (state == 2) {
        func_ov002_020b1674(c, p);
    } else {
        func_ov002_020b1884(c, p);
    }
}
