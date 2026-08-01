//cpp
// @symbol _ZN6Player21St_OpeningWakeUp_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int f, unsigned int g);

int Player::St_OpeningWakeUp_Init()
{
  mSleepStage=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0xb2,0x40000000,0x1000,0);
  mStateStep=0;
  return 1;
}
