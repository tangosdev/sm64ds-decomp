//cpp
// @symbol _ZN6Player17St_SlopeJump_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int func_ov002_020e2b6c(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int func_ov002_020e2ad0(void*);
extern int func_ov002_020e25f0(void*,int);
}

int Player::St_SlopeJump_Init()
{
  mJumpedFromQuicksand=0;
  if(func_ov002_020e2b6c(((char*)this))) return 1;
  mIsInAirState=1;
  mIsFallScreaming=0;
  mJumpComboStage=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x53,0x40000000,0x1000,0);
  mVertSpeed=0x2a000;
  func_ov002_020e2ad0(((char*)this));
  if(mHorzSpeed>0){
    mHorzSpeed=(int)(((long long)mHorzSpeed*0xc00+0x800)>>12);
  }
  func_ov002_020e25f0(((char*)this),0);
  return 1;
}
