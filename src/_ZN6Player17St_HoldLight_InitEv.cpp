//cpp
// @symbol _ZN6Player17St_HoldLight_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern int Player_ScaleByCharFactor(char* c, int a);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char* c, unsigned int anim, int a, int fix, unsigned int z);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void* v);

int Player::St_HoldLight_Init()
{
  if(mHorzSpeed >= Player_ScaleByCharFactor(((char*)this), 0x1c000)){
    mHorzSpeed = Player_ScaleByCharFactor(((char*)this), 0x1c000);
  }
  if(mStateStep == 0){
    char* o = *(char**)((char*)&mHeldObj);
    unsigned int anim = 0x2f;
    if(o != 0){
      int b = (int)((*(int*)(o+0xb0) & 0x8000) != 0);
      if(b != 0){
        anim = 0x86;
        if(mParam == 2){
          if((int)(*(unsigned short*)(o+0xc) == 0xce) != 0) anim = 0x2f;
        }
      }
    }
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), anim, 0x40000000, 0x1000, 0);
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x12, ((char*)this)+0x74);
  }
  mStateWork = 0;
  *(unsigned short*)(((char*)this)+0x600+0x9c) = 0;
  mStateArg = 0;
  return 1;
}
