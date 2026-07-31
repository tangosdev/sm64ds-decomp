//cpp
// @symbol _ZN6Player19St_SwingPlayer_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
}

int Player::St_SwingPlayer_Init()
{
  mHorzSpeed = 0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x6c, 0x40000000, 0x1000, 0);
  mStateStep = 0;
  mAngleYSpeed = mAngleY - unk_6d6;
  {
    void* obj = *(void**)((char*)&mHeldObj);
    if (obj != 0)
      *(int*)(((long long)(int)((char*)obj+0xb0))) |= 0x800;
  }
  return 1;
}
