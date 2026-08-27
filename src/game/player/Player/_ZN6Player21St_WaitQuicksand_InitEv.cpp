//cpp
// @symbol _ZN6Player21St_WaitQuicksand_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

int Player::St_WaitQuicksand_Init()
{
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x47,0,0x1000,0);
  mStateWork=0;
  return 1;
}
