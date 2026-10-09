//cpp
// @symbol _ZN6Player14St_Owl_CleanupEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int func_ov002_020bd8c0(void*,int);
}

int Player::St_Owl_Cleanup()
{
  func_ov002_020bd8c0(((char*)this),0x2f);
  mHeldObj=0;
  return 1;
}
