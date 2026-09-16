//cpp
#include "dActor_c.h"
extern "C" void DeathTable_ClearBit(int id);

void dActor_c::UntrackInDeathTable()
{
    DeathTable_ClearBit(mDeathTableID);
}
