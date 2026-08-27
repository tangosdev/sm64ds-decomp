//cpp
// @symbol _ZN6Player12Unk_020ca150Eh
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player11ChangeStateERNS_5StateE(void *c, Player::State *s);
}
extern Player::State data_ov002_0211022c;
extern Player::State data_ov002_0211013c;

int Player::Unk_020ca150(unsigned char a)
{
  if(IsState(data_ov002_0211022c)){
    if(a==4){
      _ZN6Player11ChangeStateERNS_5StateE(((void *)this), &data_ov002_0211013c);
      return 1;
    }
    if(a==5) *(unsigned char*)((char*)&mStateStep)=5;
  }
  return 0;
}
