//cpp
// @symbol _ZN6Player12St_Swim_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void func_ov002_020dab14(void*);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned,unsigned,void*);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
extern int data_0209f318[];
}

int Player::St_Swim_Init()
{
  func_ov002_020dab14(((char*)this));
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter,0x2e,((char*)this)+0x74);
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0xa7,0x40000000,0x1000,0);
  mStateStep=0;
  mVertAccel=0;
  mTerminalVelocity=-0xc000;
  mIsUnderwater=1;
  func_0200d768(*(void**)data_0209f318,mPlayerNo);
  mPrevVertSpeed=mVertSpeed;
  mHorzSpeed=0;
  mVertSpeed=0;
  mAngleYSpeed=0;
  mStateWork=0;
  mStateArg=0;
  mJumpComboTimer=0;
  return 1;
}
