//cpp
// @symbol _ZN6Player20St_CeilingGrate_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

int Player::St_CeilingGrate_Init()
{
  func_ov002_020cf384(((char*)this));
  mStateStep=0;
  mStateWork=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x5b,0x40000000,0x1000,0);
  return 1;
}
