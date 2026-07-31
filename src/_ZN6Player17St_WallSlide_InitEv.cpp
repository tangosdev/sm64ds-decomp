//cpp
// @symbol _ZN6Player17St_WallSlide_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

int Player::St_WallSlide_Init()
{
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x29,0x40000000,0x1000,0);
  mHorzSpeed=0;
  if(mVertSpeed >= 0) mVertSpeed=0;
  mStateTimer=7;
  return 1;
}
