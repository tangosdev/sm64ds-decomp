//cpp
// @symbol _ZN6Player6IsAnimEj
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" unsigned int _ZNK6Player14GetBodyModelIDEjb(char*,unsigned int,char);
extern int data_ov002_020ff480[];

int Player::IsAnim(unsigned int a)
{
  int ip = mParam;
  int* p = (int*)data_ov002_020ff480[ip + a*4];
  int v = p[1];
  unsigned int id = _ZNK6Player14GetBodyModelIDEjb(((char*)this), ip&0xff, 0);
  int* m = *(int**)(((char*)this) + id*4 + 0xdc);
  int r=(m[24]==v); return r?1:0;
}
