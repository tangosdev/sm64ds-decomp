//cpp
// @symbol _ZN6Player11St_Owl_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int data_0209f318[];
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern void func_ov002_020bd928(char*,unsigned int);
}

int Player::St_Owl_Init()
{
  mTerminalVelocity=-0x4b000;
  mVertAccel=0;
  mStateStep=0;
  mHorzSpeed=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x5b,0x40000000,0x1000,0);
  func_0200d6b4((void*)data_0209f318[0], mPlayerNo);
  func_ov002_020bd928(((char*)this),0x2f);
  return 1;
}
