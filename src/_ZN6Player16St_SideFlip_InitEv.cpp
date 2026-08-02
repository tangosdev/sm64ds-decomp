//cpp
// @symbol _ZN6Player16St_SideFlip_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int Fix12i;
extern int func_ov002_020e2be4(void*);
extern int func_ov002_020e2ba8(void*);
extern int func_ov002_020e2b6c(void*);
extern int func_ov002_020e2ad0(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
extern int func_ov002_020e25f0(void*, int);

int Player::St_SideFlip_Init()
{
  *(char*)((char*)&mJumpedFromQuicksand)=0;
  if (func_ov002_020e2be4(((void*)this))) return 1;
  if (func_ov002_020e2ba8(((void*)this))) return 1;
  if (func_ov002_020e2b6c(((void*)this))) return 1;
  *(char*)((char*)&mIsInAirState)=1;
  *(char*)((char*)&mIsFallScreaming)=0;
  *(char*)((char*)&mJumpComboStage)=0;
  *(char*)((char*)&mIsAirborne)=1;
  *(char*)((char*)&mLandSoundPlayed)=0;
  *(int*)((char*)&mHorzSpeed)=0x8000;
  *(int*)((char*)&mVertSpeed)=0x3e000;
  func_ov002_020e2ad0(((void*)this));
  _ZN6Player7SetAnimEji5Fix12IiEj(((void*)this), 0x4d, 0x40000000, 0x1000, 0);
  *(short*)((char*)&mPrevAngleY) = *(short*)((char*)&mDesiredAngleY);
  *(short*)((char*)&mAngleY) = *(short*)((char*)&mDesiredAngleY) + 0x8000;
  func_ov002_020e25f0(((void*)this), 0);
  return 1;
}
