//cpp
// @symbol _ZN6Player14St_Squish_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int Fix12i;
extern void Player_ReleaseHeldActor(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);

int Player::St_Squish_Init()
{
  Player_ReleaseHeldActor(((void*)this));
  _ZN6Player7SetAnimEji5Fix12IiEj(((void*)this), 0x47, 0, 0x1000, 0);
  *(unsigned char*)((char*)&mStateWork) = 0;
  *(unsigned char*)((char*)&unk_6e6) = 0;
  *(int*)((char*)&mHorzSpeed) = 0;
  *(int*)((char*)&mVertSpeed) = 0;
  *(int*)((char*)&mVertAccel) = 0;
  *(unsigned char*)((char*)&mStateStep) = 0;
  *(unsigned char*)((char*)&unk_716) = 1;
  *(int*)((char*)&mAttachOffsetY) = *(int*)((char*)&mPosY);
  {
    int *p = (int *)(((int)((void*)this) + 0x2ec));
    *p |= 4;
  }
  return 1;
}
