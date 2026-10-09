//cpp
// @symbol _ZN6Player21St_JumpQuicksand_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

int Player::St_JumpQuicksand_Init()
{
  mJumpedFromQuicksand=1;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x53,0x40000000,0x1000,0);
  mStateWork=0;
  mVertSpeed=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  return 1;
}
