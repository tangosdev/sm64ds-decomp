//cpp
// @symbol _ZN13TreasureChest6State1Ev
#include "TreasureChest.h"
#include "Player.h"

void TreasureChest::State1()
{
    mModelAnim.Advance();
    if (((u32)(mModelAnim.currFrame << 4) >> 0x10) == 0x14) {
        Player *player = ClosestPlayer();

        Vector3 spawnPos;
        spawnPos.x = mPosX;
        spawnPos.y = mPosY;
        spawnPos.z = mPosZ;
        spawnPos.y += 0xc8000;

        if (mIsLastChest == 0) {
            Vector3 numberPos;
            numberPos.x = spawnPos.x;
            numberPos.y = spawnPos.y;
            numberPos.z = spawnPos.z;
            SpawnNumber(numberPos, mOrder, false, 0, 0);
        }

        if (player->mIsUnderwater != 0) {
            dActor_c *bubble = dActor_c::Spawn(
                0x123, 0, spawnPos, 0, mAreaId, -1);
            bubble->unk_0a4 = 0;
            bubble->mVertSpeed = 0x800;
            bubble->unk_0ac = 0;
        }
    }

    if (mModelAnim.Finished() != 0)
        SetState(2);
}
