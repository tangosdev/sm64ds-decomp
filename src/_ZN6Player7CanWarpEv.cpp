//cpp
// @symbol _ZN6Player7CanWarpEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
struct State;
extern State _ZN6Player7ST_WAITE;
extern State data_ov002_021102a4;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
}

int Player::CanWarp()
{
  if(_ZN6Player7IsStateERNS_5StateE(((void*)this), &_ZN6Player7ST_WAITE) || _ZN6Player7IsStateERNS_5StateE(((void*)this), &data_ov002_021102a4)) return 1;
  return 0;
}
