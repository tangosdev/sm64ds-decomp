//cpp
// @symbol _ZN6Player21St_HeadstandJump_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,void*);
}

int Player::St_HeadstandJump_Init()
{
  mJumpedFromQuicksand=0;
  mIsInAirState=1;
  mIsFallScreaming=0;
  mJumpComboStage=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x1c,0x40000000,0x1000,0);
  mVertSpeed=0x3e000;
  mHorzSpeed=0x18000;
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter,0xc,(char*)((char*)this)+0x74);
  return 1;
}
