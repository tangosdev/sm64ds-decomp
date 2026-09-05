//cpp
// @symbol _ZN12daStarGate_c23St_TalkingToPlayer_MainEP6Player
#include "daStarGate_c.h"
#include "Player.h"

extern "C" int func_ov100_02145014(void);
extern daStarGateInfo data_ov100_02148390[];

bool daStarGate_c::St_TalkingToPlayer_Main(Player *player)
{
  func_ov100_02145014();
  if (player->GetTalkState() == -1) {
    if (data_ov100_02148390[param1].numStars == 0x50) {
      TryOpenDoor(player);
    } else if (!IsInFrontOfPlayer(player)) {
      ChangeState(&ST_STAY_CLOSED, player);
    }
  }
  return true;
}
