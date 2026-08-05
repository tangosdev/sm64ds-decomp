//cpp
// @symbol _ZN6Player12Unk_020ca8f8Ev
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
struct State { int a; int b; };
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void *c, struct State *s);
}
extern struct State data_ov002_0211064c;
extern struct State data_ov002_02110664;

int Player::Unk_020ca8f8()
{
  if(_ZN6Player7IsStateERNS_5StateE(((void *)this), &data_ov002_0211064c)) return 1;
  if(_ZN6Player7IsStateERNS_5StateE(((void *)this), &data_ov002_02110664)) return 2;
  return 0;
}
