//cpp
// @symbol _ZN6Player12Unk_020c4f40Et
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
struct State;
extern State data_ov002_0211046c;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
}

int Player::Unk_020c4f40(unsigned short x)
{
  if(_ZN6Player7IsStateERNS_5StateE(((char*)this),&data_ov002_0211046c) && mStateStep==3){
    mStateWaitTimer=x;
    mStateStep=5;
    return 1;
  }
  return 0;
}
