//cpp
// @symbol _ZN6Player5ShockEj
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
struct State;
extern State data_ov002_021100c4;
extern int func_ov002_020d91e0(void* c, int a, int b, int d);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, State* st);
}

int Player::Shock(unsigned int j)
{
  if (func_ov002_020d82f0(((char*)this)) == 0) return 0;
  if (mIsMetal) j = 0;
  mStateWork = 0;
  if (func_ov002_020d91e0(((char*)this), j<<8, 1, 0)) mStateWork = 1;
  if (mIsUnderwater) {
    unsigned char* p = (unsigned char*)(((int)((char*)this) + 0x6e5) & 0xFFFFFFFFFFFFFFFF);
    *p |= 2;
  }
  _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_021100c4);
  return 1;
}
