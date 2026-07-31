//cpp
// @symbol _ZN6Player22St_GrabBowserTail_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
extern int* data_0209f318;
}

int Player::St_GrabBowserTail_Init()
{
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x81,0x40000000,0x1000,0);
  mStateStep=0;
  mAngleYSpeed=0;
  mStateWork=0;
  mStateArg=0;
  func_0200d10c((void*)data_0209f318, mPlayerNo);
  return 1;
}
