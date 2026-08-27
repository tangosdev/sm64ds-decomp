//cpp
// @symbol _ZN6Player21St_SmallLaunchUp_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

int Player::St_SmallLaunchUp_Init()
{
  mJumpedFromQuicksand=0;
  mIsInAirState=1;
  mIsFallScreaming=0;
  mJumpComboStage=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x56,0,0x1000,0);
  mVertSpeed=0x2a000;
  mHorzSpeed=0;
  mAngleX=mPrevAngleX;
  mAngleY=mPrevAngleY;
  mAngleZ=mPrevAngleZ;
  return 1;
}
