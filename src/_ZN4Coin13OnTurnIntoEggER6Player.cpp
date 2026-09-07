//cpp
// @symbol _ZN4Coin13OnTurnIntoEggER6Player
/* Reconstructed C++ method, vtable slot 19. Each path delegates the coin
 * payout to a helper and returns no value, matching the shared actor hook's
 * void contract. Calls and epilogues alone do not identify an original type. */
#include "decl_common.h"
#include "Coin.h"

void Coin::OnTurnIntoEgg(Player &player)
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
