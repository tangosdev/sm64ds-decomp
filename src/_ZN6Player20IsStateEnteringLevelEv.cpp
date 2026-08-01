//cpp
// @symbol _ZN6Player20IsStateEnteringLevelEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int data_ov002_021104b4[];
extern int _ZN6Player7IsStateERNS_5StateE(char*c, void*s);
}

int Player::IsStateEnteringLevel()
{
  if(_ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_021104b4)) return 1;
  return _ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_0211025c);
}
