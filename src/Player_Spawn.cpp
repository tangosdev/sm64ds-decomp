//cpp
#include "Player.h"

extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
Player *_ZN6PlayerC1Ev(Player *player);

/* This address allocates and initializes a Player, but mwccarm cannot emit the
   old C3 spelling: its real constructors are C1/C2. Keep only the allocation
   boundary in C linkage and describe the object itself with the real type. */
Player *Player_Spawn()
{
    Player *player = (Player *)_ZN7fBase_cnwEj(sizeof(Player));
    if (!player)
        return player;

    return _ZN6PlayerC1Ev(player);
}
}
