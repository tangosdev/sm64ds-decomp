//cpp
// @symbol _ZN6Player17St_SweepKick_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern int func_ov002_020d8a50(void*,int);
extern int _ZN6Player12FinishedAnimEv(void*);
extern int func_ov002_020bedd4(void*);
extern unsigned char data_020a0e40[];
extern unsigned short data_0209f49e[];
extern int data_ov002_021104e4[];
}

int Player::St_SweepKick_Main()
{
  if(*(unsigned short*)((char*)data_0209f49e + data_020a0e40[0]*0x18) & 2)
    _ZN6Player11ChangeStateERNS_5StateE(((void*)this),data_ov002_0211019c);
  func_ov002_020d8a50(((void*)this),3);
  if(_ZN6Player12FinishedAnimEv(((void*)this))){
    *(char*)((char*)&mStateStep)=1;
    _ZN6Player11ChangeStateERNS_5StateE(((void*)this),data_ov002_021104e4);
  }
  func_ov002_020bedd4(((void*)this));
  return 1;
}
