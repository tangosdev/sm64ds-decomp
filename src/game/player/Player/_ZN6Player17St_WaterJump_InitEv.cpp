//cpp
// @symbol _ZN6Player17St_WaterJump_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int,void*);
extern int func_ov002_020e25f0(void*,int);
}

int Player::St_WaterJump_Init()
{
  mJumpedFromQuicksand=0;
  mIsInAirState=1;
  mIsFallScreaming=0;
  mJumpComboStage=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x53,0x40000000,0x1000,0);
  mVertSpeed=0x2a000;
  mHorzSpeed=0x12000;
  _ZN5Sound9PlayBank0EjRK7Vector3(0x18,(char*)((char*)this)+0x74);
  func_ov002_020e25f0(((char*)this),0);
  return 1;
}
