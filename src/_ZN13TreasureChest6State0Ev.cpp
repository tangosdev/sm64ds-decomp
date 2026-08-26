//cpp
// @symbol _ZN13TreasureChest6State0Ev
#include "TreasureChest.h"
#include "Player.h"

extern "C" {
void DecIfAbove0_Short(u16 *value);
void func_02012790(u32 code);
s16 Vec3_HorzAngle(const Vector3 *from, const Vector3 *to);
s32 AngleDiff(s32 a, s32 b);
void func_02012694(u32 code, const Vector3 *pos);
}

void TreasureChest::State0()
{
    DecIfAbove0_Short(&mStateTimer);

    if (mStateTimer == 0x58)
        func_02012790(0xe);

    u32 id = mCylinder.otherOwner;
    if (id == 0)
        return;
    if (mStateTimer != 0)
        return;

    Player *player = (Player *)FindWithID(id);
    if (player == 0)
        return;

    {
        int t = player->actorID;
        t = t == 0xbf;
        if (t != false) {
            Vector3 *playerPosPtr = (Vector3 *)&player->mPosX;
            Vector3 playerPos;
            playerPos.x = playerPosPtr->x;
            playerPos.y = playerPosPtr->y;
            playerPos.z = playerPosPtr->z;
            if (AngleDiff(Vec3_HorzAngle((Vector3 *)&mPosX, &playerPos), mAngleY) < 0x4000) {
                int sbcount = 0;
                int count = 0;
                dActor_c *cur = FindWithActorID(0xd, 0);
                while (cur != 0) {
                    count++;
                    if (cur != this) {
                        s32 fv = ((TreasureChest *)cur)->mState;
                        int r1v = 1;
                        if (fv != 1) {
                            if (fv != 2)
                                r1v = 0;
                        }
                        if (r1v != 0)
                            sbcount++;
                    }
                    cur = FindWithActorID(0xd, cur);
                }

                if (sbcount + 1 == mOrder) {
                    if (count == mOrder) {
                        SpawnSoundObj(0);
                        mIsLastChest = 1;
                    } else {
                        func_02012790(0x26);
                    }
                    if (player->mIsUnderwater != 0)
                        func_02012694(0x22, (Vector3 *)&mCamSpacePosX);
                    else
                        func_02012694(0x20, (Vector3 *)&mCamSpacePosX);
                    SetState(1);
                } else {
                    mStateTimer = 0x5a;
                    if (player->mIsMetal != 0)
                        player->Shock(0);
                    else
                        player->Shock(1);

                    for (;;) {
                        cur = FindWithActorID(0xd, cur);
                        if (cur == 0)
                            break;
                        if (cur != this)
                            ((TreasureChest *)cur)->SetState(0);
                    }
                }
            }
        }
    }
}
