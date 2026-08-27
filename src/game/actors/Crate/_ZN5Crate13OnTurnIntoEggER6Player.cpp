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

/* NO VALUE IS RETURNED, AND THAT IS FAITHFUL. The header declares `int`; this
 * body sets no return value and the ROM does not either. Adding an explicit
 * `return <v>;` emits a real instruction and breaks the match (measured on
 * PushBlock::OnPushed, 2026-08-22). The legacy .c declared this `void` and never
 * included the header, so nothing checked the disagreement until it became a
 * real method. Harmless for the ROM build; a host-port caller that reads this
 * result gets garbage. Fix upstream in the header's return type if it is wrong --
 * never with a `return` here. */
int Crate::OnTurnIntoEgg(Player &player)
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
