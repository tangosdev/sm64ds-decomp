//cpp
// @symbol _ZN6Player21St_WaitQuicksand_MainEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7ST_WAITE;
extern int _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int func_ov002_020c5dec(void*,int);
extern int func_ov002_020d22ec(void*,int);
extern int Player_AdvanceAnims(void*);
}

int Player::St_WaitQuicksand_Main()
{
  int v=mSinkDepth;
  if(v<0x1e000){
    _ZN6Player11ChangeStateERNS_5StateE(((char*)this),&_ZN6Player7ST_WAITE);
    return 1;
  }
  if(v>=0x46000){
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x82,0,0x1000,0);
  }
  if(mSinkDepth>=0xb4000){
    if(func_ov002_020c5dec(((char*)this),3)) return 1;
  }
  func_ov002_020d22ec(((char*)this),0);
  Player_AdvanceAnims(((char*)this));
  return 1;
}
