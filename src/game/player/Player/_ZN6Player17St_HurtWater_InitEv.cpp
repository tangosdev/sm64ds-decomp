//cpp
// @symbol _ZN6Player17St_HurtWater_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int data_0209f318[];
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
extern void func_ov002_020d93ac(void*);
extern void func_ov002_020d94cc(void*);
}

int Player::St_HurtWater_Init()
{
  unsigned a=data_ov002_02109fe4[mStateStep&1];
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),a,0x40000000,0x1000,0);
  mStateWork=0;
  mStateArg=0;
  mVertAccel=0;
  mTerminalVelocity=-0xc000;
  mVertSpeed=0x10000;
  mIsTakingDamage=1;
  if(mHurtDamage){
    int b=mStateStep&0xf0;
    if(b==0 || b==0x10){
      func_ov002_020d93ac(((char*)this));
      func_ov002_020d94cc(((char*)this));
    }
  }
  func_0200d89c(*(char**)data_0209f318);
  return 1;
}
