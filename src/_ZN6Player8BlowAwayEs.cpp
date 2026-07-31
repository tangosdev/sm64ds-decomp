//cpp
// @symbol _ZN6Player8BlowAwayEs
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
}

void Player::BlowAway(short v)
{
  if(mIsMetal) return;
  if(mIsBalloon) return;
  mTargetAngleY=v;
  mAngleY=(short)(v+0x8000);
  _ZN6Player11ChangeStateERNS_5StateE(((char*)this),data_ov002_0211037c);
}
