//cpp
#include "dActor_c.h"
extern "C" void DeathTable_SetBit(int id);

void dActor_c::TrackInDeathTable()
{
    DeathTable_SetBit(mDeathTableID);
}
