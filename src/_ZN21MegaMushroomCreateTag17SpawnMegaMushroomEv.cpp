//cpp
// @symbol _ZN16daObjKinokoTag_c17SpawnMegaMushroomEv

#include "MegaMushroomCreateTag.h"

void daObjKinokoTag_c::SpawnMegaMushroom()
{
    struct PositionWords {
        Fix12i x;
        Fix12i y;
        Fix12i z;
    };

    daObjKinokoTag_c *tag =
        (daObjKinokoTag_c *)FindWithActorID(0x13f, 0);
    while (tag != 0) {
        if (mGroupId == tag->mGroupId) {
            PositionWords *tagPos = (PositionWords *)&tag->mPosX;
            PositionWords spawnPos;
            spawnPos.x = tagPos->x;
            spawnPos.y = tagPos->y;
            spawnPos.z = tagPos->z;

            LandingDustAt(*(Vector3 *)&spawnPos, true);
            dActor_c *mushroom = Spawn(
                0x115, 0xd, *(Vector3 *)&tag->mPosX, 0, mAreaId, -1);
            mushroom->mVertSpeed = 0x28000;

            tag->KillAndTrackInDeathTable();
            KillAndTrackInDeathTable();
            return;
        }

        tag = (daObjKinokoTag_c *)FindWithActorID(0x13f, tag);
    }
}
