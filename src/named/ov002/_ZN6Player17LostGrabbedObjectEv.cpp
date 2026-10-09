//cpp
// @symbol _ZN6Player17LostGrabbedObjectEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
}

int Player::LostGrabbedObject()
{
  return ((Player *)(((void*)this)))->IsAnim(0x18) || ((Player *)(((void*)this)))->IsAnim(0x8b);
}
