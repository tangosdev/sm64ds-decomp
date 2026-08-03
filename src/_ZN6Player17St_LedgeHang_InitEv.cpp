//cpp
// @symbol _ZN6Player17St_LedgeHang_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
struct Vec3 { int x, y, z; };
extern "C" {
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char* c, unsigned int anim, int a, int fix, unsigned int z);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void* v);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(char* o, struct Vec3* p);
}

int Player::St_LedgeHang_Init()
{
  *(unsigned short*)(((char*)this)+0x600+0xa6) = 0;
  if(mStateStep == 0){
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x22, 0x40000000, 0x1000, 0);
  }else{
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x21, 0, 0x1000, 0);
  }
  *(unsigned short*)(((char*)this)+0x600+0xa6) = 2;
  mPrevAngleY = mAngleY;
  mVertSpeed = 0;
  mHorzSpeed = 0;
  mIsAirborne = 0;
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x16, ((char*)this)+0x74);
  {
    struct Vec3 z3 = {0, 0, 0};
    struct Vec3 off;
    z3.y = -0x64000;
    off.x = z3.x;
    off.y = z3.y;
    off.z = z3.z;
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(((char*)this)+0x2d4, &off);
  }
  unk_6e6 = 0;
  return 1;
}
