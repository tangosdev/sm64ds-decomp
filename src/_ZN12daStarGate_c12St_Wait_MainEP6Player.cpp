//cpp
// @symbol _ZN12daStarGate_c12St_Wait_MainEP6Player
#include "daStarGate_c.h"
#include "Player.h"

extern "C" unsigned char NumStars(void);
extern "C" int func_ov100_02144f84(void);

extern daStarGateInfo data_ov100_02148390[];
extern char data_0209caa0[];

bool daStarGate_c::St_Wait_Main(Player *player)
{
    daStarGateInfo *entry;
    int isSpecial, notEnoughStars, isMario, messageID;
    Vector3 messagePos;

    if (IsInFrontOfPlayer(player)) {
        entry = &data_ov100_02148390[param1];
        isSpecial = entry->numStars == 0x50;
        notEnoughStars = (int)NumStars() < entry->numStars;
        isMario = *(unsigned char *)(data_0209caa0 + 0x41) == 0;

        if (*(int *)(data_0209caa0 + 4) & (0x8000 << entry->saveFlag))
            goto tryOpen;
        if (isSpecial) {
            if (mScaleZ <= 0)
                goto tryOpen;
            if (!notEnoughStars && isMario)
                goto tryOpen;
        }

        {
            int x = mPosX;
            int z = mPosZ;
            int y = mPosY + 0xb4000;
            messagePos.x = x;
            messagePos.y = y;
            messagePos.z = z;
        }

        if (!(isMario && !isSpecial && !notEnoughStars)) {
            if (func_ov100_02144f84() == 0)
                return true;
            messageID = isMario ? entry->notEnoughStarsMsgID
                                : entry->notMarioMsgID;
            if (player->ShowMessage(*this, messageID, &messagePos, 0, 2) == 0)
                goto tryOpen;
            if (isSpecial)
                player->OpenBigDoor();
            ChangeState(&ST_TALKING_TO_PLAYER, player);
            return true;
        } else {
            if (player->TryTalkToDoor(1) == 0)
                goto tryOpen;
            ChangeState(&ST_UNLOCKING, player);
            return true;
        }

    tryOpen:
        TryOpenDoor(player);
    }
    return true;
}
