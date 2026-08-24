//cpp
// @symbol _ZN6Player15IsCollectingCapEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"


int Player::IsCollectingCap()
{
  unsigned short v = mCapFlags;
  if(v!=0 && (v&7)<5) return 1;
  return 0;
}
