//cpp
// @symbol _ZN6Player21St_SmallLaunchUp_MainEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
struct State;
extern State data_ov002_02110424;
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,State*);
extern void func_ov002_020bedd4(void*);
}

int Player::St_SmallLaunchUp_Main()
{
  if(mIsAirborne==0){
    _ZN6Player11ChangeStateERNS_5StateE(((char*)this),&data_ov002_02110424);
  }
  func_ov002_020bedd4(((char*)this));
  return 1;
}
