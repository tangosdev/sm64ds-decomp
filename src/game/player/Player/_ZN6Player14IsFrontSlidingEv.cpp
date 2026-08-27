//cpp
// @symbol _ZN6Player14IsFrontSlidingEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
int _ZN6Player6IsAnimEj(void*, unsigned int);
}

int Player::IsFrontSliding()
{
  return _ZN6Player6IsAnimEj(((void*)this), 0x43) || _ZN6Player6IsAnimEj(((void*)this), 0x40);
}
