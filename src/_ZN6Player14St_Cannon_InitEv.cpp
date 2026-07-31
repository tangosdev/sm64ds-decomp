//cpp
// @symbol _ZN6Player14St_Cannon_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int func_ov002_020c9e40(void*);
extern int func_ov002_020dab14(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

int Player::St_Cannon_Init()
{
  func_ov002_020c9e40(((char*)this));
  mIsUnderwater=0;
  mIsInAirState=1;
  mIsFallScreaming=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  func_ov002_020dab14(((char*)this));
  mStateStep=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x73,0,0x1000,0);
  mVertAccel=-0x1200;
  mOpacity=0;
  mIsBodyClsnEnabled=0;
  mStateWork=0;
  mIsControlDisabled=1;
  return 1;
}
