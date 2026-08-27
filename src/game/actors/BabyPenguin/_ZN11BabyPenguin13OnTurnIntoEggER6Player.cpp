//cpp
// @symbol _ZN11BabyPenguin13OnTurnIntoEggER6Player
/* recovered: real C++ method, vtable slot 19. func_ov072_02121fa0 was a
 * tail-call veneer to fBase_c::MarkForDestruction; that call is now a real
 * member call. */
#include "BabyPenguin.h"

int BabyPenguin::OnTurnIntoEgg(Player &player)
{
    MarkForDestruction();
}
