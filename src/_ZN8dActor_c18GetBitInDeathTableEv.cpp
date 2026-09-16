//cpp
#include "dActor_c.h"
extern "C" int DeathTable_GetBit(int id);

int dActor_c::GetBitInDeathTable()
{
    return DeathTable_GetBit(mDeathTableID);
}
