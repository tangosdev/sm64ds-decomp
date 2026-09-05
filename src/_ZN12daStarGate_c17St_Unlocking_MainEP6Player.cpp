//cpp
// @symbol _ZN12daStarGate_c17St_Unlocking_MainEP6Player
#include "daStarGate_c.h"
#include "Player.h"

extern daStarGateInfo data_ov100_02148390[];
extern int data_0209caa0[];

bool daStarGate_c::St_Unlocking_Main(Player *player)
{
    if (!player->IsOpeningDoorWithStar()) {
        daStarGateInfo *entry = (daStarGateInfo *)
            ((char *)data_ov100_02148390 + param1 * 6);
        s8 sh = entry->saveFlag;
        data_0209caa0[1] |= 0x8000 << sh;
        TryOpenDoor(player);
    }
    return true;
}
