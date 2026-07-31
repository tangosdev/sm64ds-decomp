//cpp
// @symbol _ZN6Player9IsOnShellEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
struct State { int a; int b; };
extern int _ZN6Player7IsStateERNS_5StateE(void *c, struct State *s);
extern struct State data_ov002_02110304;

int Player::IsOnShell()
{
  if(_ZN6Player7IsStateERNS_5StateE(((void *)this), &data_ov002_02110304)) return 1;
  *(int*)((char*)&mRidingShell)=0;
  return 0;
}
