//cpp
// @symbol _ZN6Player16St_Shell_CleanupEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void func_ov002_020bd8c0(char* c, unsigned int r1);
}

int Player::St_Shell_Cleanup()
{
  char* p = *(char**)((char*)&mRidingShell);
  if (p != 0) {
    *(int*)(p + 0x3c0) = 0;
    mRidingShell = 0;
    *(int*)(((int)((char*)this) + 0x60) & 0xFFFFFFFFFFFFFFFFLL) += 0x46000;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    func_ov002_020bd8c0(((char*)this), 0x33);
  }
  return 1;
}
