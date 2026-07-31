//cpp
// @symbol _ZN6Player16St_LongJump_MainEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int func_ov002_020e28d4(void*,int,int);
extern int _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern int func_ov002_020bedd4(void*);
extern int data_ov002_02110424[];
}

int Player::St_LongJump_Main()
{
  func_ov002_020e28d4(((char*)this),0x1800,0x800);
  if(mIsAirborne==0){
    _ZN6Player11ChangeStateERNS_5StateE(((char*)this),data_ov002_02110424);
  }
  func_ov002_020bedd4(((char*)this));
  return 1;
}
