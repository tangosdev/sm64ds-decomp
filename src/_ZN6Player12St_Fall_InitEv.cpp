//cpp
// @symbol _ZN6Player12St_Fall_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int Fix12i;
extern int _ZN6Player6IsAnimEj(void*, unsigned int);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);

int Player::St_Fall_Init()
{
  *(char*)((char*)&mJumpedFromQuicksand)=0;
  *(char*)((char*)&mIsInAirState)=1;
  *(char*)((char*)&mIsFallScreaming)=0;
  *(char*)((char*)&mJumpComboStage)=0;
  *(char*)((char*)&mIsAirborne)=1;
  *(char*)((char*)&mLandSoundPlayed)=0;
  if (_ZN6Player6IsAnimEj(((void*)this), 0x41)) {
    _ZN6Player7SetAnimEji5Fix12IiEj(((void*)this), 0x44, 0x40000000, 0x1000, 0);
    *(char*)((char*)&mStateStep)=1;
  } else {
    _ZN6Player7SetAnimEji5Fix12IiEj(((void*)this), 0x54, 0x40000000, 0x1000, 0);
    *(char*)((char*)&mStateStep)=0;
  }
  *(int*)((char*)&mPeakY) = *(int*)((char*)&mPosY);
  return 1;
}
