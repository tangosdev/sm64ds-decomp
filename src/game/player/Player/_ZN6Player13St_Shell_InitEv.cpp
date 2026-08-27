//cpp
// @symbol _ZN6Player13St_Shell_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int Player_ReleaseHeldActor(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int func_ov002_020bd928(void*,int);
}

int Player::St_Shell_Init()
{
  Player_ReleaseHeldActor(((char*)this));
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x35,0x40000000,0x1000,0);
  mStateStep=0;
  func_ov002_020bd928(((char*)this),0x33);
  return 1;
}
