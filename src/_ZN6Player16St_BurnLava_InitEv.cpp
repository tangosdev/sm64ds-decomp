//cpp
// @symbol _ZN6Player16St_BurnLava_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int func_ov002_020d91e0(void*,int,int);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,struct Vector3*);
}

int Player::St_BurnLava_Init()
{
  mIsInAirState=1;
  mJumpComboStage=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  mIsTakingDamage=1;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x16,0,0x1000,0);
  mHorzSpeed=0;
  mVertSpeed=0x64000;
  mStateWork=0;
  mStateStep=0;
  if(func_ov006_020e3078(((char*)this),data_ov002_021100f4)==0){
    mStateArg=0;
  }
  func_ov002_020d91e0(((char*)this),0x300,1);
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter,0x23,(struct Vector3*)((char*)&mCamSpacePos));
  return 1;
}
