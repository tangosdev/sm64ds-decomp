//cpp
// @symbol _ZN6Player14St_OnWall_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

int Player::St_OnWall_Init()
{
  int r3=0x1000;
  if(param1==2){
    if(mStateStep==2) r3=0x2000;
  }
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_020ff164[mStateStep], 0, r3, 0);
  mHorzSpeed=0;
  return 1;
}
