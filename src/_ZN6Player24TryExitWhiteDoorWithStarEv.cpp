//cpp
// @symbol _ZN6Player24TryExitWhiteDoorWithStarEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player17SetNoControlStateEhih(void *c, unsigned char a, int b, unsigned char d);
}
extern Player::State data_ov002_0211022c;

int Player::TryExitWhiteDoorWithStar()
{
  if(IsState(data_ov002_0211022c)){
    if(*(unsigned char*)((char*)&mStateStep)!=0x13 || *(unsigned char*)((char*)&mIsOpeningBigDoor)==0) return 0;
  }
  return _ZN6Player17SetNoControlStateEhih(((void *)this),0x11,-1,1)!=0;
}
