//cpp
// @symbol _ZN6Player11St_Fly_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int data_0209f318[];
extern void Player_ReleaseHeldActor(char*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern void func_0200d6f0(void*,unsigned char);
}

int Player::St_Fly_Init()
{
  unk_69e=0;
  mAngleYSpeed=0;
  mPrevAngleX=0;
  mPrevAngleZ=0;
  mTargetAngleY=mAngleY;
  Player_ReleaseHeldActor(((char*)this));
  if(mStateStep==0){
    mVertSpeed=0x5d000;
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x4a,0x40000000,0x1000,0);
    mTerminalVelocity=-0x4000;
  } else {
    mVertAccel=0;
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x49,0,0x1000,0);
  }
  func_0200d6f0((void*)data_0209f318[0], mPlayerNo);
  return 1;
}
