//cpp
// @symbol _ZN6Player12GetTalkStateEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
struct State;
extern State data_ov002_0211046c;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
}

int Player::GetTalkState()
{
  if(!_ZN6Player7IsStateERNS_5StateE(((char*)this),&data_ov002_0211046c)) return -1;
  unsigned char v=mStateStep;
  if(v==0) return 0;
  if(v==3) return 2;
  if(v==5||v==7) return 3;
  return 1;
}
