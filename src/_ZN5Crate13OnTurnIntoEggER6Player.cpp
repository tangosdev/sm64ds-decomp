//cpp
// @symbol _ZN5Crate13OnTurnIntoEggER6Player
/* recovered: named members + real C++ method */
/* Crate::OnTurnIntoEgg(Player &) -- vtable slot 19. Pays out three coins the
 * first time only, tracked by mCoinsPaid: handed over directly if the player is
 * mid cap-collect, otherwise banked on the egg. Either way the crate goes to
 * state 6. */
#include "Crate.h"

extern "C" {
int _ZN6Player15IsCollectingCapEv(char *player);
void _ZN8dActor_c15GivePlayerCoinsER6Playerhj(char *self, char *player, unsigned char n, unsigned int j);
void _ZN6Player20RegisterEggCoinCountEjbb(char *player, unsigned int n, char b1, char b2);
void Crate_SetState(char *c, int i);
}

/* The legacy free function returned void. The shared actor hook now agrees,
 * so this method can finish after changing state without an invented result.
 * The earlier int declaration made that fallthrough an invalid C++ contract;
 * matching residual register contents did not make it a defined return value. */
void Crate::OnTurnIntoEgg(Player &player)
{
    char *r4 = (char *)&player;
    if (_ZN6Player15IsCollectingCapEv(r4)) {
        if (mCoinsPaid != 1) {
            _ZN8dActor_c15GivePlayerCoinsER6Playerhj((char *)this, r4, 3, 0);
            mCoinsPaid = 1;
        }
    } else {
        unsigned int count = 0;
        if (mCoinsPaid != 1) {
            mCoinsPaid = 1;
            count = 3;
        }
        _ZN6Player20RegisterEggCoinCountEjbb(r4, count, 0, 0);
    }
    Crate_SetState((char *)this, 6);
}
