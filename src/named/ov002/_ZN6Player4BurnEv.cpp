//cpp
// @symbol _ZN6Player4BurnEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int data_ov002_021100dc[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
}

int Player::Burn()
{
  if(func_ov002_020d82f0(((char*)this))==0) return 0;
  if(mIsMetal || mIsUnderwater) return 0;
  _ZN6Player11ChangeStateERNS_5StateE(((char*)this),data_ov002_021100dc);
  return 1;
}
