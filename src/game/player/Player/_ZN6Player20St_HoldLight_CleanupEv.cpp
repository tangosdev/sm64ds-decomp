//cpp
// @symbol _ZN6Player20St_HoldLight_CleanupEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"


int Player::St_HoldLight_Cleanup()
{
  char* p = *(char**)((char*)&mHeldObj);
  if (p) {
    *(unsigned int*)(p + 0xb0) |= 0x4000;
  }
  return 1;
}
