//cpp
#include "Sound.h"
// @symbol _ZN6Player17St_LedgeGrab_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

int Player::St_LedgeGrab_Init()
{
  if(mStateStep){
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x20,0x40000000,0x1000,0);
    Sound::PlayCharVoice(mCharacter, 0x1a, *(const Vector3 *)(((char*)&mCamSpacePosX)));
  } else {
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x23,0x40000000,0x1000,0);
    Sound::PlayCharVoice(mCharacter, 0x17, *(const Vector3 *)(((char*)&mCamSpacePosX)));
  }
  return 1;
}
