//cpp
// @symbol _ZN6Player17LostGrabbedObjectEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
int _ZN6Player6IsAnimEj(void*, unsigned int);
}

int Player::LostGrabbedObject()
{
  return _ZN6Player6IsAnimEj(((void*)this), 0x18) || _ZN6Player6IsAnimEj(((void*)this), 0x8b);
}
