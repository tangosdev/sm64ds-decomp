//cpp
// @symbol _ZN6Player8CanPauseEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
struct State;
extern State data_ov002_0211067c;
extern State data_ov002_021106ac;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
}

int Player::CanPause()
{
  if(*(unsigned char*)((char*)&mIsUnderwater)){
    if(_ZN6Player7IsStateERNS_5StateE(((void*)this), &data_ov002_0211067c)) goto ret1a;
    if(_ZN6Player7IsStateERNS_5StateE(((void*)this), &data_ov002_021106ac)) goto ret1a;
    goto ret0;
ret1a:
    return 1;
  }
  if(*(unsigned char*)((char*)&mIsAirborne)) goto ret0;
  if(*(unsigned char*)((char*)&mIsTakingDamage)) goto ret0;
  if(*(unsigned char*)((char*)&mIsNoControl)) goto ret0;
  return 1;
ret0:
  return 0;
}
