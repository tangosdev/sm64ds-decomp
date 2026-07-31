//cpp
// @symbol _ZN6Player24St_BowserEarthquake_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int f, unsigned int g);

int Player::St_BowserEarthquake_Init()
{
  if(mStateStep==0){
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x12,0x40000000,0x1000,0);
  } else {
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x47,0,0x1000,0);
    mStateTimer=0x30;
  }
  return 1;
}
