//cpp
// @symbol _ZN12daStarGate_c18St_StayClosed_MainEP6Player
#include "daStarGate_c.h"

extern "C" int func_ov100_02144fcc(daStarGate_c *gate, Player *player);

bool daStarGate_c::St_StayClosed_Main(Player *player)
{
    if (func_ov100_02144fcc(this, player))
        ChangeState(&ST_WAIT, player);
    return true;
}
