//cpp
// @symbol _ZN6Player8IsDivingEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int data_ov002_021105bc[];
int _ZN6Player7IsStateERNS_5StateE(void*, void*);
}

int Player::IsDiving()
{
  return _ZN6Player7IsStateERNS_5StateE(((void*)this), data_ov002_021105bc) != 0;
}
