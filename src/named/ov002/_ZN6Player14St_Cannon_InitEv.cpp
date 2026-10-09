//cpp
// @symbol _ZN6Player14St_Cannon_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int Player_DisableInteraction(void*);
extern int Player_ReleaseHeldActor(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

int Player::St_Cannon_Init()
{
  Player_DisableInteraction(((char*)this));
  mIsUnderwater=0;
  mIsInAirState=1;
  mIsFallScreaming=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  Player_ReleaseHeldActor(((char*)this));
  mStateStep=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x73,0,0x1000,0);
  mVertAccel=-0x1200;
  mOpacity=0;
  mIsBodyClsnEnabled=0;
  mStateWork=0;
  mIsControlDisabled=1;
  return 1;
}
