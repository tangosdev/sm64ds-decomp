//cpp
// @symbol _ZN6Player15St_Grabbed_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void func_ov002_020da9d4(void* c);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int, unsigned int, void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
}

int Player::St_Grabbed_Init()
{
  func_ov002_020da9d4(((char*)this));
  *(int*)(int)(((unsigned long long)(int)((char*)&mMovingCylinderClsnWithPos.flags)) & 0xFFFFFFFFFFFFFFFFull) |= 2;
  mIsBodyClsnEnabled = 0;
  unk_716 = 1;
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 6, (void*)((char*)&mCamSpacePosX));
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x71, 0, 0x1000, 0);
  unk_717 = 1;
  mStateStep = 0;
  return 1;
}
