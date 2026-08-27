//cpp
// @symbol _ZN6Player16IsInsideOfCannonEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
struct State;
extern State data_ov002_021102d4;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
}

int Player::IsInsideOfCannon()
{
  if(!_ZN6Player7IsStateERNS_5StateE(((char*)this), &data_ov002_021102d4)) return 0;
  return mStateStep!=2 ? 1 : 0;
}
