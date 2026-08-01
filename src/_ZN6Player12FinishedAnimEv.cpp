//cpp
// @symbol _ZN6Player12FinishedAnimEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" unsigned int _ZNK6Player14GetBodyModelIDEjb(char*,unsigned int,char);
extern "C" int _ZN9Animation8FinishedEv(char*);

int Player::FinishedAnim()
{
  unsigned int id = _ZNK6Player14GetBodyModelIDEjb(((char*)this), mParam&0xff, 0);
  char* m = *(char**)(((char*)this) + id*4 + 0xdc);
  return _ZN9Animation8FinishedEv(m+0x50) != 0;
}
