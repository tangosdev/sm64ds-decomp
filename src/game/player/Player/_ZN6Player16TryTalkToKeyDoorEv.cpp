//cpp
// @symbol _ZN6Player16TryTalkToKeyDoorEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void*,void*);
extern int _ZN6Player17SetNoControlStateEhih(void*,unsigned char,int,unsigned char);
extern int data_ov002_0211013c[];
extern int data_ov002_0211043c[];
}

int Player::TryTalkToKeyDoor()
{
  if(_ZN6Player7IsStateERNS_5StateE(((void*)this),data_ov002_0211013c)
     || _ZN6Player7IsStateERNS_5StateE(((void*)this),&ST_WAIT)
     || _ZN6Player7IsStateERNS_5StateE(((void*)this),data_ov002_0211043c)){
    _ZN6Player17SetNoControlStateEhih(((void*)this),0xb,-1,0);
    return 1;
  }
  return 0;
}
