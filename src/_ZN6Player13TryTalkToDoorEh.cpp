//cpp
// @symbol _ZN6Player13TryTalkToDoorEh
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void*,void*);
extern int _ZN6Player17SetNoControlStateEhih(void*,unsigned char,int,unsigned char);
extern int data_ov002_0211013c[];
extern int _ZN6Player7ST_WAITE[];
extern int data_ov002_0211043c[];
}

int Player::TryTalkToDoor(unsigned char a)
{
  if(_ZN6Player7IsStateERNS_5StateE(((void*)this),data_ov002_0211013c)
     || _ZN6Player7IsStateERNS_5StateE(((void*)this),_ZN6Player7ST_WAITE)
     || _ZN6Player7IsStateERNS_5StateE(((void*)this),data_ov002_0211043c)){
    *(unsigned char*)((char*)&mStateArg)=a;
    _ZN6Player17SetNoControlStateEhih(((void*)this),0xe,-1,0);
    return 1;
  }
  return 0;
}
