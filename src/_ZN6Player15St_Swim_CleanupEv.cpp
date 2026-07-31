//cpp
// @symbol _ZN6Player15St_Swim_CleanupEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int func_ov002_020bd8c0(void*,int);
}

int Player::St_Swim_Cleanup()
{
  if(unk_6f7!=0){
    unk_6f7=0;
    func_ov002_020bd8c0(((char*)this),0x33);
  }
  return 1;
}
