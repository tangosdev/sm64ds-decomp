//cpp
// @symbol _ZN6Player18St_TurnAround_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

int Player::St_TurnAround_Init()
{
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x46,0x40000000,0x1000,0);
  mStateStep=0;
  return 1;
}
