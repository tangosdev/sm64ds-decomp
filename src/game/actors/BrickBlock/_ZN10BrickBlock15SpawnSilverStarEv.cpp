//cpp
// @symbol _ZN10BrickBlock15SpawnSilverStarEv
#include "BrickBlock.h"
#include "common.h"

extern "C" void LinkSilverStarAndStarMarker(void *starMarker, void *silverStar);

void BrickBlock::SpawnSilverStar()
{
    Vector3 spawnPos;
    dActor_c *starMarker;
    dActor_c *silverStar;

    UntrackStar(mTrackStarID);
    GetSpawnPos(spawnPos, *this);
    starMarker = Spawn(0xb4, 0x50, spawnPos, 0, mAreaId, -1);
    silverStar = Spawn(0xb3, mStarID | 0x10, spawnPos, 0, mAreaId, -1);
    if (starMarker == 0)
        return;
    if (silverStar == 0)
        return;

    /* Silver Star's marker-owner unique ID is the word at 0x434. */
    *(u32 *)((char *)silverStar + 0x434) = starMarker->uniqueID;
    LinkSilverStarAndStarMarker(starMarker, silverStar);
}
