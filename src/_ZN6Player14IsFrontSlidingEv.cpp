//cpp
// @symbol _ZN6Player14IsFrontSlidingEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
}

int Player::IsFrontSliding()
{
  return ((Player *)(((void*)this)))->IsAnim(0x43) || ((Player *)(((void*)this)))->IsAnim(0x40);
}
