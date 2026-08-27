//cpp
#include "WaterDiamond.h"
#include "WDW_Water.h"

void WaterDiamond::SetWaterID()
{
  dActor_c *water;
  if (mWaterID != 0) return;
  water = 0;
  for (;;) {
    water = dActor_c::FindWithActorID(0x65, water);
    if (water == 0) goto done;
    if (mWaterParam == ((WDW_Water *)water)->mUseSpawnPosY) break;
  }
  mWaterID = water->uniqueID;
done:
  if (mWaterID != 0) return;
  MarkForDestruction();
}
