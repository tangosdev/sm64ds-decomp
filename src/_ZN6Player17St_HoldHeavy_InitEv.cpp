//cpp
// @symbol _ZN6Player17St_HoldHeavy_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,struct Vector3*);
}

int Player::St_HoldHeavy_Init()
{
  mStateWork=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_020ff254[mStateWork], 0x40000000, 0x1000, 0);
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x12, (struct Vector3*)((char*)&mCamSpacePos));
  return 1;
}
