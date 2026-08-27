//cpp
// @symbol _ZN6Player12GetHurtStateEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void* c, void* st);
extern int data_ov002_02110094[];
}

int Player::GetHurtState()
{
  if(_ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_02110094))
    return mStateStep & 7;
  return -1;
}
