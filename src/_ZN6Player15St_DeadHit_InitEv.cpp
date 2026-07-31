//cpp
// @symbol _ZN6Player15St_DeadHit_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void func_ov002_020c9e40(char*c);
extern char* data_0209f318;
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
}

int Player::St_DeadHit_Init()
{
  func_ov002_020c9e40(((char*)this));
  mIsBodyClsnEnabled=0;
  int idx=mStateStep&1;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),data_ov002_0210a424[idx],0x40000000,0x1000,0);
  mStateArg=0;
  mStateWork=0;
  func_0200d89c(data_0209f318);
  return 1;
}
