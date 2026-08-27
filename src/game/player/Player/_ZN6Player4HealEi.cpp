//cpp
// @symbol _ZN6Player4HealEi
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" int _ZN6Player7IsStateERNS_5StateE(char*, void*);
extern int data_ov002_0211010c[];
extern int data_ov002_02110124[];

int Player::Heal(int amt)
{
  int r = _ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_0211010c);
  if(r) return r;
  r = _ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_02110124);
  if(r) return r;
  return GiveHealth(mPlayerNo, amt);
}
