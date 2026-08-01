//cpp
// @symbol _ZN6Player19St_GroundPound_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int Fix12i;
extern int Player_ReleaseHeldActor(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);

int Player::St_GroundPound_Init()
{
  unsigned int anim;
  Player_ReleaseHeldActor(((void*)this));
  *(char*)((char*)&mJumpComboStage)=0;
  *(int*)((char*)&mVertSpeed)=0;
  *(int*)((char*)&mHorzSpeed)=0;
  *(short*)((char*)&mTargetAngleY)=*(short*)((char*)&mAngleY);
  anim=0x3b;
  if (*(unsigned char*)((char*)&mIsMega)) anim=0xa3;
  _ZN6Player7SetAnimEji5Fix12IiEj(((void*)this), anim, 0x40000000, 0x1000, 0);
  *(char*)((char*)&mStateStep)=2;
  _ZN5Sound9PlayBank0EjRK7Vector3(6, (char*)((void*)this)+0x74);
  *(int*)((int*)(((int)((void*)this) + 0x2ec))) |= 0x20;
  return 1;
}
