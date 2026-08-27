//cpp
// @symbol _ZN6Player22St_SwingPlayer_CleanupEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"


int Player::St_SwingPlayer_Cleanup()
{
  char* p = *(char**)((char*)&mHeldObj);
  if (p) {
    *(unsigned int*)(p + 0xb0) &= ~0x800;
  }
  mPrevAngleY = mAngleY;
  return 1;
}
