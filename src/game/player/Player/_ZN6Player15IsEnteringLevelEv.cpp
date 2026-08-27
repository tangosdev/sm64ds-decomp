//cpp
// @symbol _ZN6Player15IsEnteringLevelEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player20IsStateEnteringLevelEv(char*c);
}

int Player::IsEnteringLevel()
{
  if(_ZN6Player20IsStateEnteringLevelEv(((char*)this))==0) return 0;
  unsigned char s=mStateStep;
  if(s==8||s==9||s==0x10) return 1;
  return 0;
}
