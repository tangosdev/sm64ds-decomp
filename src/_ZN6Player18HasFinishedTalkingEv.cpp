//cpp
// @symbol _ZN6Player18HasFinishedTalkingEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
struct State;
extern State data_ov002_0211046c;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
}

int Player::HasFinishedTalking()
{
  if(_ZN6Player7IsStateERNS_5StateE(((char*)this),&data_ov002_0211046c) && mStateStep==3){
    mStateStep=4;
    return 1;
  }
  return 0;
}
