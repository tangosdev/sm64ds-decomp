//cpp
// @symbol _ZN10Scuttlebug13OnTurnIntoEggER6Player
/* daSpd_c::OnTurnIntoEgg -- vtable slot 19, recovered from vtable slot identity.
 * Pays the player mCoinCount coins (a cap-collection coin if Yoshi is wearing
 * the cap, an egg coin otherwise). A spawned-child Scuttlebug (param1 != 0)
 * resets to its egg state instead of dying; the original (param1 == 0) marks
 * itself for destruction. mCoinCount lives at +0x3aa, set to 3 by InitResources
 * and read/written only through raw offsets elsewhere in this class too. */
#include "Scuttlebug.h"
#include "Player.h"

extern "C" {
extern int _ZN6Player15IsCollectingCapEv(void *p);
extern void _ZN8dActor_c15GivePlayerCoinsER6Playerhj(void *a, void *p, unsigned char n, unsigned int u);
extern void _ZN6Player20RegisterEggCoinCountEjbb(void *p, unsigned int n, int b1, int b2);
extern void Scuttlebug_SetState(void *a, int idx);
extern void _ZN7fBase_c18MarkForDestructionEv(void *a);
}

int Scuttlebug::OnTurnIntoEgg(Player &player)
{
    volatile int force_stack;
    char *a = (char *)this;
    void *p = &player;
    int *bp;
    int t;
    if (_ZN6Player15IsCollectingCapEv(p))
        _ZN8dActor_c15GivePlayerCoinsER6Playerhj(a, p, *(unsigned char *)(a + 0x3aa), 0);
    else
        _ZN6Player20RegisterEggCoinCountEjbb(p, *(unsigned char *)(a + 0x3aa), 0, 0);
    if (*(int *)(a + 8) != 0) {
        a[0x3aa] = 0;
        bp = (int *)((int)(a) + 0xb0);
        t = *bp;
        t &= ~0x40000;
        *bp = t;
        Scuttlebug_SetState(a, 0);
    } else {
        _ZN7fBase_c18MarkForDestructionEv(a);
    }
}
