//cpp
// @symbol _ZN6Player21IsOpeningDoorWithStarEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player12Unk_020c9e5cEh(void *c, unsigned char a);
extern int _ZN6Player12GetTalkStateEv(void *c);
}

int Player::IsOpeningDoorWithStar()
{
  return _ZN6Player12Unk_020c9e5cEh(((void *)this),0xe) || _ZN6Player12GetTalkStateEv(((void *)this)) > 0;
}
