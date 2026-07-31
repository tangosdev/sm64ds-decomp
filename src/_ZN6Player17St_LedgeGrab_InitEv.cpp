//cpp
// @symbol _ZN6Player17St_LedgeGrab_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,void*);
}

int Player::St_LedgeGrab_Init()
{
  if(mStateStep){
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x20,0x40000000,0x1000,0);
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter,0x1a,(void*)((char*)&mCamSpacePos));
  } else {
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x23,0x40000000,0x1000,0);
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter,0x17,(void*)((char*)&mCamSpacePos));
  }
  return 1;
}
