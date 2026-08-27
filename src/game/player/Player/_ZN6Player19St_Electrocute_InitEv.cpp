//cpp
// @symbol _ZN6Player19St_Electrocute_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int func_ov002_020da9d4(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,void*);
}

int Player::St_Electrocute_Init()
{
  func_ov002_020da9d4(((void*)this));
  *(char*)((char*)&mIsTakingDamage)=1;
  *(short*)((char*)&mStateTimer)=0x1e;
  *(short*)((char*)&mInvincibleTimer)=0x1e;
  *(int*)((char*)&mHorzSpeed)=0;
  *(int*)((char*)&mVertSpeed)=0;
  *(int*)((char*)&mVertAccel)=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((void*)this),0x11,0,0x1000,0);
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)((char*)&mCharacter),7,(char*)((void*)this)+0x74);
  return 1;
}
