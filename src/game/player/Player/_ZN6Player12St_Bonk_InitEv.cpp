//cpp
// @symbol _ZN6Player12St_Bonk_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

int Player::St_Bonk_Init()
{
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x13,0x40000000,0x1000,0);
  mStateTimer=0x10;
  mHorzSpeed=0x40;
  mVertSpeed=0xa000;
  mStateWork=0;
  return 1;
}
