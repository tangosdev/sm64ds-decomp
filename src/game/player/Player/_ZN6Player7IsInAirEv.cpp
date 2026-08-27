//cpp
// @symbol _ZN6Player7IsInAirEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"


int Player::IsInAir()
{
  if(mIsAirborne) return mIsInAirState!=0;
  return 0;
}
