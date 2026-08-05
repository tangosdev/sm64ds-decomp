//cpp
// @symbol _ZN6Player17St_EndingFly_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int Player_DisableInteraction(void* c);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int f, unsigned int g);
}

int Player::St_EndingFly_Init()
{
  Player_DisableInteraction(((char*)this));
  mIsControlDisabled=1;
  unk_6e6=0;
  mStateWork=unk_6e6;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x54,0x40000000,0x1000,0);
  return 1;
}
