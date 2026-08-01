//cpp
// @symbol _ZN6Player13St_Crawl_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

int Player::St_Crawl_Init()
{
  mStateStep=3;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x64,0x40000000,0x1000,0);
  return 1;
}
