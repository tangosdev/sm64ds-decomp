//cpp
// @symbol _ZN6Player19St_TornadoSpin_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int Player_ReleaseHeldActor(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int data_0209f318[];
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,void*);
}

int Player::St_TornadoSpin_Init()
{
  Player_ReleaseHeldActor(((char*)this));
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x5f,0,0x1000,0);
  mVertSpeed=0;
  mVertAccel=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  func_0200d6b4(*(void**)data_0209f318, mPlayerNo);
  mAttachOffsetY=mPosY - *(int*)(mAttachedActor+0x60);
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter,0x26,(void*)((char*)&mCamSpacePosX));
  return 1;
}
