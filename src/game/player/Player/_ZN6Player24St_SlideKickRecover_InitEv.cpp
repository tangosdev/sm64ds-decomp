//cpp
// @symbol _ZN6Player24St_SlideKickRecover_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int Fix12i;
extern "C" {
extern int Player_ReleaseHeldActor(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
extern int func_ov002_020e25f0(void*, int);
extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);
}

int Player::St_SlideKickRecover_Init()
{
  *(char*)((char*)&mJumpedFromQuicksand)=0;
  *(char*)((char*)&mIsInAirState)=1;
  *(char*)((char*)&mIsFallScreaming)=0;
  Player_ReleaseHeldActor(((void*)this));
  *(char*)((char*)&mJumpComboStage)=0;
  *(char*)((char*)&mIsAirborne)=1;
  *(char*)((char*)&mLandSoundPlayed)=0;
  *(int*)((char*)&mVertSpeed)=0x20000;
  _ZN6Player7SetAnimEji5Fix12IiEj(((void*)this), 0x56, 0x40000000, 0x1000, 0);
  func_ov002_020e25f0(((void*)this), 0);
  _ZN5Sound9PlayBank0EjRK7Vector3(0xf, (char*)((void*)this)+0x74);
  return 1;
}
