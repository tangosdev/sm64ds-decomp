//cpp
// @symbol _ZN6Player12St_Talk_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int Player_DisableInteraction(void* c);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int f, unsigned int g);
}

int Player::St_Talk_Init()
{
  mIsControlDisabled=1;
  Player_DisableInteraction(((char*)this));
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x47,0,0x1000,0);
  mAngleYSpeed=0;
  mHorzSpeed=0;
  mNoCtrlKind=0;
  mVertSpeed=0;
  mVertAccel=mVertSpeed;
  unk_716=1;
  mIsBodyClsnEnabled=0;
  mPosX=mPreClsnPosX;
  mPosY=mPreClsnPosY;
  mPosZ=mPreClsnPosZ;
  mStateWork=0;
  mIsOpeningBigDoor=0;
  return 1;
}
