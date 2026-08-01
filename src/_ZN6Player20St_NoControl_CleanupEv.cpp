//cpp
// @symbol _ZN6Player20St_NoControl_CleanupEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int EndKuppaScript(void);
}

int Player::St_NoControl_Cleanup()
{
  int v=0x1000;
  if(mIsMega) v=0x3000;
  mScaleX=v;
  mScaleY=v;
  mScaleZ=v;
  if(mNoCtrlKind==0x11) EndKuppaScript();
  return 1;
}
